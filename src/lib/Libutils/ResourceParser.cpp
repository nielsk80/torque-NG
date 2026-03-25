/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "ResourceParser.hpp"
#include <algorithm>
#include <cctype>
#include <charconv> // For high-performance std::from_chars
#include <cmath>

namespace torque_ng::utils {

/**
 * @brief Converts a human-readable size string to a raw byte count.
 * * Supports suffixes: b, kb, mb, gb, tb, pb (case-insensitive).
 * Defaults to bytes if no suffix is provided.
 * * @param size_str The string view to parse (e.g., "2gb", "512mb").
 * @return uint64_t The equivalent size in bytes. Returns 0 on parse failure.
 */
uint64_t ResourceParser::string_to_size(std::string_view size_str) {
    if (size_str.empty()) return 0;

    // Find the first non-digit to identify the suffix
    auto first_alpha = std::find_if(size_str.begin(), size_str.end(), [](unsigned char c) {
        return std::isalpha(c);
    });

    uint64_t value = 0;
    const char* end_ptr = (first_alpha == size_str.end()) ? 
                       size_str.data() + size_str.size() : 
                       &(*first_alpha);

   auto [ptr, ec] = std::from_chars(size_str.data(), end_ptr, value);

    if (ec != std::errc() && ec != std::errc::invalid_argument) {
        return 0; // Parse error for the numeric part
    }

    // If no suffix, assume bytes
    if (first_alpha == size_str.end()) {
        return value;
    }

    // 1. Extract suffix as a view (No heap allocation)
    std::string_view suffix = size_str.substr(std::distance(size_str.begin(), first_alpha));

    // 2. Create a small lambda for case-insensitive matching
    auto iequals = [](std::string_view a, std::string_view b) {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                         [](char a_char, char b_char) {
                             return std::tolower(static_cast<unsigned char>(a_char)) == 
                                    std::tolower(static_cast<unsigned char>(b_char));
                         });
    };

    // 3. Map suffixes using the case-insensitive helper
    if (iequals(suffix, "kb") || iequals(suffix, "k")) return value * 1024ULL;
    if (iequals(suffix, "mb") || iequals(suffix, "m")) return value * 1024ULL * 1024ULL;
    if (iequals(suffix, "gb") || iequals(suffix, "g")) return value * 1024ULL * 1024ULL * 1024ULL;
    if (iequals(suffix, "tb") || iequals(suffix, "t")) return value * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    if (iequals(suffix, "pb") || iequals(suffix, "p")) return value * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;

    return value;
}

/**
 * @brief Converts a Torque-style duration string (HH:MM:SS) to total seconds.
 * * Supports formats:
 * - HH:MM:SS
 * - MM:SS
 * - SS
 * * @param dur_str The string view to parse.
 * @return uint64_t Total duration in seconds. Returns 0 on parse failure.
 */
uint64_t ResourceParser::string_to_duration(std::string_view dur_str) {
    if (dur_str.empty()) return 0;

    // Use a fixed-size array to avoid heap allocation from std::vector
    uint64_t parts[3] = {0, 0, 0};
    int count = 0;

    size_t start = 0;
    size_t end = dur_str.find(':');

    while (start < dur_str.length() && count < 3) {
        std::string_view token = dur_str.substr(start, end - start);
        uint64_t val = 0;
        auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), val);
        
        if (ec == std::errc()) {
            parts[count++] = val;
        }

        if (end == std::string_view::npos) break;
        start = end + 1;
        end = dur_str.find(':', start);
    }

    // Torque convention: 
    // 1 part  -> seconds
    // 2 parts -> MM:SS
    // 3 parts -> HH:MM:SS
    if (count == 3) return (parts[0] * 3600) + (parts[1] * 60) + parts[2];
    if (count == 2) return (parts[0] * 60) + parts[1];
    if (count == 1) return parts[0];

    return 0;
}

/**
 * @brief Main entry point to parse a full resource string (e.g., "mem=2gb,nodes=1:ppn=2").
 * @param raw_resources The full string of comma-separated resources.
 * @param target Pointer to the Protobuf ResourceList message to populate.
 */
void ResourceParser::parse_into(std::string_view raw_resources, 
                                torque_ng::ResourceList* target) {
    if (raw_resources.empty() || target == nullptr) return;

    size_t start = 0;
    size_t end = raw_resources.find(',');

    while (start < raw_resources.length()) {
        // Extract a single resource pair (e.g., "mem=2gb")
        std::string_view token = raw_resources.substr(start, end - start);
        
        if (!token.empty()) {
            size_t eq_pos = token.find('=');
            if (eq_pos != std::string_view::npos) {
                std::string_view key = token.substr(0, eq_pos);
                std::string_view value = token.substr(eq_pos + 1);
                
                // Pass the key-value pair to our specialized logic
                handle_resource(key, value, target);
            }
        }

        if (end == std::string_view::npos) break;
        
        start = end + 1;
        end = raw_resources.find(',', start);
    }
}

/**
 * @brief Routes individual key-value pairs to the appropriate Protobuf fields.
 * Now supports legacy Torque colon-delimited sub-resources (e.g., nodes=1:ppn=8).
 */
void ResourceParser::handle_resource(std::string_view key, 
                                     std::string_view value, 
                                     torque_ng::ResourceList* target) {
    
    // 1. Handle Time Resources (Highest Priority to avoid colon-split issues)
    if (key == "walltime" || key == "cput" || key == "pcput") {
        uint64_t seconds = string_to_duration(value);
        (*target->mutable_size_resources())[std::string(key)] = seconds;
        return;
    }

    // 2. Handle standard Memory/Size Resources
    if (key == "mem" || key == "vmem" || key == "pmem") {
        uint64_t bytes = string_to_size(value);
        (*target->mutable_size_resources())[std::string(key)] = bytes;
        return;
    }

    // 3. Handle legacy colon syntax (e.g., nodes=1:ppn=8)
    // We only reach here if it wasn't a time or memory key
    if (value.find(':') != std::string_view::npos) {
        size_t sub_start = 0;
        size_t sub_end = value.find(':');

        while (sub_start < value.length()) {
            std::string_view sub_token = value.substr(sub_start, sub_end - sub_start);
            
            if (size_t sub_eq = sub_token.find('='); sub_eq != std::string_view::npos) {
                std::string_view sub_key = sub_token.substr(0, sub_eq);
                std::string_view sub_val = sub_token.substr(sub_eq + 1);
                (*target->mutable_string_resources())[std::string(sub_key)] = std::string(sub_val);
            } else {
                // Positional sub-tokens (the '1' in nodes=1)
                std::string complex_key = std::string(key) + "_value";
                (*target->mutable_string_resources())[complex_key] = std::string(sub_token);
            }

            if (sub_end == std::string_view::npos) break;
            sub_start = sub_end + 1;
            sub_end = value.find(':', sub_start);
        }
        return; 
    }

    // 4. Default Fallback: Store as a generic string resource
    (*target->mutable_string_resources())[std::string(key)] = std::string(value);
}
} // namespace torque_ng::utils
