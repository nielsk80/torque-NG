#include "ResourceParser.hpp"
#include <iostream>
#include <cassert>
#include <iomanip>

using namespace torque_ng::utils;

void test_size_parsing() {
    std::cout << "Testing Size Parsing...\n";
    
    // Test Case: Standard suffixes
    assert(ResourceParser::string_to_size("2gb") == 2147483648ULL);
    assert(ResourceParser::string_to_size("512mb") == 536870912ULL);
    
    // Test Case: Case insensitivity
    assert(ResourceParser::string_to_size("10Kb") == 10240ULL);
    
    // Test Case: No suffix (defaults to bytes)
    assert(ResourceParser::string_to_size("1024") == 1024ULL);
    
    // Test Case: Large values (Petabytes)
    assert(ResourceParser::string_to_size("1pb") == 1125899906842624ULL);

    std::cout << "  [PASS] Size tests successful.\n";
}

void test_duration_parsing() {
    std::cout << "Testing Duration Parsing...\n";
    
    // Test Case: HH:MM:SS
    assert(ResourceParser::string_to_duration("01:30:00") == 5400ULL);
    
    // Test Case: MM:SS
    assert(ResourceParser::string_to_duration("10:00") == 600ULL);
    
    // Test Case: Large hours
    assert(ResourceParser::string_to_duration("100:00:00") == 360000ULL);

    std::cout << "  [PASS] Duration tests successful.\n";
}

void test_full_parsing() {
    std::cout << "Testing Full String Parsing...\n";
    
    torque_ng::ResourceList list;
    
    // Complex input: mixed time, size, and legacy colon syntax
    std::string_view input = "mem=2gb,walltime=01:00:00,nodes=2:ppn=8,arch=x86_64";
    ResourceParser::parse_into(input, &list);

    // Verify Memory
    assert(list.size_resources().at("mem") == 2147483648ULL);
    
    // Verify Walltime
    assert(list.size_resources().at("walltime") == 3600ULL);
    
    // Verify Legacy Colon Logic
    assert(list.string_resources().at("nodes_value") == "2");
    assert(list.string_resources().at("ppn") == "8");
    
    // Verify Generic String
    assert(list.string_resources().at("arch") == "x86_64");

    std::cout << "  [PASS] Full string parsing successful.\n";
}

int main() {
    try {
        test_size_parsing();
        test_duration_parsing();
        test_full_parsing();
        
        std::cout << "\nALL TESTS PASSED SUCCESSFULLY\n";
    } catch (const std::exception& e) {
        std::cerr << "\nTEST FAILED: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
