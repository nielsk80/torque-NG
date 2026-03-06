/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#pragma once

#include <string_view>

namespace Torque {

/**
 * pbse_to_txt - Returns the human-readable string for a PBS error code.
 * Replaces the legacy pbs_err_text array.
 */
std::string_view pbse_to_txt(int err_code);

} // namespace Torque