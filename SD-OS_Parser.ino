  #include "SD-OS_Pico.h"

/**************************************************/
/*! \brief fnc_AUTO                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_AUTO(const char* szCmdLn)
{
    /* place your code here */
    if (bAuto) 
    {
      // tickHd100.detach();
      Serial.print(F(": off"));
      bAuto= false;
    } else {
      // tickHd100.attach_ms(100, task_ti100ms, (int) 1);
      Serial.print(F(": on"));
      bAuto = true;
    }
    return( eAUTO );
}  /* end of fnc_AUTO */


/**************************************************/
/*! \brief fnc_CD                                   
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_CD(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE]={""};
  char* psL;
  if(strlen(szCmdLn) > 1){
    if (!strcmp(szCmdLn, " /")){
       strcpy(sLine,"/");
    } else
    if (strstr(szCmdLn, " ..")) {
      strcpy(sLine, sPath);
      psL= strrchr(sLine, '/');
      *psL = '\0';
      if (strlen(sLine) <= 1) {
        strcpy(sLine,"/");
      }
    } else
    if (strstr(szCmdLn, " .")) {
      strcpy(sLine, sPath);
      strcat(sLine,"/");
      strcat(sLine, szCmdLn + 2);
    } else {  
      strcpy(sLine, sPath);
      if (strcmp(sLine, "/"))
      {
        strcat(sLine,"/");
      }
      strcat(sLine, szCmdLn + 1);
    }
  } else {
    strcpy(sLine, sPath);
  }
  if (SD.begin( SDCRD)) 
  {
    digitalWrite(PIN_LED, 1);
    File dir = SD.open(sLine);
    if (dir) {
      strcpy(sPath, sLine);
    } else {
      Serial.println();
      Serial.print(sLine);
      Serial.print(F(" not found"));
    }
    SD.end();
    digitalWrite(PIN_LED, 0);
  } 
  return( eCD );
}  /* end of fnc_CD */
 
/**************************************************/
/*! \brief fnc_CLS                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_CLS(const char* szCmdLn)
{
   /* place your code here */
   Serial.print(F("\e[0H\e[2J"));
   return( eCLS );
}  /* end of fnc_CLS */
 
/**************************************************/
/*! \brief fnc_COPY                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_COPY(const char* szCmdLn)
{
  /* place your code here */
  File FH1, FH2;
  char sFnTo[ILINE];
  char sFnFrom[ILINE];
  char s1[40], s2[40];
  #define IBUFFER 64
  char cBuff[IBUFFER];

  szCmdLn++;
  int iRet= sscanf(szCmdLn,"%s %s", &s1, &s2);
  if (iRet ==2) {
    strcpy(sFnFrom, sPath);
    strcat(sFnFrom, "/");
    strcat(sFnFrom, s1);
    strcpy(sFnTo, sPath);
    strcat(sFnTo, "/");
    strcat(sFnTo, s2);

    if (SD.begin( SDCRD)) 
    {
      digitalWrite(PIN_LED, 1);
      FH1 = SD.open(sFnFrom, FILE_READ);
      if (FH1) {
        FH2 = SD.open(sFnTo, FILE_WRITE);
        if (FH2) {
          while (FH1.available() > 0)
          {
            size_t iLen = FH1.readBytes(cBuff, IBUFFER);
            FH2.write(cBuff, iLen);
          } 
          Serial.print(F(" OK"));
        } else {
          Serial.print(F(" no Dest."));
        }
        FH2.close();
        FH1.close();
      } else {
        Serial.print(sFnFrom);
        Serial.print(F(" no Source"));
      }
      digitalWrite(PIN_LED, 0);
    }
    SD.end();
  }  
  return( eCOPY );
}  /* end of fnc_COPY */
 
