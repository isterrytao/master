/**
 * \file VcuComm_messages.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 整车通信报文文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170316 |
 */
// #include <String.h>
#include "App_Types.h"
#include "Cpu.h"
#include "Det.h"
#include "VcuComm_Types.h"
#include "VcuComm_Messages.h"
#include "CurrentM.h"
#include "Statistic.h"
#include "Soc.h"
#include "Soh.h"
#include "Insu.h"
#include "ChargeConnectM.h"
#include "ChargerComm.h"
#include "HWDiagnosis.h"
#include "AppInfo.h"
#include "ParameterM.h"
#include "CellDataM.h"
#include "RelayM.h"
#include "PrechargeM.h"
#include "BalanceM.h"
#include "TemperatureM.h"
//#include "SystemConnection_Lcfg.h"
#include "VcuComm_Types.h"
#include "PduR_LCfg.h"
#include "Diagnosis.h"
#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
#include "Modules.h"
#endif

#define VCUCOMM_MESSAGES_E_PARAM_INVALID_PTR            0U

#define VCUCOMM_MESSAGES_API_ID_ReceiveCbk              0U

#define VCUCOMM_MESSAGE_RESERVE_UINT8 0xFFU
#define VCUCOMM_MESSAGE_RESERVE_UINT16 0xFFFFU

#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
#define VALIDATE_PTR(_ptr, _api) \
    if (_ptr == NULL) { \
        Det_ReportError(MODULE_ID_VCUCOMM_MESSAGES, 0U, _api, VCUCOMM_MESSAGES_E_PARAM_INVALID_PTR); \
        goto cleanup; \
    }
#else
#define VALIDATE_PTR(_ptr, _api)
#endif

#define VCUCOMM_CURRENT_OFFSET      (32000)
#define VCUCOMM_VOLT_4_DISPLAY(volt)    (VOLT_4_DISPLAY(volt) < 10000U ? VOLT_4_DISPLAY(volt) : 10000U)

VcuComm_MsgDataType VcuComm_MsgData;

void VcuComm_MessageInit(Async_LooperType *looper)
{
    (void)looper;
    (void)VcuComm_SendIPduRegister(&VcuComm_IPduSendConfigInfo[0], VcuComm_SendIPduNum);
}

void VcuComm_GetStatusMsg_0x444(uint8 *buf, uint16 *Length) {
    uint16 uval;
    uint16 index = 0U;

    /**< 预留 */
    WRITE_BT_UINT16(buf, index, VCUCOMM_MESSAGE_RESERVE_UINT16);
    WRITE_BT_UINT16(buf, index, VCUCOMM_MESSAGE_RESERVE_UINT16);
    WRITE_BT_UINT16(buf, index, VCUCOMM_MESSAGE_RESERVE_UINT16);
    uval = Soc_Get();
    uval = (uint16)SOC_TO_PERCENT(uval);
    WRITE_BT_UINT8(buf, index, uval);
    WRITE_BT_UINT8(buf, index, VCUCOMM_MESSAGE_RESERVE_UINT8);
    *Length = index;
}

static boolean DchgNotAllowFlg(void)
{
    boolean res = FALSE;

    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV) > DIAGNOSIS_LEVEL_FIRST)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) > DIAGNOSIS_LEVEL_FIRST)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HTV) > DIAGNOSIS_LEVEL_FIRST)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HV) > DIAGNOSIS_LEVEL_FIRST)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT) > DIAGNOSIS_LEVEL_FIRST)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT) > DIAGNOSIS_LEVEL_FIRST)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC) > DIAGNOSIS_LEVEL_FIRST)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_FB_OC) > DIAGNOSIS_LEVEL_FIRST)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_SP_OC) > DIAGNOSIS_LEVEL_FIRST)
    {
        res = TRUE;
    }
    else
    {}
    return res;
}

void VcuComm_GetStatusMsg_0x244(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    uint16 temp;
    uint8 u8temp;

    temp = 0U;
    /**< discharge allow 放电允许 */
    if (!CHARGECONNECTM_IS_CONNECT()) {
        // if (DchgNotAllowFlg() == FALSE) {
        //     temp |= (uint16)1U << 0;
        // }
#ifdef RELAYM_FN_POSITIVE_MAIN
        if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_MAIN) == RELAYM_ACTUAL_ON)
        {
            temp |= (uint16)1U << 0;
        }
