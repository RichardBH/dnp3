/*
 * demooutstation.cpp
 *
 * Created: 8/27/2014 7:55:11 AM
 *  Author: jadamcrain
 */ 
 
#define ARDUINO_MAIN
#include <Arduino.h>


// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }






/*
 * \brief Main entry point of Arduino application
 */
int main( void )
{
	__disable_irq();
	init();
	initVariant();
//	delay(1);

#if defined(USBCON)
	USBDevice.attach();
#endif


	

	
	setup();
	__enable_irq();

	for (;;)
	{
	
		
		loop();
		if (serialEventRun) serialEventRun();
	}

	return 0;
}


