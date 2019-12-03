/**
 * \file HvProcess_Chg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 高压充电流程控制配置文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170320 |
 */
#include "HvProcess_Types.h"
#include "HvProcess_Chg.h"

const HvProcess_StateInfoType HvProcess_ChgStartInfoConfig[] = {
    {
        HvProcess_ChgStateStartCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStateStartAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEATER_RLY_FAULT_CHECK, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHeatRlyFaultChkInfoConfig[] = {
    {
        HvProcess_ChgChargeConnectionCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgChargeConnectionAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgFaultCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgHeaterRelayIsNormal, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeaterRelayNormalAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEAT_JUDGE, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHeatJudgeInfoConfig[] = {
    {
        HvProcess_ChgChargeConnectionCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgChargeConnectionAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgFaultCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgStartHeatCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartHeatAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START_HEAT, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgStartChargeCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartChargeAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HV_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgStartHeatInfoConfig[] = {
    {
        HvProcess_ChgChargeConnectionCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgChargeConnectionAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgFaultCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgStartHeatTimeoutCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartHeatTimeoutAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgStartHeatFinishCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartHeatFinishAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEATING, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgHeatCurrentIsOk, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeatRelayOn, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START_HEAT, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHeatingInfoConfig[] = {
    {
        HvProcess_ChgChargeConnectionCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgChargeConnectionAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgFaultCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgHeatPowerFaultCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeatFaultAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgHeatFinishCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeatFinishAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START_CHARGE, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgHeatFinishCheckCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeatFinishCheckAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEATING, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgStartChargeInfoConfig[] = {
    {
        HvProcess_ChgChargeConnectionCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgChargeConnectionAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgFaultCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgStartChgFinishCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartChgFinishAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HV_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHvOnInfoConfig[] = {
    {
        HvProcess_ChgChargeConnectionCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgChargeConnectionAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgFaultCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgFaultAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgStartHeatCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartHeatAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START_HEAT, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgRelayOffDelayInfoConfig[] = {
    {
        HvProcess_ChgRelayOffDelayCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgRelayOffDelayAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RESTART_JUDGE, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgReStartJudgeInfoConfig[] = {
    {
        HvProcess_ChgReStartJudgeCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgReStartJudgeAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START, /**< Next: 下一状态 */
    },
};

const HvProcess_StateConfigType HvProcess_ChgStateConfig[HVPROCESS_CHG_STATE_MAX] = {
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgStartInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgStartInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgHeatRlyFaultChkInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHeatRlyFaultChkInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgHeatJudgeInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHeatJudgeInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgStartHeatInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgStartHeatInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgHeatingInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHeatingInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgStartChargeInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgStartChargeInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgHvOnInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHvOnInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgRelayOffDelayInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgRelayOffDelayInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgReStartJudgeInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgReStartJudgeInfoConfig, /**< State: 状态信息指针 */
    },
};
