/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "log_event.h"
#include "pbs_log.h"

/**
 * log_event - Formats legacy calls for the new logging engine.
 */
void log_event(int eventtype, int objclass, std::string_view objname, std::string_view text) {
    // We pass directly to our modernized bridge in pbs_log.cpp
    log_record(eventtype, objclass, objname, text);
}