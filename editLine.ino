/****************************************************************************/
#ifndef EDITLINE
/**************************************************/
/**
  @brief Interactive VT100- command input 
  @param [in, out] psLine Buffer to read and edit
  @param [in] inChar serial input byte to interpret
*/
bool editLine(char *psLine, char inChar)
{
  static char sBack[ILINE];
  static char sTemp[ILINE];
  static unsigned char iIndx=0;
  static uint8_t iESC= 0;
  static bool bEditMode= false;
      if (inChar == '\r')       // CR key
      {
        for (unsigned int i=1; i<=(strlen(psLine)-iIndx); i++) {
          Serial.print(F("\e[C"));
        }
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
        if ((inChar == 'C') && (iESC ==2))  // key right
        {
          if (strlen(psLine) > iIndx) {
            Serial.print(F("\e[C"));
            iIndx++;
          }
          iESC= 0;
        } else 
        if (inChar == C_BS)       // backpace key 
        {
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
              Serial.print(iIndx+3+strlen(sPath));
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
    return (bEditMode);
}
#define EDITLINE
#endif