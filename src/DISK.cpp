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


#include "DISK.h"


DISK::DISK()
{
  m_RAM         = NULL;
  m_RAMSize     = 0;
  m_IoIn        = NULL;
  m_IoOut       = NULL;
  m_IOSize      = 0;
  m_diskEnable  = false;
  m_2kRomLoaded = false;

  m_Z80.setCallbacks( this, readByte, writeByte, readWord, writeWord, readIO, writeIO );
}


DISK::~DISK()
{
}


bool DISK::romLoad( const char * fileName )
{
  size_t IOSize = 256;
  if ( !m_IoIn )
  {
    m_IoIn = (uint8_t *)malloc( IOSize );
    if ( !m_IoIn ) return false;
    memset( m_IoIn, 0x00, IOSize );
  }
  if ( !m_IoOut )
  {
    m_IoOut = (uint8_t *)malloc( IOSize );
    if ( !m_IoOut ) return false;
    memset( m_IoOut, 0x00, IOSize );
    m_IOSize = IOSize;
  }

  size_t size = 32768;
  if ( !m_RAM )
  {
    m_RAM = (uint8_t *)malloc( size );
    if ( !m_RAM ) return false;
    memset( m_RAM, 0, size );
    m_RAMSize = size;
    // used : 16 + 256 bytes
    m_d88ReadWriteBuffer = &m_RAM[0x2000];
  }

  if ( !m_RAM ) return false;
  bool romload = false;
  char path[256];
  strcpy( path, MOUNT_PATH );
  strcat( path, fileName );
  auto fp = fopen( path, "r" );
  if ( !fp ) return false;
  fseek( fp, 0, SEEK_END );
  size_t fileSize = ftell( fp );
  fseek( fp, 0, SEEK_SET );
  if ( fileSize == 2048 || fileSize == 8192 )
  {
    size_t result = fread( &m_RAM[0], 1, fileSize, fp );
    if ( result == 2048 )
      if ( m_RAM[0x07EF] == 0xEF )
      {
        m_2kRomLoaded = true;
        romload = true;
      }
    if ( result == 8192 )
      if ( m_RAM[0x07EE] == 0xFE && m_RAM[0x07EF] == 0xEF )
        romload = true;
  }
  fclose( fp );
  return romload;
}


bool DISK::insert( int drive, bool writeProtected, const char * imgFileName )
{
  if ( !m_diskEnable ) return false;
  if ( drive < 0 || drive >= ARRAY_SIZE( m_media ) ) return false;
  if ( !m_2kRomLoaded && drive >= 2 ) return false;
  bool result = m_media[drive].insert( writeProtected, imgFileName );
  return result;
}


void DISK::eject( int drive )
{
  if ( !m_diskEnable ) return;
  if ( drive < 0 || drive >= ARRAY_SIZE( m_media ) ) return;
  m_media[drive].eject();
}


bool DISK::reset( bool diskEnable, SoundGenerator * soundGenerator )
{
  m_diskEnable = diskEnable;
  if ( !m_diskEnable ) return true;

  m_disk2Pc8255PortC = 0;
  m_requestIRQ       = false;
  m_beforeWriteData  = 0xFFFF;

  m_uPD765A.setCallbacks( this, shiftOutData );
  bool result = m_uPD765A.reset( (bool *)&m_requestIRQ,
                                 &m_media[0],
                                 m_d88ReadWriteBuffer,
                                 soundGenerator );
  if ( !result ) return false;

#ifdef _DEBUG
  // Queue
  m_queueHandle = xQueueCreate( 10, sizeof( msgRequestCpuCmd ) );
  //debugBreak( false, 0x010E );
  //debugBreak( false, 0x048F );
#endif // _DEBUG

  m_beforeTickTime = micros();

  UBaseType_t uxPriority   = 1;
  const BaseType_t xCoreID = 0;
  xTaskCreatePinnedToCore( &cpuTask, "diskCpuTask", 4096, this, uxPriority, &m_taskHandle, xCoreID );
  return true;
}


