
#include "LinkParserImpl.h"

#include "CriticalSection.h"
#include <variant.h>

LinkParserImpl* gLinkParser = nullptr;
	
LinkParserImpl::LinkParserImpl(openpal::LogRoot& root, openpal::IExecutor& exe, opendnp3::ILinkContext& context) : 	
	pExecutor(&exe),
	pContext(&context),
	parser(root.GetLogger())
{
	
}

void LinkParserImpl::Init()
{
	gLinkParser = this;
	//Configure the USART and enable rx/tx interrupts
	Serial.begin(9600);
}

void LinkParserImpl::Receive(uint8_t byte)
{	
	// comes in on the interrupt
	rxBuffer.Put(byte);
}

void LinkParserImpl::CheckTransmit()
{	
	// Wait for empty transmit buffer
	if(txQueue.IsNotEmpty())// && (UCSR0A & (1<<UDRE0))) 
	{		
		auto pTx = txQueue.Peek();
//		UDR0 = pTx->buffer[0];

		//Using arduino Serial port.
		Serial.write(pTx->buffer[0]);
		pTx->buffer.Advance(1);
		
		if(pTx->buffer.IsEmpty())
		{
			txQueue.Pop();
			auto pri = pTx->primary;
			auto callback = [this, pri]() { pContext->OnTransmitResult(pri, true); };
			pExecutor->PostLambda(callback);
		}
	}
		
}

void LinkParserImpl::ProcessRx()
{
	auto num = CopyRxBuffer();

	if(num > 0)
	{
		parser.OnRead(num, pContext);
	}

}

uint32_t LinkParserImpl::CopyRxBuffer()
{	
	// disable interrupts and copy contents of ring buffer to the receiver's write buffer
	CriticalSection cs; 
	
	
	auto buffer = parser.WriteBuff();
	return rxBuffer.Read(buffer);
	
}
	
void LinkParserImpl::QueueTransmit(const openpal::ReadOnlyBuffer& buffer, opendnp3::ILinkContext* pContext, bool primary)
{
	txQueue.Enqueue(Transmission(buffer, primary));
	this->CheckTransmit();
}

//This function is called in the Serial receive interrupt.
//UARTClass.cpp
//	dnp3_receive(_pUart->UART_RHR);
void dnp3_receive(uint8_t rx)
{
	//uint8_t rx;
	//rx = UDR0;
	//rx = Serial.read();
	if(gLinkParser)
	{
		gLinkParser->Receive(rx);
	}
}

//No need.
/* TODO - ARM specific USART rx/tx interrupt handlers
ISR(USART0_TX_vect)
{	
	if(gLinkParser)
	{
		gLinkParser->CheckTransmit();
	}
}

ISR(USART0_RX_vect)
{
	uint8_t rx;
	rx = UDR0;	
	if(gLinkParser)
	{
		gLinkParser->Receive(rx);
	}	
}
*/
