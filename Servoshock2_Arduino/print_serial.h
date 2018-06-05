#ifndef PRINT_SERIAL_H_
#define PRINT_SERIAL_H_


//Subroutine to print packet data.
void SerialWriteHex(unsigned char byte)
{
  if (byte < 0x10) Serial.print('0');
  Serial.print(byte,HEX);
}

void SerialWriteIntHex(unsigned int uinteger)
{
  if (uinteger < 0x1000) Serial.print('0');
  if (uinteger < 0x0100) Serial.print('0');
  if (uinteger < 0x0010) Serial.print('0');
  Serial.print(uinteger,HEX);
}

void SerialWriteDec(unsigned char byte)
{
  if (byte < 100) Serial.print('0');
  if (byte < 10) Serial.print('0');
  Serial.print(byte,DEC);
}

void SerialWriteIntDec(unsigned int uinteger)
{
  if (uinteger < 10000) Serial.print('0');
  if (uinteger < 1000) Serial.print('0');
  if (uinteger < 100) Serial.print('0');
  if (uinteger < 10) Serial.print('0');
  Serial.print(uinteger,DEC);
}


void SerialWriteBin(unsigned char byte)
{
  unsigned char mask = 0b10000000;
  for (unsigned char i=0; i<8 ; i++)
  {
    if (mask & byte) {
        Serial.print('1');
    }
    else {
        Serial.print('0');
    }
    mask = mask>>1;
  }
}

#endif
