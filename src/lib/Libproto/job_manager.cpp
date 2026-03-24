/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "torque_ng.pb.h"
#include <string_view>
#include <iostream>

namespace torque_ng {

void send_job_update(const JobStatusUpdate& update) {
    // Protobuf gives you direct serialization to strings or streams
    std::string binary_data;
    if (update.SerializeToString(&binary_data)) {
        // Pass binary_data directly to your new Libnet socket logic
        std::cout << "Sending " << binary_data.size() << " bytes of job data.\n";
    }
}

} // namespace torque_ng