/**************************************************/
/*! \brief fnc_CONFIG                               
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_CONFIG(const char* szCmdLn)
{
  /* place your code here */
  Serial.println(F(": "));
  Serial.println(BOARD_NAME);
  Serial.print(F("CPU- Frequency: "));
  Serial.print(F_CPU/1000000);
  Serial.println(F(" MHz"));
  Serial.println(F("SPI-Interface"));  
  Serial.print(F("SPI : "));
  if (!SD.begin( SDCRD)) 
  {
     Serial.println(F("failed"));
  } else {
    digitalWrite(PIN_LED, 1);
    Serial.println(F("OK"));
#if not defined MAKERGPIO && not defined WS147GPIO
    Serial.print(F("MISO : "));
    Serial.println(PIN_MISO);
    Serial.print(F("MOSI : "));
    Serial.println(PIN_MOSI);
    Serial.print(F("SCK  : "));
    Serial.println(PIN_SCK);
    Serial.print(F("CS   : "));
    Serial.println(PIN_SS);
#endif    
    Serial.print(F("SD-Card type:      "));
    switch (SD.type()) {
      case 0:
        Serial.println(F("SD"));
        break;
      case 1:
        Serial.println(F("SD1"));
        break;
      case 2:
        Serial.println(F("SD2"));
        break;
      case 3:
        Serial.println(F("SDHC/SDXC"));
        break;
      default:
      Serial.println(F("Unknown"));
    } 
  }
  SD.end(); 
  if (bRTC){
    Wire.begin();
    int i2c = 0x57;
    Serial.print("EEPROM at [");
    Serial.print(i2c, HEX);
    Serial.print(F("H]:  "));
    Wire.beginTransmission(i2c);
    int result = Wire.endTransmission();
    if (result==0)
      Serial.println(F("  found"));
    else
      Serial.println(F("not found"));
  }
  digitalWrite(PIN_LED, 0);
  return( eCONFIG );
}  /* end of fnc_CONFIG */

/**************************************************/
/*! \brief fnc_DATE                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_DATE(const char* szCmdLn)
{
  char sLine[ILINE];
  struct tm mytm;
  time_t clk;

  /* place your code here */
  clk= time(nullptr);
  mytm= *localtime(&clk);
  if (strlen(szCmdLn) >= 8){
    int16_t iResult= sscanf( szCmdLn,"%02d.%02d.%04d", &day, &mon, &year);   
    if (iResult == 3) {
      if (bRTC) {
        RTC.setDate((uint8_t) day, (uint8_t) mon, (uint16_t) year);
        hour  = RTC.getHours();
        minute= RTC.getMinutes();
        second= RTC.getSeconds();
      } else {
        hour  = mytm.tm_hour;
        minute= mytm.tm_min;
        second= mytm.tm_sec;
      }
      tiV.tv_sec = dateTime2Unix( year, mon, day, hour, minute, second); 
      tiV.tv_usec = 0;
      settimeofday(&tiV, nullptr);
    }
  } else {
    Serial.print(F(" : "));
    if (bRTC){
      Serial.print(F("RTC: "));
      sprintf(sLine,"%2.2d.%2.2d.%4.4d ",RTC.getDay(),RTC.getMonth(),RTC.getYear());
      Serial.print(sLine);
    }
    Serial.print(F("CPU: "));
    time(&tiUx);
    strftime(sLine, sizeof(sLine), "(%A) %0d.%0m.20%0y", localtime(&tiUx));
    Serial.print(sLine);
  }
  return( eDATE );
}  /* end of fnc_DATE */
 
/**************************************************/
/*! \brief fnc_DEL                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_DEL(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE]={""};
  char* psL;
  if(strlen(szCmdLn) > 1){
    if (!strcmp(szCmdLn, " /")){
       strcpy(sLine,"/");
    } else
    if (strstr(szCmdLn, " ..")) {
      strcpy(sLine, sPath);
      psL= strrchr(sLine, '/');
      *psL = '\0';
      if (strlen(sLine) <= 1) {
        strcpy(sLine,"/");
      }
    } else
    if (strstr(szCmdLn, " .")) {
      strcpy(sLine, sPath);
      strcat(sLine,"/");
      strcat(sLine, szCmdLn + 2);
    } else {  
      strcpy(sLine, sPath);
      if (strcmp(sLine, "/"))
      {
        strcat(sLine,"/");
      }
      strcat(sLine, szCmdLn + 1);
    }
  } else {
    strcpy(sLine, sPath);
  }
  Serial.print(F(" : "));
  if (SD.begin(SDCRD))
  {
    if (SD.exists(sLine))
    {
      digitalWrite(PIN_LED, 1);
      int16_t iRes= SD.remove(sLine);
      if (iRes>0)
      {
        Serial.print(F("removed"));
      } else {
        Serial.print(F("not removed!"));
      }
    } else {
      Serial.print(F("not Found"));
    }
    SD.end();
    digitalWrite(PIN_LED, 0);
  }   
  return( eDEL );
}  /* end of fnc_DEL */
 
