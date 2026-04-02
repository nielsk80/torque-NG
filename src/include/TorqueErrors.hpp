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
 * Enforces error checking via [[nodiscard]].
 */
enum class [[nodiscard]] ErrorCode : int32_t {
  None = 0,

  // Core PBS Errors (15000+)
  UnknownJobId = 15001,         // PBSE_UNKJOBID
  NoAttribute = 15002,          // PBSE_NOATTR
  AttrReadOnly = 15003,         // PBSE_ATTRRO
  InvalidValue = 15004,         // PBSE_IVALREQ
  UnknownRequest = 15005,       // PBSE_UNKREQ
  TooManySubmitRetries = 15006, // PBSE_TOOMANY
  Permission = 15007,           // PBSE_PERM
  BadHost = 15008,              // PBSE_BADHOST
  JobExists = 15009,            // PBSE_JOBEXIST
  SystemError = 15010,          // PBSE_SYSTEM
  Internal = 15011,             // PBSE_INTERNAL
  RegRoute = 15012,             // PBSE_REGROUTE
  UnknownSignal = 15013,        // PBSE_UNKSIG
  BadAttrValue = 15014,         // PBSE_BADATVAL
  AttrModRun = 15015,           // PBSE_MODATRRUN
  BadJobState = 15016,          // PBSE_BADSTATE
  UnknownQueue = 15018,         // PBSE_UNKQUE
  BadCredential = 15019,        // PBSE_BADCRED
  ExpiredCredential = 15020,    // PBSE_EXPIRED
  QueueNotEnabled = 15021,      // PBSE_QUNOENB
  QueueAccessDenied = 15022,    // PBSE_QACESS
  BadUser = 15023,              // PBSE_BADUSER
  HopCountExceeded = 15024,     // PBSE_HOPCOUNT
  QueueExists = 15025,          // PBSE_QUEEXIST
  AttrTypeMismatch = 15026,     // PBSE_ATTRTYPE
  QueueBusy = 15027,            // PBSE_QUEBUSY
  QueueNameTooLong = 15028,     // PBSE_QUENBIG
  NotSupported = 15029,         // PBSE_NOSUP
  ProtocolError = 15031,        // PBSE_PROTOCOL
  NoConnections = 15033,        // PBSE_NOCONNECTS
  NoServer = 15034,             // PBSE_NOSERVER
  UnknownResource = 15035,      // PBSE_UNKRESC
  JobExceedsLimits = 15036,     // PBSE_EXCQRESC
  NoDefaultQueue = 15037,       // PBSE_QUENODFLT
  NotRerunnable = 15038,        // PBSE_NORERUN
  RouteRejected = 15039,        // PBSE_ROUTEREJ
  MomReject = 15041,            // PBSE_MOMREJECT
  BadScript = 15042,            // PBSE_BADSCRIPT
  StageInFail = 15043,          // PBSE_STAGEIN
  ResourcesUnavailable = 15044, // PBSE_RESCUNAV
  BadGroup = 15045,             // PBSE_BADGRP
  MaxQueued = 15046,            // PBSE_MAXQUED
  CheckpointBusy = 15047,       // PBSE_CKPBSY
  LimitExceeded = 15048,        // PBSE_EXLIMIT
  BadAccount = 15049,           // PBSE_BADACCT
  AlreadyExiting = 15050,       // PBSE_ALRDYEXIT
  NoCopyFile = 15051,           // PBSE_NOCOPYFILE
  CleanedOut = 15052,           // PBSE_CLEANEDOUT
  BadDependency = 15054,        // PBSE_BADDEPEND
  DuplicateList = 15055,        // PBSE_DUPLIST
  DisProtocolError = 15056,     // PBSE_DISPROTO
  ServerDown = 15060,           // PBSE_SVRDOWN
  UnknownNode = 15062,          // PBSE_UNKNODE
  UnknownNodeAttr = 15063,      // PBSE_UNKNODEATR
  NoNodes = 15064,              // PBSE_NONODES
  NodeNameTooBig = 15065,       // PBSE_NODENBIG
  NodeExists = 15066,           // PBSE_NODEEXIST
  BadNodeAttrValue = 15067,     // PBSE_BADNDATVAL
  CgroupFail = 15082,           // torque-specific

  // RM Errors (15200+)
  RmUnknown = 15201,
  RmBadParam = 15202,
  RmNoParam = 15203,
  RmExist = 15204,
  RmNoConfig = 15205,
  RmNoConnect = 15206
};

/**
 * @brief Global interface to get the human-readable string.
 * Marking this inline/constexpr allows the compiler to optimize lookups.
 */
