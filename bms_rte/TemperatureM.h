/**
 * \file TemperatureM.h
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 温度管理头文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初版本, 完成讨论部分的定义. | UD00004 | 20161212 |
 */

/**
 * \addtogroup TemperatureM 温度管理
 * \{
 */

#ifndef TEMPERATUREM_H_
#define TEMPERATUREM_H_

#include "Std_Types.h"
#include "Charge_Types.h"
#include "Async_Looper.h"
#include "Async_Event.h"

extern const uint8 TemperatureM_ThermRunawayStrategy;

#define TEMPERATUREM_ONLY_HEAT_CURRENT_DEF  STD_ON
#define TEMPERATUREM_THERMALRUNAWAY_STRATEGY_1 (1U)
#define TEMPERATUREM_THERMALRUNAWAY_STRATEGY_2 (2U)

/**
 * \brief 加热状态类型定义
 */
typedef enum{
    TEMPERATUREM_HEAT_STATE_NONE = 0, /**< 无加热状态 */
    TEMPERATUREM_ONLY_HEAT_START = 1, /**< 仅加热电流起始 */
    TEMPERATUREM_HEAT_STATE_OLT = TEMPERATUREM_ONLY_HEAT_START, /**< 过低温加热状态 */
    TEMPERATUREM_HEAT_STATE_LT = 2, /**< 低温加热状态 */
    TEMPERATUREM_ONLY_HEAT_STOP = TEMPERATUREM_HEAT_STATE_LT, /**< 仅加热电流结束 */
    TEMPERATUREM_HEAT_STATE_FAULT = 3, /** 加热故障状态 */
}TemperatureM_HeatStateType;

/**
 * \brief 制冷状态类型定义
 */
typedef enum{
    TEMPERATUREM_REFRI_STATE_NONE, /**< 无制冷状态 */
    TEMPERATUREM_REFRI_STATE_ON, /**< 制冷状态 */
    TEMPERATUREM_REFRI_STATE_FAULT, /**< 制冷故障状态 */
}TemperatureM_RefrigerationStateType;

/**
 * \brief 加热配置参数类型
 */
typedef struct{
    uint16 temperatureOn; /**< 开启温度 */
    uint16 deltaTemperatureOn; /**< 开启温差 */
    uint16 temperatureOnDelay; /**< 开启温度延时 10ms/bit */
    uint16 deltaTemperatureOnDelay; /**< 开启温差延时 10ms/bit */
    uint16 temperatureOff; /**< 关闭温度 */
    uint16 deltaTemperatureOff; /**< 关闭温差 */
    uint16 temperatureOffDelay; /**< 关闭温度延时 10ms/bit */
    uint16 deltaTemperatureOffDelay; /**< 关闭温差延时 10ms/bit */
}TemperatureM_HeatConfigType;

/**
 * \brief 制冷配置参数类型
 */
typedef struct{
    uint16 temperatureOn; /**< 开启温度 */
    uint16 deltaTemperatureOn; /**< 开启温差 */
    uint16 temperatureOnDelay; /**< 开启温度延时 10ms/bit */
    uint16 deltaTemperatureOnDelay; /**< 开启温差延时 10ms/bit */
    uint16 temperatureOff; /**< 关闭温度 */
    uint16 deltaTemperatureOff; /**< 关闭温差 */
    uint16 temperatureOffDelay; /**< 关闭温度延时 10ms/bit */
    uint16 deltaTemperatureOffDelay; /**< 关闭温差延时 10ms/bit */
}TemperatureM_RefrigerationConfigType;

/**
 * \brief 温度管理配置参数类型
 */
typedef struct{
    TemperatureM_HeatConfigType chgHeatPara; /**< 充电加热参数 */
    TemperatureM_HeatConfigType dchgHeatPara; /**< 放电加热参数 */
    TemperatureM_RefrigerationConfigType chgRefrigerationPara; /**< 充电制冷参数 */
    TemperatureM_RefrigerationConfigType dchgRefrigerationPara; /**< 放电制冷参数 */
}TemperatureM_ConfigType;

/**
 * \brief 热失控管理配置参数类型
 */
typedef struct{
    uint8 limitTemperature; /**< 触发温度 */
    uint8 deltaTemperature; /**< 触发温差 */
    uint8 temperatureRise; /**< 触发温升 */
    uint16 limitTemperatureDelay; /**< 触发温度延时 10ms/bit */
    uint16 deltaTemperatureDelay; /**< 触发温差延时 10ms/bit */
    uint16 temperatureRiseConditionDelay; /**< 触发温升延时 10ms/bit */
    uint16 temperatureRiseValidDelay; /**< 温升生效时间 10ms/bit */
    uint16 voltageDrop; /**< 单体压降, 单位0.1%/s */
    uint16 voltageDropConditionDelay; /**< 触发压降延时 10ms/bit */
    uint16 voltageDropValidDelay; /**< 压降生效时间 10ms/bit */
}ThermalRunaway_CalibConfigType;