/**************************************************/
/*! \brief fnc_DIR                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
void printDirectory(File dir, int numTabs) {
  static File entry;
  while ( entry =  dir.openNextFile()) {
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print("\e[");
      Serial.print(i*2);
      Serial.print("C");
    }
    Serial.print(entry.name());
    if (entry.isDirectory())   // files have sizes, directories do not
    {
      Serial.println(F("\\\e[28G\e[1m<Dir>\e[0m"));
      // printDirectory(entry, numTabs + 1);  // uncommend for recursive funcion call
    } else
    if (entry.isFile()) {
      Serial.print(F("\e[24G"));
      Serial.print(entry.size(), DEC);
      Serial.println(F(" Byte"));
    }
    entry.close();
  }
}

int fnc_DIR(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE];
  if (strlen(szCmdLn)>1){
    strcpy(sLine, szCmdLn+1);
  } else {
    strcpy(sLine, sPath);
  }
  Serial.println(F(" : "));
  if (SD.begin(SDCRD)) 
  {
    digitalWrite(PIN_LED, 1);
    File dir = SD.open(sLine);
    printDirectory(dir, 0);
    SD.end();
    digitalWrite(PIN_LED, 0);
  }
   return( eDIR );
}  /* end of fnc_DIR */
 
/**************************************************/
/*! \brief fnc_ECHO                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_ECHO(const char* szCmdLn)
{
  /* place your code here */
  File FH1;
  Serial.print(F("\r\n"));
  if (SD.begin(SDCRD))   {
    digitalWrite(PIN_LED, 1);
    FH1 = SD.open(sLogFn, FILE_WRITE);
    if (FH1) {
      Serial.println(szCmdLn+1);
      FH1.println(szCmdLn+1);
      FH1.close();
    } else {
      Serial.print(F("no File ..."));
      Serial.print(sLogFn);
    }
    SD.end();
    digitalWrite(PIN_LED, 0);
  }
   return( eECHO );
}  /* end of fnc_ECHO */
 
/**************************************************/
/*! \brief fnc_FORMAT                               
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_FORMAT(const char* szCmdLn)
{
  /* place your code here */
  Serial.println(F(" : "));
  if (SD.begin(SDCRD)) {
    digitalWrite(PIN_LED, 1);
    //bool stFS= SD.format(); // 
    Serial.println(F("A \"format\" function was not implemented"));
    SD.end();
    digitalWrite(PIN_LED, 0);
  }
  return( eFORMAT );
}  /* end of fnc_FORMAT */
 
/**************************************************/
/*! \brief fnc_HELP                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_HELP(const char* szCmdLn)
{
   /* place your code here */
   Serial.println(F(" :"));
   Serial.println(F("AUTO"));
   Serial.println(F("CLS"));
   Serial.println(F("CONFIG"));
   Serial.println(F("COPY"));
   Serial.println(F("DATE"));
   Serial.println(F("DEL"));
   Serial.println(F("DIR"));
   Serial.println(F("ECHO"));
   Serial.println(F("FORMAT"));
   Serial.println(F("HELP"));
   Serial.println(F("PATH"));
   Serial.println(F("REM"));
   Serial.println(F("REN"));
   Serial.println(F("CD"));
   Serial.println(F("MD"));
   Serial.println(F("RD"));
   Serial.println(F("TIME"));
   Serial.println(F("TEMP"));
   Serial.println(F("TYPE"));
   Serial.println(F("VER"));
   Serial.println(F("VOL"));
   return( eHELP );
}  /* end of fnc_HELP */
 
