/*
 * Quadraped Robot
 * v1.0
 * Ciaran Farley
 */

 #include <SPI.h>
 #include "Servoshock2_Arduino/servoshock_PS4.h"

 //servoshock setup for use as slave to arduino
 const int slaveSelect = 10;
 Servoshock Servoshock1(slaveSelect);

 void setup() {
    //initialize SPI:
    digitalWrite(slaveSelect, HIGH);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    SPI.setBitOrder(MSBFIRST);
    SPI.begin();
  }

  
