#ifndef SERVOSHOCK_H_
#define SERVOSHOCK_H_

#define INPUT_PACKET_SIZE 96
#define OUTPUT_PACKET_SIZE 39

/*
The library enables the Arduino to interact with the Servoshock 2 board for the PS4 controller.
  It can read the controller state, read the output state of the Servoshock board, write the
  Servoshock board outputs, and control the rumble motors and lightbar on the controller.


The basic structure of the program is:

	#include <SPI.h>
	#include "servoshock_PS4.h"

	// set the slave select pin for the Servoshock.  Set jumper JP3 on the Shield to D10 if using digital output 10.
	const int slaveSelect = 10;
	Servoshock Servoshock1(slaveSelect);  //create instance of Servoshock

	void setup()
	{
		//initialize SPI:
		digitalWrite(slaveSelect, HIGH);
		SPI.setDataMode(SPI_MODE0);
		SPI.setClockDivider(SPI_CLOCK_DIV16);
		SPI.setBitOrder(MSBFIRST);
		SPI.begin();
	}

	void loop()
	{
		Servoshock1.Update(); //this updates the input and output SPI packets.

		//Rest of the program, for example:
		if (Servoshock1.inPacket.dPadUp)
		{
			...
		}

		//if you want to control a servo:
		Servoshock1.outPacket.overrideLStickX = 1;
		Servoshock1.outPacket.lStickX_uS = servo_uS; //output this pulse width in microseconds.

		delay(10); //don't update faster than 100Hz.
	}
*/

/************************************************************************************************
 struct TOUCHPAD_RAW_DATA
 *************************************************************************************************
 raw touchpad data from Servoshock
 ************************************************************************************************/
typedef struct __attribute__((packed)) _FINGER {
	unsigned char touchID :7; // Increments every time a finger is touching the touchpad
	unsigned char noFinger :1; // The top bit is cleared if the finger is touching the touchpad
	unsigned int x :12;  //x position
	unsigned int y :12;  //y position
} FINGER_DATA;

typedef struct __attribute__((packed)) _TOUCHPAD_RAW_DATA {
	unsigned char packetCounter;
	FINGER_DATA finger[2]; // 0 = first finger, 1 = second finger
} TOUCHPAD_RAW_DATA;

/************************************************************************************************
 struct SPI_INPUT_PACKET
 *************************************************************************************************
 structure to hold SPI input packet (Servoshock to Arduino)
 ************************************************************************************************/
typedef union __attribute__((packed)) _INPUT_FIELDS //WARNING: if you edit this, you need to change the PS4_REPORT_LENGTH!!!
{
	struct {
		unsigned :8; //old buffer data, discard.  //byte 0
		unsigned :8; //old buffer data, discard.
		unsigned :8; //old buffer data, discard.
		unsigned :8; //old buffer data, discard.
		unsigned :8; //old buffer data, discard.
		unsigned :8; //old buffer data, discard.
		unsigned :8; //old buffer data, discard.
		unsigned :8; //old buffer data, discard.

		//start of packet in both USB and BT modes
		unsigned char reportID; //byte 8

		//Stick analog values
		unsigned char lStickX; //left stick X axis (0-255)
		unsigned char lStickY; //left stick Y axis (0-255)
		unsigned char rStickX; //right stick X axis (0-255)
		unsigned char rStickY; //right stick Y axis (0-255)

		//Button states
		unsigned dPadUp :1;
		unsigned dPadRight :1;
		unsigned dPadDown :1;
		unsigned dPadLeft :1; 
		unsigned square :1;
		unsigned cross :1;
		unsigned circle :1;
		unsigned triangle :1;

		unsigned lBumper :1;
		unsigned rBumper :1;
		unsigned lTriggerDigital :1;
		unsigned rTriggerDigital :1;
		unsigned share :1;
		unsigned options :1;
		unsigned lStickPress :1;
		unsigned rStickPress :1;

		unsigned psButton :1; //byte 15
		unsigned tpadPress :1;
		unsigned counter :6;

		unsigned char lTriggerAnalog; //(0-255)
		unsigned char rTriggerAnalog; //(0-255)
		unsigned char bytes10_12[3];

		//IMU readings
		signed int gyroX;
		signed int gyroY;
		signed int gyroZ;
		signed int accelX;
		signed int accelY;
		signed int accelZ;

		unsigned char bytes25_29[5]; //33-37
		unsigned char battery :4;
		unsigned char USBData :1; //usb plugged in
		unsigned char :3; //
    
		unsigned char bytes31_32[2]; //39-40

		//TOUCHPAD readings
		unsigned char numTpadPackets; //byte 41
		TOUCHPAD_RAW_DATA tpad[3]; //bytes 42-68

		//These members will tell pulse width of the servo signal
		unsigned int lStickXState_uS; //byte 0-1 of spiOutputStatePacket in Servoshock firmware
		unsigned int lStickYState_uS; //2-3
		unsigned int rStickXState_uS; //4-5
		unsigned int rStickYState_uS; //6-7
		unsigned int lTriggerAnalogState_uS; //8-9
		unsigned int rTriggerAnalogState_uS; //10-11
		unsigned int lTpadXState_uS; //12-13
		unsigned int lTpadYState_uS; //14-15
		unsigned int rTpadXState_uS; //16-17
		unsigned int rTpadYState_uS; //18-19
		unsigned int tiltXState_uS; //20-21
		unsigned int tiltYState_uS; //22-23

		//these members tell the state of the I/O output
		unsigned dPadUpState :1; //byte 24
		unsigned dPadRightState :1;
		unsigned dPadDownState :1;
		unsigned dPadLeftState :1;
		unsigned triangleState :1;
		unsigned circleState :1;
		unsigned crossState :1;
		unsigned squareState :1;

		unsigned lBumperState :1; //byte 25
		unsigned rBumperState :1;
		unsigned lTriggerDigitalState :1;
		unsigned rTriggerDigitalState :1;
		unsigned lStickPressState :1;
		unsigned rStickPressState :1;
		unsigned shareState :1;
		unsigned optionsState :1;

		unsigned tpadPressState :1; //byte 26
		unsigned psButtonState :1;
		unsigned :6;

	};
	unsigned char array[INPUT_PACKET_SIZE];
} SPI_INPUT_PACKET; //96 bytes total

