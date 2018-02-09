#ifndef POWERM_H__
#define POWERM_H__

/**
 * \file PowerM.h
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 功率管理头文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20161212 |
 */

/**
 * \addtogroup PowerM 功率管理
 * \{
 */

#include "Std_Types.h"
#include "Current_Types.h"
#include "CellDataM.h"
#include "Async_Looper.h"
#include "Diagnosis.h"
#include "Charge_Types.h"

/**
 * \brief 功率控制电流类型
 */
typedef enum {
    POWERM_CUR_DCHARGE_CONTINUE, /**< 持续放电电流类型 */
    POWERM_CUR_DCHARGE_PEAK, /**< 峰值放电电流类型 */
    POWERM_CUR_DCHARGE_FEEDBACK, /**< 回馈电流类型 */
    POWERM_CUR_CHARGE_CONTINUE, /**< 充电最大电流类型 */
    POWERM_CUR_CHARGE_DC, /**< 直流快充充电电流 */
    POWERM_CUR_CHARGE_AC, /**< 交流慢充充电电流 */
    POWERM_CUR_CHARGE_HEATER, /**< 充电加热电流 */
    POWERM_CUR_EXTERN, /**< 扩展电流 */

    POWERM_CUR_TYPE_NUM,
}PowerM_CurrentTypeType;


void PowerM_DiagnosisCtlCbk(Diagnosis_ItemType item, Diagnosis_LevelType level, Diagnosis_EventType event);

/**
 * \brief 功率管理模块初始化
 */
void PowerM_Init(Async_LooperType *looper);

/**
 * \brief 获取功率电流
 * \note 根据功率电流类型获取对应的功率电流值
 *
 * \param Type 功率电流类型
 * \return 功率电流值
 */
Current_CurrentType PowerM_GetCurrent(PowerM_CurrentTypeType Type);

/**
 * \brief 获取没有经过故障降功率的功率电流
 * \note 根据功率电流类型获取对应的功率电流值
 *
 * \param Type 功率电流类型
 * \return 功率电流值
 */
Current_CurrentType PowerM_GetCurrentWithoutDiag(PowerM_CurrentTypeType Type);

/**
 * \brief 获取充电总压
 * \note 根据充电电压类型获取对应的电压值
 *
 * \param Type 充电电压类型
 * \return 电压值
 */
App_Tv100mvType PowerM_GetChargeVoltage(Charge_ChargeType Type);

/**
 * \brief 获取当前功率总压值
 * \note 获取当前允许的功率总压，主要用于直流快充和交流慢充充电

 * \return 功率总压值
 */
App_Tv100mvType PowerM_GetTotalVoltage(void);

/**
 * \brief 判断末端降流是否完成

 * \retval TRUE 降流完成
 * \retval FALSE 未进入降流阶段或降流未完成
 */
boolean PowerM_IsChargeEndFinished(PowerM_CurrentTypeType Type);

/**
 * \brief 判断末端降流是否正在降流中

 * \retval TRUE 降流中
 * \retval FALSE 未进入降流阶段或降流未完成
 */
boolean PowerM_IsChargeEndDecreasing(PowerM_CurrentTypeType Type);

/**
 * \brief 复位功率管理的状态

 * \param Type 需要的复位的类型
 */
void PowerM_Reset(PowerM_CurrentTypeType Type);


void PowerM_DumpDestWithoutDiagResult(void);
void PowerM_DumpDestWithDiagResult(void);

typedef struct {
    Diagnosis_ItemType diagItemType;
    uint8 percent[DIAGNOSIS_SUPPORT_LEVEL];
} PowerM_DiagnosisPercentMapType;

#define POWERM_CHARGE_END_DEC_TO_PERCENT  0x00U
#define POWERM_CHARGE_END_DEC_CONSTVAL    0x01U
#define POWERM_CHARGE_END_DEC_TO_CONSTVAL 0x02U

