
#include "Stdafx.h"
#include "CommandProcessorAdapter.h"
#include "Conversions.h"
#include "Lock.h"

using namespace System::Threading;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			CommandProcessorAdapter::CommandProcessorAdapter(opendnp3::ICommandProcessor* apProxy) : mpProxy(apProxy)
			{

			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::SelectAndOperate(ControlRelayOutputBlock^ command, System::UInt32 index)
			{
				return this->SelectAndOperateT(command, index);
			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::SelectAndOperate(AnalogOutputInt32^ command, System::UInt32 index)
			{
				return this->SelectAndOperateT(command, index);
			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::SelectAndOperate(AnalogOutputInt16^ command, System::UInt32 index)
			{
				return this->SelectAndOperateT(command, index);
			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::SelectAndOperate(AnalogOutputFloat32^ command, System::UInt32 index)
			{
				return this->SelectAndOperateT(command, index);
			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::SelectAndOperate(AnalogOutputDouble64^ command, System::UInt32 index)
			{
				return this->SelectAndOperateT(command, index);
			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::DirectOperate(ControlRelayOutputBlock^ command, System::UInt32 index)
			{
				return this->DirectOperateT(command, index);
			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::DirectOperate(AnalogOutputInt32^ command, System::UInt32 index)
			{
				return this->DirectOperateT(command, index);
			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::DirectOperate(AnalogOutputInt16^ command, System::UInt32 index)
			{
				return this->DirectOperateT(command, index);
			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::DirectOperate(AnalogOutputFloat32^ command, System::UInt32 index)
			{
				return this->DirectOperateT(command, index);
			}

			IFuture<CommandResponse>^ CommandProcessorAdapter::DirectOperate(AnalogOutputDouble64^ command, System::UInt32 index)
			{
				return this->DirectOperateT(command, index);
			}

		}
	}
}