/************************************************************************************************
 struct SPI_OUTPUT_PACKET
 *************************************************************************************************
 structure to hold SPI output packet (Arduino to Servoshock)
 Setting the overrideXXXX bits will enable you to set the outputs with the Arduino instead of the controller.
 For example, to override the left stick X axis, use:
	Servoshock1.outPacket.overrideLStickX = 1; //enable override
	Servoshock1.outPacket.lStickX_uS = 1500; //set output pulse width to 1500 microseconds

To give control back to the Dualshock, set the override bit back to 0.

 ************************************************************************************************/
typedef struct __attribute__((packed)) _OUTPUT_FIELDS {

	union {

		struct {
			//override feedback
			unsigned overrideLED :1; //byte 0 bit 0
			unsigned overrideRumbleL :1;
			unsigned overrideRumbleH :1;
			unsigned :5; //bit 7

			unsigned char LEDRed;  //byte 1
			unsigned char LEDGreen;
			unsigned char LEDBlue;
			unsigned char LEDBlinkOnDuration;
			unsigned char LEDBlinkOffDuration;
			unsigned char rumbleL;
			unsigned char rumbleH; //byte 7

			unsigned overrideLStickX :1; //byte 8
			unsigned overrideLStickY :1;
			unsigned overrideRStickX :1;
			unsigned overrideRStickY :1;
			unsigned overrideLTriggerAnalog :1;
			unsigned overrideRTriggerAnalog :1;
			unsigned overrideLTpadX :1;
			unsigned overrideLTpadY :1;

			unsigned overrideRTpadX :1; //byte 9
			unsigned overrideRTpadY :1;
			unsigned overrideTiltX :1;
			unsigned overrideTiltY :1;
			unsigned overrideDPadUp :1;
			unsigned overrideDPadRight :1;
			unsigned overrideDPadDown :1;
			unsigned overrideDPadLeft :1;

			unsigned overrideTriangle :1; //byte 10
			unsigned overrideCircle :1;
			unsigned overrideCross :1;
			unsigned overrideSquare :1;
			unsigned overrideLBumper :1;
			unsigned overrideRBumper :1;
			unsigned overrideLTriggerDigital :1;
			unsigned overrideRTriggerDigital :1;

			unsigned overrideLStickPress :1; //byte 11
			unsigned overrideRStickPress :1;
			unsigned overrideShare :1;
			unsigned overrideOptions :1;
			unsigned overrideTpadPress :1;
			unsigned overridePsButton :1;
			unsigned :2;

			unsigned int lStickX_uS; //byte 12-13
			unsigned int lStickY_uS;
			unsigned int rStickX_uS;
			unsigned int rStickY_uS;
			unsigned int lTriggerAnalog_uS; //byte 20-21
			unsigned int rTriggerAnalog_uS;
			unsigned int lTpadX_uS;
			unsigned int lTpadY_uS;
			unsigned int rTpadX_uS;
			unsigned int rTpadY_uS; //byte 30-31
			unsigned int tiltX_uS;
			unsigned int tiltY_uS; //byte 34-35

			unsigned dPadUp :1; //byte 36
			unsigned dPadRight :1;
			unsigned dPadDown :1;
			unsigned dPadLeft :1;
			unsigned triangle :1;
			unsigned circle :1;
			unsigned cross :1;
			unsigned square :1;

			unsigned lBumper :1; //byte 37
			unsigned rBumper :1;
			unsigned lTriggerDigital :1;
			unsigned rTriggerDigital :1;
			unsigned lStickPress :1;
			unsigned rStickPress :1;
			unsigned share :1;
			unsigned options :1;

			unsigned tpadPress :1; //byte 38
			unsigned psButton :1;
			unsigned :6;
		};
		unsigned char array[OUTPUT_PACKET_SIZE];
	};
} SPI_OUTPUT_PACKET;



