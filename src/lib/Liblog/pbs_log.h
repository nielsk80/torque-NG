#ifndef _PBS_LOG_H
#define _PBS_LOG_H
#include "license_pbs.h" /* See here for the software license */

#include "log.h"
#include <string_view>
#include <string>

int log_init(std::string_view suffix, std::string_view hostname);
int log_open(std::string_view filename, std::string_view directory);
int log_open_unlocked(std::string_view filename, std::string_view directory);

int job_log_open(std::string_view filename, std::string_view directory); 

void log_err(int errnum, std::string_view routine, std::string_view text); 

void log_ext(int errnum, std::string_view routine, std::string_view text, int severity); 

int log_job_record(std::string_view buf);
int log_job_record_unlocked(std::string_view buf);

void log_record(int eventtype, int objclass, std::string_view objname, std::string_view text); 
void log_record_unlocked(int eventtype, int objclass, std::string_view objname, std::string_view text); 

void log_close(int msg);

void job_log_close(int msg);

int log_remove_old(char *DirPath, unsigned long ExpireTime); 

void log_roll(int max_depth);

long log_size(void);

long job_log_size(void);

void print_trace(int socknum);

void log_get_set_eventclass(int *objclass, SGetter action);

void log_format_trq_timestamp(char *time_formatted_str, unsigned int buflen);

void log_set_hostname_sharelogging(const char *server_name, const char *server_port);

void log_get_host_port(char *host_n_port, size_t s);

#endif /* _PBS_LOG_H */