void IRAM_ATTR DISK::cpuTask( void * pvParameters )
{
  auto m = (DISK *)pvParameters;
#ifdef _DEBUG
  msgRequestCpuCmd msg;
  bool enableSelBreadAdrs = false;
  int seqNo = 0;
  uint16_t selBreakAdrs = 0;
#endif // _DEBUG
  m->m_Z80.reset();
  m->m_Z80.setPC( 0 );

  while ( true )
  {
    // time in microseconds
    uint32_t t0 = micros();
    int cycles = 0;

#ifdef _DEBUG
    msg.cmd = cmdNon;
    xQueueReceive( m->m_queueHandle, &msg, 0 );
    switch ( (int)msg.cmd )
    {
      case cmdReset:
        enableSelBreadAdrs = false;
        m->m_Z80.reset();
        m->m_Z80.setPC( 0 );
        m->registerDump( pvParameters, "reset" );
        seqNo = 0;
        break;
      case cmdAnyBreak:
        m->registerDump( pvParameters, "break" );
        seqNo = 10;
        break;
      case cmdSelBreak:
        enableSelBreadAdrs = true;
        selBreakAdrs = msg.BP;
        break;
      case cmdStep:
        seqNo = 20;
        break;
      case cmdContinue:
        seqNo = 30;
        break;
    }
    if ( m->m_Z80.getStatus() == Z80_STATUS_HALT )
    {
      if ( m->m_requestIRQ )
      {
        if ( m->m_Z80.getIM() == Z80_INTERRUPT_MODE_0 && m->m_Z80.getIFF1() )
        {
          // NOP : 00H
          cycles = m->m_Z80.IRQ( 0x00 );
          if ( cycles > 0 )
            m->m_requestIRQ = false;
        }
      }
    }
    else
    {
      switch ( seqNo )
      {
        case 0:
          if ( enableSelBreadAdrs )
          {
            if ( m->m_Z80.getPC() == selBreakAdrs )
            {
              m->registerDump( pvParameters, "break" );
              seqNo = 10;
              break;
            }
          }
          cycles = m->m_Z80.step();
          break;
        case 10:
          // non
          break;
        case 20:
          cycles = m->m_Z80.step();
          m->registerDump( pvParameters, "step" );
          seqNo = 10;
          break;
        case 30:
          cycles = m->m_Z80.step();
          m->registerDump( pvParameters, "continue" );
          seqNo = 0;
          break;
      }
    }
#else
    if ( m->m_Z80.getStatus() == Z80_STATUS_HALT )
    {
      if ( m->m_requestIRQ )
      {
        if ( m->m_Z80.getIM() == Z80_INTERRUPT_MODE_0 && m->m_Z80.getIFF1() )
        {
          // NOP : 00H
          cycles = m->m_Z80.IRQ( 0x00 );
          if ( cycles > 0 )
            m->m_requestIRQ = false;
        }
      }
    }
    else
      cycles = m->m_Z80.step();
#endif // _DEBUG

    m->m_uPD765A.tick();

    // time in microseconds
    uint32_t t1 = micros();
    int dt = t1 - t0;
    if ( dt < 0 )
      dt = t1 + ( 0xFFFFFFFF - t0 );

    // at 2MHz each cycle last 0.5us, so instruction time is cycles*0.5, that is cycles/2
    // NEC PC-8031-2W/PC-80S31 4MHz
    int t = ( cycles / 4 ) - dt;
    if ( t > 0 )
      delayMicroseconds( t );

    // tick counter up
    m->m_ticksCounter++;
  }
}


int DISK::readByte( void * context, int address )
{
  auto m = (DISK *)context;
  int romLimitAdrs;
  int value = 0xFF;
  if ( m->m_2kRomLoaded )
    romLimitAdrs = 0x07FF;
  else
    romLimitAdrs = 0x1FFF;
  if ( ( address >= 0x0000 && address <= romLimitAdrs )
    || ( address >= 0x4000 && address <= 0x7FFF ) )
    value = m->m_RAM[address];
  return value;
}


void DISK::writeByte( void * context, int address, int value )
{
  auto m = (DISK *)context;
  if ( address >= 0x4000 && address <= 0x7FFF )
    m->m_RAM[address] = value;
}


