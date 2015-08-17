/*
  TLV5610.cpp

  Arduino library for Texas Instruments TLV5610 8-channel 12-bit SPI DAC
  Partially based on the Wiblocks library http://wiblocks.luciani.org/src/lib/DAC/classDAC__TLV5618.html
  based on the TLV5618 library by:
  2012-09-29 @machinesalem,  (cc) https://creativecommons.org/licenses/by/3.0/

  Adapted to TLV5610 by Thomas Van Ta on 17. August 2015
*/

#include "SPI.h"
#include "TLV5610.h"


TLV5610::TLV5610( uint8_t cs_pin )
{
  _cs_pin = cs_pin;
  _control = TLV5610_SPEED_SLOW | TLV5610_POWER_NORM;
};

TLV5610::TLV5610( uint8_t cs_pin, uint8_t control )
{
  _cs_pin = cs_pin;
  _control = control;
};


void TLV5610::begin()
{
  /* Initialize SPI */
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV2);  // (you could try other speeds)

  /* !Chip select (low to enable) */
  pinMode(_cs_pin, OUTPUT);
  digitalWrite(_cs_pin,  1);
};


/* Chip-select (1 to select) */
void TLV5610::select( int b )
{
  digitalWrite( _cs_pin, !b );
  delayMicroseconds( 10 );
}

/* Write without chip-select */
//inline void TLV5618::write_data_no_cs( uint8_t cmd, uint16_t value )
//{
//  SPI.transfer( ((value & 0xF00)>>8) | cmd | _control ); SPI.transfer(   value & 0xFF );
//};


/* Write using one of the TLV5618_CMD_xxx */
void TLV5610::write_data( uint8_t cmd, uint16_t value )
{
  digitalWrite( _cs_pin, 0 );
  delayMicroseconds( 1 );
  SPI.transfer( ((value & 0x0F00)>>8) | cmd | _control );
  SPI.transfer(   value & 0xFF );
  delayMicroseconds( 1 );
  digitalWrite( _cs_pin,  1 );
};

#define SPI_TRANSFER(data) { SPDR=data; while(!(SPSR & (1<<SPIF))); }

void TLV5610::write_fast( uint8_t cmd, uint16_t value )
{
  digitalWrite( _cs_pin, 0 );
  cli();
  SPI_TRANSFER( ((value & 0x0F00)>>8) | cmd | _control );
  SPI_TRANSFER(   value & 0xFF );
  sei();
  digitalWrite( _cs_pin,  1 );
};

/* Convenient method to write channels A and B at the same time */
void TLV5610::write( uint16_t valueA, uint16_t valueB )
{
  digitalWrite( _cs_pin, 0 );
  delayMicroseconds( 1 );
  SPI.transfer( ((valueA & 0xF00)>>8) | TLV5610_CMD_WRITE_BUFFER | _control );
  SPI.transfer(   valueA & 0xFF );
  delayMicroseconds( 1 );
  digitalWrite( _cs_pin,  1 );
  delayMicroseconds( 1 );
  digitalWrite( _cs_pin, 0 );
  delayMicroseconds( 1 );
  SPI.transfer( ((valueB & 0xF00)>>8) | TLV5610_CMD_WRITE_A_UPDATE_B | _control );
  SPI.transfer(   valueB & 0xFF );
  delayMicroseconds( 1 );
  digitalWrite( _cs_pin,  1 );
};
