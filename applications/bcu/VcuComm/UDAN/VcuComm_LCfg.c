/**
 * \file VcuComm_LCfg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 整车通信链接配置文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20161209 |
 */

#include "VcuComm_Types.h"
#include "PduR_Lcfg.h"
#include "VcuComm.h"
#include "VcuComm_Messages.h"
#include "SystemConnection_Lcfg.h"

const uint16 VcuComm_StageIdle = (uint16)VCUCOMM_STAGE_IDLE;
const uint16 VcuComm_StageStart = (uint16)VCUCOMM_STAGE_START;
const uint16 VcuComm_StageStop = (uint16)VCUCOMM_STAGE_STOP;
const uint16 VcuComm_StageMax = (uint16)VCUCOMM_STAGE_MAX;

const uint8 VcuComm_SendIPduNum = (uint8)PDUR_CFG_VCUCOMM_TX_PDUID_NUM;
const VcuComm_SendIPduCfgType VcuComm_IPduSendConfigInfo[PDUR_CFG_VCUCOMM_TX_PDUID_NUM] = {
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000001E1, /**< IPdu */
        8U, /**< Length */
        100U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x1E1, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000001E4, /**< IPdu */
        8U, /**< Length */
        100U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x1E4, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000001F3, /**< IPdu */
        8U, /**< Length */
        100U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x1F3, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000001F5, /**< IPdu */
        8U, /**< Length */
        100U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x1F5, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000360, /**< IPdu */
        8U, /**< Length */
        100U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x360, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000361, /**< IPdu */
        8U, /**< Length */
        100U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x361, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000208, /**< IPdu */
        8U, /**< Length */
        100U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x208, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000351, /**< IPdu */
        8U, /**< Length */
        100U, /**< Cycle */
        NULL, /**< CondFunc */
        NULL, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000358, /**< IPdu */
        8U, /**< Length */
        200U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x358, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000352, /**< IPdu */
        8U, /**< Length */
        200U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x352, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000353, /**< IPdu */
        8U, /**< Length */
        200U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x353, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000354, /**< IPdu */
        8U, /**< Length */
        200U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x354, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000355, /**< IPdu */
        8U, /**< Length */
        200U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x355, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000003F6, /**< IPdu */
        8U, /**< Length */
        200U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x3F6, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000211, /**< IPdu */
        8U, /**< Length */
        500U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x211, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000001F4, /**< IPdu */
        8U, /**< Length */
        500U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x1F4, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000356, /**< IPdu */
        8U, /**< Length */
#if 3000U/SYSTEM_BATTERY_CELL_NUM > 1000UL
        1000U, /**< Cycle */
#else
        3000U/SYSTEM_BATTERY_CELL_NUM, /**< Cycle */
#endif
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x356, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000357, /**< IPdu */
        8U, /**< Length */
#if 6000U/SYSTEM_TEMP_CELL_NUM > 1000UL
        1000U, /**< Cycle */
#else
        6000U/SYSTEM_TEMP_CELL_NUM, /**< Cycle */
#endif
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x357, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000003F3, /**< IPdu */
        8U, /**< Length */
        1000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x3F3, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000003F4, /**< IPdu */
        8U, /**< Length */
        1000U, /**< Cycle */
        NULL, /**< CondFunc */
        NULL, //VcuComm_GetStatusMsg_0x3F4, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000003F5, /**< IPdu */
        8U, /**< Length */
        1000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x3F5, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_000003FF, /**< IPdu */
        8U, /**< Length */
        1000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x3FF, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000201, /**< IPdu */
        8U, /**< Length */
        1000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x201, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000202, /**< IPdu */
        8U, /**< Length */
        1000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x202, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000205, /**< IPdu */
        8U, /**< Length */
        1000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x205, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000206, /**< IPdu */
        8U, /**< Length */
        1000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x206, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000207, /**< IPdu */
        8U, /**< Length */
        1000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x207, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000203, /**< IPdu */
        8U, /**< Length */
        2000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x203, /**< Func */
    },
    {
        (uint16)PDUR_VCUCOMM_TX_PDUID_00000204, /**< IPdu */
        8U, /**< Length */
        2000U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x204, /**< Func */
    },
};

static const VcuComm_RecIPduInfoType VcuComm_Stage1RecItem[] = {
    {
        (uint16)PDUR_VCUCOMM_RX_PDUID_12345678, /**< IPdu */
        8U,
        VCUCOMM_REC_TIMEOUT_DEFAULT, /**< Timeout: 接收超时时间 */
        NULL, /**< TimeoutFunc: 获取IPdu接收超时时间回调函数 */
        NULL, /**< RecFunc: 数据接收回调函数 */
        NULL, /**< TimeoutEventFunc: 数据接收超时事件回调函数 */
    },
};


/**
 * \brief 整车通信接收报文配置
 */
const VcuComm_RecIPduCfgType VcuComm_IPduRecConfigInfo[VCUCOMM_STAGE_MAX] = {
    {
        (uint8)ARRAY_SIZE(VcuComm_Stage1RecItem), /**< IPduNum: 报文个数 */
        VcuComm_Stage1RecItem, /**< IPduInfo: 各IPdu配置信息 */
    },
    {
        (uint8)ARRAY_SIZE(VcuComm_Stage1RecItem), /**< IPduNum: 报文个数 */
        VcuComm_Stage1RecItem, /**< IPduInfo: 各IPdu配置信息 */
    },
};


const uint16 VcuComm_RxIPduStart =  PDUR_CFG_VCUCOMM_TX_PDUID_NUM;
