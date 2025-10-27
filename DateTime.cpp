/*
 * DateTime.cpp - DateTime Operations
 * Sourcecode from Internet : german wikipedia example
 */
#ifndef UNIX_TIME_CONV
#define UNIX_TIME_CONV
#include <cstdint>
/** Konvertiert gegliederte UTC-Angaben in Unix-Zeit.
 * Parameter und ihre Werte-Bereiche:
 * - jahr [1970..2038]
 * - monat [1..12]
 * - tag [1..31]
 * - stunde [0..23]
 * - minute [0..59]
 * - sekunde [0..59]
 */
uint64_t dateTime2Unix(int jahr, int monat, int tag,
                   int stunde, int minute, int sekunde)
{
  const short tage_seit_jahresanfang[12] = /* Anzahl der Tage seit Jahresanfang ohne Tage des aktuellen Monats und ohne Schalttag */
    {0,31,59,90,120,151,181,212,243,273,304,334};

  int schaltjahre = ((jahr-1)-1968)/4 /* Anzahl der Schaltjahre seit 1970 (ohne das evtl. laufende Schaltjahr) */
                  - ((jahr-1)-1900)/100
                  + ((jahr-1)-1600)/400;

  long long tage_seit_1970 = (jahr-1970)*365 + schaltjahre
                           + tage_seit_jahresanfang[monat-1] + tag-1;

  if ( (monat>2) && (jahr%4==0 && (jahr%100!=0 || jahr%400==0)) )
    tage_seit_1970 += 1; /* +Schalttag, wenn jahr Schaltjahr ist */

  return sekunde + 60 * ( minute + 60 * (stunde + 24*tage_seit_1970) );
}

void unix2DateTime(uint64_t unixtime,
                           int16_t *pJahr, int16_t *pMonat, int16_t *pTag,
                           int16_t *pStunde, int16_t *pMinute, int16_t *pSekunde)
{
    const uint64_t SEKUNDEN_PRO_TAG   =  86400ul; /*  24* 60 * 60 */
    const uint64_t TAGE_IM_GEMEINJAHR =    365ul; /* kein Schaltjahr */
    const uint64_t TAGE_IN_4_JAHREN   =   1461ul; /*   4*365 +   1 */
    const uint64_t TAGE_IN_100_JAHREN =  36524ul; /* 100*365 +  25 - 1 */
    const uint64_t TAGE_IN_400_JAHREN = 146097ul; /* 400*365 + 100 - 4 + 1 */
    const uint64_t TAGN_AD_1970_01_01 = 719468ul; /* Tagnummer bezogen auf den 1. Maerz des Jahres "Null" */

    uint64_t TagN = TAGN_AD_1970_01_01 + unixtime/SEKUNDEN_PRO_TAG;
    uint64_t Sekunden_seit_Mitternacht = unixtime%SEKUNDEN_PRO_TAG;
    uint64_t temp;

    /* Schaltjahrregel des Gregorianischen Kalenders:
       Jedes durch 100 teilbare Jahr ist kein Schaltjahr, es sei denn, es ist durch 400 teilbar. */
    temp = 4 * (TagN + TAGE_IN_100_JAHREN + 1) / TAGE_IN_400_JAHREN - 1;
    *pJahr = 100 * temp;
    TagN -= TAGE_IN_100_JAHREN * temp + temp / 4;
    /* Schaltjahrregel des Julianischen Kalenders:
       Jedes durch 4 teilbare Jahr ist ein Schaltjahr. */
    temp = 4 * (TagN + TAGE_IM_GEMEINJAHR + 1) / TAGE_IN_4_JAHREN - 1;
    *pJahr += temp;
    TagN -= TAGE_IM_GEMEINJAHR * temp + temp / 4;
    /* TagN enthaelt jetzt nur noch die Tage des errechneten Jahres bezogen auf den 1. Maerz. */
    *pMonat = (5 * TagN + 2) / 153;
    *pTag = TagN - (*pMonat * 153 + 2) / 5 + 1;
    /*  153 = 31+30+31+30+31 Tage fuer die 5 Monate von Maerz bis Juli
        153 = 31+30+31+30+31 Tage fuer die 5 Monate von August bis Dezember
              31+28          Tage fuer Januar und Februar (siehe unten)
        +2: Justierung der Rundung
        +1: Der erste Tag im Monat ist 1 (und nicht 0).
    */
    *pMonat += 3; /* vom Jahr, das am 1. Maerz beginnt auf unser normales Jahr umrechnen: */
    if (*pMonat > 12)
    {   /* Monate 13 und 14 entsprechen 1 (Januar) und 2 (Februar) des naechsten Jahres */
        *pMonat -= 12;
        ++*pJahr;
    }
    *pStunde  = Sekunden_seit_Mitternacht / 3600;
    *pMinute  = Sekunden_seit_Mitternacht % 3600 / 60;
    *pSekunde = Sekunden_seit_Mitternacht  % 60;
}
#endif
