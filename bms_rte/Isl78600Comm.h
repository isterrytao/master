#ifndef __ISL78600_COMM_H__
#define __ISL78600_COMM_H__


#include "App_Types.h"
#include "Isl78600.h"
#include "Async_Event.h"
#include "Async_Looper.h"
#include "InnerTp.h"

#define ISL78600SN_LEN 4U
extern uint8 isl78600SN[ISL78600SN_LEN] ;

Async_LooperType *Isl78600Comm_Init(uint8 pri, boolean istestmode);
void Isl78600Comm_HandlerAllInOne(const InnerTp_RequestHandlerType* requestHandler, const PduInfoType *InfoPtr);
App_VoltageType* Isl78600Comm_GetVoltageBuffPtr(void);
App_TemperatureType* Isl78600Comm_GetTemperatureBuffPtr(void);
void Isl78600Comm_SetBalanceFlag(const uint8 *balance, uint8 length);
uint8* Isl78600Comm_GetBalanceBuffPtr(void);


extern const Dio_ChannelType Isl78600_DaisyChainPowerEnableCtrl;
extern const boolean Isl78600_IsHighWhenDaisyChainPowerEnableCtrlOn;
extern const Dio_ChannelType Isl78600_PowerEnableCtrl;
extern const boolean Isl78600_IsHighWhenPowerEnableCtrlOn;


#endif
