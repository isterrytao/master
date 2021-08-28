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
#include "ChargeM.h"
#include "DischargeM.h"
#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
#include "Modules.h"
#endif

#define VCUCOMM_MESSAGES_E_PARAM_INVALID_PTR            0U

#define VCUCOMM_MESSAGES_API_ID_ReceiveCbk              0U


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

void VcuComm_GetStatusMsg_0x244(uint8 *buf, uint16 *Length) {
    uint16 uval = 0U;
    uint16 index = 0U;
    Diagnosis_LevelType level1, level2;

    level1 = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT);
    level2 = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT);
    if (level2 > level1)
    {
        level1 = level2;
    }
    if (level1 >= DIAGNOSIS_LEVEL_SECOND)
    {
        level1 = DIAGNOSIS_LEVEL_SECOND;
    }
    uval |= (uint16)level1 << 4;
    level1 = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK);
    // if (level1 >= DIAGNOSIS_LEVEL_SECOND)
    // {
    //     level1 = DIAGNOSIS_LEVEL_SECOND;
    // }
    // uval |= (uint16)level1 << 6;
    if (level1 != DIAGNOSIS_LEVEL_NONE)
    {
        uval |= 1U << 6;
    }
    WRITE_LT_UINT8(buf, index, uval);
    WRITE_LT_UINT8(buf, index, 0U);
    uval = 0U;
    level1 = Diagnosis_GetDiagLevelMax();
    if (CHARGECONNECTM_IS_CONNECT())
    {
        if (ChargeM_DiagnosisIsFaultActionExcludeItem(DIAGNOSIS_ITEM_FULL_CHARGE) == E_OK)
        {
            uval |= (uint16)1U << 2;
        }
        else if (level1 >= DIAGNOSIS_LEVEL_SECOND)
        {
            uval |= (uint16)1U << 2;
        }
        else
        {}
    }
    else
    {
        if (DischargeM_DiagnosisIsFaultAction() == E_OK)
        {
            uval |= (uint16)1U << 2;
        }
        else if (level1 >= DIAGNOSIS_LEVEL_SECOND)
        {
            uval |= (uint16)1U << 2;
        }
        else
        {}
    }
    WRITE_LT_UINT8(buf, index, uval);
    WRITE_LT_UINT32(buf, index, 0U);
    WRITE_LT_UINT8(buf, index, 0U);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x444(uint8 *buf, uint16 *Length) {
    uint16 uval;
    uint16 index = 0U;

    WRITE_LT_UINT32(buf, index, 0U);
    WRITE_LT_UINT8(buf, index, 0U);
    uval = Soc_Get();
    uval = (uint16)SOC_TO_PERCENT(uval);
    WRITE_LT_UINT8(buf, index, uval);
    WRITE_LT_UINT16(buf, index, 0U);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x447(uint8 *buf, uint16 *Length) {
    uint8 val8;
    uint16 index = 0U;

    WRITE_LT_UINT8(buf, index, 0U);
    val8 = Statistic_GetBcuAverageTemperature();
    val8 = (uint8)TEMP_TO_40_OFFSET(val8);
    WRITE_LT_UINT8(buf, index, val8);
    WRITE_LT_UINT32(buf, index, 0U);
    WRITE_LT_UINT16(buf, index, 0U);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x446(uint8 *buf, uint16 *Length) {
    uint16 uval;
    uint16 index = 0U;

    WRITE_LT_UINT32(buf, index, 0U);
    WRITE_LT_UINT8(buf, index, 0U);
    uval = Statistic_GetBcuAverageVoltage();
    uval = (uint16)DIVISION(uval, 10U);
    WRITE_LT_UINT16(buf, index, uval);
    WRITE_LT_UINT8(buf, index, 0U);
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

Std_ReturnType VcuComm_SendMsgCond_0x7F4(uint16 IPdu)
{
    Std_ReturnType res = E_NOT_OK;
    uint16 temp;
    uint16 index = 0U;
    Diagnosis_LevelType diaChgLevel, diaLevel;
    static uint8 count = 0U;

    (void)IPdu;
    temp = 0U;
    // 单体过压
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HV);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HV);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 0);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 0);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 0);
    // }
    else
    {

    }
    //单体欠压
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LV);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 2);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 2);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 2);
    // }
    else
    {

    }
    //总压过压
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HTV);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HTV);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 4);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 4);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 4);
    // }
    else
    {

    }
    //总压欠压
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LTV);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 6);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 6);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 6);
    // }
    else
    {

    }
    WRITE_LT_UINT8(VcuComm_MsgData.faultBuff, index, temp);
    temp = 0U;
    //单体压差过大
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DV);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DV);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 0);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 0);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 0);
    // }
    else
    {

    }
    //放电过流
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_SP_OC);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 2);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 2);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 2);
    // }
    else
    {

    }
    //充电过流
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_AC_CHG_OC);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DC_CHG_OC);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_FB_OC);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 4);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 4);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 4);
    // }
    else
    {

    }
    //温度过高
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 6);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 6);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 6);
    // }
    else
    {

    }
    WRITE_LT_UINT8(VcuComm_MsgData.faultBuff, index, temp);
    temp = 0U;
    //温度过低
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LT);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 0);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 0);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 0);
    // }
    else
    {

    }
    //温差过大
    diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DT);
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DT);
    if (diaChgLevel >= diaLevel)
    {
        diaLevel = diaChgLevel;
    }
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 2);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 2);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 2);
    // }
    else
    {

    }
    //soc过低
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LSOC);
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 4);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 4);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 4);
    // }
    else
    {

    }
    //绝缘故障
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK);
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 6);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 6);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 6);
    // }
    else
    {

    }
    WRITE_LT_UINT8(VcuComm_MsgData.faultBuff, index, temp);
    temp = 0U;
    //高压互锁故障
    //外部通信故障
    //内部通信故障
    diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_INTER_COMM);
    if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= ((uint16)1U << 4);
    }
    else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
    {
        temp |= ((uint16)2U << 4);
    }
    // else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= ((uint16)3U << 4);
    // }
    else
    {

    }
    WRITE_LT_UINT8(VcuComm_MsgData.faultBuff, index, temp);
    WRITE_LT_UINT16(VcuComm_MsgData.faultBuff, index, 0xFFFFU);
    WRITE_LT_UINT16(VcuComm_MsgData.faultBuff, index, 0xFFFFU);

    if (VcuComm_MsgData.faultBuff[0] ||
        VcuComm_MsgData.faultBuff[1] ||
        VcuComm_MsgData.faultBuff[2] ||
        VcuComm_MsgData.faultBuff[3])
    {
        res = E_OK;
    }
    if (res == E_NOT_OK)
    {
        if (count < 3U)
        {
            res = E_OK;
        }
        else
        {
            count = 5U;
        }
        count ++;
    }
    else
    {
        count = 0U;
    }
    return res;
}
