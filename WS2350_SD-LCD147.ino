/**************************************************/
/*! \file PicoCommandLine_Parser.ino
    \brief Raspberry pico command line interface (CLI)
    \defgroup command line parser
    \author Achim Jenne 18.10.2025
    \sa ... */
/**************************************************/

/**************************************************/
/*! \brief Automatic generated enum- commands
    \defgroup token_parser
    \sa datetime_t */
/**************************************************/
enum eCmdSt {eNoToken=0,
      eAUTO, eCD, eCLS, eCONFIG, eCOPY, eDATE, eDEL, eDIR,
      eECHO, eFORMAT, eHELP, eMD, ePATH, eRD, eREN, eTEMP,
      eTIME, eTYPE, eVER, eVOL, eXREC, eXTRAN};  

/**************************************************/
#include "SD-OS_Pico.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
//#include <arduino.h>
//#include <string.h>

//#include "pico/stdlib.h"
#include "I2C_RTC.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "LCD_1in47.h"

#include <SPI.h>
#include <SD.h>
#include <FatFS.h>
#include <Adafruit_NeoPixel.h>
/**************************************************/
static DS3231 RTC;
File myFile;

char sLogFn[40]= "start.txt";
char sPath[ILINE]={"/"};
volatile bool bAuto = false;
bool bRTC = false;
bool bSDCRD;
time_t tiUxOld, tiUx= (1762005636+1800);  // date of SW- Release;

struct timeval tiV;
uint16_t *BlackImage;

#if defined WS147GPIO || defined WS147SPI1
const int RGB_PIN = 22;
Adafruit_NeoPixel pixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);
#elif defined MAKERGPIO || defined MAKERSPI1
const int RGB_PIN = 28;
Adafruit_NeoPixel pixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);
#endif
//#define F2PI 3.1415926535*2/360.0
#define SECTICK 60
// sinus and cosinus table for wath hand on clock face 
float sinTab[SECTICK];
float cosTab[SECTICK];
/**************************************************/

/**************************************************/
void setup() {
  // put your setup code here, to run once:
  char sLine[ILINE];
  Serial.begin(115200);
  // wait on Serial to be available on Leonardo
  while (!Serial){
    delay(10);
  } 
  delay(10);
  pinMode(PIN_LED, OUTPUT);

  analogReadResolution(12);
  analogReadTemp(3.3f);

  Serial.print(S_CLS);
  Serial.println(BOARD_NAME);
  Serial.print(F("CPU- Frequency:   "));
  Serial.print(F_CPU/1000000);
  Serial.println(F(" MHz"));
  Serial.print(F("Pico SDK Version: "));
  Serial.println(PICO_SDK_VERSION_STRING);
  Serial.print(F("Arduino Version:  "));
  Serial.println(ARDUINO_PICO_VERSION_STR);

#if defined MAKERSPI1
  // check available SD_Card
  gpio_init(PIN_SS);
  gpio_set_dir(PIN_SS, false);
  gpio_disable_pulls(PIN_SS);
  gpio_put(PIN_SS, 0);
  uint8_t bSD1= 0;
  while ((bSD1= gpio_get(PIN_SS)) == 0) {
    delay(100);
    Serial.print(F("No Card ("));
    Serial.print(PIN_SS);
    Serial.print(F(":"));
    Serial.print(bSD1, DEC);
    Serial.print(F(")\e[14D"));
  }
  gpio_deinit(PIN_SS);
  delay(10);  
#endif

#if defined(MAKERGPIO) || defined(WS147GPIO)
  for (int i=0; i<SECTICK; i++) {
    sinTab[i]= sin((i*6 - 90) * DEG_TO_RAD);
    cosTab[i]= cos((i*6 - 90) * DEG_TO_RAD);
  }
#else
  SDSPI.setMISO(PIN_MISO);
  SDSPI.setMOSI(PIN_MOSI);
  SDSPI.setSCK(PIN_SCK);
  SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
#endif
  Serial.print(F("Init SD-Card  ... "));
  bool bInitSD = false;
  bInitSD = SD.begin( SDCRD);
  if (bInitSD) {
    Serial.println(F("OK"));
    bSDCRD= true;
  } else {
    Serial.println(F("failed"));
    bSDCRD= false;
  }
  SD.end(); 

  Serial.print(F("CPU- Temperature:  "));
  Serial.println(analogReadTemp(3.3f));

  //*** external RTC Modul ***
  if (RTC.begin()) {
    bRTC = true;
    if (!RTC.isRunning()) {
      Serial.println(F("RTC starting"));
      RTC.startClock();
    } 
    if (RTC.getHourMode() != CLOCK_H24) {
      Serial.println(F("RTC set to 24h Format"));
      RTC.setHourMode(CLOCK_H24);
    }
    /* load internal RTC from external RTC */
    tiV.tv_sec = dateTime2Unix(RTC.getYear(),RTC.getMonth(),RTC.getDay(),RTC.getHours(),RTC.getMinutes(),RTC.getSeconds()) ; 
    tiV.tv_usec = 0;
    settimeofday(&tiV, nullptr);
  
    Serial.print(F("RTC- Temperature:  "));
    Serial.println(RTC.getTemp());
    Serial.print(F("RTC- Date&Time:    "));
    sprintf(sLine, "%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d",
                   RTC.getDay(),   RTC.getMonth(),   RTC.getYear(), 
                   RTC.getHours(), RTC.getMinutes(), RTC.getSeconds()); 
    Serial.println(sLine); 
  } else {
    tiV.tv_sec = tiUx;
    tiV.tv_usec = 0;
    settimeofday(&tiV, nullptr);
  }

  time(&tiUx); 
  Serial.print(F("Pico internal RTC: "));
  strftime(sLine, sizeof(sLine), "%0d.%0m.20%0y %0H:%0M:%0S", localtime(&tiUx));
  Serial.println(sLine);
  // strftime(sLine, sizeof(sLine), " %0d %B 20%0y (%A) %0H:%0M:%0S", localtime(&tiUx));
  // Serial.println(sLine);

  /* LCD Init */
#if defined WS147GPIO || defined WS147SPI1   
    // LCD SPI Config
  Serial.print("LCD-Init ");
  if(DEV_Module_Init() == 0){
    Serial.println(F("OK"));
  } else {
    Serial.println(F("Error!"));
  }
  LCD_1IN47_Init(VERTICAL);
  LCD_1IN47_Clear(BLACK);
  DEV_SET_PWM(95);
  uint32_t Imagesize = (LCD_1IN47_HEIGHT+1)*(LCD_1IN47_WIDTH) *2;
  if((BlackImage = (uint16_t *) malloc(Imagesize)) == NULL) {
      Serial.println("Failed to allocate memory...");
      while (1) {}
    }
  Paint_NewImage((uint8_t *)BlackImage, LCD_1IN47.WIDTH, LCD_1IN47.HEIGHT, 0, BLACK);
  Paint_SetScale(65);
  Paint_SetRotate(ROTATE_0);
  Paint_Clear(BLACK);
#endif
#if defined WS147GPIO || defined WS147SPI1 || defined MAKERGPIO || defined MAKERSPI1
  pixel.begin();
  pixel.setBrightness(100);
#endif

  Serial.print(sPath);
  Serial.print(F(">"));
}