/**************************************************/
/*! \brief fnc_MD                                   
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_MD(const char* szCmdLn)
{
   /* place your code here */
  char sLine[ILINE]={""};
  char* psL;
  if(strlen(szCmdLn) > 1){
    if (!strcmp(szCmdLn, " /")){
       strcpy(sLine,"/");
    } else
    if (strstr(szCmdLn, " ..")) {
      strcpy(sLine, sPath);
      psL= strrchr(sLine, '/');
      *psL = '\0';
      if (strlen(sLine) <= 1) {
        strcpy(sLine,"/");
      }
    } else
    if (strstr(szCmdLn, " .")) {
      strcpy(sLine, sPath);
      strcat(sLine,"/");
      strcat(sLine, szCmdLn + 2);
    } else {  
      strcpy(sLine, sPath);
      if (strcmp(sLine, "/"))
      {
        strcat(sLine,"/");
      }
      strcat(sLine, szCmdLn + 1);
    }
  } else {
    strcpy(sLine, sPath);
  }
  Serial.print(F(" : "));
  digitalWrite(PIN_LED, 1);
  if (SD.begin(SDCRD)) {
    if (!SD.exists(sLine)) {
      int16_t iRes= SD.mkdir(sLine);
      if (iRes>0) {
        Serial.print(F("Direcory created"));
      } else {
        Serial.print(F("Direcory not created"));
      }
    } else {
      Serial.print(F(" is an existing Directory"));
    }
    SD.end();
    digitalWrite(PIN_LED, 0);
  }
  return( eMD );
}  /* end of fnc_MD */

/**************************************************/
/*! \brief fnc_PATH                               
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_PATH(const char* szCmdLn)
{
   /* place your code here */
   Serial.print(" : ");
   Serial.print(sPath);
   return( ePATH );
}  /* end of fnc_PROMPT */
 
/**************************************************/
/*! \brief fnc_RD                                   
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_RD(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE]={""};
  char* psL;
  if(strlen(szCmdLn) > 1){
    if (!strcmp(szCmdLn, " /")){
       strcpy(sLine,"/");
    } else
    if (strstr(szCmdLn, " ..")) {
      strcpy(sLine, sPath);
      psL= strrchr(sLine, '/');
      *psL = '\0';
      if (strlen(sLine) <= 1) {
        strcpy(sLine,"/");
      }
    } else
    if (strstr(szCmdLn, " .")) {
      strcpy(sLine, sPath);
      strcat(sLine,"/");
      strcat(sLine, szCmdLn + 2);
    } else {  
      strcpy(sLine, sPath);
      if (strcmp(sLine, "/"))
      {
        strcat(sLine,"/");
      }
      strcat(sLine, szCmdLn + 1);
    }
  } else {
    strcpy(sLine, sPath);
  }
  Serial.print(F(" : "));
  digitalWrite(PIN_LED, 1);
  if (SD.begin(SDCRD)) {
    if (SD.exists(sLine)) {
      int16_t iRes= SD.rmdir(sLine);
      if (iRes>0) {
        Serial.print(F("Direcory removed"));
      } else {
        Serial.print(F("Direcory not removed"));
      }
    } else {
      Serial.print(F(" Directory not found"));
    }
    SD.end();
  }
  digitalWrite(PIN_LED, 0);
  return( eRD );
}  /* end of fnc_RD */
 
