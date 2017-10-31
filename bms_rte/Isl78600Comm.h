#ifndef __ISL78600_COMM_H__
#define __ISL78600_COMM_H__



#include "Isl78600.h"
#include "Async_Event.h"
#include "Async_Looper.h"
#include "InnerTp.h"

Async_LooperType *Isl78600Comm_Init(uint8 pri, boolean istestmode);
void Isl78600Comm_HandlerAllInOne(const InnerTp_RequestHandlerType* requestHandler, const PduInfoType *InfoPtr);


extern const Dio_ChannelType Isl78600_DaisyChainPowerEnableCtrl;
extern const boolean Isl78600_IsHighWhenDaisyChainPowerEnableCtrlOn;
extern const Dio_ChannelType Isl78600_PowerEnableCtrl;
extern const boolean Isl78600_IsHighWhenPowerEnableCtrlOn;


#endif