/************************************************************************************************
 class Servoshock
 *************************************************************************************************
 Create an instance of Servoshock.

 Input: Slave Select digital I/O pin number

 Usage Example:
 Servoshock servoshock1(10);
 ************************************************************************************************/
class Servoshock {
public:
	Servoshock(int slaveSelectPin);
	void Update(void);
	void SetLED(unsigned char red, unsigned char green, unsigned char blue,
	unsigned char blinkOnDuration, unsigned char blinkOffDuration);


	SPI_OUTPUT_PACKET outPacket = { };
	SPI_INPUT_PACKET inPacket = { };

private:
	int SSPin;
};

Servoshock::Servoshock(int slaveSelectPin) {
	pinMode(slaveSelectPin, OUTPUT);
	SSPin = slaveSelectPin;
}
;

/************************************************************************************************
 function Servoshock::update
 *************************************************************************************************
 Performs the SPI packet transfer to read from and write to the Servoshock module.  Also converts
 the 16-bit parameters (accels and gyro) between little endian (Arduino) to big endian (DualShock)

 Usage Example:
 Servoshock1.Update();
 ************************************************************************************************/
void Servoshock::Update(void) {
	unsigned char dPadDirection;
	digitalWrite(SSPin, LOW);
	for (unsigned int i = 0; i < INPUT_PACKET_SIZE; i++) {
		if (i < OUTPUT_PACKET_SIZE) {
			inPacket.array[i] = SPI.transfer(outPacket.array[i]);
		} else {
			inPacket.array[i] = SPI.transfer(0xCD); //padding bytes
		}
	}
	//dPad bits are actually a 4-bit number indicating direction, but we want button state.
	dPadDirection = inPacket.dPadUp + inPacket.dPadRight*2 + inPacket.dPadDown*4 + inPacket.dPadLeft*8;
	inPacket.dPadUp =  (dPadDirection == 7 || dPadDirection == 0 || dPadDirection == 1);
	inPacket.dPadRight = (dPadDirection == 1 || dPadDirection == 2 || dPadDirection == 3);
	inPacket.dPadDown = (dPadDirection == 3 || dPadDirection == 4 || dPadDirection == 5);
	inPacket.dPadLeft = (dPadDirection == 5 || dPadDirection == 6 || dPadDirection == 7);

	digitalWrite(SSPin, HIGH);
}

/************************************************************************************************
 function Servoshock::SetLED
 *************************************************************************************************
Sets the LEDs.  Takes RGB values 0-255, and blink on and off durations.  Durations are from 0-255, 255 is about 2.5 seconds.
 Usage Example:
Servoshock1.SetLED(255, 255, 255, 0, 0);
 ************************************************************************************************/
void Servoshock::SetLED(unsigned char red, unsigned char green,
		unsigned char blue, unsigned char blinkOnDuration,
		unsigned char blinkOffDuration) {
	outPacket.LEDRed = red;
	outPacket.LEDGreen = green;
	outPacket.LEDBlue = blue;
	outPacket.LEDBlinkOnDuration = blinkOnDuration;
	outPacket.LEDBlinkOffDuration = blinkOffDuration;
}


#endif