/**************************************************/
/*! \brief fnc_REN                               
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_REN(const char* szCmdLn)
{
  /* place your code here */
  char sFrom[ILINE];
  char sTo[ILINE];
  char s1[40];
  char s2[40];
  Serial.print(F(" : "));
  szCmdLn++;
  int iRet= sscanf(szCmdLn,"%s %s", &s1, &s2);
  if (iRet ==2) {
    strcpy(sFrom, sPath);
    strcat(sFrom, "/");
    strcat(sFrom, s1);
    strcpy(sTo, sPath);
    strcat(sTo, "/");
    strcat(sTo, s2);
    if (SD.begin( SDCRD)) {
      digitalWrite(PIN_LED, 1);
      bool stFS= SD.rename(sFrom, sTo);
      if (stFS) {
        Serial.print(F("OK"));
      } else {
        Serial.print(F("Error"));
      }
      SD.end();
      digitalWrite(PIN_LED, 0);
    }
  } /* end if */
  return( eREN );
}  /* end of fnc_RENAME */
 
/**************************************************/
/*! \brief fnc_TEMP                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_TEMP(const char* szCmdLn)
{
  /* place your code here */
  if (bRTC) {
    Serial.print(" RTC: ");
    Serial.print(RTC.getTemp());
  } 
  Serial.print(" CPU: ");
  Serial.print(analogReadTemp(3.3f));
  return( eTEMP );
}  /* end of fnc_TEMP */
 
/**************************************************/
/*! \brief fnc_TIME                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_TIME(const char* szCmdLn)
{
   /* place your code here */   
  char sLine[ILINE];
  struct tm mytm;
  time_t clk;
  clk= time(nullptr);
  //Serial.println(clk);
  mytm= *localtime(&clk);
  if (strlen(szCmdLn) >= 3) {
    int iResult= sscanf( szCmdLn,"%02d:%02d:%02d", &hour, &minute, &second);
    if (iResult == 3) {
      if (bRTC) {
        RTC.setTime((uint8_t) hour, (uint8_t) minute, (uint8_t) second);
      } 
    } 
    if (bRTC) {
      year = RTC.getYear();
      mon =  RTC.getMonth();
      day =  RTC.getDay();
    } else {
      year = mytm.tm_year + 1900; 
      mon =  mytm.tm_mon + 1;
      day =  mytm.tm_mday;
    }
    tiV.tv_sec = dateTime2Unix( year, mon, day, hour, minute, second); 
    tiV.tv_usec = 0;
    settimeofday(&tiV, nullptr);
  } else {
    Serial.print(" : ");
    if (bRTC) {
      Serial.print("RTC: ");
      sprintf(sLine,"%2.2d:%2.2d:%2.2d", RTC.getHours(), RTC.getMinutes(), RTC.getSeconds());
      Serial.print(sLine);
    }
    Serial.print(" CPU: ");
    time(&tiUx);
    strftime(sLine, sizeof(sLine), "%0H:%0M:%0S", localtime(&tiUx));
    Serial.print(sLine);
  }
  return( eTIME );
}  /* end of fnc_TIME */
 
/**************************************************/
/*! \brief fnc_TYPE                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_TYPE(const char* szCmdLn)
{
   /* place your code here */
  char sLine[ILINE]={""};
  char* psL;
  if(strlen(szCmdLn) > 1){
    if (!strcmp(szCmdLn, " /")){
       strcpy(sLine,"/");
    } else
    if (strstr(szCmdLn, " ..")) {
      strcpy(sLine, sPath);
      psL= strrchr(sLine, '/');
      *psL = '\0';
      if (strlen(sLine) <= 1) {
        strcpy(sLine,"/");
      }
    } else
    if (strstr(szCmdLn, " .")) {
      strcpy(sLine, sPath);
      strcat(sLine,"/");
      strcat(sLine, szCmdLn + 2);
    } else {  
      strcpy(sLine, sPath);
      if (strcmp(sLine, "/"))
      {
        strcat(sLine,"/");
      }
      strcat(sLine, szCmdLn + 1);
    }
  } else {
    strcpy(sLine, sPath);
  }
  Serial.print(F(" : "));
  Serial.println(sLine);
  digitalWrite(PIN_LED,1);
  if (SD.begin(SDCRD)) {
    File FH1 = SD.open(sLine, FILE_READ);
    if (FH1) {
      while (FH1.available()) 
      {
        Serial.write(FH1.read());
      }
      Serial.print(F("\r\nFilesize: "));
      Serial.print(FH1.size(), DEC);
      Serial.print(F(" Byte"));
      FH1.close();
    } else {
      Serial.print(sLine);
      Serial.println(F(" not found!"));
    } 
    SD.end();
    digitalWrite(PIN_LED,0);
  }
   return( eTYPE );
}  /* end of fnc_TYPE */
 
