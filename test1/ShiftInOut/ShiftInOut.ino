/*
  Created by tan (trinity09181718@gmail.com)
  Copyright (c) 2022 tan
  All rights reserved.

  Arduino IDE board : Arduino ESP32 v2.0.5
  library           : non
  target            : ESP32 DEV Module
  flash size        : 4MB(32Mb)
  partition scheme  : Huge APP (3MB No OTA/1MB SPIFFS)
  PSRAM             : Disabled

* Please contact trinity09181718@gmail.com if you need a commercial license.
* This software is available under GPL v3.

 */

#pragma GCC optimize ("O2")

#include <Arduino.h>

#define IN_SL           23    // ESP32 GPIO -> LEVEL SHIFT -> SN74HC165N SH/LD
#define IN_CLK          19    // ESP32 GPIO -> LEVEL SHIFT -> SN74HC165N CLK
#define IN_QH           18    // ESP32 GPIO <- LEVEL SHIFT <- SN74HC165N QH
#define OUT_SER         5     // ESP32 GPIO -> LEVEL SHIFT -> SN74HC595N SER
#define OUT_RCLK        4     // ESP32 GPIO -> LEVEL SHIFT -> SN74HC595N RCLK
#define OUT_SRCLK       15    // ESP32 GPIO -> LEVEL SHIFT -> SN74HC595N SRCLK
#define SYSTEM_STATUS   33    // ESP32 GPIO SYSTEM STATUS LED

uint8_t   outLedData;
uint16_t  oldInData;
int       ledFlag;
int       systemStatusLed;
int       systemStatusLedTimer;


uint16_t shiftInput( void )
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

void  shiftOutput( uint16_t value )
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

void setup()
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
  pinMode( SYSTEM_STATUS, OUTPUT );
  digitalWrite( SYSTEM_STATUS, LOW );

  Serial.begin( 115200 );
  while (!Serial && !Serial.available());
  Serial.printf( "boot\r\n" );

  outLedData           = 0x00;
  oldInData            = 0xFFFF;
  ledFlag              = 0;
  systemStatusLed      = LOW;
  systemStatusLedTimer = millis();
  shiftOutput( 0x0000 );
}

void loop()
{
  int dt = millis() - systemStatusLedTimer;
  if ( dt >= 500 )
  {
    systemStatusLedTimer = millis();
    if ( systemStatusLed == LOW )
      systemStatusLed = HIGH;
    else
      systemStatusLed = LOW;
    digitalWrite( SYSTEM_STATUS, systemStatusLed );
  }

  uint32_t t0 = micros();
  uint16_t inData = shiftInput();
  uint32_t t1 = micros();
  dt = t1 - t0;
  if ( inData != oldInData )
  {
    oldInData = inData;
    Serial.printf( "IN: 0x%04X [dt %d usec] ", inData, dt );

    uint16_t outData = ~inData;
    if ( ledFlag == 0 )
    {
      if ( outLedData == 0 )
        outLedData = 0x01;
      outData &= 0x0FFF;
      outData |= ( (uint16_t)outLedData ) << 12;
      outLedData <<= 1;
      if ( outLedData == 0x10 )
      {
        outLedData = 0x00;
        ledFlag = 1;
      }
    }
    else
    {
      if ( outLedData == 0 )
        outLedData = 0x08;
      outData &= 0x0FFF;
      outData |= ( (uint16_t)outLedData ) << 12;
      outLedData >>= 1;
      if ( outLedData == 0x00 )
      {
        outLedData = 0x00;
        ledFlag = 0;
      }
    }

    t0 = micros();
    shiftOutput( outData );
    t1 = micros();
    dt = t1 - t0;
    Serial.printf( "OUT:0x%04X [dt %d usec]\r\n", outData, dt );
  }
}
