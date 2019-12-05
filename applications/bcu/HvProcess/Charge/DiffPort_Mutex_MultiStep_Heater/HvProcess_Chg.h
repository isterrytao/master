/**
 * \file HvProcess_Chg.h
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 高压充电流程控制头文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170320 |
 */

/**
 * \addtogroup HvProcess 高压流程控制
 * \{
 */

#ifndef HV_PROCESS__CHG_H_
#define HV_PROCESS__CHG_H_

#include "HvProcess_Types.h"
#include "Async_Looper.h"
#include "Async_Event.h"
#include "RelayM_Cfg.h"

/**
 * \brief 充电高压流程控制状态类型
 */
typedef enum{
    HVPROCESS_CHG_START = 0, /**< 启动状态 */
    HVPROCESS_CHG_HEATER_RLY_FAULT_CHECK, /**< 加热继电器检测状态 */
    HVPROCESS_CHG_HEAT_TEMP_JUDGE, /**< 温度判定状态 */
    HVPROCESS_CHG_START_TO_HEAT_CURR_VALID_JUDGE, /**< 纯加热电流判定状态 */
    HVPROCESS_CHG_START_TO_HEAT_RELAY_DELAY_ACTION, /**< 纯加热延时状态 */
    HVPROCESS_CHG_HEAT_AND_CHARGE_TO_HEAT_PREPARE, /**< 边充电边加热到纯加热跳转状态 */
    HVPROCESS_CHG_HEAT_ON, /**< 纯加热状态 */
    HVPROCESS_CHG_START_TO_HEAT_AND_CHARGE_CURR_VALID_JUDGE, /**< 边充电变加热电流判定状态 */
    HVPROCESS_CHG_HEAT_TO_HEAT_AND_CHARGE_PREPARE, /**< 纯加热到边充电边加热跳转状态 */
    HVPROCESS_CHG_CHARGE_TO_HEAT_AND_CHARGE_PREPARE, /**< 纯充电到边充电边加热跳转状态 */
    HVPROCESS_CHG_HEAT_AND_CHARGE_ON, /**< 边充电边加热状态 */
    HVPROCESS_CHG_HEAT_AND_CHARGE_TO_CHARGE_PREPARE, /**< 边充电边加热到纯充电跳转状态 */
    HVPROCESS_CHG_HV_ON, /**< 纯充电状态 */
    HVPROCESS_CHG_RELAY_OFF_DELAY, /**< 继电器断开延时状态 */
    HVPROCESS_CHG_RESTART_JUDGE, /**< 重启判断 */
    HVPROCESS_CHG_STATE_MAX, /**< 充电高压流程控制状态最大值 */
}HvProcess_ChgStateType;

/**
 * \brief 充电高压流程控制内部数据类型
 */
typedef struct{
    uint8 chgFinishFlag; /**< 充电完成标志 */
    HvProcess_ChgStateType State; /**< 充电高压流程状态 */
    uint32 RelayOffTick; /**< 继电器断开计时 */
    boolean RelayAdhesCheckFlag; /**< 继电器粘连检测标志 */
    boolean HeatRelayFaultCheckFlag;
    boolean HeatIsFinish;
    uint32 startToHeatRelayActionTime;
    RelayM_FunctionType chgRelay;
}HvProcess_ChgInnerDataType;


extern const HvProcess_StateConfigType HvProcess_ChgStateConfig[HVPROCESS_CHG_STATE_MAX];


/**
 * \brief 充电高压流程控制初始化
 */
void HvProcess_ChgInit(Async_LooperType *looper);


/**
 * \brief 获取充电高压流程控制状态
 * \return 充电流程状态
 */
HvProcess_ChgStateType HvProcess_GetChgState(void);

/**
 * \brief 设置充电高压流程控制状态
 *
 * \param state 充电流程状态
 */
void HvProcess_SetChgState(HvProcess_ChgStateType state);

/**
 * \brief 充电高压流程控制周期函数
 * \details 用于需要周期性更新的策略
 */
void HvProcess_ChgPoll(void);

/**
 * \brief 充电启动状态条件函数
 * \return TRUE-正常 FALSE-故障
 */
boolean HvProcess_ChgStateStartCond(void);

/**
 * \brief 充电启动状态动作函数
 */
void HvProcess_ChgStateStartAction(void);

/**
 * \brief 充电是否完成条件
 * \return TRUE-完成 FALSE-未完成
 */
boolean HvProcess_ChgIsFinishCond(void);

/**
 * \brief 充电完成动作
 */