/**************************************************/
/*! \brief fnc_VER                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_VER(const char* szCmdLn)
{
   /* place your code here */
  Serial.print(F(" : "));
  Serial.println(BOARD_NAME);
  Serial.print(F("CPU- Frequency: "));
  Serial.print(F_CPU/1000000);
  Serial.println(F(" MHz"));
  Serial.print(F("Pico SDK Version: "));
  Serial.println(PICO_SDK_VERSION_STRING);
  Serial.print(F("Arduino Version:  "));
  Serial.println(ARDUINO_PICO_VERSION_STR);  
  Serial.print(F("Softwarebuild "));
  Serial.print(__DATE__);
  Serial.print(F(" "));
  Serial.print(__TIME__);
  return( eVER );
}  /* end of fnc_VER */
 
/**************************************************/
/*! \brief fnc_VOL                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_VOL(const char* szCmdLn)
{
  /* place your code here */
  Serial.println(F(": "));
  digitalWrite(PIN_LED, 1);
  if (SD.begin(SDCRD)) {
    Serial.print(F("SD-Card type: "));
    switch (SD.type()) {
      case SD_CARD_TYPE_SD1:
        Serial.println(F("SD1"));
        break;
      case SD_CARD_TYPE_SD2:
        Serial.println(F("SD2"));
        break;
      case SD_CARD_TYPE_SDHC:
        Serial.println(F("SDHC"));
        break;
      default:
        Serial.println(F("Unknown"));
    }
    Serial.print(F("SD-card size: "));
    Serial.print(SD.size64()/(1024*1024));
    Serial.println(F(" MByte"));
    Serial.printf("Blocksize  : %d Byte\r\n", SD.blockSize());
    Serial.printf("Blocks     : %d\r\n", SD.totalBlocks());
    Serial.printf("Clustersize: %d Byte\r\n", SD.clusterSize());
    Serial.printf("Cluster    : %d\r\n", SD.totalClusters());
  }
  SD.end();
  digitalWrite(PIN_LED, 0);
  return( eVOL );
}  /* end of fnc_VOL */
 
/**************************************************/
/*! \brief fnc_TokenNotFound
    \param argument string: pointer of char
    \return always 0
    \ingroup token_parser */
/**************************************************/
int fnc_TokenNotFound(const char* szCmdLn)
{
   /* place your code here */
   Serial.print(F(" <?>"));
   return( eNoToken );
}  /* end of fnc_TokenNotFound */
 
 
/**************************************************/
/*! \brief parser main function
     fnSDOS_Parser                                  
    \param szCmdLn is a pointer of char Cmd-Line
    \return int value of token; 0: no token found
    \ingroup token_parser
    \sa datetime_t */