int DISK::readIO( void * context, int address )
{
  auto m = (DISK *)context;
#ifdef _DEBUG
  int pc = m->m_Z80.getPC() - 1;
#endif // _DEBUG
  int value = 0xFF;

  switch ( address )
  {
    case 0xF8:
      // FDC TC signal
      m->m_uPD765A.write_signal( SIG_UPD765A_TC );
      break;
    case 0xFA:
    case 0xFB:
      // FDC
      value = m->m_uPD765A.readIO( address );
      m->m_IoIn[address] = value;
      break;
    case 0xFC:
      // 8255 Port A
      {
#ifdef _SHIFT_IO_TIME
        uint32_t t0 = micros();
#endif // _SHIFT_IO_TIME
        uint16_t readData = m->shiftInput();
#ifdef _SHIFT_IO_TIME
        uint32_t t1 = micros();
        int dt = t1 - t0;
        _MSG_PRINT( "DISK %s(%d): 0x%02X      [dt %d usec]\r\n", __func__, __LINE__, address, dt );
#endif // _SHIFT_IO_TIME
        value  = 0x00;
        value |= ( ( readData & 0x0001 ) != 0 ) ? 0x01 : 0x00;  // PC 8255 Port B bit0
        value |= ( ( readData & 0x0002 ) != 0 ) ? 0x02 : 0x00;  // PC 8255 Port B bit1
        value |= ( ( readData & 0x0004 ) != 0 ) ? 0x04 : 0x00;  // PC 8255 Port B bit2
        value |= ( ( readData & 0x0008 ) != 0 ) ? 0x08 : 0x00;  // PC 8255 Port B bit3
        value |= ( ( readData & 0x0010 ) != 0 ) ? 0x10 : 0x00;  // PC 8255 Port B bit4
        value |= ( ( readData & 0x0020 ) != 0 ) ? 0x20 : 0x00;  // PC 8255 Port B bit5
        value |= ( ( readData & 0x0040 ) != 0 ) ? 0x40 : 0x00;  // PC 8255 Port B bit6
        value |= ( ( readData & 0x0080 ) != 0 ) ? 0x80 : 0x00;  // PC 8255 Port B bit7
        m->m_IoIn[address] = value;
      }
      break;
    case 0xFE:
      // 8255 Port C
      {
#ifdef _SHIFT_IO_TIME
        uint32_t t0 = micros();
#endif // _SHIFT_IO_TIME
        uint16_t readData = m->shiftInput();
#ifdef _SHIFT_IO_TIME
        uint32_t t1 = micros();
        int dt = t1 - t0;
        _MSG_PRINT( "DISK %s(%d): 0x%02X      [dt %d usec]\r\n", __func__, __LINE__, address, dt );
#endif // _SHIFT_IO_TIME
        value  = 0x00;
        value |= ( ( readData & 0x0100 ) != 0 ) ? 0x01 : 0x00;  // PC 8255 Port C bit4
        value |= ( ( readData & 0x0200 ) != 0 ) ? 0x02 : 0x00;  // PC 8255 Port C bit5
        value |= ( ( readData & 0x0400 ) != 0 ) ? 0x04 : 0x00;  // PC 8255 Port C bit6
        value |= ( ( readData & 0x0800 ) != 0 ) ? 0x08 : 0x00;  // PC 8255 Port C bit7
        m->m_IoIn[address] = value;
      }
      break;
    case 0xFF:
      // 8255 Control Port
      m->m_IoIn[address] = m->m_IoOut[address];
      value = m->m_IoIn[address];
      break;
    default:
      // not handlded!
      _DEBUG_PRINT( "DISK %s(%d):0x%02X [PC 0x%04X] not handlded!\r\n", __func__, __LINE__, address, pc );
      break;
  }

  return value;
}


