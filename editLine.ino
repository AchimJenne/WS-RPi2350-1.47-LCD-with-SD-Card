/****************************************************************************/
#ifndef EDITLINE
/**************************************************/
/**
  @brief Interactive VT100- Terminal command input 
  @param [in, out] psLine Buffer to read and edit
  @param [in] inChar serial input byte to interpret
*/
bool editLine(char *psLine, char inChar)
{
  static char sBack[ILINE];
  char sTemp[ILINE];
  static unsigned char iIndx= 0;
  static uint8_t iESC= 0, iL2C= 0;
  static bool bEditMode= false;
  if ((inChar != SOH) && (inChar != NAK) && (inChar != ACK)) {
      if (inChar == '\r')       // CR key
      {
        Serial.print(F("\e["));
        Serial.print( strlen(psLine) + strlen(sPath)+2);
        Serial.print(F("G"));   // cursor position End of Line
        iIndx= 0;
        iESC= 0;
        strcpy(&sBack[0], psLine);
        bEditMode= true;
      } else {
        bEditMode= false;
        if (inChar == '\n')   // new line
        {
          Serial.write(inChar);
        } else 
        if ((inChar == '[') && (iESC ==1)) {
          iESC = 2;
        } else 
        if (((inChar == 'A')||(inChar == 'B')) && (iESC ==2)) // cursor up-down
        {
          for (char i=0; i<iIndx; i++) {
            Serial.print(F("\b \b"));
          }
          strcpy(&sTemp[0], psLine);
          strcpy(psLine, &sBack[0]);
          strcpy(&sBack[0], &sTemp[0]);
          Serial.print(F("\e[0G"));
          Serial.print(sPath);
          Serial.print(F(">"));
          Serial.print(psLine);
          iIndx= strlen(psLine);
          iESC= 0;
        } else 
        if ((inChar == 'D') && (iESC ==2)) // key left
        {
          if (iIndx >=1) {
            Serial.print(F("\e[D"));
            iIndx--;
          }
          iESC= 0;
        } else 
        if ((inChar == 'C') && (iESC ==2)) {  // key right
          if (strlen(psLine) > iIndx) {
            Serial.print(F("\e[C"));
            iIndx++;
          }
          iESC= 0;
        } else
        if ((inChar == '1') && (iESC ==2)) {
          iL2C= 1;
          iESC= 3;
        } else
        if ((inChar == '4') && (iESC ==2)) {
          iL2C= 4;
          iESC= 3;
        } else 
        if ((inChar == '5') && (iESC ==2)) {
          iL2C= 5;
          iESC= 3;
        } else 
        if ((inChar == '6') && (iESC ==2)) {
          iL2C= 6;
          iESC= 3;
        } else
        if ((inChar == '~') && (iESC ==3)) {  
          if ((iL2C ==1)){
            Serial.print(F("\e["));
            Serial.print( strlen(sPath)+2);
            Serial.print(F("G"));
            iIndx= strlen(psLine);
            iESC= 0;
          } else
          if ((iL2C ==4)){
            Serial.print(F("\e[0G"));
            Serial.print(sPath);
            Serial.print(F(">"));
            Serial.print(psLine);
            iIndx= strlen(psLine);          
            iESC= 0;
          }
          if ((iL2C ==5)){
            Serial.print(F("\e[25A"));
            //Serial.println(F("PgUp"));
            iESC= 0;
          }
          if ((iL2C ==6)){
            Serial.print(F("\e[25B"));
            //Serial.println(F("PgDown"));
            iESC= 0;
          }          
        } else
        if ((iESC >=2)) { // unknown key 
          Serial.println();
          Serial.print(inChar);
          Serial.print(" ");
          Serial.println(inChar, HEX);
          //iESC= 0;
        } else
        if ((inChar == C_BS)&&(iESC ==0)) { // backpace key 
          if (strlen(psLine) >= 1) {
            Serial.print(F("\b"));
            int iLen= strlen(psLine);
            if (iLen> iIndx) {
              for (int iLoop=iIndx; iLoop <= iLen; iLoop++) {
                *(psLine+iLoop-1) = *(psLine+iLoop);
              }
              iIndx--;
              Serial.print(psLine + iIndx);
              Serial.print(F(" \e["));
              Serial.print(iLen - iIndx);
              Serial.print(F("D"));
            } else {
              Serial.print(F(" \b"));
              iIndx--;
              *(psLine+iIndx) = 0x00;
            }            
          }
        } else 
        if (inChar == C_ESC)    // escape key
        {
          if (iESC >= 1) {
            for (char i=0; i<iIndx; i++) {
               Serial.print(F("\b \b"));
            }
            *psLine= 0;
            iIndx= 0;
            bEditMode = false;
            iESC= 0;
          } else {
            iESC= 1;
          }
        } else           
        if (inChar == 0x7f) {
          int iLen= strlen(psLine);
          if (iLen> iIndx) {           
            for (int iLoop=iIndx+1; iLoop <= iLen; iLoop++){
              *(psLine+iLoop-1) = *(psLine+iLoop);
            }
            Serial.print(psLine + iIndx);
            Serial.print(F(" \e["));
            Serial.print(iLen - iIndx);
            Serial.print(F("D"));
          }          
        } else {
          if (iIndx < ILINE) {
            Serial.write(inChar);
            int iLen= strlen(psLine);
            if (iLen> iIndx) {
              for (int iLoop=iLen; iLoop >= iIndx; iLoop--){
                *(psLine+iLoop+1) = *(psLine+iLoop);
              } /* end for */
              *(psLine+iIndx) = inChar;
              Serial.print(psLine + iIndx + 1);              
              Serial.print(F("\e["));
              Serial.print(iIndx + 3 + strlen(sPath));
              Serial.print(F("G"));
              iIndx++;
            } else {
              *(psLine+iIndx) = inChar;
              iIndx++;
              *(psLine+iIndx) = 0x00;
            }
          }
        }
      }
    } else {
      // do not accept XModem token
#if defined WS147GPIO || defined WS147SPI1 || defined MAKERGPIO || defined MAKERSPI1
      pixel.fill(0x00ff00);
      pixel.show();
#endif      
      delay(50);
      bEditMode= false;

#if defined WS147GPIO || defined WS147SPI1 || defined MAKERGPIO || defined MAKERSPI1
      pixel.fill(0x000000);
      pixel.show();
#endif      
    }
  return (bEditMode);
}
/**************************************************/
/**
  @brief argPathFn - handle actual Path and Filename
  @param [in, out] pointer to arguments and Buffer to read and edit
*/
int argPathFn(const char* szArgLn, char* szPathFn)
{
  char sLine[ILINE+1];
  char *psL= &sLine[0];
  int iRC;

  if (*szArgLn == ' ') {
    szArgLn++;
  }
  if((iRC= strlen(szArgLn)) > 0) {
    if (strncmp(szArgLn, "/", 1) == 0){
      strcpy(psL, szArgLn);
    } else
    if ((*szArgLn == '.')&&(*(szArgLn+1) == '.')) {
      strcpy(psL, sPath);
      *strrchr(psL, '/') = 0x00;  // find last '/' and substitude to EoS
      if (strlen(psL) <= 1) {
        strcpy(psL, "/");
      }
    } else
    if ((*szArgLn == '.')&&(*(szArgLn+1) == '/')) {
      strcpy(psL, sPath);
      strncat(psL, "/", ILINE);
      strncat(psL, szArgLn +2, ILINE);
    } else {  
      strcpy(psL, sPath);
      if (*(psL+strlen(psL)-1) != '/' ) {
        strncat(psL, "/", ILINE);
      }
      strncat(psL, szArgLn, ILINE);    
    }
  } else {
    strcpy(psL, sPath);
    if (*(psL+strlen(psL)-1) != '/' ) {
      strncat(psL, "/", ILINE);
    }
    iRC= 0;
  }
  strcpy(szPathFn, psL);
  return(iRC);
} /* end of function */