/**************************************************/
int fnSDOS_Parser(char *szCmdLn)
{
   int iCmdLn;
   int iCmdPos;
   int iRet;
 
   iCmdPos= strcspn(szCmdLn," ");
   if (iCmdPos <= 0) iCmdPos= strlen(szCmdLn);
 
   iCmdLn= strncmp( szCmdLn, "FORMAT", (iCmdPos>(const size_t)strlen("FORMAT")? iCmdPos: (const size_t)strlen("FORMAT")));
   if (iCmdLn < 0) // is less than FORMAT
   {
      iCmdLn= strncmp( szCmdLn, "COPY", (iCmdPos>(const size_t)strlen("COPY")? iCmdPos: (const size_t)strlen("COPY")));
      if (iCmdLn < 0) // is less than COPY
      {
         iCmdLn= strncmp( szCmdLn, "CD", (iCmdPos>(const size_t)strlen("CD")? iCmdPos: (const size_t)strlen("CD")));
         if (iCmdLn < 0) // is less than CD
         {
            if (strncmp( szCmdLn, "AUTO", (iCmdPos>(const size_t)strlen("AUTO")? iCmdPos: (const size_t)strlen("AUTO")))== 0)
            {
               iRet= fnc_AUTO(szCmdLn+((const size_t) strlen("AUTO")));
            } else { //unknown token)
               iRet= fnc_TokenNotFound(szCmdLn);
            } // End of(1:AUTO)
         } else {
            if (iCmdLn > 0) // is higher than CD
            {
               if (strncmp( szCmdLn, "CLS", (iCmdPos>(const size_t)strlen("CLS")? iCmdPos: (const size_t)strlen("CLS")))== 0)
               {
                  iRet= fnc_CLS(szCmdLn+((const size_t) strlen("CLS")));
               } else { // not CLS
                  if (strncmp( szCmdLn, "CONFIG", (iCmdPos>(const size_t)strlen("CONFIG")? iCmdPos: (const size_t)strlen("CONFIG")))== 0)
                  {
                     iRet= fnc_CONFIG(szCmdLn+((const size_t) strlen("CONFIG")));
                  } else { //unknown token
                     iRet= fnc_TokenNotFound(szCmdLn);
                  } // End of(4:CONFIG)
               } // End of(3:CLS)
            } else {
               if (iCmdLn == 0) // Token CD found
               {
                  iRet= fnc_CD(szCmdLn+((const size_t) strlen("CD")));
               } // End of(2:CD)
            }
         }
      } else {
         if (iCmdLn > 0) // is higher than COPY
         {
            iCmdLn= strncmp( szCmdLn, "DEL", (iCmdPos>(const size_t)strlen("DEL")? iCmdPos: (const size_t)strlen("DEL")));
            if (iCmdLn < 0) // is less than DEL
            {
               if (strncmp( szCmdLn, "DATE", (iCmdPos>(const size_t)strlen("DATE")? iCmdPos: (const size_t)strlen("DATE")))== 0)
               {
                  iRet= fnc_DATE(szCmdLn+((const size_t) strlen("DATE")));
               } else { //unknown token)
                  iRet= fnc_TokenNotFound(szCmdLn);
               } // End of(6:DATE)
            } else {
               if (iCmdLn > 0) // is higher than DEL
               {
                  if (strncmp( szCmdLn, "DIR", (iCmdPos>(const size_t)strlen("DIR")? iCmdPos: (const size_t)strlen("DIR")))== 0)
                  {
                     iRet= fnc_DIR(szCmdLn+((const size_t) strlen("DIR")));
                  } else { // not DIR
                     if (strncmp( szCmdLn, "ECHO", (iCmdPos>(const size_t)strlen("ECHO")? iCmdPos: (const size_t)strlen("ECHO")))== 0)
                     {
                        iRet= fnc_ECHO(szCmdLn+((const size_t) strlen("ECHO")));
                     } else { //unknown token
                        iRet= fnc_TokenNotFound(szCmdLn);
                     } // End of(9:ECHO)
                  } // End of(8:DIR)
               } else {
                  if (iCmdLn == 0) // Token DEL found
                  {
                     iRet= fnc_DEL(szCmdLn+((const size_t) strlen("DEL")));
                  } // End of(7:DEL)
               }
            }
         } else {
            if (iCmdLn == 0) // Token COPY found
            {
               iRet= fnc_COPY(szCmdLn+((const size_t) strlen("COPY")));
            } // End of(5:COPY)
         }
      }
   } else {
      if (iCmdLn > 0) // is higher than FORMAT
      {
         iCmdLn= strncmp( szCmdLn, "REN", (iCmdPos>(const size_t)strlen("REN")? iCmdPos: (const size_t)strlen("REN")));
         if (iCmdLn < 0) // is less than REN
         {
            iCmdLn= strncmp( szCmdLn, "MD", (iCmdPos>(const size_t)strlen("MD")? iCmdPos: (const size_t)strlen("MD")));
            if (iCmdLn < 0) // is less than MD
            {
               if (strncmp( szCmdLn, "HELP", (iCmdPos>(const size_t)strlen("HELP")? iCmdPos: (const size_t)strlen("HELP")))== 0)
               {
                  iRet= fnc_HELP(szCmdLn+((const size_t) strlen("HELP")));
               } else { //unknown token)
                  iRet= fnc_TokenNotFound(szCmdLn);
               } // End of(11:HELP)
            } else {
               if (iCmdLn > 0) // is higher than MD
               {
                  if (strncmp( szCmdLn, "PATH", (iCmdPos>(const size_t)strlen("PATH")? iCmdPos: (const size_t)strlen("PATH")))== 0)
                  {
                     iRet= fnc_PATH(szCmdLn+((const size_t) strlen("PATH")));
                  } else { // not PATH
                     if (strncmp( szCmdLn, "RD", (iCmdPos>(const size_t)strlen("RD")? iCmdPos: (const size_t)strlen("RD")))== 0)
                     {
                        iRet= fnc_RD(szCmdLn+((const size_t) strlen("RD")));
                     } else { //unknown token
                        iRet= fnc_TokenNotFound(szCmdLn);
                     } // End of(14:RD)
                  } // End of(13:PATH)
               } else {
                  if (iCmdLn == 0) // Token MD found
                  {
                     iRet= fnc_MD(szCmdLn+((const size_t) strlen("MD")));
                  } // End of(12:MD)
               }
            }
         } else {
            if (iCmdLn > 0) // is higher than REN
            {
               iCmdLn= strncmp( szCmdLn, "TYPE", (iCmdPos>(const size_t)strlen("TYPE")? iCmdPos: (const size_t)strlen("TYPE")));
               if (iCmdLn < 0) // is less than TYPE
               {
                  if (strncmp( szCmdLn, "TEMP", (iCmdPos>(const size_t)strlen("TEMP")? iCmdPos: (const size_t)strlen("TEMP")))== 0)
                  {
                     iRet= fnc_TEMP(szCmdLn+((const size_t) strlen("TEMP")));
                  } else { // not TEMP
                     if (strncmp( szCmdLn, "TIME", (iCmdPos>(const size_t)strlen("TIME")? iCmdPos: (const size_t)strlen("TIME")))== 0)
                     {
                        iRet= fnc_TIME(szCmdLn+((const size_t) strlen("TIME")));
                     } else { //unknown token
                        iRet= fnc_TokenNotFound(szCmdLn);
                     } // End of(17:TIME)
                  } // End of(16:TEMP)
               } else {
                  if (iCmdLn > 0) // is higher than TYPE
                  {
                     if (strncmp( szCmdLn, "VER", (iCmdPos>(const size_t)strlen("VER")? iCmdPos: (const size_t)strlen("VER")))== 0)
                     {
                        iRet= fnc_VER(szCmdLn+((const size_t) strlen("VER")));
                     } else { // not VER
                        if (strncmp( szCmdLn, "VOL", (iCmdPos>(const size_t)strlen("VOL")? iCmdPos: (const size_t)strlen("VOL")))== 0)
                        {
                           iRet= fnc_VOL(szCmdLn+((const size_t) strlen("VOL")));
                        } else { //unknown token
                           iRet= fnc_TokenNotFound(szCmdLn);
                        } // End of(20:VOL)
                     } // End of(19:VER)
                  } else {
                     if (iCmdLn == 0) // Token TYPE found
                     {
                        iRet= fnc_TYPE(szCmdLn+((const size_t) strlen("TYPE")));
                     } // End of(18:TYPE)
                  }
               }
            } else {
               if (iCmdLn == 0) // Token REN found
               {
                  iRet= fnc_REN(szCmdLn+((const size_t) strlen("REN")));
               } // End of(15:REN)
            }
         }
      } else {
         if (iCmdLn == 0) // Token FORMAT found
         {
            iRet= fnc_FORMAT(szCmdLn+((const size_t) strlen("FORMAT")));
         } // End of(10:FORMAT)
      }
   }
   return(iRet);
} /* end of function fnSDOS_Parser */ 