typedef struct PowerM_ChargeEndConfig {
    const struct PowerM_ChargeEndConfig *next; /**/
    uint16 startVal; /**< 进入这个阶段的值 */
    uint16 delayTime; /**< 这个阶段最小维持的时间 */
    uint8 decType; /**< 降流方式，POWERM_CHARGE_END_DEC_TO_XXX */
    uint16 decParam; /**< 降流参数，与decType相关 */
    uint16 minVal; /**< 该阶段最小值 */
} PowerM_ChargeEndConfigType;

#define POWERM_XAXIS_SUPPORT_L2R   0x01U
#define POWERM_XAXIS_SUPPORT_R2L   0x02U
#define POWERM_XAXIS_SUPPORT_BDIR  0x03U
#define POWERM_YAXIS_USE_LT        0x00U
#define POWERM_YAXIS_USE_HT        0x01U
#define POWERM_YAXIS_USE_MAX       0x02U
#define POWERM_YAXIS_USE_MIN       0x03U

typedef struct {
    uint16 xAxisNum; /**< 功率控制表X轴分隔数量 */
    uint16 yAxisNum; /**< 功率控制表Y轴分隔数量 */
    const uint16 *xAxisValue; /**< 功率控制表X轴分隔表 */
    const sint16 *yAxisValue; /**< 功率控制表Y轴分隔表 */
    const sint16 *yAltAxisValue; /**< 功率控制表Y轴回滚分隔表 */
    const uint16 *tableValue; /**< 各个分隔对应的值 */
    uint8 yUseWhichValue; /**< 在yAxis和yAltAxis查询到的值取怎样取值，POWERM_YAXIS_USE_XX */
    uint8 yAxisIncUseAlt; /**< 在y轴增加时使用Alt表 */
    uint8 xAxisSupportDir; /**< X轴支持变动方向 */
    uint8 xAxisIsLinear; /**< X轴区间线性插值 */
    uint8 diagnosisPercentMapNum; /**<  诊断故障功率百分比数量 */
    const PowerM_DiagnosisPercentMapType *diagnosisPercentMap; /**<  断故障功率百分比表 */
    Std_ReturnType (*getXVal)(uint16 *v); /**< 获取X轴值的函数 */
    uint16 increaseTime; /**< 功率输出时增加斜率时间参数 */
    uint16 increaseStep; /**< 功率输出时增加斜率步进参数 */
    uint16 decreaseTime; /**< 功率输出时减小斜率时间参数 */
    uint16 decreaseStep; /**< 功率输出时减小斜率步进参数 */
    uint16 chargeEndVal; /**< 充电末端X轴达到这个值时进入降流阶段 */
    const PowerM_ChargeEndConfigType *chargeEndConfigs; /**< 降流链表 */
    Std_ReturnType (*getChargeEndCheckVal)(uint16 *v);  /**< 获取降流末端比较值函数 */
} PowerM_ConfigParamType;

typedef struct {
    uint16 destWithoutDiag;
    uint16 dest;
    uint16 out;
    uint32 outUpdateTick;
    uint16 xv;
    uint16 htvalue;
    uint16 ltValue;
    uint8 ht;
    uint8 lt;
    boolean chargeEndFinished;
    boolean chargeEndDecreasing;
    const PowerM_ChargeEndConfigType *curEndCfg;
    uint32 chargeEndTick;
} PowerM_InnerChannelDataType;

extern const PowerM_CurrentTypeType PowerMChannelNum;
extern const PowerM_ConfigParamType *const PowerMConfigParamMaps[];
extern const PowerM_CurrentTypeType PowerMChannelCurrentDC;
extern const PowerM_CurrentTypeType PowerMChannelCurrentAC;
extern const PowerM_CurrentTypeType PowerMChannelCurrentHeater;
extern PowerM_InnerChannelDataType PowerMInnerChannelData[];

#endif

/**
 * \}
 */
