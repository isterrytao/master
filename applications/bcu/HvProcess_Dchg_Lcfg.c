/**
 * \file HvProcess_Dchg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 高压放电流程控制配置文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170320 |
 */

#include "HvProcess_Dchg.h"

const HvProcess_StateInfoType HvProcess_DchgStartInfoConfig[] = {
    {
        HvProcess_DchgStateStartCond, /**< Cond: 条件函数指针 */
        HvProcess_DchgStateStartAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_DCHG_HV_ON, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_DchgHvOnInfoConfig[] = {
    {
        HvProcess_DchgFaultCond, /**< Cond: 条件函数指针 */
        HvProcess_DchgFaultAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_DCHG_RELAY_OFF_DELAY, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_DchgRelayOffDelayInfoConfig[] = {
    {
        HvProcess_DchgChrIsStopCond, /**< Cond: 条件函数指针 */
        NULL, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_DCHG_CHG_TO_DCHG_WAIT, /**< Next: 下一状态 */
    },
    {
        HvProcess_DchgRelayOffDelayCond, /**< Cond: 条件函数指针 */
        HvProcess_DchgRelayOffDelayAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_DCHG_ALLOW_RESTART_CHECK, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_DchgChgToDchgWaitInfoConfig[] = {
    {
        HvProcess_DchgConnectionCond, /**< Cond: 条件函数指针 */
        HvProcess_DchgConnectionAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_DCHG_HV_ON, /**< Next: 下一状态 */
    },
    {
        HvProcess_DchgChgOhvCond, /**< Cond: 条件函数指针 */
        HvProcess_DchgRelayOffDelayAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_DCHG_ALLOW_RESTART_CHECK, /**< Next: 下一状态 */
    },
};

const HvProcess_StateInfoType HvProcess_DchgAllowRestartInfoConfig[] = {
    {
        HvProcess_DchgIsAllowRestartCond, /**< Cond: 条件函数指针 */
        HvProcess_DchgIsAllowRestartAction, /**< Action: 动作函数指针 */
        (uint16)HVPROCESS_DCHG_START, /**< Next: 下一状态 */
    },
};

const HvProcess_StateConfigType HvProcess_DchgStateConfig[HVPROCESS_DCHG_STATE_MAX] = {
    {
        (uint8)ARRAY_SIZE(HvProcess_DchgStartInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_DchgStartInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_DchgHvOnInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_DchgHvOnInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_DchgRelayOffDelayInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_DchgRelayOffDelayInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_DchgChgToDchgWaitInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_DchgChgToDchgWaitInfoConfig, /**< State: 状态信息指针 */
    },
    {
        (uint8)ARRAY_SIZE(HvProcess_DchgAllowRestartInfoConfig), /**< Num: 状态配置个数 */
        HvProcess_DchgAllowRestartInfoConfig, /**< State: 状态信息指针 */
    },
};

