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
// #include "RelayM_Lcfg.h"

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
        (uint16)HVPROCESS_CHG_HEAT_TEMP_JUDGE, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHeatTempJudgeInfoConfig[] = {
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
        HvProcess_ChgStartHeatTempCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartHeatTempAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START_TO_HEAT_CURR_VALID_JUDGE, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgStartHeatAndChargeTempCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartHeatAndChargeTempAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START_TO_HEAT_AND_CHARGE_CURR_VALID_JUDGE, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgStartChargeTempCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartChargeTempAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HV_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgPowerOnCurrValidJudgeInfoConfig[] = {
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
        HvProcess_ChgPowerOnCurrInValidCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgPowerOnCurrInValidAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgPowerOnCurrIsValidCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgPowerOnCurrIsValidAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_START_TO_HEAT_RELAY_DELAY_ACTION, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgPowerOnToHeatRelayDelayActionInfoConfig[] = {
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
        HvProcess_ChgPowerOnToHeatRelayDelayActionCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgPowerOnToHeatRelayDelayActionAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEAT_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHeatAndChargeToHeatPrepareInfoConfig[] = {
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
        HvProcess_ChgHeatAndChargeToHeatPrepareCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeatAndChargeToHeatPrepareAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEAT_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHeatOnInfoConfig[] = {
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
        HvProcess_ChgStopHeatTempCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStopHeatTempAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEAT_TO_HEAT_AND_CHARGE_PREPARE, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgStartToHeatAndChargeCurrValidJudgeInfoConfig[] = {
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
        HvProcess_ChgStartHeatAndChargeCurrInValidCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartHeatAndChargeCurrInValidAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgStartHeatAndChargeCurrIsValidCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgStartHeatAndChargeCurrIsValidAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEAT_AND_CHARGE_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHeatToHeatAndChargePrepareInfoConfig[] = {
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
        HvProcess_ChgHeatToHeatAndChargePrepareCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeatToHeatAndChargePrepareAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEAT_AND_CHARGE_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgChargeToHeatAndChargePrepareInfoConfig[] = {
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
        HvProcess_ChgChargeToHeatAndChargePrepareCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgChargeToHeatAndChargePrepareAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEAT_AND_CHARGE_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHeatAndChargeOnInfoConfig[] = {
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
        HvProcess_ChgHeatAndChargeToHeatTempCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeatAndChargeToHeatTempAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEAT_AND_CHARGE_TO_HEAT_PREPARE, /**< Next: 下一状态 */
    },
    {
        HvProcess_ChgHeatAndChargeToChargeTempCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeatAndChargeToChargeTempAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HEAT_AND_CHARGE_TO_CHARGE_PREPARE, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHeatAndChargeToChargePrepareInfoConfig[] = {
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
        HvProcess_ChgHeatAndChargeToChargePrepareCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgHeatAndChargeToChargePrepareAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_HV_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_ChgHvOnInfoConfig[] = {
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
        HvProcess_ChgHvOnToHeatAndChargeTempCond, /**< Cond: 条件函数指针 */
        HvProcess_ChgHvOnToHeatAndChargeTempAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_CHG_CHARGE_TO_HEAT_AND_CHARGE_PREPARE, /**< Next: 下一状态 */
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
        (uint8)ARRAY_SIZE(HvProcess_ChgHeatTempJudgeInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHeatTempJudgeInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgPowerOnCurrValidJudgeInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgPowerOnCurrValidJudgeInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgPowerOnToHeatRelayDelayActionInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgPowerOnToHeatRelayDelayActionInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgHeatAndChargeToHeatPrepareInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHeatAndChargeToHeatPrepareInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgHeatOnInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHeatOnInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgStartToHeatAndChargeCurrValidJudgeInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgStartToHeatAndChargeCurrValidJudgeInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgHeatToHeatAndChargePrepareInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHeatToHeatAndChargePrepareInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgChargeToHeatAndChargePrepareInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgChargeToHeatAndChargePrepareInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgHeatAndChargeOnInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHeatAndChargeOnInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_ChgHeatAndChargeToChargePrepareInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_ChgHeatAndChargeToChargePrepareInfoConfig, /**< State: 状态信息指针 */
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
