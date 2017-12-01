#ifndef BRIDGE_INSU_H
#define BRIDGE_INSU_H

#include "Std_Types.h"
#include "Dio.h"
#include "BridgeInsu_Cfg.h"
#include "Async_Looper.h"
#include "Adc.h"

#define BRIDGEINSU_INVALID_RESISTANCE       0xFFFFU
#define BRIDGEINSU_RESISTANCE_MAX           0xFFFEU

#define BRIDGEINSU_MODE_OFF                 0U
#define BRIDGEINSU_MODE_WITH_MOS            1U
#define BRIDGEINSU_MODE_WITHOUT_MOS         2U

typedef struct {
    uint32 detectInterval;
    uint8 maxWaitSampleTimes;
    Dio_ChannelType posSwitchCtrl;
    Dio_ChannelType negSwitchCtrl;
    boolean isDoHighTurnOnSwitch;
    uint16 totalResistor;
    float (*PGAControl)(uint8 *withMos);
    uint16 diffAbs;
    uint16 diffPercentFactorToTotal;
} BridgeInsu_ConfigType;

void BridgeInsu_Init(Async_LooperType *looper);
void BridgeInsu_Start(void);
void BridgeInsu_Stop(void);
uint16 BridgeInsu_GetPositive(void);
uint16 BridgeInsu_GetNegative(void);
boolean BridgeInsu_IsCalculated(void);
void dump_insu(void);

#endif
