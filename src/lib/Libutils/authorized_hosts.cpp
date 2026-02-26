#include "authorized_hosts.hpp"
#include "pbs_log.h"
#include <sstream>
#include <algorithm>
#include <vector>

authorized_hosts::authorized_hosts() : auth_map() {}

/**
 * Internal helper to format IP addresses into a human-readable string.
 * Replaces legacy sprintf logic.
 */
std::string format_ip(unsigned long addr) {
    return std::to_string((addr & 0xFF000000) >> 24) + "." +
           std::to_string((addr & 0x00FF0000) >> 16) + "." +
           std::to_string((addr & 0x0000FF00) >> 8) + "." +
           std::to_string(addr & 0x000000FF);
}

/**
 * Internal helper: Checks authorization without acquiring a lock.
 * Used by public methods that already hold the mutex.
 */
bool authorized_hosts::is_authorized_impl(unsigned long addr, unsigned short port) const {
    auto it = auth_map.find(addr);
    if (it != auth_map.end()) {
        return it->second.find(port) != it->second.end();
    }
    return false;
}

void authorized_hosts::add_authorized_address(
    unsigned long addr, 
    unsigned short port, 
    const std::string &host) {
    
    std::lock_guard<std::mutex> lock(this->auth_mutex);
    
    this->auth_map[addr][port] = host;

    std::string msg = "Added authorized host: " + host + " (" + format_ip(addr) + ":" + std::to_string(port) + ")";
    log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, "Auth", msg.c_str());
}

void authorized_hosts::clear() {
    std::lock_guard<std::mutex> lock(this->auth_mutex);
    this->auth_map.clear();
    log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, "Auth", "Authorized hosts map cleared.");
}

bool authorized_hosts::is_authorized(unsigned long addr) {
    std::lock_guard<std::mutex> lock(this->auth_mutex);
    return this->auth_map.find(addr) != this->auth_map.end();
}

bool authorized_hosts::is_authorized(unsigned long addr, unsigned short port) {
    std::lock_guard<std::mutex> lock(this->auth_mutex);
    return this->is_authorized_impl(addr, port);
}

void authorized_hosts::list_authorized_hosts(std::string &output) {
    std::lock_guard<std::mutex> lock(this->auth_mutex);
    std::stringstream ss;

    for (auto const& [addr, port_map] : this->auth_map) {
        for (auto const& [port, host] : port_map) {
            if (!ss.str().empty()) {
                ss << ",";
            }
            ss << format_ip(addr) << ":" << port;
        }
    }
    output = ss.str();
}

bool authorized_hosts::remove_address(unsigned long addr, unsigned short port) {
    std::lock_guard<std::mutex> lock(this->auth_mutex);
    
    auto it = this->auth_map.find(addr);
    if (it != this->auth_map.end()) {
        if (it->second.erase(port) > 0) {
            // If the port map for this IP is now empty, remove the IP entry entirely
            if (it->second.empty()) {
                this->auth_map.erase(it);
            }
            return true;
        }
    }
    return false;
}

// Note: get_authorized_node implementations would go here, 
// likely returning a pointer from the existing pbs_nodes structures.