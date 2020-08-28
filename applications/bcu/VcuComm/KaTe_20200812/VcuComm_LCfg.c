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
#include "PduR_LCfg.h"
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
        (uint16)PDUR_VCUCOMM_TX_PDUID_000002F0, /**< IPdu */
        8U, /**< Length */
        50U, /**< Cycle */
        NULL, /**< CondFunc */
        VcuComm_GetStatusMsg_0x2F0, /**< Func */
    },
};

static const VcuComm_RecIPduInfoType VcuComm_Stage1RecItem[] = {
    {
        (uint16)PDUR_VCUCOMM_RX_PDUID_00000370, /**< IPdu */
        8U,
        VCUCOMM_REC_TIMEOUT_DEFAULT, /**< Timeout: 接收超时时间 */
        NULL, /**< TimeoutFunc: 获取IPdu接收超时时间回调函数 */
        VcuComm_ReceiveCbk, /**< RecFunc: 数据接收回调函数 */
        VcuComm_RecTimeoutCbk, /**< TimeoutEventFunc: 数据接收超时事件回调函数 */
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

