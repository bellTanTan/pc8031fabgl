/*
  Created by tan (trinity09181718@gmail.com)
  Copyright (c) 2022 tan
  All rights reserved.

  Arduino IDE board : Arduino ESP32 v2.0.5
  library           : FabGL v1.0.9
  target            : ESP32 DEV Module
  flash size        : 4MB(32Mb)
  partition scheme  : Huge APP (3MB No OTA/1MB SPIFFS)
  PSRAM             : Disabled

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

// 問題点
//
// 1. seek音が微妙
//    n80FabGLと同様ｗ
//
// microSD
// /
// +--PC8001
//    +-- PC-8031-2W.ROM [2048bytes]
//    +-- PC-80S31.ROM   [2048bytes=PC-8801mkIISR/FR/...]
//    +-- DISK.ROM       [8192bytes=PC-8801MA/MC/MH/...]
//    |   注) 上記ファイル名で1つ用意。複数用意した場合ロード優先順位は上記順。               
//    |
//    +--MEDIA
//       +-- DISK
//           +--- *.d88  5.25 inch 2D Type

#include <memory>
#include <Preferences.h>

#include "fabgl.h"
#include "fabutils.h"

#include "src/machine.h"

using std::unique_ptr;

Preferences   preferences;
WebServer     webServer;
Machine *     m;
bool          writeProtected = false;


void setup()
{
  m = new Machine;
  m->initShiftInOut();
  m->initWifi();
  
  Serial.begin( 115200 );
  while ( !Serial && !Serial.available() );
  m->outputBootMessage();

  disableCore0WDT();
  // experienced crashes without this delay!
  delay( 100 );
  disableCore1WDT();

  preferences.begin( "pc8031FabGL", false );

  // uncomment to clear preferences
  //preferences.clear();

  char fdImgFileName[MAX_DRIVE][256];
  memset( fdImgFileName, 0, sizeof( fdImgFileName ) );
  for ( int drv = 0; drv < ARRAY_SIZE( fdImgFileName ); drv++ )
  {
    char key[16];
    sprintf( key, "fd%d", drv );
    preferences.getString( key, fdImgFileName[drv], sizeof( fdImgFileName[drv] ) );
    _MSG_PRINT( "%s: '%s'\r\n", key, fdImgFileName[drv] );
  }

  int result;
  if ( (result = m->init()) != 0 )
  {
    const char * errMsg = "?";
    uint16_t ledOutData = (uint16_t)( result * -1 ) << 12;
    switch ( result )
    {
      case -1:
        errMsg = "SD mount failed";
        break;
      case -2:
        errMsg = "ROM file load failed";
        break;
      case -3:
        errMsg = "Out of memory (DISK::uPD765A SoundGenerator)";
        break;
    }
    _MSG_PRINT( "%s(%d) init failed. %s\r\n", __func__, __LINE__, errMsg );
    systemFailedHalt( ledOutData );
  }

  if ( m->getDiskRomEnable() )
  {
    for ( int drv = 0; drv < ARRAY_SIZE( fdImgFileName ); drv++ )
    {
      result = m->setDiskImage( drv, writeProtected, fdImgFileName[drv] );
    }
  }
}

void loop()
{
  m->backGround();
}

void  systemFailedHalt( uint16_t ledOutData )
{
  int systemStatusLed = LOW;
  int timer = millis();
  uint16_t outData = 0x0000;
  m->outputSystemStatusLed( systemStatusLed );
  m->shiftOutput( outData );
  while ( 1 )
  {
    int dt = millis() - timer;
    if ( dt >= 500 )
    {
      timer = millis();
      if ( systemStatusLed == LOW )
        systemStatusLed = HIGH;
      else
        systemStatusLed = LOW;
      if ( outData == 0x0000 )
        outData = ledOutData;
      else
        outData = 0x0000;
      m->outputSystemStatusLed( systemStatusLed );
      m->shiftOutput( outData );
    }
  }
}

void webHandleRoot( void )
{
  m->eventWebHandleRoot();
}

void webHandleApply( void )
{
  if ( m->eventWebHandleApply() == true )
  {
    constexpr int MAXNAMELEN = 256;
    unique_ptr<char[]> filename( new char[MAXNAMELEN + 1] { 0 } );
    for ( int drv = 0; drv < MAX_DRIVE; drv++ )
    {
      const char * imageFileName = m->getDiskImageFileName( drv );
      if ( imageFileName )
      {
        char key[16];
        char * p = filename.get();
        strcpy( p, imageFileName );
        sprintf( key, "fd%d", drv );
        preferences.putString( key, p );
      }
    }
  }
}

void webHandleNotFound( void )
{
  m->eventWebHandleNotFound();
}
