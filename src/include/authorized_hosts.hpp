// authorized_hosts.hpp
#pragma once

#include <map>
#include <string>
#include <mutex>
#include "pbs_nodes.h" // Needed for pbsnode return types

class authorized_hosts {
private:
    // Using recursive_mutex to match legacy pthread behavior if re-entry occurs
    mutable std::mutex auth_mutex;
    
    // Map key: IP address, Value: Port-to-Hostname map
    std::map<unsigned long, std::map<unsigned short, std::string>> auth_map;

    // Private "impl" methods - These assume the lock is ALREADY held
    bool is_authorized_impl(unsigned long addr, unsigned short port) const;
    void log_auth_event(const std::string& action, const std::string& host, unsigned long addr, unsigned short port);

public:
    authorized_hosts();

    void add_authorized_address(unsigned long addr, unsigned short port, const std::string &host);
    void clear();

    bool     is_authorized(unsigned long addr);
    bool     is_authorized(unsigned long addr, unsigned short port);
    pbsnode *get_authorized_node(unsigned long addr);
    pbsnode *get_authorized_node(unsigned long addr, unsigned short port);
    void     list_authorized_hosts(std::string &output);
    bool     remove_address(unsigned long addr, unsigned short port);
};

extern authorized_hosts auth_hosts;