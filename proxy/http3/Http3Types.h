/** @file
 *
 *  A brief file description
 *
 *  @section license License
 *
 *  Licensed to the Apache Software Foundation (ASF) under one
 *  or more contributor license agreements.  See the NOTICE file
 *  distributed with this work for additional information
 *  regarding copyright ownership.  The ASF licenses this file
 *  to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include "tscore/ink_platform.h"

#include <memory>

enum class Http3StreamType : uint8_t {
  CONTROL       = 0x43, ///< HTTP/3
  QPACK_ENCODER = 0x48, ///< QPACK
  PUSH          = 0x50, ///< HTTP/3
  QPACK_DECODER = 0x68, ///< QPACK
  RESERVED      = 0x1F,
  UNKOWN        = 0xFF,
};

enum class Http3SettingsId : uint16_t {
  HEADER_TABLE_SIZE     = 0x01, ///< QPACK Settings
  RESERVED_1            = 0x02, ///< HTTP/3 Settings
  RESERVED_2            = 0x03, ///< HTTP/3 Settings
  RESERVED_3            = 0x04, ///< HTTP/3 Settings
  RESERVED_4            = 0x05, ///< HTTP/3 Settings
  MAX_HEADER_LIST_SIZE  = 0x06, ///< HTTP/3 Settings
  QPACK_BLOCKED_STREAMS = 0x07, ///< QPACK Settings
  NUM_PLACEHOLDERS      = 0x08, ///< HTTP/3 Settings
  UNKNOWN               = 0x0A0A,
};

// Update Http3Frame::type(const uint8_t *) too when you modify this list
enum class Http3FrameType : uint8_t {
  DATA              = 0x00,
  HEADERS           = 0x01,
  PRIORITY          = 0x02,
  CANCEL_PUSH       = 0x03,
  SETTINGS          = 0x04,
  PUSH_PROMISE      = 0x05,
  X_RESERVED_1      = 0x06,
  GOAWAY            = 0x07,
  X_RESERVED_2      = 0x08,
  X_RESERVED_3      = 0x09,
  MAX_PUSH_ID       = 0x0D,
  DUPLICATE_PUSH_ID = 0x0E,
  X_MAX_DEFINED     = 0x0E,
  UNKNOWN           = 0xFF,
};

enum class Http3ErrorClass {
  NONE,
  APPLICATION,
};

// Actual error code of QPACK is not decided yet on qpack-05. It will be changed.
enum class Http3ErrorCode : uint16_t {
  NO_ERROR                = 0x00,
  WRONG_SETTING_DIRECTION = 0x01,
  PUSH_REFUSED            = 0x02,
  INTERNAL_ERROR          = 0x03,
  PUSH_ALREADY_IN_CACHE   = 0x04,
  REQUEST_CANCELLED       = 0x05,
  INCOMPLETE_REQUEST      = 0x06,
  CONNECT_ERROR           = 0x07,
  EXCESSIVE_LOAD          = 0x08,
  VERSION_FALLBACK        = 0x09,
  WRONG_STREAM            = 0x0A,
  PUSH_LIMIT_EXCEEDED     = 0x0B,
  DUPLICATE_PUSH          = 0x0C,
  UNKNOWN_STREAM_TYPE     = 0x0D,
  WRONG_STREAM_COUNT      = 0x0E,
  CLOSED_CRITICAL_STREAM  = 0x0F,
  WRONG_STREAM_DIRECTION  = 0x10,
  EARLY_RESPONSE          = 0x11,
  MISSING_SETTINGS        = 0x12,
  UNEXPECTED_FRAME        = 0x13,
  QPACK_DECOMPRESSION_FAILED, ///< QPACK Error Code
  QPACK_ENCODER_STREAM_ERROR, ///< QPACK Error Code
  QPACK_DECODER_STREAM_ERROR, ///< QPACK Error Code
  GENERAL_PROTOCOL_ERROR = 0xFF,
  MALFORMED_FRAME        = 0x0100,
};

class Http3Error
{
public:
  virtual ~Http3Error() {}
  uint16_t code();

  Http3ErrorClass cls = Http3ErrorClass::NONE;
  union {
    Http3ErrorCode app_error_code;
  };
  const char *msg = nullptr;

protected:
  Http3Error(){};
  Http3Error(const Http3ErrorCode error_code, const char *error_msg = nullptr)
    : cls(Http3ErrorClass::APPLICATION), app_error_code(error_code), msg(error_msg){};
};

class Http3NoError : public Http3Error
{
public:
  Http3NoError() : Http3Error() {}
};

class Http3ConnectionError : public Http3Error
{
public:
  Http3ConnectionError() : Http3Error() {}
  Http3ConnectionError(const Http3ErrorCode error_code, const char *error_msg = nullptr) : Http3Error(error_code, error_msg){};
};

class Http3Stream;

class Http3StreamError : public Http3Error
{
public:
  Http3StreamError() : Http3Error() {}
  Http3StreamError(Http3Stream *s, const Http3ErrorCode error_code, const char *error_msg = nullptr)
    : Http3Error(error_code, error_msg), stream(s){};

  Http3Stream *stream;
};

using Http3ErrorUPtr           = std::unique_ptr<Http3Error>;
using Http3ConnectionErrorUPtr = std::unique_ptr<Http3ConnectionError>;
using Http3StreamErrorUPtr     = std::unique_ptr<Http3StreamError>;
