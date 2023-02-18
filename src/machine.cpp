/*
  Created by tan (trinity09181718@gmail.com)
  Copyright (c) 2022 tan
  All rights reserved.

  This program is based on FabGL/examples/VGA/Altair8800/src/machine.cpp and
  FabGL/examples/VGA/PCEmulator/machine.cpp
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


#include "Arduino.h"

#include "machine.h"
#include "const.h"


Machine::Machine()
{
  m_diskRomEnable = false;
  memset( m_fdImgFileName, 0, sizeof( m_fdImgFileName ) );

  m_diskDirListCount = 0;
  m_diskDirList      = NULL;
  m_wifiSeqNo        = -1;

  sprintf( m_version, "pc8031FabGL v%d.%d.%d SDK %s",
           PC8031FABGL_VERSION_MAJOR,
           PC8031FABGL_VERSION_MINOR,
           PC8031FABGL_VERSION_REVISION,
           ESP.getSdkVersion() );

#ifdef _DEBUG
  m_serialRecvIndex        = 0;
  m_lastDumpDiskIoInAdrs   = 0;
  m_lastDumpDiskIoOutAdrs  = 0;
  m_lastDumpDiskMemoryAdrs = 0;
  memset( m_serialRecvBuf, 0, sizeof( m_serialRecvBuf ) );
#endif // _DEBUG
}


Machine::~Machine()
{
}


int Machine::init( void )
{
#ifndef _USED_SPIFFS
  if ( !FileBrowser::mountSDCard( false, MOUNT_PATH ) ) return -1;
#else
  if ( !FileBrowser::mountSPIFFS( false, MOUNT_PATH ) ) return -1;
#endif // _USED_SPIFFS

  // DISK ROM
  m_diskRomEnable = m_DISK.romLoad( PC80312W_ROM );
  if ( !m_diskRomEnable )
    m_diskRomEnable = m_DISK.romLoad( PC80S31_ROM );
    if ( !m_diskRomEnable )
      m_diskRomEnable = m_DISK.romLoad( DISK_ROM );
  if ( !m_diskRomEnable )
    return -2;

  // Device reset
  bool result = m_DISK.reset( true, &m_soundGenerator );
  if ( !result ) return -3;

  return 0;
}


void Machine::initShiftInOut( void )
{
  pinMode( IN_SL,     OUTPUT );
  pinMode( IN_CLK,    OUTPUT );
  pinMode( IN_QH,     INPUT );
  pinMode( OUT_SER,   OUTPUT );
  pinMode( OUT_RCLK,  OUTPUT );
  pinMode( OUT_SRCLK, OUTPUT );
  digitalWrite( IN_SL,     HIGH );
  digitalWrite( IN_CLK,    LOW );
  digitalWrite( OUT_RCLK,  LOW );
  digitalWrite( OUT_SRCLK, LOW );

  m_DISK.shiftOutput( 0x0000 );
}


void Machine::initWifi( void )
{
  pinMode( SYSTEM_STATUS, OUTPUT );
  outputSystemStatusLed( LOW );

  WiFi.disconnect();
  WiFi.softAPdisconnect( true );
  WiFi.mode( WIFI_STA );
  WiFi.config( ip, gateway, subnet, DNS );
  delay( 10 );
  WiFi.begin( ssid, password );

  m_wifiConnectCheckCnt = 0;
  m_wifiTimer           = 0;
  m_wifiSeqNo           = 0;
  m_ledFlickerCount     = 0;
  m_fdSettingComplete   = false;
}


void Machine::backGround( void )
{
  wifiMain();
#ifdef _DEBUG
  if ( Serial.available() )
  {
    int recvData = Serial.read();
    m_serialRecvBuf[m_serialRecvIndex] = recvData;
    m_serialRecvIndex++;
    if ( m_serialRecvIndex >= ARRAY_SIZE( m_serialRecvBuf ) )
      m_serialRecvIndex = 0;
    if ( recvData == 0x0A )
      debugCommand();
  }
#endif // _DEBUG
  delay( 5 );
}


void Machine::wifiMain( void )
{
  switch ( m_wifiSeqNo )
  {
    case 0:
      if ( Serial )
      {
        _MSG_PRINT( "Wi-Fi connecting %s ", ssid );
        m_wifiTimer = millis();
        m_wifiSeqNo = 10;
      }
      break;
    case 10:
      if ( WiFi.status() == WL_CONNECTED )
      {
        checkWifiRssi();
        _MSG_PRINT( "Wi-Fi connected.\r\n" );
        _MSG_PRINT( "ip address: %s\r\n", WiFi.localIP().toString().c_str() );
        outputSystemStatusLed( LOW );
        m_wifiSeqNo = 100;
        break;
      }
      {
        int dt = millis() - m_wifiTimer;
        if ( dt >= 500 )
        {
          m_wifiConnectCheckCnt++;
          if ( m_wifiConnectCheckCnt > 10 )
          {
            checkWifiRssi();
            _MSG_PRINT( "Wi-Fi connection failed\r\n" );
            m_wifiTimer = millis();
            m_wifiSeqNo = 90;
            break;
          }
          _MSG_PRINT( "." );
          m_wifiTimer = millis();
          int value = m_wifiStatusLed;
          if ( value == LOW )
            value = HIGH;
          else
            value = LOW;
          outputSystemStatusLed( value );
        }
      }
      break;
    case 90:
      {
        int dt = millis() - m_wifiTimer;
        if ( dt >= 100 )
        {
          m_wifiTimer = millis();
          int value = m_wifiStatusLed;
          if ( value == LOW )
            value = HIGH;
          else
            value = LOW;
          outputSystemStatusLed( value );
        }
      }
      break;
    case 100:
      if ( ntpServer != NULL && strlen( ntpServer) > 0 )
      {
        _MSG_PRINT( "contacting %s time server\r\n", ntpServer );
        configTime( gmtOffsetSec, daylightOffsetSec, ntpServer );
        struct tm tm;
        memset( &tm, 0, sizeof( tm ) );
        getLocalTime( &tm, 10 * 1000 );
        _MSG_PRINT( "date: %d/%02d/%02d %02d:%02d:%02d\r\n",
                    tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec );
      }
      m_wifiSeqNo = 110;
      break;
    case 110:
#ifdef _USED_SPIFFS
      if ( ntpServer != NULL && strlen( ntpServer) > 0 )
        updateSpiffsFileDateTime();
#endif // _USED_SPIFFS
      _MSG_PRINT( "'%s%s' directory listup\r\n", MOUNT_PATH, PC8001MEDIA_DISK );
      if ( listDir() )
       qsort( m_diskDirList, m_diskDirListCount, sizeof( *m_diskDirList ), qsortComp );
      m_wifiSeqNo = 120;
      break;
    case 120:
      if ( !MDNS.begin( mDNS_NAME ) )
      {
        _MSG_PRINT( "mDNS setting failed\r\n" );
        m_wifiTimer = millis();
        m_wifiSeqNo = 90;
        break;
      }
      _MSG_PRINT( "mDNS(%s) responder started\r\n", mDNS_NAME );
      m_wifiSeqNo = 130;
      break;
    case 130:
      webServer.on( "/", webHandleRoot );
      webServer.on( "/apply.cgi", webHandleApply );
      webServer.onNotFound( webHandleNotFound );
      // TCP(http) server start
      webServer.begin( httpTcpPort );
      if ( httpTcpPort == 80 )
        _MSG_PRINT( "TCP(http port:%d) [http://%s.local/ or http://%s/] server started\r\n",
                    httpTcpPort, mDNS_NAME, WiFi.localIP().toString().c_str() );
      else
        _MSG_PRINT( "TCP(http port:%d) [http://%s.local:%d/ or http://%s:%d/] server started\r\n",
                    httpTcpPort, mDNS_NAME, httpTcpPort, WiFi.localIP().toString().c_str(), httpTcpPort );
      outputFreeHeapMessage();
      outputSystemStatusLed( HIGH );
      m_wifiSeqNo = 200;
      break;
    case 200:
      webServer.handleClient();
      if ( WiFi.status() != WL_CONNECTED )
      {
        _MSG_PRINT( "Wi-Fi connection down.\r\n" );
        webServer.stop();
        MDNS.end();
        m_wifiTimer = millis();
        m_wifiSeqNo = 210;
        break;
      }
      if ( m_fdSettingComplete )
      {
        int dt = millis() - m_wifiTimer;
        if ( dt >= 50 )
        {
          m_wifiTimer = millis();
          int value = m_wifiStatusLed;
          if ( value == LOW )
            value = HIGH;
          else
            value = LOW;
          outputSystemStatusLed( value );
          m_ledFlickerCount--;
          if ( m_ledFlickerCount <= 0 )
            m_fdSettingComplete = false;
        }
      }
      break;
    case 210:
      {
        int dt = millis() - m_wifiTimer;
        if ( dt >= 500 )
        {
          freeDir();
          initWifi();
          m_wifiSeqNo = 0;
        }
      }
      break;
  }
}


void Machine::checkWifiRssi( void )
{
  int8_t rssiData = WiFi.RSSI();
  const char * rssiStatus;
  if ( rssiData >= -30 )
    rssiStatus = "very good.";
  else if ( rssiData >= -67 )
    rssiStatus = "good.";
  else if ( rssiData >= -70 )
    rssiStatus = "okay.";
  else if ( rssiData >= -80 )
    rssiStatus = "no good.";
  else
    rssiStatus = "unusable.";
  _MSG_PRINT( "\r\nrssi: %d dBm %s\r\n", rssiData, rssiStatus );
}


void Machine::outputSystemStatusLed( int value )
{
  m_wifiStatusLed = value;
  digitalWrite( SYSTEM_STATUS, m_wifiStatusLed );
}


void Machine::eventWebHandleRoot( void )
{
  String res;
  res = "<!DOCTYPE HTML>\n<html>\n";
  res += "<head><meta charset=\"UTF-8\"><title>";
  res += m_version;
  res += "</title></head>\n";
  res += "<body>\n";
  res += "<h3>";
  res += m_version;
  res += " : fd media setting</h3>\n";
  res += "<form method=\"POST\" action=\"apply.cgi\">\n";
  int maxDrive;
  if ( m_DISK.get2kRomLoaded() )
    maxDrive = ARRAY_SIZE( m_fdImgFileName );
  else
    maxDrive = ARRAY_SIZE( m_fdImgFileName ) / 2;
  for ( int fd = 0; fd < maxDrive; fd++ )
  {
    const char * selected;
    char item[256];
    sprintf( item, "  <p align=\"left\">fd%d [drive %d] [drive %c:]\n", fd, fd+1, 'A'+fd );
    res += item;
    sprintf( item, "    <select name=\"fd%d\" size=\"1\">\n", fd );
    res += item;
    int selPos = -1;
    const char * fdImgFileName = &m_fdImgFileName[fd][0];
    const char * p1 = strrchr( fdImgFileName, '/' );
    if ( p1 == NULL )
      p1 = fdImgFileName;
    else
      p1++;
    for ( int i = 0; i < m_diskDirListCount; i++ )
    {
      if ( strcmp( p1, m_diskDirList[i].name ) == 0 )
      {
        selPos = i;
        break;
      }
    }            
    _HTTP_DEBUG_PRINT( "%s(%d) fd%d '%s' %d\r\n", __func__, __LINE__, fd, p1, selPos );
    if ( selPos == -1 )
      selected = " selected";
    else
      selected = "";
    sprintf( item, "      <option%s value=\"-2\"> </option>\n", selected );
    res += item;
    res += "      <option value=\"-1\">eject</option>\n";
    for ( int i = 0; i < m_diskDirListCount; i++ )
    {
      if ( i == selPos )
        selected = " selected";
      else
        selected = "";
      sprintf( item, "      <option%s value=\"%d\">%s</option>\n", selected, i, m_diskDirList[i].name );
      res += item;
    }
    res += "    </select>\n";
    res += "  </p>\n";
  }
  res += "  <p><input type=\"submit\" value=\"apply\"></p>\n";
  res += "</form>\n";
  res += "</body>\n";
  res += "</html>\n\n";
  webServer.send( 200, "text/html", res );
}


bool Machine::eventWebHandleApply( void )
{
  bool result = false;
  if ( webServer.method() == HTTP_POST )
  {
    int args = webServer.args();
    for ( int i = 0; i < args; i++ )
    {
      String argName  = webServer.argName(i);
      String argValue = webServer.arg(i);
      const char * p = argName.c_str();
      if ( p && p[0] == 'f' && p[1] == 'd' )
      {
        int fdNo    = atoi( &p[2] );
        int selData = atoi( argValue.c_str() );
        _HTTP_DEBUG_PRINT( "%s(%d) fd%d %d\r\n", __func__, __LINE__, fdNo, selData );
        if ( fdNo >= 0 && fdNo < ARRAY_SIZE( m_fdImgFileName ) )
        {
          if ( selData == -1 )
          {
            setDiskImage( fdNo, false, NULL );
            result = true;
          }
          if ( selData >= 0 && selData < m_diskDirListCount )
          {
            setDiskImage( fdNo, false, m_diskDirList[selData].name );
            result = true;
          }
        }
      }
    }
  }
  if ( result )
    eventWebHandleRoot();
  else
    webServer.send( 500, "text/plain", "POST/arg failed\n" );
  return result;
}


void Machine::eventWebHandleNotFound( void )
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += ( webServer.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for ( int i = 0; i < webServer.args(); i++ )
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  webServer.send( 404, "text/plain", message );
}


bool Machine::listDir( void )
{
  FileBrowser fb;
  char path[256];
  sprintf( path, "%s%s", MOUNT_PATH, PC8001MEDIA_DISK );
  if ( fb.setDirectory( path ) == false )
    return false;
  int count = fb.count();
  if ( count <= 0 )
    return false;
  m_diskDirList = (PDIRLIST)malloc( sizeof( DIRLIST ) * count );
  if ( m_diskDirList == NULL )
    return false;
  m_diskDirListCount = 0;
  for ( int i = 0; i < count; i++ )
  {
    DirItem const * dirItem = fb.get( i );
    if ( !( dirItem && !dirItem->isDir ) )
      continue;
    if ( strcasestr( dirItem->name, D88_FILE_EXTENSION ) == NULL )
      continue;
    m_diskDirList[m_diskDirListCount].name       = strdup( dirItem->name );
    m_diskDirList[m_diskDirListCount].tLastWrite = 0;
    m_diskDirListCount++;
  }
  return true;
}


void Machine::freeDir( void )
{
  if ( m_diskDirList )
  {
    for ( int i = 0; i < m_diskDirListCount; i++ )
    {
      if ( m_diskDirList[i].name )
      {
        free( m_diskDirList[i].name );
        m_diskDirList[i].name = NULL;
      }
    }
    free( m_diskDirList );
    m_diskDirList      = NULL;
    m_diskDirListCount = 0;
  }
}


int Machine::qsortComp( const void * p0, const void * p1 )
{
  PDIRLIST d0 = (PDIRLIST)p0;
  PDIRLIST d1 = (PDIRLIST)p1;
  return strcmp( d0->name, d1->name );
}


#ifdef _USED_SPIFFS
bool Machine::updateSpiffsFileDateTime( void )
{
  uint8_t * binBuf = NULL;

  // spiffs イメージ生成の日時補完用のファイル日時情報メモリロード
  bool fFileDateTimeListLoadFailed = true;
  char szPath[512];
  sprintf( szPath, "%s/%s", MOUNT_PATH, FILE_DATETIME_LIST );
  auto fp = fopen( szPath, "r" );
  if ( fp )
  {
    fseek( fp, 0, SEEK_END );
    size_t fileSize = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    if ( fileSize > 0 )
    {
      binBuf = (uint8_t *)malloc( fileSize );
      if ( binBuf != NULL )
      {
        size_t result = fread( binBuf, 1, fileSize, fp );
        if ( result == fileSize )
        {
          _MSG_PRINT( "'%s' spiffs file datetime info load\r\n", szPath );
          fFileDateTimeListLoadFailed = false;
        }
      }
    }
    fclose( fp );
  }
  else
    fFileDateTimeListLoadFailed = false;
  if ( fFileDateTimeListLoadFailed )
    return false;

  if ( binBuf != NULL )
  {
    _MSG_PRINT( "delete '%s'\r\n", szPath );
    unlink( szPath );
  }
  else
    return true;

  FileBrowser fb;
  sprintf( szPath, "%s/", MOUNT_PATH );
  if ( fb.setDirectory( szPath ) == false )
  {
    if ( binBuf ) free( binBuf );
    return false;
  }
  int count = fb.count();
  if ( count <= 0 )
  {
    if ( binBuf ) free( binBuf );
    return false;
  }
  _MSG_PRINT( "spiffs file datetime update start\r\n" );
  for ( int i = 0; i < count; i++ )
  {
    DirItem const * dirItem = fb.get( i );
    if ( !( dirItem && !dirItem->isDir ) )
      continue;
    if ( !strcmp( dirItem->name, FILE_DATETIME_LIST ) )
      continue;
    time_t tLastWrite = getFileDateTime( binBuf, dirItem->name );
    char szFullPath[512];
    sprintf( szFullPath, "%s/%s", MOUNT_PATH, dirItem->name );
    struct utimbuf ut;
    memset( &ut, 0, sizeof( ut ) );
    ut.actime  = tLastWrite;
    ut.modtime = tLastWrite;
    int result = utime( szFullPath, &ut );
    if ( result == 0 )
    {
      struct tm * ptm = localtime( &tLastWrite );
      if ( ptm )
      {
        _MSG_PRINT( "%d/%02d/%02d %02d:%02d:%02d '%s'\r\n",
                    ptm->tm_year+1900,
                    ptm->tm_mon+1,
                    ptm->tm_mday,
                    ptm->tm_hour,
                    ptm->tm_min,
                    ptm->tm_sec,
                    dirItem->name );
      }
    }
  }
  if ( binBuf ) free( binBuf );
  _MSG_PRINT( "spiffs file datetime update end\r\n" );
  return true;
}


time_t Machine::getFileDateTime( uint8_t * binBuf, const char * pszPath )
{
  // UNIX & *BSD & linux & Macintosh(Mac OS X以降)
  // ls -lan --time-style="+%Y-%m-%d %H:%M:%S" > fileDateTimeList.txt
  // -rw-r--r-- 1 1000 1000  4154 1991-01-27 08:25:20 3BY4 Part2 {mon L}.cmt
  //
  // Windowsコマンドプロンプトのdirはファイル日時の秒値を表示させる機能がオプション指定込みで無い。
  // そのためforfiles(Windows Vista以降)を活用
  // forfiles /c "cmd /c echo @fsize @fdate @ftime @file" > fileDateTimeList.txt
  // 4154 1991/01/27 08:25:20 3BY4 Part2 {mon L}.cmt
  time_t result = 0;
  char * p = strstr( (const char *)binBuf, pszPath );
  if ( p == NULL )
    return result;
  char * pszDateTop = p - 20;
  char szDateTime[32];
  memset( szDateTime, 0, sizeof( szDateTime ) );
  strncpy( szDateTime, pszDateTop, 19 );
  struct tm tm;
  memset( &tm, 0, sizeof( tm ) );
  //           1
  // 0123456789012345678
  // 1991-01-27 08:25:20
  // 1991/01/27 08:25:20
  szDateTime[4]  = '\0';
  szDateTime[7]  = '\0';
  szDateTime[10] = '\0';
  szDateTime[13] = '\0';
  szDateTime[16] = '\0';
  tm.tm_year = atoi( &szDateTime[0] ) - 1900;
  tm.tm_mon  = atoi( &szDateTime[5] ) - 1;
  tm.tm_mday = atoi( &szDateTime[8] );
  tm.tm_hour = atoi( &szDateTime[11] );
  tm.tm_min  = atoi( &szDateTime[14] );
  tm.tm_sec  = atoi( &szDateTime[17] );
  result = mktime( &tm );

  return result;  
}
#endif // _USED_SPIFFS


bool Machine::setDiskImage( int drive, bool writeProtected, const char * imgFileName )
{
  char path[256];
  if ( drive >= 0 && drive < ARRAY_SIZE( m_fdImgFileName ) )
    memset( m_fdImgFileName[drive], 0, sizeof( m_fdImgFileName[drive] ) );
  m_DISK.eject( drive );
  if ( !( imgFileName && imgFileName[0] != 0 ) ) return true;
  strcpy( path, MOUNT_PATH );
  strcat( path, PC8001MEDIA_DISK );
  int len = strlen( path );
  if ( len >= 1 && path[len-1] != '/' )
    strcat( path, "/" );
  strcat( path, imgFileName );
  bool result = m_DISK.insert( drive, writeProtected, path );
  if ( !result ) return false;
  if ( drive >= 0 && drive < ARRAY_SIZE( m_fdImgFileName ) )
    strncpy( m_fdImgFileName[drive], imgFileName, sizeof( m_fdImgFileName[drive] ) );
  return true;
}


const char * Machine::getDiskImageFileName( int drive )
{
  const char * fileName = NULL;
  if ( drive >= 0 && drive < ARRAY_SIZE( m_fdImgFileName ) )
    fileName = m_fdImgFileName[drive];
  return fileName;
}


#ifdef _DEBUG
void Machine::debugHelp( void )
{
  _DEBUG_PRINT( "\r\nhelp for debug.\r\n" );
  _DEBUG_PRINT( "DISK CPU\r\n" );
  _DEBUG_PRINT( "B[adrs] : breakpoint setting\r\n" );
  _DEBUG_PRINT( "T       : step excute\r\n" );
  _DEBUG_PRINT( "C       : continue excute\r\n" );
  _DEBUG_PRINT( "R       : dump for CPU register\r\n" );
  _DEBUG_PRINT( "D[adrs] : memory dump for 256 bytes\r\n" );
  _DEBUG_PRINT( "I[adrs] : I/O dump for 1byte input port\r\n" );
  _DEBUG_PRINT( "O[adrs] : I/O dump for 1byte output port\r\n" );
}


void Machine::debugCommand( void )
{
  char buf[16];
  char * p = NULL;
  int adrs;
  bool any = false;
  p = strchr( m_serialRecvBuf, 0x0D );
  if ( p ) p[0] = 0x00;
  p = strchr( m_serialRecvBuf, 0x0A );
  if ( p ) p[0] = 0x00;
  _DEBUG_PRINT( "\r\n%s\r\n", m_serialRecvBuf );

  switch ( m_serialRecvBuf[0] )
  {
    case 'h':
      debugHelp();
      break;
    case 'B':
      memset( buf, 0, sizeof( buf ) );
      buf[0] = m_serialRecvBuf[1];
      buf[1] = m_serialRecvBuf[2];
      buf[2] = m_serialRecvBuf[3];
      buf[3] = m_serialRecvBuf[4];
      if ( strlen( buf ) > 0 )
      {
        any = false;
        sscanf( buf, "%x", &adrs );
      }
      else
        any = true;
      m_DISK.debugBreak( any, adrs );
      break;
    case 'T':
      m_DISK.debugStep();
      break;
    case 'C':
      m_DISK.debugContinue();
      break;
    case 'R':
      m_DISK.registerDump( &m_DISK );
      break;
    case 'D':
      memset( buf, 0, sizeof( buf ) );
      buf[0] = m_serialRecvBuf[1];
      buf[1] = m_serialRecvBuf[2];
      buf[2] = m_serialRecvBuf[3];
      buf[3] = m_serialRecvBuf[4];
      if ( strlen( buf ) > 0 )
        sscanf( buf, "%x", &adrs );
      else
        adrs = m_lastDumpDiskMemoryAdrs;
      m_DISK.memoryDump( adrs, 256 );
      m_lastDumpDiskMemoryAdrs = adrs + 256;
      if ( m_lastDumpDiskMemoryAdrs >= m_DISK.getRAMSize() )
        m_lastDumpDiskMemoryAdrs = 0;
      break;
    case 'I':
      memset( buf, 0, sizeof( buf ) );
      buf[0] = m_serialRecvBuf[1];
      buf[1] = m_serialRecvBuf[2];
      if ( strlen( buf ) > 0 )
        sscanf( buf, "%x", &adrs );
      else
        adrs = m_lastDumpDiskIoInAdrs;
      m_DISK.ioDump( 0, adrs );
      m_lastDumpDiskIoInAdrs = adrs + 1;
      if ( m_lastDumpDiskIoInAdrs >= m_DISK.getIOSize() )
        m_lastDumpDiskIoInAdrs = 0;
      break;
    case 'O':
      memset( buf, 0, sizeof( buf ) );
      buf[0] = m_serialRecvBuf[1];
      buf[1] = m_serialRecvBuf[2];
      if ( strlen( buf ) > 0 )
        sscanf( buf, "%x", &adrs );
      else
        adrs = m_lastDumpDiskIoOutAdrs;
      m_DISK.ioDump( 1, adrs );
      m_lastDumpDiskIoOutAdrs = adrs + 1;
      if ( m_lastDumpDiskIoOutAdrs >= m_DISK.getIOSize() )
        m_lastDumpDiskIoOutAdrs = 0;
      break;
  }

  m_serialRecvIndex = 0;
  memset( m_serialRecvBuf, 0, sizeof( m_serialRecvBuf ) );
}


void _DEBUG_PRINT( const char * format, ... )
{
  va_list ap;
  va_start( ap, format );
  int size = vsnprintf( NULL, 0, format, ap ) + 1;
  if ( size > 0 )
  {
    va_end( ap );
    va_start( ap, format );
    char buf[size + 1];
    vsnprintf( buf, size, format, ap );
    Serial.printf( "%s", buf );
  }
  va_end( ap );
}
#endif // _DEBUG


#ifdef _UPD765A_DEBUG
void _UPD765A_DEBUG_PRINT( const char * format, ... )
{
  va_list ap;
  va_start( ap, format );
  int size = vsnprintf( NULL, 0, format, ap ) + 1;
  if ( size > 0 )
  {
    va_end( ap );
    va_start( ap, format );
    char buf[size + 1];
    vsnprintf( buf, size, format, ap );
    Serial.printf( "%s", buf );
  }
  va_end( ap );
}
#endif // _UPD765A_DEBUG


#ifdef _HTTP_DEBUG
void _HTTP_DEBUG_PRINT( const char * format, ... )
{
  va_list ap;
  va_start( ap, format );
  int size = vsnprintf( NULL, 0, format, ap ) + 1;
  if ( size > 0 )
  {
    va_end( ap );
    va_start( ap, format );
    char buf[size + 1];
    vsnprintf( buf, size, format, ap );
    Serial.printf( "%s", buf );
  }
  va_end( ap );
}
#endif // _HTTP_DEBUG


void _MSG_PRINT( const char * format, ... )
{
  va_list ap;
  va_start( ap, format );
  int size = vsnprintf( NULL, 0, format, ap ) + 1;
  if ( size > 0 )
  {
    va_end( ap );
    va_start( ap, format );
    char buf[size + 1];
    vsnprintf( buf, size, format, ap );
    Serial.printf( "%s", buf );
  }
  va_end( ap );
}
