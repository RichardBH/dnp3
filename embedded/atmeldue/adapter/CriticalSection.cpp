
#include "CriticalSection.h"

// Includes Atmel CMSIS
#include "chip.h"


CriticalSection::CriticalSection()
{
	// TODO - disable all interrupts, e.g. on AVR, cli();
	__disable_irq();

}

CriticalSection::~CriticalSection()
{
	// TODO - disable all interrupts, e.g. on AVR, sei();
	__enable_irq();

}
	

