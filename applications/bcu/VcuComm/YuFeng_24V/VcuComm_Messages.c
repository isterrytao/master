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
#include <stdlib.h>
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

void VcuComm_GetStatusMsg_0x1AC(uint8 *buf, uint16 *Length)
{
    Current_CurrentType current;
    uint16 index = 0U, temp;
    App_SocType soc;
    sint16 temperature;
    App_TemperatureType ht;
    Diagnosis_LevelType level;

    /**< 状态 */
    temp = 0U;
    if (CHARGECONNECTM_IS_CONNECT())
    {
        temp |= ((uint16)1 << 0);
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LV);
        if (ChargerComm_GetCommunicationStatus())
        {
            temp |= (uint16)1U << 7;
        }
    }
    else
    {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV);
    }
    soc = Soc_Get();
    if (soc <= 200U)
    {
        temp |= ((uint16)1 << 2);
    }
    else if (soc <= 300U)
    {
        temp |= ((uint16)1 << 1);
    }
    else
    {
    }
    ht = Statistic_GetBcuHt(0U);
    if (CellDataM_TemperatureIsValid((uint16)ht))
    {
        if (ht >= (App_TemperatureType)TEMPERATURE_FROM_C(70))
        {
            temp |= ((uint16)1 << 4);
        }
        if (ht >= (App_TemperatureType)TEMPERATURE_FROM_C(65))
        {
            temp |= ((uint16)1 << 3);
        }
    }
    if (level >= 2U)
    {
        temp |= ((uint16)1 << 6);
    }
    else if (level >= 1U)
    {
        temp |= ((uint16)1 << 5);
    }
    else
    {}
    WRITE_BT_UINT8(buf, index, temp);
    /**< SOC */
    soc = (App_SocType)SOC_TO_PERCENT(soc);
    WRITE_BT_UINT8(buf, index, soc);
    /**< 总压 */
    temp = Statistic_GetBcu100mvTotalVoltage();
    if (Statistic_TotalVoltageIsValid(temp))
    {
        if (temp > 655U)
        {
            temp = 0xFFFFU;
        }
        else
        {
            temp *= 100U;
        }
    }
    WRITE_BT_UINT16(buf, index, temp);
    /**< 总电流 */
    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    if (CurrentM_IsValidCurrent(current))
    {
        temp = (uint16)abs(current);
        if (temp <= 6553U)
        {
            temp *= 10U;
        }
        else
        {
            temp = 0xFFFFU;
        }
    }
    else
    {
        temp = 0xFFFFU;
    }
    WRITE_BT_UINT16(buf, index, temp);
    temperature = (sint16)Statistic_GetBcuLt(0U);
    if (CellDataM_TemperatureIsValid((uint16)ht))
    {
        temperature -= 50;
        if (temperature >= 0)
        {
            WRITE_BT_UINT8(buf, index, 0U);
        }
        else
        {
            WRITE_BT_UINT8(buf, index, 1U);
        }
        temperature = abs(temperature);
        WRITE_BT_UINT8(buf, index, temperature);
    }
    else
    {
        WRITE_BT_UINT8(buf, index, 0U);
        WRITE_BT_UINT8(buf, index, 255U);
    }

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
