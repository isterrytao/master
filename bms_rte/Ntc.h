#ifndef NTC_H_
#define NTC_H_

#include "Std_Types.h"

typedef struct {
    uint16 dataId;
    sint8 offset;
    uint8 length;
    const uint16 *tabV4096;
} NTC_ConfigDataType;

extern const NTC_ConfigDataType *const CellNTCConfigData;
extern const NTC_ConfigDataType *const ChgSckNTCConfigData;
extern const NTC_ConfigDataType *const OnboardNTCConfigData;

uint8 NTC_CellFromRef4096(uint16 vol);
uint8 NTC_ChgSckTemperatureFromTAB(uint16 vol);
uint8 NTC_OnboardFromRef4096(uint16 vol);

#endif
