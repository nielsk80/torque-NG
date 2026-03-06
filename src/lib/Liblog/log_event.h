/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include <string_view>

/**
 * log_event - Standard entry point for logging events.
 * This is a thin wrapper around the SafeLog singleton.
 */
void log_event(int eventtype, int objclass, std::string_view objname, std::string_view text);
