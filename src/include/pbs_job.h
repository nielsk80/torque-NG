/*
 * Copyright (C) 2026 Kenneth Nielson
 * Part of the torque-NG project.
 */

#pragma once

#include "pbs_config.h"
#include "list_link.h"
#include "attribute.h"

#ifdef __cplusplus
#include <string>
#include <vector>
#include <memory>
#include "torque_ng.pb.h" // Include your new Protobuf definitions

namespace torque_ng {
    // A modern wrapper for the Job structure if needed for C++ logic
    class JobWrapper {
    public:
        torque_ng::Job proto_data;
        // Add modern methods here for job manipulation
    };
}

extern "C" {
#endif

/*
 * The legacy 'job' structure. 
 * We keep the struct name so mom_main.c doesn't break, 
 * but we guard C++ members.
 */
typedef struct job {
  list_link  ji_alljobs;  /* list of all jobs */
  list_link  ji_state;    /* list of jobs in current state */
  
  char       ji_qs[64];   /* legacy job id */
  int        ji_state_code; 

#ifdef __cplusplus
  // Modern C++ members only visible to g++
  std::string ji_external_id;
  std::unique_ptr<torque_ng::Job> ji_proto;
#endif

} job;

// Legacy function signatures
job *pbs_job_create(void);
void pbs_job_delete(job *);

#ifdef __cplusplus
} // extern "C"
#endif