void DISK::writeIO( void * context, int address, int value )
{
  auto m = (DISK *)context;
#ifdef _DEBUG
  int pc = m->m_Z80.getPC() - 1;
#endif // _DEBUG

  switch ( address )
  {
    case 0xF4:
      // ignore : drive mode
      m->m_IoOut[address] = value;
      _DEBUG_PRINT( "DISK %s(%d):0x%02X 0x%02X [PC 0x%04X] drive mode\r\n", __func__, __LINE__, address, value, pc );
      break;
    case 0xF7:
      // ignore : write pre compensation
      m->m_IoOut[address] = value;
      _DEBUG_PRINT( "DISK %s(%d):0x%02X 0x%02X [PC 0x%04X] write pre compensation\r\n", __func__, __LINE__, address, value, pc );
      break;
    case 0xF8:
      // ignore : drive motor
      m->m_IoOut[address] = value;
      _DEBUG_PRINT( "DISK %s(%d):0x%02X 0x%02X [PC 0x%04X] drive motor\r\n", __func__, __LINE__, address, value, pc );
      break;
    case 0xFB:
      // FDC
      m->m_IoOut[address] = value;
      m->m_uPD765A.writeIO( address, value );
      break;
    case 0xFD:
      // 8255 Port B
      m->m_IoOut[address] = value;
      shiftOutData( context, address );
      break;
    case 0xFF:
      // 8255 Control Port
      _DEBUG_PRINT( "DISK %s(%d):0x%02X 0x%02X [PC 0x%04X]\r\n", __func__, __LINE__, address, value, pc );
      m->m_IoOut[address] = value;
      if ( !( value & 0x80) )
      {
        // BIT SET/RESET
        int data = m->m_disk2Pc8255PortC;
        int bitcount = ( value >> 1 ) & 0x07;
        int bitdata  = 1 << bitcount;
        if ( value & 0x01 )
          data |= bitdata;
        else
          data &= ~bitdata;
        m->m_disk2Pc8255PortC = data;
        data &= 0xF0;
        data >>= 4;
        _DEBUG_PRINT( "DISK %s(%d):0x%02X 0x%02X [PC 0x%04X]\r\n", __func__, __LINE__, 0xFE, data, pc );
        m->m_IoOut[0xFE] = data;
        shiftOutData( context, 0xFE );
      }
      else
        m->m_disk2Pc8255PortC = 0;
      break;
    default:
      // not handlded!
      _DEBUG_PRINT( "DISK %s(%d):0x%02X 0x%02X [PC 0x%04X] not handlded!\r\n", __func__, __LINE__, address, value, pc );
      break;
  }
}


void DISK::shiftOutData( void * context, int address )
{
  auto m = (DISK *)context;
  uint16_t writeData = m->m_IoOut[0xFD];
  int data = m->m_disk2Pc8255PortC;
  data &= 0xF0;
  data <<= 4;
  writeData |= data;
  int drvStatus = m->m_uPD765A.getMaintatus();
  drvStatus &= 0x0F;
  drvStatus <<= 12;
  writeData |= drvStatus;
  if ( m->m_beforeWriteData != writeData )
  {
    m->m_beforeWriteData = writeData;
#ifdef _SHIFT_IO_TIME
    uint32_t t0 = micros();
#endif // _SHIFT_IO_TIME
    m->shiftOutput( writeData );
#ifdef _SHIFT_IO_TIME
    uint32_t t1 = micros();
    int dt = t1 - t0;
    _MSG_PRINT( "DISK %s(%d):0x%02X      [dt %d usec]\r\n", __func__, __LINE__, address, dt );
#endif // _SHIFT_IO_TIME
  }
}


uint16_t DISK::shiftInput( void )
{
  digitalWrite( IN_SL, LOW );
  delayMicroseconds( 1 );
  digitalWrite( IN_SL, HIGH );
  uint16_t result = digitalRead( IN_QH );
  for ( int i = 1; i < 16; i++ )
  {
    digitalWrite( IN_CLK, HIGH );
    result = result << 1 | digitalRead( IN_QH );
    digitalWrite( IN_CLK, LOW );
  }
  return result;
}


void  DISK::shiftOutput( uint16_t value )
{
  for ( int i = 0; i < 16; i++ )
  {
    int data = ( value & 0x0001 ) ? HIGH : LOW;
    digitalWrite( OUT_SER, data );
    digitalWrite( OUT_SRCLK, HIGH );
    digitalWrite( OUT_SRCLK, LOW );
    value >>= 1;
  }
  digitalWrite( OUT_RCLK, HIGH );
  delayMicroseconds( 1 );
  digitalWrite( OUT_RCLK, LOW );
}


