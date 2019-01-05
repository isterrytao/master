#ifndef __PCF8563_H__
#define __PCF8563_H__

#include "Std_Types.h"
#include "Rtc.h"
#include "i2c.h"


#define PCF8563_STATUS_NOT_INIT 0U
#define PCF8563_STATUS_OK    1U
#define PCF8563_STATUS_RESET 2U

typedef struct {
    const struct i2c_bus bus;
    uint8 status;
    uint8 isRamValid;
} PCF8563_RtcChipType;

Std_ReturnType PCF8563_Init(PCF8563_RtcChipType *chip);
Std_ReturnType PCF8563_ReadTime(const PCF8563_RtcChipType *chip, Datetime_SecondType *seconds);
Std_ReturnType PCF8563_WriteTime(const PCF8563_RtcChipType *chip, Datetime_SecondType seconds);
uint8 PCF8563_GetRamSizeBits (PCF8563_RtcChipType *chip);
Std_ReturnType PCF8563_WriteRamData(PCF8563_RtcChipType *chip, const uint8 *dat, uint8 offset, uint8 len);
Std_ReturnType PCF8563_ReadRamData(const PCF8563_RtcChipType *chip, uint8 *dat, uint8 offset, uint8 len);
Std_ReturnType PCF8563_ConfigInterrupt(const PCF8563_RtcChipType *chip, Rtc_InterruptTypeType int_type, Rtc_InterruptSingleType int_type_single);
Std_ReturnType PCF8563_ClearInterrupt(const PCF8563_RtcChipType *chip);
Std_ReturnType PCF8563_SetAlarmTime(const PCF8563_RtcChipType *chip, Datetime_SecondType seconds);
Std_ReturnType PCF8563_GetAlarmTime(const PCF8563_RtcChipType *chip, Datetime_SecondType *seconds);
Std_ReturnType PCF8563_ReadTemperature(const PCF8563_RtcChipType *chip, sint16 *pTemp);

extern const Rtc_OpsType PCF8563RtcOps;

#endif