/**************************************************/
/*! \brief parser main function
     func_MonParser                                 
    \param szCmdLn is a pointer of char Cmd-Line
    \return int value of token; 0: no token found
    \ingroup token_parser
    \sa datetime_t */
/**************************************************/
int func_MonParser(char *szCmdLn)
{
   int iCmdLn;
   int iCmdPos;
   int iRet;
 
   iCmdPos= strcspn(szCmdLn," ");
   if (iCmdPos <= 0) iCmdPos= strlen(szCmdLn);
 
   iCmdLn= strncmp( szCmdLn, "Jul", (iCmdPos>(const size_t)strlen("Jul")? iCmdPos: (const size_t)strlen("Jul")));
   if (iCmdLn < 0) // is less than Jul
   {
      iCmdLn= strncmp( szCmdLn, "Dec", (iCmdPos>(const size_t)strlen("Dec")? iCmdPos: (const size_t)strlen("Dec")));
      if (iCmdLn < 0) // is less than Dec
      {
         if (strncmp( szCmdLn, "Apr", (iCmdPos>(const size_t)strlen("Apr")? iCmdPos: (const size_t)strlen("Apr")))== 0)
         {
            iRet= 4;
         } else { // not Apr
            if (strncmp( szCmdLn, "Aug", (iCmdPos>(const size_t)strlen("Aug")? iCmdPos: (const size_t)strlen("Aug")))== 0)
            {
               iRet= 8;
            } else { //unknown token
               iRet= 0;
            } // End of(2:Aug)
         } // End of(1:Apr)
      } else {
         if (iCmdLn > 0) // is higher than Dec
         {
            if (strncmp( szCmdLn, "Feb", (iCmdPos>(const size_t)strlen("Feb")? iCmdPos: (const size_t)strlen("Feb")))== 0)
            {
               iRet= 2;
            } else { // not Feb
               if (strncmp( szCmdLn, "Jan", (iCmdPos>(const size_t)strlen("Jan")? iCmdPos: (const size_t)strlen("Jan")))== 0)
               {
                  iRet= 1;
               } else { //unknown token
                  iRet= 0;
               } // End of(5:Jan)
            } // End of(4:Feb)
         } else {
            if (iCmdLn == 0) // Token Dec found
            {
               iRet= 12;
            } // End of(3:Dec)
         }
      }
   } else {
      if (iCmdLn > 0) // is higher than Jul
      {
         iCmdLn= strncmp( szCmdLn, "Mar", (iCmdPos>(const size_t)strlen("Mar")? iCmdPos: (const size_t)strlen("Mar")));
         if (iCmdLn < 0) // is less than Mar
         {
            if (strncmp( szCmdLn, "Jun", (iCmdPos>(const size_t)strlen("Jun")? iCmdPos: (const size_t)strlen("Jun")))== 0)
            {
               iRet= 6;
            } else { // not Jun
               if (strncmp( szCmdLn, "Mai", (iCmdPos>(const size_t)strlen("Mai")? iCmdPos: (const size_t)strlen("Mai")))== 0)
               {
                  iRet= 5;
               } else { //unknown token
                  iRet= 0;
               } // End of(8:Mai)
            } // End of(7:Jun)
         } else {
            if (iCmdLn > 0) // is higher than Mar
            {
               iCmdLn= strncmp( szCmdLn, "Oct", (iCmdPos>(const size_t)strlen("Oct")? iCmdPos: (const size_t)strlen("Oct")));
               if (iCmdLn == 0)
               {
                  iRet= 10;
               } else { // not Oct
                  if (iCmdLn < 0)
                  {
                     iCmdLn= strncmp( szCmdLn, "Nov", (iCmdPos>(const size_t)strlen("Nov")? iCmdPos: (const size_t)strlen("Nov")));
                     if (iCmdLn == 0)
                     {
                        iRet= 11;
                     } else { //unknown token
                        iRet= 0;
                     }
                  } else {
                     if (strncmp( szCmdLn, "Sep", (iCmdPos>(const size_t)strlen("Sep")? iCmdPos: (const size_t)strlen("Sep")))== 0)
                     {
                        iRet= 9;
                     } else { //unknown token
                        iRet= 0;
                     } // End of(12:Sep)
                  } // End of(10:Nov)
               } // End of(11:Oct)
            } else {
               if (iCmdLn == 0) // Token Mar found
               {
                  iRet= 3;
               } // End of(9:Mar)
            }
         }
      } else {
         if (iCmdLn == 0) // Token Jul found
         {
            iRet= 7;
         } // End of(6:Jul)
      }
   }
   return(iRet);
} /* end of function func_MonParser */  
#define EDITLINE
#endif