#ifdef _DEBUG
void DISK::setRequestCpuCmd( requestCpuCmd cmd, uint16_t BP )
{
  msgRequestCpuCmd msg;
  msg.cmd = cmd;
  msg.BP  = BP;
  xQueueSend( m_queueHandle, &msg, 0 );
}


void DISK::debugBreak( bool any, int address )
{
  if ( any )
    setRequestCpuCmd( cmdAnyBreak );
  else
    setRequestCpuCmd( cmdSelBreak, address );
}


void DISK::debugContinue( void )
{
  setRequestCpuCmd( cmdContinue );
}


void DISK::debugStep( void )
{
  setRequestCpuCmd( cmdStep );
}
#endif // _DEBUG


void DISK::registerDump( void * context, const char * msg )
{
  auto m = (DISK *)context;
  uint16_t reg[7];
  uint16_t PC = m->m_Z80.getPC();
  reg[0] = m->m_Z80.readRegWord( Z80_BC );
  reg[1] = m->m_Z80.readRegWord( Z80_DE );
  reg[2] = m->m_Z80.readRegWord( Z80_HL );
  reg[3] = m->m_Z80.readRegWord( Z80_AF );
  reg[4] = m->m_Z80.readRegWord( Z80_IX );
  reg[5] = m->m_Z80.readRegWord( Z80_IY );
  reg[6] = m->m_Z80.readRegWord( Z80_SP );
  uint8_t flag = reg[3] & 0x00FF;
  if ( msg )
    _MSG_PRINT( "\r\n%s\r\n", msg );
  _MSG_PRINT( "DISK CPU:IM(%d) IFF1(%d) IFF2(%d)\r\n",
              m->m_Z80.getIM(),
              m->m_Z80.getIFF1(),
              m->m_Z80.getIFF2() );
  _MSG_PRINT( "AF:%04X BC:%04X DE:%04X HL:%04X  S Z Y H X PV N C\r\n", reg[3], reg[0], reg[1], reg[2] );
  _MSG_PRINT( "IX:%04X IY:%04X SP:%04X PC:%04X  %d %d %d %d %d  %d %d %d\r\n",
              reg[4], reg[5], reg[6], PC,
              ( flag & Z80_S_FLAG )  != 0 ? true : false, 
              ( flag & Z80_Z_FLAG )  != 0 ? true : false, 
              ( flag & Z80_Y_FLAG )  != 0 ? true : false, 
              ( flag & Z80_H_FLAG )  != 0 ? true : false, 
              ( flag & Z80_X_FLAG )  != 0 ? true : false, 
              ( flag & Z80_PV_FLAG ) != 0 ? true : false, 
              ( flag & Z80_N_FLAG )  != 0 ? true : false, 
              ( flag & Z80_C_FLAG )  != 0 ? true : false );
}


void DISK::memoryDump( int address, int size )
{
  if ( address < 0 || address >= m_RAMSize || ( address + size ) > m_RAMSize ) return;
  _MSG_PRINT( "DISK CPU\r\nADRS +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F 0123456789ABCDEF\r\n" );
  for ( int i = address; i < ( address + size ); i += 16 )
  {
    char hexbuf[128];
    char ascbuf[64];
    sprintf( hexbuf, "%04X ", i );
    memset( ascbuf, 0, sizeof( ascbuf ) );
    for ( int j = 0; j < 16; j++ )
    {
      uint8_t data = 0xFF;
      int adrs = i + j;
      if ( adrs >= 0 && adrs < m_RAMSize )
        data = m_RAM[i+j];
      char work[16];
      sprintf( work, "%02X ", data );
      strcat( hexbuf, work );
      if ( data >= 0x20 && data < 0x7F )
        ascbuf[j] = data;
      else
        ascbuf[j] = '.';
    }
    _MSG_PRINT( "%s%s\r\n", hexbuf, ascbuf );
  }
}


void DISK::ioDump( int type, int address )
{
  uint8_t * ioBuf = ( type == 0 ) ? &m_IoIn[0] : &m_IoOut[0];
  if ( address < 0 || address >= m_IOSize ) return;
  _MSG_PRINT( "DISK CPU\r\n%s Adrs %02X : %02X\r\n", ( type == 0 ) ? "IN" : "OUT", address, ioBuf[address] );
}