void HvProcess_ChgFinishAction(void);

/**
 * \brief 充电高压上电状态条件1函数
 * \return TRUE-正常 FALSE-故障
 */
boolean HvProcess_ChgChargeConnectionCond(void);

/**
 * \brief 充电高压上电状态动作1函数
 */
void HvProcess_ChgChargeConnectionAction(void);

/**
 * \brief 充电高压上电状态条件2函数
 * \return TRUE-正常 FALSE-故障
 */
boolean HvProcess_ChgFaultCond(void);

/**
 * \brief 充电高压上电状态动作2函数
 */
void HvProcess_ChgFaultAction(void);

/**
 * \brief 继电器断开延时条件
 * \return TRUE-成立 FALSE-不成立
 */
boolean HvProcess_ChgRelayOffDelayCond(void);

/**
 * \brief 继电器断开动作
 */
void HvProcess_ChgRelayOffDelayAction(void);


boolean HvProcess_ChgReStartJudgeCond(void);

void HvProcess_ChgReStartJudgeAction(void);

void HvProcess_ChgSetHeatStateNone(void);

boolean HvProcess_ChgHeaterRelayIsNormal(void);
void HvProcess_ChgHeaterRelayNormalAction(void);
boolean HvProcess_ChgStartHeatTempCond(void);
void HvProcess_ChgStartHeatTempAction(void);
boolean HvProcess_ChgStartHeatAndChargeTempCond(void);
void HvProcess_ChgStartHeatAndChargeTempAction(void);
boolean HvProcess_ChgStartChargeTempCond(void);
void HvProcess_ChgStartChargeTempAction(void);
boolean HvProcess_ChgPowerOnCurrIsValidCond(void);
void HvProcess_ChgPowerOnCurrIsValidAction(void);
boolean HvProcess_ChgPowerOnCurrInValidCond(void);
void HvProcess_ChgPowerOnCurrInValidAction(void);
boolean HvProcess_ChgPowerOnToHeatRelayDelayActionCond(void);
void HvProcess_ChgPowerOnToHeatRelayDelayActionAction(void);
boolean HvProcess_ChgHeatAndChargeToHeatPrepareCond(void);
void HvProcess_ChgHeatAndChargeToHeatPrepareAction(void);
boolean HvProcess_ChgStopHeatTempCond(void);
void HvProcess_ChgStopHeatTempAction(void);
boolean HvProcess_ChgStartHeatAndChargeCurrIsValidCond(void);
void HvProcess_ChgStartHeatAndChargeCurrIsValidAction(void);
boolean HvProcess_ChgStartHeatAndChargeCurrInValidCond(void);
void HvProcess_ChgStartHeatAndChargeCurrInValidAction(void);
boolean HvProcess_ChgHeatToHeatAndChargePrepareCond(void);
void HvProcess_ChgHeatToHeatAndChargePrepareAction(void);
boolean HvProcess_ChgChargeToHeatAndChargePrepareCond(void);
void HvProcess_ChgChargeToHeatAndChargePrepareAction(void);
boolean HvProcess_ChgHeatAndChargeToHeatTempCond(void);
void HvProcess_ChgHeatAndChargeToHeatTempAction(void);
boolean HvProcess_ChgHeatAndChargeToChargeTempCond(void);
void HvProcess_ChgHeatAndChargeToChargeTempAction(void);
boolean HvProcess_ChgHeatAndChargeToChargePrepareCond(void);
void HvProcess_ChgHeatAndChargeToChargePrepareAction(void);
boolean HvProcess_ChgHvOnToHeatAndChargeTempCond(void);
void HvProcess_ChgHvOnToHeatAndChargeTempAction(void);
uint8 HvProcess_ChgGetStartToHeatTemp(void);
uint8 HvProcess_ChgGetStartToChargeTemp(void);
uint8 HvProcess_ChgGetHeatToHeatAndChargeTemp(void);
uint8 HvProcess_ChgGetHeatAndChargeToHeatTemp(void);
uint8 HvProcess_ChgGetHeatAndChargeToChargeTemp(void);
uint8 HvProcess_ChgGetChargeToHeatAndChargeTemp(void);
boolean HvProcess_ChargerIsHeatMode(void);
boolean HvProcess_IsJumpMode(void);
boolean HvProcess_IsHeatAndChargeMode(void);

boolean HvProcess_ChgHeatPowerFaultCond(void);
void HvProcess_ChgHeatFaultAction(void);

#endif

/**
 * \}
 */
