/**
 * \file VcuComm_messages.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 整车通信报文文件.
 * 在原先协议基础上屏蔽绝缘故障上报，所有一级故障不上报
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170316 |
 */
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
#include "VcuComm_Types.h"
#include "PduR_LCfg.h"
#include "Diagnosis.h"
#include "ChargeM.h"
#include "DischargeM.h"
#include "PowerM.h"
#include <stdlib.h>
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

void VcuComm_GetStatusMsg_0x241(uint8 *buf, uint16 *Length) {
    uint16 temp = 0U;
    uint16 index = 0U;
    boolean isConn = CHARGECONNECTM_IS_CONNECT();
    uint8 level = Diagnosis_GetDiagLevelMax();
    uint16 alarmNum;
    sint16 current;

    if (isConn)
    {
        temp |= (uint16)1U << 0;
    }
    if (isConn)
    {
        if (ChargeM_ChargeIsAllowed() == E_NOT_OK)
        {
            temp |= (uint16)1U << 1;
        }
    }
    else
    {
        if (DischargeM_DischargeIsAllowed() == E_NOT_OK)
        {
            temp |= (uint16)1U << 1;
        }
    }

    if (level == DIAGNOSIS_LEVEL_SECOND)
    {
        alarmNum = Diagnosis_GetAlarmNumWithLevel(DIAGNOSIS_LEVEL_SECOND);
        if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK) == DIAGNOSIS_LEVEL_SECOND &&
            alarmNum == 1U)
        {
        }
        else
        {
            temp |= (uint16)1U << 3;
        }
    }
    else if (level == DIAGNOSIS_LEVEL_THIRD)
    {
        alarmNum = Diagnosis_GetAlarmNumWithLevel(DIAGNOSIS_LEVEL_THIRD);
        if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK) == DIAGNOSIS_LEVEL_THIRD &&
            alarmNum == 1U)
        {
        }
        else
        {
            temp |= (uint16)1U << 3;
        }
    }
    else if (level == DIAGNOSIS_LEVEL_FOURTH)
    {
        alarmNum = Diagnosis_GetAlarmNumWithLevel(DIAGNOSIS_LEVEL_FOURTH);
        if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK) == DIAGNOSIS_LEVEL_FOURTH &&
            alarmNum == 1U)
        {
        }
        else
        {
            temp |= (uint16)1U << 3;
        }
    }
    else
    {}
    if (level <= DIAGNOSIS_LEVEL_FIRST)
    {
        temp |= (uint16)1U << 4;
    }
    WRITE_LT_UINT8(buf, index, temp);
    temp = Soc_Get();
    temp = (uint16)SOC_TO_PERCENT(temp);
    WRITE_LT_UINT8(buf, index, temp);
    temp = (uint16)PowerM_GetCurrent(POWERM_CUR_DCHARGE_CONTINUE);
    if (level >= DIAGNOSIS_LEVEL_SECOND)
    {
        if (temp > 1500U)
        {
            temp = 1500U;
        }
    }
    WRITE_BT_UINT16(buf, index, temp);
    temp = (uint16)PowerM_GetCurrent(POWERM_CUR_DCHARGE_FEEDBACK);
    if (level >= DIAGNOSIS_LEVEL_SECOND)
    {
        if (temp > 2000U)
        {
            temp = 2000U;
        }
    }
    WRITE_BT_UINT16(buf, index, temp);
    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    if (!CurrentM_IsValidCurrent(current))
    {
        temp = 0U;
    }
    else
    {
        temp = (uint16)abs(current);
    }
    WRITE_BT_UINT16(buf, index, temp);
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