#endif
#ifdef RELAYM_FN_DISCHARGE
        if (RelayM_GetActualStatus(RELAYM_FN_DISCHARGE) == RELAYM_ACTUAL_ON)
        {
            temp |= (uint16)1U << 0;
        }
#endif
    }
    /**< over discharge 过放 */
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV) >= DIAGNOSIS_LEVEL_THIRD) {
        temp |= (uint16)1U << 1;
    } else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) >= DIAGNOSIS_LEVEL_THIRD) {
        temp |= (uint16)1U << 1;
    } else {

    }
    /**< battery current exceeds 过流 */
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC) >= DIAGNOSIS_LEVEL_SECOND) {
        temp |= (uint16)1U << 2;
    } else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_FB_OC) >= DIAGNOSIS_LEVEL_SECOND) {
        temp |= (uint16)1U << 2;
    } else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_SP_OC) >= DIAGNOSIS_LEVEL_SECOND) {
        temp |= (uint16)1U << 2;
    } else
    {}
    /**< 漏电 */
    u8temp = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK);
    if (u8temp == DIAGNOSIS_LEVEL_SECOND)
    {
        temp|= (uint16)1U << 3;
    }
    else if (u8temp >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)1U << 4;
    }
    else
    {

    }
    /**< battery high voltage 电池过压 */
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)1U << 5;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HTV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)1U << 5;
    }
    else{
    }
    /**< cell voltage low 单体电压过低 */
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) >= DIAGNOSIS_LEVEL_SECOND) {
        temp |= (uint16)1U << 6;
    } else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV) >= DIAGNOSIS_LEVEL_SECOND) {
        temp |= (uint16)1U << 6;
    } else {

    }
    WRITE_BT_UINT8(buf, index, temp);

    /**< battery low temperature  电池低温*/
    temp = 0U;
    u8temp = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT);
    if (u8temp >= DIAGNOSIS_LEVEL_THIRD) {
        temp |= (uint16)1U << 0;
    }
    /**< battery high temperature  电池高温 */
    u8temp = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT);
    if (u8temp == DIAGNOSIS_LEVEL_SECOND) {
        temp |= (uint16)1 << 1;
    } else if (u8temp >= DIAGNOSIS_LEVEL_THIRD) {
        temp |= (uint16)2 << 1;
    } else {

    }
    WRITE_BT_UINT8(buf, index, temp);

    /**< 预留 */
    WRITE_BT_UINT16(buf, index, VCUCOMM_MESSAGE_RESERVE_UINT16);
    WRITE_BT_UINT16(buf, index, VCUCOMM_MESSAGE_RESERVE_UINT16);
    WRITE_BT_UINT16(buf, index, VCUCOMM_MESSAGE_RESERVE_UINT16);
    *Length = index;
}

void VcuComm_ReceiveCbk(uint8 *Buffer, uint16 Length)
{
    VALIDATE_PTR(Buffer, VCUCOMM_MESSAGES_API_ID_ReceiveCbk);

    VcuComm_SetCommunicationStatus(TRUE);
    VcuComm_ClrCommAbortMessageFlag((uint16)PDUR_VCUCOMM_RX_PDUID_12345678);
    (void)Buffer;
    (void)Length;

#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
cleanup:
#endif
    return;
}

void VcuComm_RecTimeoutCbk(void)
{
    VcuComm_MsgData.PowerCommand = VCUCOMM_POWER_OFF;
    VcuComm_SetCommunicationStatus(FALSE);
    VcuComm_SetCommAbortMessageFlag((uint16)PDUR_VCUCOMM_RX_PDUID_12345678);
}



// Condition Check function define
Std_ReturnType VcuComm_SendConditionCheck(uint16 IPdu)
{
    Std_ReturnType res = E_NOT_OK;

    (void)IPdu;
    if (VcuComm_GetCurrentRecStage() == (uint16)VCUCOMM_STAGE_STAGE1)
    {
        res = E_OK;
    }
    return res;
}

Std_ReturnType VcuComm_RecConditionCheck(void)
{
    Std_ReturnType res = E_NOT_OK;
    uint16 stage;

    stage = VcuComm_GetCurrentRecStage();
    if (stage == (uint16)VCUCOMM_STAGE_STAGE1)
    {
        res = E_OK;
    }
    return res;
}