[[nodiscard]] inline constexpr std::string_view to_string(ErrorCode ec) {
  switch (ec) {
  case ErrorCode::None:
    return "PBSE_NONE";

  // Client Errors
  case ErrorCode::UnknownJobId:
    return "PBSE_UNKJOBID";
  case ErrorCode::NoAttribute:
    return "PBSE_NOATTR";
  case ErrorCode::AttrReadOnly:
    return "PBSE_ATTRRO";
  case ErrorCode::InvalidValue:
    return "PBSE_IVALREQ";
  case ErrorCode::UnknownRequest:
    return "PBSE_UNKREQ";
  case ErrorCode::TooManySubmitRetries:
    return "PBSE_TOOMANY";
  case ErrorCode::Permission:
    return "PBSE_PERM";
  case ErrorCode::BadHost:
    return "PBSE_BADHOST";
  case ErrorCode::JobExists:
    return "PBSE_JOBEXIST";
  case ErrorCode::SystemError:
    return "PBSE_SYSTEM";
  case ErrorCode::Internal:
    return "PBSE_INTERNAL";
  case ErrorCode::RegRoute:
    return "PBSE_REGROUTE";
  case ErrorCode::UnknownSignal:
    return "PBSE_UNKSIG";
  case ErrorCode::BadAttrValue:
    return "PBSE_BADATVAL";
  case ErrorCode::AttrModRun:
    return "PBSE_MODATRRUN";
  case ErrorCode::BadJobState:
    return "PBSE_BADSTATE";
  case ErrorCode::UnknownQueue:
    return "PBSE_UNKQUE";
  case ErrorCode::BadCredential:
    return "PBSE_BADCRED";
  case ErrorCode::ExpiredCredential:
    return "PBSE_EXPIRED";
  case ErrorCode::QueueNotEnabled:
    return "PBSE_QUNOENB";
  case ErrorCode::QueueAccessDenied:
    return "PBSE_QACESS";
  case ErrorCode::BadUser:
    return "PBSE_BADUSER";
  case ErrorCode::HopCountExceeded:
    return "PBSE_HOPCOUNT";
  case ErrorCode::QueueExists:
    return "PBSE_QUEEXIST";
  case ErrorCode::AttrTypeMismatch:
    return "PBSE_ATTRTYPE";
  case ErrorCode::QueueBusy:
    return "PBSE_QUEBUSY";
  case ErrorCode::QueueNameTooLong:
    return "PBSE_QUENBIG";
  case ErrorCode::NotSupported:
    return "PBSE_NOSUP";
  case ErrorCode::ProtocolError:
    return "PBSE_PROTOCOL";
  case ErrorCode::NoConnections:
    return "PBSE_NOCONNECTS";
  case ErrorCode::NoServer:
    return "PBSE_NOSERVER";
  case ErrorCode::UnknownResource:
    return "PBSE_UNKRESC";
  case ErrorCode::JobExceedsLimits:
    return "PBSE_EXCQRESC";
  case ErrorCode::NoDefaultQueue:
    return "PBSE_QUENODFLT";
  case ErrorCode::NotRerunnable:
    return "PBSE_NORERUN";
  case ErrorCode::RouteRejected:
    return "PBSE_ROUTEREJ";
  case ErrorCode::MomReject:
    return "PBSE_MOMREJECT";
  case ErrorCode::BadScript:
    return "PBSE_BADSCRIPT";
  case ErrorCode::StageInFail:
    return "PBSE_STAGEIN";
  case ErrorCode::ResourcesUnavailable:
    return "PBSE_RESCUNAV";
  case ErrorCode::BadGroup:
    return "PBSE_BADGRP";
  case ErrorCode::MaxQueued:
    return "PBSE_MAXQUED";
  case ErrorCode::CheckpointBusy:
    return "PBSE_CKPBSY";
  case ErrorCode::LimitExceeded:
    return "PBSE_EXLIMIT";
  case ErrorCode::BadAccount:
    return "PBSE_BADACCT";
  case ErrorCode::AlreadyExiting:
    return "PBSE_ALRDYEXIT";
  case ErrorCode::NoCopyFile:
    return "PBSE_NOCOPYFILE";
  case ErrorCode::CleanedOut:
    return "PBSE_CLEANEDOUT";
  case ErrorCode::BadDependency:
    return "PBSE_BADDEPEND";
  case ErrorCode::DuplicateList:
    return "PBSE_DUPLIST";
  case ErrorCode::DisProtocolError:
    return "PBSE_DISPROTO";
  case ErrorCode::ServerDown:
    return "PBSE_SVRDOWN";
  case ErrorCode::UnknownNode:
    return "PBSE_UNKNODE";
  case ErrorCode::UnknownNodeAttr:
    return "PBSE_UNKNODEATR";
  case ErrorCode::NoNodes:
    return "PBSE_NONODES";
  case ErrorCode::NodeNameTooBig:
    return "PBSE_NODENBIG";
  case ErrorCode::NodeExists:
    return "PBSE_NODEEXIST";
  case ErrorCode::BadNodeAttrValue:
    return "PBSE_BADNDATVAL";
  case ErrorCode::CgroupFail:
    return "TORQUE_CGROUP_FAIL";

  // RM Errors
  case ErrorCode::RmUnknown:
    return "RM_ERR_UNKNOWN";
  case ErrorCode::RmBadParam:
    return "RM_ERR_BADPARAM";
  case ErrorCode::RmNoParam:
    return "RM_ERR_NOPARAM";
  case ErrorCode::RmExist:
    return "RM_ERR_EXIST";
  case ErrorCode::RmNoConfig:
    return "RM_ERR_NOCONFIG";
  case ErrorCode::RmNoConnect:
    return "RM_ERR_NOCONNECT";

  default:
    return "PBSE_UNKNOWN_ERROR";
  }
}

} // namespace Torque