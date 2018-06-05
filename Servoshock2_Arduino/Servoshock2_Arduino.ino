/*
 This example reads the Dualshock 4 controller state from the Servoshock controller and outputs it to the serial console.

Pressing the right-side joystick button will put make the Servoshock take control of the digital and servo outputs.
Pressing it again will return control to the PS4 controller.

 It will also set the controller LEDs to a changing rainbow color and the controller will rumble when the triggers are pulled.

 Set the serial baud rate to 115200 bps.


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

//Uncomment below to select printout
//#define PRINT_INPUT_PACKET
#define PRINT_BUTTONS
//#define PRINT_IO_STATE
#define PRINT_ANALOG
#define PRINT_TOUCHPAD
#define PRINT_SERVO_US
#define PRINT_SENSORS
#define PRINT_BATT

// include the SPI library:
#include <SPI.h>
#include "servoshock_PS4.h"
#include "print_serial.h"

// set the slave select pin for the Servoshock.  Set jumper JP2 on the Shield to D10 if using digital output 10.
const int slaveSelect = 10;
Servoshock Servoshock1(slaveSelect);  //create instance of Servoshock

unsigned char LEDRed = 255;
unsigned char LEDGreen = 0;
unsigned char LEDBlue = 0;
unsigned int LEDCounter = 0;
unsigned int servo_uS = 1000;
unsigned char servoDirection = 1;
bool rStickLast = 0;
char overrideFlag = 0;

void setup() {
	//initialize SPI:
	digitalWrite(slaveSelect, HIGH);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV16);
	SPI.setBitOrder(MSBFIRST);
	SPI.begin();
	Serial.begin(115200);  //initialize serial monitor
}

void loop() {
	Servoshock1.Update();

#ifdef PRINT_INPUT_PACKET
	for (i=0; i<INPUT_PACKET_SIZE; i++)
	{
		SerialWriteHex (Servoshock1.inPacket.array[i]);
		Serial.print(' ');
	}
#endif

	//print buttons
#ifdef PRINT_BUTTONS
	Serial.print(Servoshock1.inPacket.share);
	Serial.print(Servoshock1.inPacket.lStickPress);
	Serial.print(Servoshock1.inPacket.rStickPress);
	Serial.print(Servoshock1.inPacket.options);
	Serial.print(Servoshock1.inPacket.dPadUp);
	Serial.print(Servoshock1.inPacket.dPadRight);
	Serial.print(Servoshock1.inPacket.dPadDown);
	Serial.print(Servoshock1.inPacket.dPadLeft);
	Serial.print(Servoshock1.inPacket.lTriggerDigital);
	Serial.print(Servoshock1.inPacket.rTriggerDigital);
	Serial.print(Servoshock1.inPacket.lBumper);
	Serial.print(Servoshock1.inPacket.rBumper);
	Serial.print(Servoshock1.inPacket.triangle);
	Serial.print(Servoshock1.inPacket.circle);
	Serial.print(Servoshock1.inPacket.cross);
	Serial.print(Servoshock1.inPacket.square);
	Serial.print(Servoshock1.inPacket.psButton);
	Serial.print(Servoshock1.inPacket.tpadPress);
	Serial.print('\t');
#endif

#ifdef PRINT_IO_STATE
	Serial.print(Servoshock1.inPacket.shareState);
	Serial.print(Servoshock1.inPacket.lStickPressState);
	Serial.print(Servoshock1.inPacket.rStickPressState);
	Serial.print(Servoshock1.inPacket.optionsState);
	Serial.print(Servoshock1.inPacket.dPadUpState);
	Serial.print(Servoshock1.inPacket.dPadRightState);
	Serial.print(Servoshock1.inPacket.dPadDownState);
	Serial.print(Servoshock1.inPacket.dPadLeftState);
	Serial.print(Servoshock1.inPacket.lTriggerDigitalState);
	Serial.print(Servoshock1.inPacket.rTriggerDigitalState);
	Serial.print(Servoshock1.inPacket.lBumperState);
	Serial.print(Servoshock1.inPacket.rBumperState);
	Serial.print(Servoshock1.inPacket.triangleState);
	Serial.print(Servoshock1.inPacket.circleState);
	Serial.print(Servoshock1.inPacket.crossState);
	Serial.print(Servoshock1.inPacket.squareState);
	Serial.print(Servoshock1.inPacket.psButtonState);
	Serial.print(Servoshock1.inPacket.tpadPressState);
	Serial.print('\t');
#endif

#ifdef PRINT_ANALOG
	//print analogs
	SerialWriteDec(Servoshock1.inPacket.lStickX);
	Serial.print(' ');
	SerialWriteDec(Servoshock1.inPacket.lStickY);
	Serial.print(' ');
	SerialWriteDec(Servoshock1.inPacket.rStickX);
	Serial.print(' ');
	SerialWriteDec(Servoshock1.inPacket.rStickY);
	Serial.print(' ');
	SerialWriteDec(Servoshock1.inPacket.lTriggerAnalog);
	Serial.print(' ');
	SerialWriteDec(Servoshock1.inPacket.rTriggerAnalog);
	Serial.print('\t');
#endif

#ifdef PRINT_TOUCHPAD
	//print touchpad
	Serial.print("T1:");
	Serial.print(' ');
	SerialWriteDec(Servoshock1.inPacket.tpad[0].finger[0].touchID);
	Serial.print(' ');
	Serial.print(Servoshock1.inPacket.tpad[0].finger[0].noFinger);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.tpad[0].finger[0].x);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.tpad[0].finger[0].y);
	Serial.print('\t');

	Serial.print("T2:");
	Serial.print(' ');
	SerialWriteDec(Servoshock1.inPacket.tpad[0].finger[1].touchID);
	Serial.print(' ');
	Serial.print(Servoshock1.inPacket.tpad[0].finger[1].noFinger);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.tpad[0].finger[1].x);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.tpad[0].finger[1].y);
	Serial.print('\t');
#endif

#ifdef PRINT_SERVO_US
	SerialWriteIntDec(Servoshock1.inPacket.lStickXState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.lStickYState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.rStickXState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.rStickYState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.lTriggerAnalogState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.rTriggerAnalogState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.lTpadXState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.lTpadYState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.rTpadXState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.rTpadYState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.tiltXState_uS);
	Serial.print(' ');
	SerialWriteIntDec(Servoshock1.inPacket.tiltYState_uS);
	Serial.print('\t');
#endif

#ifdef PRINT_SENSORS
	Serial.print(Servoshock1.inPacket.accelX);
	Serial.print(' ');
	Serial.print(Servoshock1.inPacket.accelY);
	Serial.print(' ');
	Serial.print(Servoshock1.inPacket.accelZ);
	Serial.print(' ');
	Serial.print(Servoshock1.inPacket.gyroX);
	Serial.print(' ');
	Serial.print(Servoshock1.inPacket.gyroY);
	Serial.print(' ');
	Serial.print(Servoshock1.inPacket.gyroZ);
	Serial.print('\t');
#endif

#ifdef PRINT_BATT
	Serial.print("Batt: ");
	Serial.print(Servoshock1.inPacket.battery);
	Serial.print('\t');
#endif
	////////////////////////Set Digital Outputs////////////////////////////


	if (overrideFlag == 0  && rStickLast == 0 && Servoshock1.inPacket.rStickPress == 1)
	{
		overrideFlag = 1;
		servo_uS = 1000;
	}
	else if (overrideFlag == 1  && rStickLast == 0 && Servoshock1.inPacket.rStickPress == 1)
	{
		overrideFlag = 0;
	}
	rStickLast = Servoshock1.inPacket.rStickPress;

	Servoshock1.outPacket.overrideDPadUp = overrideFlag;
	Servoshock1.outPacket.overrideDPadRight = overrideFlag;
	Servoshock1.outPacket.overrideDPadDown = overrideFlag;
	Servoshock1.outPacket.overrideDPadLeft = overrideFlag;
	Servoshock1.outPacket.overrideTriangle = overrideFlag;
	Servoshock1.outPacket.overrideCircle = overrideFlag;
	Servoshock1.outPacket.overrideCross = overrideFlag;
	Servoshock1.outPacket.overrideSquare = overrideFlag;
	Servoshock1.outPacket.overrideLBumper = overrideFlag;
	Servoshock1.outPacket.overrideRBumper = overrideFlag;
	Servoshock1.outPacket.overrideLTriggerDigital = overrideFlag;
	Servoshock1.outPacket.overrideRTriggerDigital = overrideFlag;
	Servoshock1.outPacket.overrideLStickPress = overrideFlag;
	Servoshock1.outPacket.overrideRStickPress = overrideFlag;
	Servoshock1.outPacket.overrideShare = overrideFlag;
	Servoshock1.outPacket.overrideOptions = overrideFlag;
	Servoshock1.outPacket.overrideTpadPress = overrideFlag;
	Servoshock1.outPacket.overridePsButton = overrideFlag;

	Servoshock1.outPacket.overrideLStickX = overrideFlag;
	Servoshock1.outPacket.overrideLStickY = overrideFlag;
	Servoshock1.outPacket.overrideRStickX = overrideFlag;
	Servoshock1.outPacket.overrideRStickY = overrideFlag;
	Servoshock1.outPacket.overrideLTriggerAnalog = overrideFlag;
	Servoshock1.outPacket.overrideRTriggerAnalog = overrideFlag;
	Servoshock1.outPacket.overrideLTpadX = overrideFlag;
	Servoshock1.outPacket.overrideLTpadY = overrideFlag;
	Servoshock1.outPacket.overrideRTpadX = overrideFlag;
	Servoshock1.outPacket.overrideRTpadY = overrideFlag;
	Servoshock1.outPacket.overrideTiltX = overrideFlag;
	Servoshock1.outPacket.overrideTiltY = overrideFlag;


	Servoshock1.outPacket.share = servo_uS/1050;
	Servoshock1.outPacket.lStickPress = servo_uS/1100;
	Servoshock1.outPacket.rStickPress = servo_uS/1150;
	Servoshock1.outPacket.options =  servo_uS/1200;
	Servoshock1.outPacket.dPadUp =  servo_uS/1250;
	Servoshock1.outPacket.dPadRight = servo_uS/1300;
	Servoshock1.outPacket.dPadDown = servo_uS/1350;
	Servoshock1.outPacket.dPadLeft = servo_uS/1400;;
	Servoshock1.outPacket.lTriggerDigital = servo_uS/1450;
	Servoshock1.outPacket.rTriggerDigital = servo_uS/1500;
	Servoshock1.outPacket.lBumper = servo_uS/1550;
	Servoshock1.outPacket.rBumper = servo_uS/1600;
	Servoshock1.outPacket.triangle = servo_uS/1650;
	Servoshock1.outPacket.circle = servo_uS/1700;
	Servoshock1.outPacket.cross = servo_uS/1750;
	Servoshock1.outPacket.square = servo_uS/1800;
	Servoshock1.outPacket.psButton = servo_uS/1850;
	Servoshock1.outPacket.tpadPress = servo_uS/1900;


	////////////////////////Set Servo Outputs////////////////////////////

	Servoshock1.outPacket.lStickX_uS = servo_uS;
	Servoshock1.outPacket.lStickY_uS  = servo_uS;
	Servoshock1.outPacket.rStickX_uS  = servo_uS;
	Servoshock1.outPacket.rStickY_uS  = servo_uS;
	Servoshock1.outPacket.lTriggerAnalog_uS  = servo_uS;
	Servoshock1.outPacket.rTriggerAnalog_uS  = servo_uS;
	Servoshock1.outPacket.lTpadX_uS  = servo_uS;
	Servoshock1.outPacket.lTpadY_uS  = servo_uS;
	Servoshock1.outPacket.rTpadX_uS  = servo_uS;
	Servoshock1.outPacket.rTpadY_uS  = servo_uS;
	Servoshock1.outPacket.tiltX_uS  = servo_uS;
	Servoshock1.outPacket.tiltY_uS  = servo_uS;

//Counter to move servos
	if (servo_uS < 1000) {
		servoDirection = 1;
	} else if (servo_uS > 2000) {
		servoDirection = 0;
	}

	if (servoDirection == 1) {
		servo_uS = servo_uS + 10;
	} else {
		servo_uS = servo_uS - 10;
	}

////////////////////////Set LED////////////////////////////
	Servoshock1.outPacket.overrideLED = 1;
	Servoshock1.SetLED(LEDRed, LEDGreen, LEDBlue, 0, 0);
	if (LEDCounter < 255) {
		LEDRed--;
		LEDGreen++;
	} else if (LEDCounter < 510) {
		LEDGreen--;
		LEDBlue++;
	} else if (LEDCounter < 765) {
		LEDBlue--;
		LEDRed++;
	}
	if (LEDCounter < 768) {
		LEDCounter++;
	} else {
		LEDCounter = 0;
	}

	////////////////////////Set Rumble////////////////////////////
	Servoshock1.outPacket.overrideRumbleH = 1;
	Servoshock1.outPacket.overrideRumbleL = 1;

	//pulling triggers makes the controller rumble
	Servoshock1.outPacket.rumbleH = Servoshock1.inPacket.rTriggerAnalog;
	Servoshock1.outPacket.rumbleL = Servoshock1.inPacket.lTriggerAnalog;
	;

	//Servoshock polls the controller at 100Hz max; go do something else in the meantime.

	Serial.print("\n\r");
	delay(20);

}

