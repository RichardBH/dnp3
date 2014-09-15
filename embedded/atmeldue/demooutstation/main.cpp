/*
 * demooutstation.cpp
 *
 * Created: 8/27/2014 7:55:11 AM
 *  Author: jadamcrain
 */ 
 
#define ARDUINO_MAIN
#include <Arduino.h>
#include <opendnp3/transport/TransportStack.h>
#include <opendnp3/outstation/Outstation.h>
#include <opendnp3/outstation/StaticallyAllocatedDatabase.h>
#include <opendnp3/outstation/StaticallyAllocatedEventBuffer.h>
#include <opendnp3/outstation/IOutstationApplication.h>

#include <openpal/logging/LogRoot.h>

#include "ExecutorImpl.h"
#include "LinkParserImpl.h"
#include "CommandHandlerImpl.h"
#include "Macros.h"

using namespace opendnp3;
using namespace sam3;
using namespace openpal;

void ToggleBinaryEvery3Seconds(IExecutor* pExecutor, Database* pDatabase, uint8_t index = 0, bool value = true);

extern ExecutorImpl* gpExecutor;

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
	static ExecutorImpl exe;
	static LogRoot root(nullptr, "root", 0);

		// Object that handles command (CROB / analog output) requests
		// This example can toggle an LED
	static CommandHandlerImpl commandHandler;

	static TransportStack stack(root, &exe, nullptr, LinkConfig(false, false));
	// 5 static binaries, 0 others
	static StaticallyAllocatedDatabase<5, 0, 0, 0, 0, 0, 0> staticBuffers;
	// allow a max of 5 events
	static StaticallyAllocatedEventBuffer<5> eventBuffers;


	static Database database(staticBuffers.GetFacade());

	static OutstationConfig config;
	config.eventBufferConfig = EventBufferConfig(5);
	config.params.allowUnsolicited = true;
	config.defaultEventResponses.binary = EventBinaryResponse::Group2Var2;


	static Outstation outstation(config, exe, root, stack.transport, commandHandler, DefaultOutstationApplication::Instance(), database, eventBuffers.GetFacade());

	stack.transport.SetAppLayer(&outstation);

	static LinkParserImpl parser(root, exe, stack.link);
	stack.link.SetRouter(&parser);
	stack.link.OnLowerLayerUp();

	// enable timer interrupts at 100Hz
	exe.Init();

	// enable USART RX/TX interrupts
	parser.Init();

	//Set LED as output.
	pinMode(13, OUTPUT);

	ToggleBinaryEvery3Seconds(&exe, &database);

	__enable_irq();

	for (;;)
	{
	
		// process any bytes that were received on the interrupt
		parser.ProcessRx();

		// run all pending events or expired timers
		exe.Run();
	
		// sleep until an interrupt occurs
		exe.Sleep();
	
		
		loop();
		if (serialEventRun) serialEventRun();
		
	}


	return 0;
}


void ToggleBinaryEvery3Seconds(IExecutor* pExecutor, Database* pDatabase, uint8_t index, bool value)
{
	uint16_t next = ((index + 1) % 5);
	
	{
		Transaction tx(pDatabase);
		pDatabase->Update(Binary(value, 0x01, pExecutor->GetTime().milliseconds), index);
	}
	
	auto lambda = [pExecutor, pDatabase, value, next]() { ToggleBinaryEvery3Seconds(pExecutor, pDatabase, next, !value); };
	pExecutor->Start(TimeDuration::Seconds(3), Action0::Bind(lambda));
}

/*
 * Cortex-M3 Systick IT handler
 */
void SysTick_Handler( void )
{
	// Increment tick count each ms
	TimeTick_Increment() ;
	

}

void HardFault_Handler(void)
{
	volatile uint32_t phantomISR = 9999;
	phantomISR = 3;
	while(1) {

	}
}