typedef struct{
    uint8 currentRise;
    uint8 maxRise;
    uint8 lastTemp;
    uint32 lastTime;
    boolean riseMark;
    boolean riseCondition;
    uint32 riseConditionTime;
    uint16 riseConditionValue;
    boolean overTempCondition;
    uint32 overTempConditionTime;
    boolean deltaTempCondition;
    uint32 deltaTempConditionTime;
}TemperatureM_ThermalTempDataType;

typedef struct{
    uint16 currentDrop;
    uint16 maxDrop;
    uint16 lastVolt;
    uint32 lastTime;
    boolean dropMark;
    boolean dropCondition;
    uint32 dropConditionTime;
    uint16 dropconditionValue;
}TemperatureM_ThermalVoltDataType;

typedef struct{
    Async_EventType ev_temp_rise; /**< 异步事件 */
    Async_EventType ev_volt_drop; /**< 异步事件 */
    TemperatureM_HeatStateType heatState; /**< 加热状态 */
    TemperatureM_RefrigerationStateType refrigerationState; /**< 制冷状态 */
    ThermalRunaway_CalibConfigType thermalPara;
    uint16 thermalDiagResults;
    TemperatureM_ThermalTempDataType thermalTempData;
    TemperatureM_ThermalVoltDataType thermalVoltData;
}TemperatureM_InnerDataType;

/**
 * \brief 温度管理初始化
 */
void TemperatureM_Init(Async_LooperType *looper);

/**
 * \brief 设置加热状态
 *
 * \param State 加热状态
 */
void TemperatureM_SetHeatState(TemperatureM_HeatStateType State);

/**
 * \brief 获取加热状态
 *
 * \return 获取到的加热状态
 */
TemperatureM_HeatStateType TemperatureM_GetHeatState(void);

/**
 * \brief 设置制冷状态
 *
 * \param State 制冷状态
 */
void TemperatureM_SetRefrigerationState(TemperatureM_RefrigerationStateType State);

/**
 * \brief 获取制冷状态
 *
 * \return 获取到的制冷状态
 */
TemperatureM_RefrigerationStateType TemperatureM_GetRefrigerationState(void);

/**
 * \brief 是否为仅加热电流状态
 * \return TRUE-是 FALSE-否
 */
boolean TemperatureM_IsHeatCurrent(void);

/**
 * \brief 是否在加热
 * \return TRUE-是 FALSE-否
 */
boolean TemperatureM_IsHeating(void);

/**
 * \brief 是否在制冷
 * \return TRUE-是 FALSE-否
 */
boolean TemperatureM_IsRefrigeration(void);

/**
 * \brief 获取充电加热时长
 * \note  此接口暂未实现
 *
 * \return 已加热时长
 */
uint16 TemperatureM_GetChgHeatTime(void);

/**
 * \brief 获取放电加热时长
 * \note  此接口暂未实现
 *
 * \return 已加热时长
 */
uint16 TemperatureM_GetDchgHeatTime(void);

/**
 * \brief 获取充电加热电流值
 * \note  此接口暂未实现
 *
 * \return 充电加热电流
 */
uint16 TemperatureM_GetChgHeatCurrent(void);

/**
 * \brief 获取放电加热电流值
 * \note  此接口暂未实现
 *
 * \return 放电加热电流
 */
uint16 TemperatureM_GetDchgHeatCurrent(void);

/**
 * \brief 是否加热故障
 * \note  当加热状态设置为TEMPERATUREM_HEAT_STATE_FAULT时,返回1
 *
 * \return 0-正常 非0-故障
 */
uint16 TemperatureM_IsHeatingFault(void);

/**
 * \brief 是否制冷故障
 * \note  当加热状态设置为TEMPERATUREM_REFRI_STATE_FAULT时,返回1
 *
 * \return 0-正常 非0-故障
 */
uint16 TemperatureM_IsRefrigerationFault(void);

/**
 * \brief 温度管理获取最大充电加热温度
 * \note  仅在充电连接且加热状态下,返回加热温度最大值,否则返回无效值,若无加热温感,返回无效值
 *
 * \return 当前最大加热温度
 */
uint16 TemperatureM_GetChgHeatTemperatureMax(void);

/**
 * \brief 温度管理获取最大充电加热温差
 * \note  仅在充电连接且加热状态下,返回加热最大温差值,否则返回无效值,若无加热温感,返回无效值
 *
 * \return 当前最大加热温差
 */
uint16 TemperatureM_GetChgHeatDeltaTemperatureMax(void);

/**
 * \brief 温度管理获取最大放电加热温度
 * \note  仅在放电连接且加热状态下,返回加热温度最大值,否则返回无效值,若无加热温感,返回无效值
 *
 * \return 当前最大加热温度
 */
uint16 TemperatureM_GetDchgHeatTemperatureMax(void);

/**
 * \brief 温度管理获取最大放电加热温差
 * \note  仅在充电连接且加热状态下,返回加热最大温差值,否则返回无效值,若无加热温感,返回无效值
 *
 * \return 当前最大加热温差
 */
uint16 TemperatureM_GetDchgHeatDeltaTemperatureMax(void);

/**
 * \brief 热失控故障
 * \details 国标热失控故障判断
 *
 * \return 0-无故障 1-有故障
 */
uint16 TemperatureM_IsThermalRunawayFault(void);

#endif

/**
 * \}
 */