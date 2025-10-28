/****************************************************************************/
/*  SD-Card configuration                                                   */
/*                                                                          */
/*                                            Achim Jenne 18.10.2025        */
/****************************************************************************/
#ifndef SDOS_PICO
#define ILINE 80
#include <pins_arduino.h>
/**************************************************/
#define WS147GPIO

/*! \brief MYSPI0                                 
    \ingroup SDOS_PICO 
    */
#if defined MYSPI0
#define SDSPD 16
#define PIN_MISO PIN_SPI0_MISO
#define PIN_MOSI PIN_SPI0_MOSI
#define PIN_SCK PIN_SPI0_SCK
#define PIN_SS PIN_SPI0_SS
#define SDSPI SPI
#define SDCRD PIN_SS,SPI
#elif defined MYSPI1
#define SDSPD 16
#define PIN_MISO PIN_SPI1_MISO
#define PIN_MOSI PIN_SPI1_MOSI
#define PIN_SCK PIN_SPI1_SCK
#define PIN_SS PIN_SPI1_SS
#define SDSPI SPI1
#define SDCRD PIN_SS,SPI1
#elif defined MAKERSPI1
#define SDSPD 40
#define PIN_MISO 12
#define PIN_MOSI 11
#define PIN_SCK 10
#define PIN_SS 15
#define SDSPI SPI1
#define SDCRD PIN_SS,SPI1
#elif defined MAKERGPIO
// If you have all 4 DAT pins wired up to the Pico you can use SDIO mode
  const uint8_t RP_CLK_GPIO = 10; // Set to CLK GPIO
  const uint8_t RP_CMD_GPIO = 11; // Set to CMD GPIO
  const uint8_t RP_DAT0_GPIO = 12; // Set to DAT0 GPIO. DAT1..3 must be consecutively 
#define SDCRD RP_CLK_GPIO,RP_CMD_GPIO,RP_DAT0_GPIO
#elif defined WS35SPI1
#define SDSPD 40
#define PIN_MISO 12
#define PIN_MOSI 11
#define PIN_SCK 10
#define PIN_SS 22
#define SDSPI SPI1
#define SDCRD PIN_SS,SPI1
#elif defined WS147SPI1
/*! \brief WS147SPI1 (the Waveshare Pico 2 with 1.47 In.)                                
    \ingroup SDOS_PICO 
    */
#define SDSPD 40
#define PIN_MISO 12
#define PIN_MOSI 11
#define PIN_SCK 10
#define PIN_SS 15
#define SDSPI SPI1
#define SDCRD PIN_SS,SPI1
#elif defined WS147GPIO
// If you have all 4 DAT pins wired up to the Pico you can use SDIO mode
  const int RP_CLK_GPIO = 10; // Set to CLK GPIO
  const int RP_CMD_GPIO = 11; // Set to CMD GPIO
  const int RP_DAT0_GPIO = 12; // Set to DAT0 GPIO. DAT1..3 must be consecutively 
#define SDCRD RP_CLK_GPIO,RP_CMD_GPIO,RP_DAT0_GPIO
#else
#error SPI- Configuration not OK
#endif

/**************************************************/
#define DELAYLED 200
#define USBPWR 24  //Raspberry Pico only
//#define EXTPWR 29
#define C_BS  0x08
#define C_ESC 0x1b
#define S_CLS F("\e[0H\e[2J")
#define S_CR  F("\r\n>")
  uint64_t dateTime2Unix(int , int , int , int , int , int );
  int day, mon, year;
  int hour, minute, second;
#define SDOS_PICO
#endif