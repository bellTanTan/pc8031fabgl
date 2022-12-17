/*
  Created by tan (trinity09181718@gmail.com)
  Copyright (c) 2022 tan
  All rights reserved.

  This program is based on FabGL/examples/VGA/Altair8800/src/machine.h and
  FabGL/examples/VGA/PCEmulator/machine.h
  (http://www.fabglib.org/)

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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_int_wdt.h"

#include "fabgl.h"
#include "fabutils.h"
#include "emudevs/Z80.h"

using namespace fabgl;

#include "emu.h"
#include "version.h"
#include "DISK.h"

extern  WebServer     webServer;
extern  void webHandleRoot( void );
extern  void webHandleApply( void );
extern  void webHandleNotFound( void );


////////////////////////////////////////////////////////////////////////////////////
// Machine


class Machine
{
public:
  typedef struct {
    char *  name;
  } DIRLIST , *PDIRLIST;

  Machine();

  ~Machine();

  int init( void );
  void initShiftInOut( void );
  void initWifi( void );
  void backGround( void );

  bool getDiskRomEnable( void )               { return m_diskRomEnable; }
  bool get2kRomLoaded( void )                 { return m_DISK.get2kRomLoaded(); }
  void shiftOutput( uint16_t value )          { m_DISK.shiftOutput( value ); }

  bool setDiskImage( int drive, bool writeProtected, const char * imgFileName );
  const char * getDiskImageFileName( int drive );  

  void outputSystemStatusLed( int value );
  void eventWebHandleRoot( void );
  bool eventWebHandleApply( void );
  void eventWebHandleNotFound( void );

  void outputBootMessage( void )
  {
    _MSG_PRINT( "\r\nboot\r\n%s\r\n", m_version );
#ifdef ESP_ARDUINO_VERSION
    _MSG_PRINT( "ESP_ARDUINO_VERSION(0x%04X)\r\n", ESP_ARDUINO_VERSION );
#endif // ESP_ARDUINO_VERSION
    _MSG_PRINT( "FABGL_ESP_IDF_VERSION(0x%04X)\r\n", FABGL_ESP_IDF_VERSION );
  }

  void outputFreeHeapMessage( void )
  {
    _MSG_PRINT( "Internal Total Heap %d bytes\r\n", ESP.getHeapSize() );
    _MSG_PRINT( "Internal Free  Heap %d bytes\r\n", ESP.getFreeHeap() );
    _MSG_PRINT( "SPIRam Total Heap %d bytes\r\n", ESP.getPsramSize() );
    _MSG_PRINT( "SPIRam Free  Heap %d bytes\r\n", ESP.getFreePsram() );
  }

private:
  void wifiMain( void );
  void checkWifiRssi( void );
  bool listDir( void );
  void freeDir( void );
  static int qsortComp( const void * p0, const void * p1 );

#ifdef _DEBUG
  void debugHelp( void );
  void debugCommand( void );
#endif // _DEBUG

  SoundGenerator  m_soundGenerator;         // fabgl library : class SoundGenerator
                                            // floppy disk 0 ~ 3 image file name (*.D88)
  char            m_fdImgFileName[MAX_DRIVE][256];
  bool            m_diskRomEnable;          // flag : floppy disk rom enable
  DISK            m_DISK;                   // PC-80S31 (=PC-8801mkII SR/FR DISK.ROM 2Kbytes)

#ifdef _DEBUG
  char            m_serialRecvBuf[64];      // debug command Serial recive buffer
  int             m_serialRecvIndex;        // debug command Serial recive buffer index
  int             m_lastDumpDiskIoInAdrs;   // debug command Floppy DISK uint I/O input port last adderss
  int             m_lastDumpDiskIoOutAdrs;  // debug command Floppy DISK uint I/O output port last adderss
  int             m_lastDumpDiskMemoryAdrs; // debug command Floppy DISK uint Memory dump last adderss
  QueueHandle_t   m_queueHandle;            // queue handle
#endif // _DEBUG

  int             m_diskDirListCount;       // d88 file list count
  PDIRLIST        m_diskDirList;            // d88 file list

  int             m_wifiSeqNo;              // wifi sequence no
  int             m_wifiStatusLed;          // wifi led status
  int             m_wifiConnectCheckCnt;    // wifi connect check counter
  int             m_wifiTimer;              // wifi connect check timer

  char            m_version[64];            // version
};
