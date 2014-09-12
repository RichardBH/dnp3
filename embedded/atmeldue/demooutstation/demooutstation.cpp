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
//using namespace arduino;

void setup();
void loop();
//#line 18
void ToggleBinaryEvery3Seconds(IExecutor* pExecutor, Database* pDatabase, uint8_t index = 0, bool value = true);
extern ExecutorImpl* gpExecutor;


LogRoot root(nullptr, "root", 0);
ExecutorImpl exe;
TransportStack stack(root, &exe, nullptr, LinkConfig(false, false));
LinkParserImpl parser(root, exe, stack.link);
// 5 static binaries, 0 others
StaticallyAllocatedDatabase<5, 0, 0, 0, 0, 0, 0> staticBuffers;
// allow a max of 5 events
StaticallyAllocatedEventBuffer<5> eventBuffers;
		
		
Database database(staticBuffers.GetFacade());

// Object that handles command (CROB / analog output) requests
// This example can toggle an LED
CommandHandlerImpl commandHandler;
OutstationConfig config;
Outstation outstation(config, exe, root, stack.transport, commandHandler, DefaultOutstationApplication::Instance(), database, eventBuffers.GetFacade());

/*
 * Cortex-M3 Systick IT handler
 */
extern void SysTick_Handler( void )
{
	// Increment tick count each ms
	TimeTick_Increment() ;
	

}

// the setup routine runs once when you press reset:
void setup() {
	

	// initialize serial communication at 9600 bits per second:
	//Serial.begin(9600);
	
		
		
		config.eventBufferConfig = EventBufferConfig(5);
		config.params.allowUnsolicited = true;
		config.defaultEventResponses.binary = EventBinaryResponse::Group2Var2;
		
		
		stack.transport.SetAppLayer(&outstation);
		
		stack.link.SetRouter(&parser);
		stack.link.OnLowerLayerUp();
		
		// enable timer interrupts at 100Hz
		exe.Init();
		
		// enable USART RX/TX interrupts
		parser.Init();
		
		//Set LED as output.
		pinMode(13, OUTPUT);

		ToggleBinaryEvery3Seconds(&exe, &database);
		

}
	
void loop()
{
//	digitalWrite(13, LOW);
//	delay(50);
//	digitalWrite(13, HIGH);
//	delay(50);
// read the input on analog pin 0:
//	volatile int sensorValue = analogRead(A0);
// print out the value you read:
//	Serial.println(sensorValue);

			// process any bytes that were received on the interrupt
			parser.ProcessRx();

			// run all pending events or expired timers
			exe.Run();
			
			// sleep until an interrupt occurs
			exe.Sleep();

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