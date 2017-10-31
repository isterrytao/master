#ifndef NTC_H_
#define NTC_H_

#include "Std_Types.h"

typedef struct {
    uint16 dataId;
    sint8 offset;
    uint8 length;
    const uint16 *tabV4096;
} NTC_ConfigDataType;

extern const NTC_ConfigDataType *const NTCConfigData;
extern const NTC_ConfigDataType *const ChgSckNTCConfigData;

uint8 NTC_FromRef4096(uint16 vol);
uint8 NTC_ChgSckTemperatureFromTAB(uint16 vol);

#endif
