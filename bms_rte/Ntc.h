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
extern const NTC_ConfigDataType *const DCChgSckNTCConfigData;
extern const NTC_ConfigDataType *const ACChgSckNTCConfigData;
extern const NTC_ConfigDataType *const OnboardNTCConfigData;

/**
 * \brief 把温感电压根据温感表转换成温度值.
 *
 * \param cfgData 温感表
 * \param vol 电压
 *
 * \return 温度值
 */
uint8 NTC_FromRef4096(const NTC_ConfigDataType *cfgData, uint16 vol);

/**
 * \brief 把单体温感的电压转换成温度值.
 *
 * \param vol 电压
 *
 * \return 温度值
 */
uint8 NTC_CellFromRef4096(uint16 vol);

/**
 * \brief 把充电插座温感的电压转换成温度值.
 *
 * \param vol 电压
 *
 * \return 温度值
 */
uint8 NTC_ACChgSckTemperatureFromTAB(uint16 vol);
uint8 NTC_DCChgSckTemperatureFromTAB(uint16 vol);

/**
 * \brief 把板载温感的电压转换成温度值.
 *
 * \param vol 电压
 *
 * \return 温度值
 */
uint8 NTC_OnboardFromRef4096(uint16 vol);

#endif
