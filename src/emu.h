/*
  Created by tan (trinity09181718@gmail.com)
  Copyright (c) 2022 tan
  All rights reserved.

* Please contact trinity09181718@gmail.com if you need a commercial license.
* This software is available under GPL v3.

* This library and related software is available under GPL v3.

  FabGL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FabGL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FabGL.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#pragma GCC optimize ("O2")

#include <stdint.h>
#include <stdarg.h>
#include <string.h>


//#define DONT_SEEK_SOUND
//#define UPD765A_DONT_WAIT_SEEK

//#define _DEBUG
//#define _SHIFT_IO_TIME
//#define _UPD765A_DEBUG
//#define _HTTP_DEBUG

#define SD_MOUNT_PATH           "/SD"
#define PC80312W_ROM            "/PC8001/PC-8031-2W.ROM"
#define PC80S31_ROM             "/PC8001/PC-80S31.ROM"
#define DISK_ROM                "/PC8001/DISK.ROM"
#define PC8001MEDIA_DISK        "/PC8001/MEDIA/DISK"
#define D88_FILE_EXTENSION      ".d88"
#define MAX_DRIVE               (4)

#define IN_SL                   23    // ESP32 GPIO -> LEVEL SHIFT -> SN74HC165N SH/LD
#define IN_CLK                  19    // ESP32 GPIO -> LEVEL SHIFT -> SN74HC165N CLK
#define IN_QH                   18    // ESP32 GPIO <- LEVEL SHIFT <- SN74HC165N QH
#define OUT_SER                 5     // ESP32 GPIO -> LEVEL SHIFT -> SN74HC595N SER
#define OUT_RCLK                4     // ESP32 GPIO -> LEVEL SHIFT -> SN74HC595N RCLK
#define OUT_SRCLK               15    // ESP32 GPIO -> LEVEL SHIFT -> SN74HC595N SRCLK
#define SYSTEM_STATUS           33    // ESP32 GPIO SYSTEM STATUS LED
                                      //  OFF                        : Wi-Fi non connected
                                      //  ON                         : pc8031FabGL web server ready
                                      //  Very First flicker(50msec) : fd setting complete
                                      //  First flicker(100msec)     : Wi-Fi connect failed
                                      //  Slow  flicker(500msec)     : Wi-Fi connecting

#define ARRAY_SIZE( array )     ( (int)( sizeof( array ) / sizeof( (array)[0] ) ) )
#define HLT                     { while ( 1 ) { delay( 500 ); } }

#ifdef _DEBUG
  extern  void _DEBUG_PRINT( const char * format, ... );
#else
  #define _DEBUG_PRINT( format, ... ) { }
#endif // _DEBUG

#ifdef _UPD765A_DEBUG
  extern  void _UPD765A_DEBUG_PRINT( const char * format, ... );
#else
  #define _UPD765A_DEBUG_PRINT( format, ... ) { }
#endif // _UPD765A_DEBUG

#ifdef _HTTP_DEBUG
  extern  void _HTTP_DEBUG_PRINT( const char * format, ... );
#else
  #define _HTTP_DEBUG_PRINT( format, ... ) { }
#endif // _HTTP_DEBUG

extern  void _MSG_PRINT( const char * format, ... );
