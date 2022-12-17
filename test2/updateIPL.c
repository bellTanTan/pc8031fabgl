//
// $ gcc -Wall -o updateIPL updateIPL.c
// $ ./updateIPL
// $ od -tx1z -v -Ax '[OS]_IPL_TEST_disk_(BASIC_formatted).d88' | more
// check offset 0x0002c0 ~ 0x0003bf
//
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define IPLCODE_FILE                "ipltest.bin"
#define D88_FILE                    "[OS]_IPL_TEST_disk_(BASIC_formatted).d88"
#define IPLCODE_WRITE_DISK_OFFSET   0x2C0

uint8_t iplcode[256];


int main( int argc, char **argv )
{
    FILE    *fp;
    bool    iplReadReady = false;
    bool    iplWriteReady = false;

    fp = fopen( IPLCODE_FILE, "r" );
    if ( fp )
    {
        size_t readByte = sizeof( iplcode );
        if ( fread( iplcode, 1, readByte, fp ) == readByte )
            iplReadReady = true;
        fclose( fp );
    }

    fp = fopen( D88_FILE, "r+" );
    if ( fp )
    {
        if ( !fseek( fp, IPLCODE_WRITE_DISK_OFFSET, SEEK_SET ) )
        {
            size_t writeByte = sizeof( iplcode );
            if ( fwrite( iplcode, 1, writeByte, fp ) == writeByte )
                iplWriteReady = true;
        }
        fclose( fp );
    }

    if ( !iplReadReady )
    {
        fprintf( stderr, "'%s' load failed\n", IPLCODE_FILE );
        return 1;
    }

    if ( !iplWriteReady )
    {
        fprintf( stderr, "'%s' ipl code write failed\n", D88_FILE );
        return 2;
    }

    fprintf( stderr, "'%s' ipl code write complete\n", D88_FILE );

    return 0;
}
