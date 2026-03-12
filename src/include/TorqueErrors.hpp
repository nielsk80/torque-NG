/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * All rights reserved.
 *
 * Licensed under the OpenPBS v2.3 Software License.
 * See the LICENSE file in the project root for full license details.
 *
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#pragma once

#include <cstdint>
#include <string_view>

namespace Torque {

/**
 * @brief Type-safe error codes for torque-NG.
 * This enum class contains every significant error code from pbs_error_db.h.
 */
enum class ErrorCode : int32_t {
    None                   = 0,
    
    // Client Errors (15000+)
    UnknownJobId           = 15001, 
    NoAttribute            = 15002, 
    AttrReadOnly           = 15003, 
    InvalidValue           = 15004, 
    Permission             = 15020, 
    UnknownQueue           = 15021,
    JobExceedsLimits       = 15033,
    InvalidArgument        = 15034,
    Internal               = 15036, 
    SystemError            = 15038, 
    NoMemory               = 15044,
    CgroupFail             = 15082, 
    
    // RM Errors (15200+)
    RmUnknown              = 15201, 
    RmBadParam             = 15202,
    RmNoParam              = 15203,
    RmExist                = 15204,
    RmNoConfig             = 15205,
    RmNoConnect            = 15206
};

/**
 * @brief Global interface to get the human-readable string.
 * The implementation is hidden in TorqueErrors.cpp.
 */
std::string_view to_string(ErrorCode ec);

} // namespace Torque