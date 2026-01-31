/**************************************************/
/*! \brief uicalcCrc                                 
    \param argument char and  UINT16 CRC 
    \return uint16_t- CRC- value
    \ingroup XModem */
/**************************************************/
uint16_t uicalcCrc(uint8_t c, uint16_t crc)
{
  int count;
  crc = crc ^ (uint16_t) c << 8;
  count = 8;
  do {
    if (crc & 0x8000) {
      crc = crc << 1 ^ 0x1021;
    } else {
      crc = crc << 1;
    }
  } while (--count);
  return crc;
}

