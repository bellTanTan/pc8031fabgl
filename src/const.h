/*
  Created by tan (trinity09181718@gmail.com)
  Copyright (c) 2022 tan
  All rights reserved.

* Please contact trinity09181718@gmail.com if you need a commercial license.
* This software is available under GPL v3.
 */

#pragma once

const char * ssid     = "test001";
const char * password = "password001";

extern const long gmtOffsetSec;

const char * ntpServer       = "192.168.1.250";
const long gmtOffsetSec      = 3600 * 9;
const int  daylightOffsetSec = 0;
                                            // 192.168.1.101 esp32-aki
const IPAddress ip( 192, 168, 1, 101 );     // for fixed IP Address
const IPAddress gateway( 192, 168, 1, 1 );  // gateway
const IPAddress subnet( 255, 255, 255, 0 ); // subnet mask
const IPAddress DNS( 192, 168, 1, 1 );      // DNS

const char * mDNS_NAME = "pc8031fabgl";
const uint16_t httpTcpPort = 80;            // http TCP Port No

