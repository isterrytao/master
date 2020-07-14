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

void VcuComm_GetStatusMsg_0x013 (uint8 *buf, uint16 *Length) {
    static uint8 life = 0U;
    uint16 index = 0U;
    uint16 temp;
    static uint8 this_index = 0U;
    Diagnosis_ItemType item;
    uint8 goto_flag = 0U;

    // 总压
    temp = Statistic_GetBcu100mvTotalVoltage();
    WRITE_BT_UINT16(buf, index, temp);
    // 电量状态
    temp = 0U;
    if (CHARGECONNECTM_IS_CONNECT())
    {
        if (Soc_Get() >= 1000U)
        {
            temp = 2U;
        }
    }
    else
    {
        if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV) != DIAGNOSIS_LEVEL_NONE)
        {
            temp = 1U;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) != DIAGNOSIS_LEVEL_NONE)
        {
            temp = 1U;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_LSOC) != DIAGNOSIS_LEVEL_NONE)
        {
            temp = 1U;
        }
        else
        {

        }
    }
    WRITE_BT_UINT8(buf, index, temp);
    // 故障码
    if (this_index > DIAGNOSIS_ITEM_DEFAULT_NUM) {
        this_index = 0U;
    }

    item = Diagnosis_IndexToItem(this_index);
    item = Diagnosis_GetNextAlarmingItem(item);
    if (item == DIAGNOSIS_ITEM_INVALID_INDEX) {
        if (this_index == 0U) {
            WRITE_BT_UINT8(buf, index, 0xFFU);
            goto_flag = 1U;
        } else {
            item = Diagnosis_IndexToItem(0U);
            item = Diagnosis_GetNextAlarmingItem(item);
            if (item == DIAGNOSIS_ITEM_INVALID_INDEX) {
                WRITE_BT_UINT8(buf, index, 0xFFU);
                this_index = 0U;
                goto_flag = 1U;
            }
        }
    }
    if (goto_flag == 0U) {
        // val = Diagnosis_GetLevel(item);
        WRITE_BT_UINT8(buf, index, item);
        this_index = Diagnosis_ItemToIndex(item) + 1U;
    }
    temp = Soc_Get();
    temp = (uint16)SOC_TO_PERCENT(temp);
    WRITE_BT_UINT8(buf, index,  temp);
    // 温度
    temp = Statistic_GetBcuHt(0U);
    if (CellDataM_TemperatureIsValid(temp))
    {
        temp = TEMP_TO_40_OFFSET(temp);
    }
    else
    {
        temp = 0xFFFFU;
    }
    WRITE_BT_UINT8(buf, index,  temp);
    // life
    WRITE_BT_UINT8(buf, index,  life);
    life ++;
    // 电池状态
    if (CHARGECONNECTM_IS_CONNECT())
    {
        temp = 1U;
    }
    else
    {
        temp = 2U;
    }
    WRITE_BT_UINT8(buf, index, temp);
    *Length = index;
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
