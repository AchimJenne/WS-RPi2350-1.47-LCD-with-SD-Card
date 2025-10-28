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
      eTIME, eTYPE, eVER, eVOL};

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
time_t tiUxOld, tiUx= (1761660036);  // date of SW- Release;

struct timeval tiV;
uint16_t *BlackImage;

#if defined WS147GPIO || defined WS147SPI1
const int RGB_PIN = 22;
Adafruit_NeoPixel pixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);
#elif defined MAKERGPIO || defined MAKERSPI1
const int RGB_PIN = 28;
Adafruit_NeoPixel pixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);
#endif

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

#else
  SDSPI.setMISO(PIN_MISO);
  SDSPI.setMOSI(PIN_MOSI);
  SDSPI.setSCK(PIN_SCK);
  SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
#endif
  Serial.print(F("Init SD-Card  ... "));
  bool bInitSD = false;
  // bInitSD = SD.begin( RP_CLK_GPIO, RP_CMD_GPIO, RP_DAT0_GPIO);
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
    Serial.printf("%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d\r\n",
                   RTC.getDay(),   RTC.getMonth(),   RTC.getYear(), 
                   RTC.getHours(), RTC.getMinutes(), RTC.getSeconds());  
  } else {
    tiV.tv_sec = tiUx;
    tiV.tv_usec = 0;
    settimeofday(&tiV, nullptr);
  }

  time(&tiUx); 
  Serial.print(F("Pico internal RTC: "));
  strftime(sLine, sizeof(sLine), "%0d.%0m.20%0y %0H:%0M:%0S", localtime(&tiUx));
  Serial.print(sLine);
  strftime(sLine, sizeof(sLine), " %0d %B 20%0y (%A) %0H:%0M:%0S", localtime(&tiUx));
  Serial.println(sLine);

  /* LCD Init */
#if defined WS147GPIO || defined WS147SPI1   
    // LCD SPI Config
  Serial.print("LCD-Init ");
  if(DEV_Module_Init() == 0){
    Serial.println(F("OK"));
  } else {
    Serial.println(F("Error!"));
  }
  // DEV_SET_PWM(0);
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
    inChar = (char)Serial.read();
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
      
#if defined WS147GPIO || defined WS147SPI1 || defined MAKERGPIO || defined MAKERSPI1
      pixel.fill(0x000000);
      pixel.show();
#endif

      Serial.print(F("\r\n"));
      Serial.print(sPath);
      Serial.print(F(">"));
    } /* end if */
  /************************************************************/
  /************************************************************/
  } else {
      time(&tiUx);
      if (tiUx != tiUxOld){
        tiUxOld= tiUx;
#if defined WS147GPIO || defined WS147SPI1        
        Paint_Clear(BLACK);
        Paint_DrawRectangle( 0, 0, 320,  34, GRAY, DOT_PIXEL_DFT,DRAW_FILL_FULL);
        Paint_DrawString( 28,  7, BOARD_NAME , &Font20, WHITE, BLUE);
        Paint_DrawString(  1, 36, "Date: ", &Font24, WHITE, BLACK);
        Paint_DrawString(  1, 60, "Time: ", &Font24, WHITE, BLACK);
        Paint_DrawString(  1, 84, "Temp: ", &Font24, WHITE, BLACK);
        strftime(sLn1, sizeof(sLine), "%0d.%0m.20%0y", localtime(&tiUx));
        Paint_DrawString( 120, 36, sLn1, &Font24, WHITE, BLACK);
        strftime(sLn1, sizeof(sLine), "%0H:%0M:%0S", localtime(&tiUx));
        Paint_DrawString( 120, 60, sLn1, &Font24, WHITE, BLACK);  
        Paint_DrawNum( 120, 84, analogReadTemp(3.3f), &Font24, 2, WHITE, BLACK);
        Paint_DrawString(  1, 108, "SD-CRD:", &Font24, WHITE, BLACK);
        if (bSDCRD) {
          Paint_DrawString(120, 108, " OK  ", &Font24, BLACK, GREEN);
        } else {
          Paint_DrawString(120, 108, " ERR ", &Font24, WHITE, RED);
        }
        LCD_1IN47_Display(BlackImage);
#endif        
      }
  } /* end if */
} /* end main loop */