/**************************************************/
void loop() {
  bool bEM;
  char inChar;
  static char sLine[ILINE]; 
  char sLn1[ILINE];
  char *psLine= &sLine[0];

  // put your main code here, to run repeatedly:
  if (Serial.available()) { 
    inChar = (char) Serial.read();
    if (bEM= editLine(psLine, inChar)) {
      /************************************************************/
      // if command
#if defined WS147GPIO || defined WS147SPI1 
      pixel.fill(0xff0000);
      pixel.show();
#elif defined MAKERGPIO || defined MAKERSPI1
      pixel.fill(0x00ff00);      
      pixel.show();
#endif
      psLine = strupr(psLine);
      int iRet= fnSDOS_Parser(psLine);
      *psLine= 0;
      Serial.print(F("\r\n"));
      Serial.print(sPath);
      Serial.print(F(">"));
      
#if defined WS147GPIO || defined WS147SPI1 || defined MAKERGPIO || defined MAKERSPI1
      pixel.fill(0x000000);
      pixel.show();
#endif
    } /* end if */
  /************************************************************/
  /************************************************************/
  } else {
      struct tm mytm;
      static bool bTiUx;
      time(&tiUx);
      if (tiUx != tiUxOld){
        tiUxOld= tiUx;
        if ((tiUx%10) >= 8) {
          bTiUx= true;
        } else {
          bTiUx= false;
        }       
#if defined WS147GPIO || defined WS147SPI1        
        Paint_Clear(BLACK);
        if (bTiUx){
          Paint_DrawRectangle( 0, 0, 320,  34, GRAY, DOT_PIXEL_DFT,DRAW_FILL_FULL);
          Paint_DrawString( 28,  7, BOARD_NAME, &Font20, WHITE, BLUE);
          Paint_DrawString(  1, 36, "Freq: ", &Font24, WHITE, BLACK);
          Paint_DrawString(  1, 60, "Date: ", &Font24, WHITE, BLACK);
          Paint_DrawString(  1, 84, "Time: ", &Font24, WHITE, BLACK);
          Paint_DrawString(  1,108, "Temp: ", &Font24, WHITE, BLACK);
          sprintf(sLn1, "%ld", ((uint32_t) F_CPU/1000000L));
          Paint_DrawString( 120, 36, sLn1, &Font24, WHITE, BLACK);
          Paint_DrawString( 188, 36, "MHz", &Font24, WHITE, BLACK);
          strftime(sLn1, sizeof(sLine), "%0d.%0m.20%0y", localtime(&tiUx));
          Paint_DrawString( 120, 60, sLn1, &Font24, WHITE, BLACK);
          strftime(sLn1, sizeof(sLine), "%0H:%0M:%0S", localtime(&tiUx));
          Paint_DrawString( 120, 84, sLn1, &Font24, WHITE, BLACK); 
          Paint_DrawNum( 120,108, analogReadTemp(3.3f), &Font24, 2, WHITE, BLACK);
          Paint_DrawString(  1, 132, "SD-CRD:", &Font24, WHITE, BLACK);
          if (bSDCRD) {
            Paint_DrawString(120, 132, " OK  ", &Font24, BLACK, GREEN);
          } else {
            Paint_DrawString(120, 132, " ERR ", &Font24, WHITE, RED);
          } /* end if */
        } else {
          Paint_DrawRectangle( 15, 0, 168+15, 171, BLUE, DOT_PIXEL_DFT,DRAW_FILL_FULL);
          /* Clock  grafic center-position and Clock face param. */
          uint16_t iXOfs = 100;
          uint16_t iYOfs = 85;
          uint16_t iClkH = 32, iClkM = 58, iClkS = 68;
          Paint_DrawCircle(iXOfs+4, iYOfs+3, 81, DGRAY, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          Paint_DrawCircle(iXOfs-2, iYOfs-2, 81, LGRAY, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          Paint_DrawCircle(iXOfs, iYOfs, 78, GRAY, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          Paint_DrawCircle(iXOfs, iYOfs, 56, DGRAY, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          Paint_DrawString(  iXOfs-40, iYOfs-35, "CPU-RTC", &Font16, GREEN, BLACK);
          for (int i=0; i<SECTICK; i++) { // draw Clock-face Dots
            if ((i%15) == 0){
              Paint_DrawPoint((uint16_t) (cosTab[i]*70 +iXOfs), (uint16_t)(sinTab[i]*70 +iYOfs), BLACK, DOT_PIXEL_5X5, DOT_FILL_AROUND);
            } else
            if ((i%5) == 0){
              Paint_DrawPoint((uint16_t) (cosTab[i]*67 +iXOfs), (uint16_t)(sinTab[i]*67 +iYOfs), BLACK, DOT_PIXEL_3X3, DOT_FILL_AROUND);
            } else {
              Paint_DrawPoint((uint16_t) (cosTab[i]*65 +iXOfs), (uint16_t)(sinTab[i]*65 +iYOfs), BLACK, DOT_PIXEL_2X2, DOT_FILL_AROUND);
            }
          } /* end for */
          mytm= *localtime(&tiUx);
          uint16_t iTmp = (mytm.tm_hour%12)*5 +(mytm.tm_min/12);
          Paint_DrawLine(iXOfs, iYOfs, (uint16_t)(cosTab[iTmp]*iClkH+iXOfs), (uint16_t)(sinTab[iTmp]*iClkH+iYOfs), WHITE, DOT_PIXEL_4X4,LINE_STYLE_SOLID);
          Paint_DrawLine(iXOfs, iYOfs, (uint16_t)(cosTab[iTmp]*iClkH+iXOfs), (uint16_t)(sinTab[iTmp]*iClkH+iYOfs), DGRAY, DOT_PIXEL_2X2,LINE_STYLE_SOLID);
          Paint_DrawLine(iXOfs, iYOfs, (uint16_t)(cosTab[mytm.tm_min]*iClkM+iXOfs), (uint16_t)(sinTab[mytm.tm_min]*iClkM+iYOfs), WHITE, DOT_PIXEL_3X3,LINE_STYLE_SOLID);
          Paint_DrawLine(iXOfs, iYOfs, (uint16_t)(cosTab[mytm.tm_min]*iClkM+iXOfs), (uint16_t)(sinTab[mytm.tm_min]*iClkM+iYOfs), BLACK, DOT_PIXEL_1X1,LINE_STYLE_SOLID);
          Paint_DrawLine(iXOfs, iYOfs, (uint16_t)(cosTab[mytm.tm_sec]*iClkS+iXOfs), (uint16_t)(sinTab[mytm.tm_sec]*iClkS+iYOfs), RED, DOT_PIXEL_2X2,  LINE_STYLE_SOLID);
          Paint_DrawCircle(iXOfs, iYOfs, 8, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          strftime(sLn1, sizeof(sLn1), "%A", &mytm);
          Paint_DrawString( 190, 1, sLn1, &Font20, WHITE, BLACK);
          sprintf(sLn1, "%2.2d.%2.2d.%4d", mytm.tm_mday, mytm.tm_mon+1, mytm.tm_year+1900 );
          Paint_DrawString( 190, 22, sLn1, &Font16, WHITE, BLACK);
        } /* end if */
        LCD_1IN47_Display(BlackImage);
#endif        
      }
  } /* end if */
} /* end main loop */
