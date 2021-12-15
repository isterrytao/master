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
// #include <stdlib.h>
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

// #define VCUCOMM_CURRENT_OFFSET      (32000)
#define VCUCOMM_VOLT_4_DISPLAY(volt)    (VOLT_4_DISPLAY(volt) < 10000U ? VOLT_4_DISPLAY(volt) : 10000U)

VcuComm_MsgDataType VcuComm_MsgData;

#ifdef __KF32__
extern const BatteryInfo_BaseConfigType BatteryInfo_BaseConfigInfo;
#endif

void VcuComm_MessageInit(Async_LooperType *looper)
{
    (void)looper;
    VcuComm_MsgData.ishandshake = FALSE;
    (void)VcuComm_SendIPduRegister(&VcuComm_IPduSendConfigInfo[0], VcuComm_SendIPduNum);
}

void VcuComm_GetStatusMsg_0x2F0(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    uint16 u16val;
    Current_CurrentType current;
    // Current_CurrentType cur_offset = VCUCOMM_CURRENT_OFFSET;
    /**< 总电压 */
    u16val = Statistic_GetBcu100mvTotalVoltage();
    if (!Statistic_TotalVoltageIsValid(u16val)){
        u16val = 0U;
    }
    WRITE_LT_UINT16(buf, index, u16val);
    /**< 总电流 */
    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    if (!CurrentM_IsValidCurrent(current))
    {
        u16val = 0xFFFFU;
        // current = 0;
    }
    else
    {
        current = current + 3000;
        u16val = (uint16)current;
    }
    // if (current < (-1) * cur_offset)
    // {
    //     u16val = 0U;
    // }
    // else if (current > cur_offset)
    // {
    //     u16val = (uint16)cur_offset * 2U;
    // }
    // else
    // {
    //     u16val = (uint16)current + (uint16)cur_offset;
    // }
    WRITE_LT_UINT16(buf, index, u16val);
    /**< SOC */
    u16val = (uint16)SOC_TO_UINT8(Soc_Get());
    WRITE_LT_UINT8(buf, index, (uint8)u16val);
    /**< 电池容量规格:和售前确认使用额定容量 */
    u16val = BatteryInfo_BaseConfigInfo.DisplayCap / 20U;
    WRITE_LT_UINT8(buf, index, u16val);
    u16val = 0U;
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HTV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        u16val |= (uint16)1U << 0;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HTV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        u16val |= (uint16)1U << 0;
    }
    else
    {}
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        u16val |= (uint16)1U << 1;
    }
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_VCU_COMM) != DIAGNOSIS_LEVEL_NONE)
    {
        u16val |= (uint16)1U << 2;
    }
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) >= DIAGNOSIS_LEVEL_SECOND)
    {
        u16val |= (uint16)1U << 3;
    }
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        u16val |= (uint16)1U << 4;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_SP_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        u16val |= (uint16)1U << 4;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_FB_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        u16val |= (uint16)1U << 4;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_AC_CHG_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        u16val |= (uint16)1U << 4;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DC_CHG_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        u16val |= (uint16)1U << 4;
    }
    else
    {}
    if (CHARGECONNECTM_IS_CONNECT())
    {
        u16val |= (uint16)1U << 7;
        if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LT) >= DIAGNOSIS_LEVEL_THIRD)
        {
            u16val |= (uint16)3U << 5;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LT) >= DIAGNOSIS_LEVEL_SECOND)
        {
            u16val |= (uint16)2U << 5;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT) >= DIAGNOSIS_LEVEL_THIRD)
        {
            u16val |= (uint16)3U << 5;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT) >= DIAGNOSIS_LEVEL_SECOND)
        {
            u16val |= (uint16)2U << 5;
        }
        else
        {}
    }
    else
    {
        if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT) >= DIAGNOSIS_LEVEL_THIRD)
        {
            u16val |= (uint16)3U << 5;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT) >= DIAGNOSIS_LEVEL_SECOND)
        {
            u16val |= (uint16)2U << 5;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT) >= DIAGNOSIS_LEVEL_THIRD)
        {
            u16val |= (uint16)3U << 5;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT) >= DIAGNOSIS_LEVEL_SECOND)
        {
            u16val |= (uint16)2U << 5;
        }
        else
        {}
    }
    WRITE_LT_UINT8(buf, index, u16val);
    u16val = 0U;
    WRITE_LT_UINT8(buf, index, u16val);
    *Length = index;
}

void VcuComm_ReceiveCbk(uint8 *Buffer, uint16 Length)
{
    uint16 index = 0U, temp ;
    VALIDATE_PTR(Buffer, VCUCOMM_MESSAGES_API_ID_ReceiveCbk);

    VcuComm_SetCommunicationStatus(TRUE);
    if (Length >= 1U)
    {
        temp = READ_BT_UINT8(Buffer, index);
        if (temp == 1U)
        {
            VcuComm_MsgData.ishandshake = TRUE;
        }
        // else
        // {
        //     VcuComm_MsgData.ishandshake = FALSE;

        // }
    }
    VcuComm_ClrCommAbortMessageFlag((uint16)PDUR_VCUCOMM_RX_PDUID_00000000);

#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
cleanup:
#endif
    return;
}

void VcuComm_RecTimeoutCbk(void)
{
    VcuComm_MsgData.ishandshake = FALSE;
    VcuComm_MsgData.PowerCommand = VCUCOMM_POWER_OFF;
    VcuComm_SetCommunicationStatus(FALSE);
    VcuComm_SetCommAbortMessageFlag((uint16)PDUR_VCUCOMM_RX_PDUID_00000000);
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

void VcuComm_GetStatusMsg_0x770(uint8 *buf, uint16 *Length) {
    uint16 index = 0U, temp;
    temp = 0x7FU;
    if (VcuComm_MsgData.ishandshake)
    {
        temp = 0x5U;
    }
    WRITE_LT_UINT8(buf, index, temp);
    WRITE_LT_UINT24(buf, index, 0U);
    WRITE_LT_UINT32(buf, index, 0U);
    *Length = index;
}
