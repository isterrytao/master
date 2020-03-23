/**
 * \file ChargerCommUser_messages.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 自定义充电机通信报文文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170315 |
 */
#include <String.h>
#include "App_Types.h"
#include "Cpu.h"
#include "Det.h"
#include "ChargerCommUser_Messages.h"
#include "ChargerComm.h"
#include "ChargeM.h"
#include "PowerM.h"
#include "TemperatureM.h"
#include "ChargerComm_LCfg.h"
#include "UserStrategy.h"
#include "EL.h"
#include "Soc.h"
#include "Statistic.h"
#include "Diagnosis.h"

#define CHARGERCOMMUSER_MESSAGES_E_PARAM_INVALID_PTR         0U

#define CHARGERCOMMUSER_MESSAGES_API_ID_ReceiveTCCbk         0U
#define CHARGERCOMMUSER_MESSAGES_API_ID_GetTCDataCbk         1U


#if ( CHARGERCOMMUSER_DEV_ERROR_DETECT == STD_ON )
#define VALIDATE_PTR(_ptr, _api) \
    if (_ptr == NULL) { \
        Det_ReportError(MODULE_ID_CHARGERCOMM_USER_MESSAGES, 0U, _api, CHARGERCOMMUSER_MESSAGES_E_PARAM_INVALID_PTR); \
        goto cleanup; \
    }
#else
#define VALIDATE_PTR(_ptr, _api)
#endif

static void getTCChgCtlData(uint8 *Buffer, uint16 *Length);
static void getTCChgStopData(uint8 *Buffer, uint16 *Length);

extern ChargerCommUser_InnerDataType ChargerCommUser_innerData;
ChargerCommUser_MessageInnerDataType ChargerCommUser_MsgInnerData;

void ChargerCommUser_MessageInit(void)
{
    (void)ChargerComm_SendIPduRegister(&ChargerComm_IPduSendConfigInfo[CHARGERCOMM_GB_CHR_TX_PDUID_NUM], CHARGERCOMM_USER_CHR_TX_PDUID_NUM);
}

Std_ReturnType ChargerCommUser_ChargeIsAllowed(void)
{
    return ChargeM_ChargeIsAllowed();
}

Std_ReturnType ChargerCommUser_TCSendConditionCheck(void)
{
#if CHARGERCOMMUSER_SEND_ALWAYS_EN == STD_ON
    return E_OK;
#else
    Std_ReturnType res = E_NOT_OK;
    Charge_ChargeType type;

    type = ChargerCommUser_GetCurrentChargeType();
    if (ChargerCommUser_innerData.startFlag == TRUE)
    {
        if (type != CHARGE_TYPE_NONE)
        {
            if (ChargeConnectM_GetConnectType() == type)
            {
                if (ChargerComm_GetCurrentRecStage() == CHARGERCOMM_STAGE_USER_TC)
                {
                    res = E_OK;
                }
            }
        }
    }
    else
    {
        if (ChargerCommUser_MsgInnerData.isNeedToSendStop)
        {
            res = E_OK;
        }
    }
    return res;
#endif
}

// Std_ReturnType ChargerCommUser_TC2SendConditionCheck(void)
// {
//     Std_ReturnType res = E_NOT_OK;
//     uint16 temp;
//     uint16 index = 0U;
//     Diagnosis_LevelType diaChgLevel, diaLevel;
//     static uint8 count = 0U;

//     temp = 0U;
//     // 单体过压
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HV);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HV);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 0);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 0);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 0);
//     }
//     else
//     {

//     }
//     //单体欠压
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LV);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 2);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 2);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 2);
//     }
//     else
//     {

//     }
//     //总压过压
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HTV);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HTV);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 4);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 4);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 4);
//     }
//     else
//     {

//     }
//     //总压欠压
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LTV);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 6);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 6);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 6);
//     }
//     else
//     {

//     }
//     WRITE_LT_UINT8(ChargerCommUser_MsgInnerData.faultBuff, index, temp);
//     temp = 0U;
//     //单体压差过大
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DV);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DV);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 0);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 0);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 0);
//     }
//     else
//     {

//     }
//     //放电过流
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_SP_OC);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 2);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 2);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 2);
//     }
//     else
//     {

//     }
//     //充电过流
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_AC_CHG_OC);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DC_CHG_OC);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_FB_OC);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 4);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 4);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 4);
//     }
//     else
//     {

//     }
//     //温度过高
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 6);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 6);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 6);
//     }
//     else
//     {

//     }
//     WRITE_LT_UINT8(ChargerCommUser_MsgInnerData.faultBuff, index, temp);
//     temp = 0U;
//     //温度过低
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LT);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 0);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 0);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 0);
//     }
//     else
//     {

//     }
//     //温差过大
//     diaChgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DT);
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DT);
//     if (diaChgLevel >= diaLevel)
//     {
//         diaLevel = diaChgLevel;
//     }
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 2);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 2);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 2);
//     }
//     else
//     {

//     }
//     //soc过低
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LSOC);
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 4);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 4);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 4);
//     }
//     else
//     {

//     }
//     //绝缘故障
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK);
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 6);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 6);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 6);
//     }
//     else
//     {

//     }
//     WRITE_LT_UINT8(ChargerCommUser_MsgInnerData.faultBuff, index, temp);
//     temp = 0U;
//     //高压互锁故障
//     //外部通信故障
//     //内部通信故障
//     diaLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_INTER_COMM);
//     if (diaLevel >= DIAGNOSIS_LEVEL_THIRD)
//     {
//         temp |= ((uint16)1U << 4);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_SECOND)
//     {
//         temp |= ((uint16)2U << 4);
//     }
//     else if (diaLevel == DIAGNOSIS_LEVEL_FIRST)
//     {
//         temp |= ((uint16)3U << 4);
//     }
//     else
//     {

//     }
//     WRITE_LT_UINT8(ChargerCommUser_MsgInnerData.faultBuff, index, temp);
//     WRITE_LT_UINT16(ChargerCommUser_MsgInnerData.faultBuff, index, 0xFFFFU);
//     WRITE_LT_UINT16(ChargerCommUser_MsgInnerData.faultBuff, index, 0xFFFFU);

//     if (ChargerCommUser_MsgInnerData.faultBuff[0] ||
//         ChargerCommUser_MsgInnerData.faultBuff[1] ||
//         ChargerCommUser_MsgInnerData.faultBuff[2] ||
//         ChargerCommUser_MsgInnerData.faultBuff[3])
//     {
//         res = E_OK;
//     }
//     if (res == E_NOT_OK)
//     {
//         if (count < 3U)
//         {
//             res = E_OK;
//         }
//         else
//         {
//             count = 5U;
//         }
//         count ++;
//     }
//     else
//     {
//         count = 0U;
//     }
//     return res;
// }

static Std_ReturnType ChargerCommUser_TCRecConditionCheck(void)
{
    uint8 flag = 1U;
    Std_ReturnType res = E_NOT_OK;
    Charge_ChargeType type, userType;

    ChargerCommUser_SetCommunication(TRUE);
    ChargerComm_SetProtocol(CHARGERCOMM_PROTOCOL_USER);
    ChargerCommUser_UpdateCurrentChargeType();
    userType = ChargerCommUser_GetCurrentChargeType();
    if (ChargerConnectM_GetConnectMode(userType) == CHARGECONNECTM_CONNECT_COMMUNICATION)
    {
        // ChargerCommUser_ChargerEnable();
        ChargerComm_SetCommunicationStatus(TRUE);
    }
    else
    {
        if (ChargeConnectM_GetConnectType() != userType)
        {
            flag = 0U;
        }
    }
    if (flag != 0U)
    {
        if (ChargerCommUser_innerData.startFlag == TRUE)
        {
            type = ChargerComm_GetChargeType();
            if (type == CHARGE_TYPE_NONE || type == userType)
            {
                res = E_OK;
            }
        }
    }
    return res;
}

void ChargerCommUser_GetTCDataCbk(uint8 *Buffer, uint16 *Length)
{
    VALIDATE_PTR(Buffer, CHARGERCOMMUSER_MESSAGES_API_ID_GetTCDataCbk);
    VALIDATE_PTR(Length, CHARGERCOMMUSER_MESSAGES_API_ID_GetTCDataCbk);

    if (ChargerCommUser_innerData.startFlag == TRUE)
    {
        getTCChgCtlData(Buffer, Length);
    }
    else
    {
        getTCChgStopData(Buffer, Length);
    }
}

void ChargerCommUser_ReceiveTCCbk(const uint8 *Buffer, uint16 Length)
{
    uint16 index = 0U;
    uint16 temp;
    // ChargerComm_StageType stage;
    VALIDATE_PTR(Buffer, CHARGERCOMMUSER_MESSAGES_API_ID_ReceiveTCCbk);

    // if (ChargerCommUser_TCRecConditionCheck() == E_OK)
    {
        // condition check
        // stage = ChargerComm_GetCurrentRecStage();
        // if(stage == CHARGERCOMM_STAGE_USER_TC)
        {
            ChargerCommUser_CommStart();
            if(Length >= 5U)
            {
                ChargerComm_SetCommunicationStatus(TRUE);
                // 充电机输出电压
                ChargerComm_SetChargerOutputHV(READ_LT_UINT16(Buffer, index));
                // 充电机输出电流
                temp = READ_LT_UINT16(Buffer, index);
                ChargerComm_SetChargerOutputCurrent((Current_CurrentType)temp);
                // 充电机充电状态
                if(temp > 0U)
                {
                    ChargerComm_SetChargingStatus(TRUE);
                }
                else
                {
                    ChargerComm_SetChargingStatus(FALSE);
                }
                // 充电机硬件故障
                temp = READ_LT_UINT8(Buffer, index);
                if (temp & 0x01U)
                {
                    (void)ChargerComm_SetChargeFaultWithIndex(CHARGERCOMM_CHR_HARDWARE_FAULT_INDEX, 0x01U);
                }
                else
                {
                    (void)ChargerComm_ClearChargerFaultWithIndex(CHARGERCOMM_CHR_HARDWARE_FAULT_INDEX);
                }
                // 充电机温度异常
                if (temp & 0x02U)
                {
                    (void)ChargerComm_SetChargeFaultWithIndex(CHARGERCOMM_CHR_OVER_TEMPERATURE_FAULT_INDEX, 0x01U);
                }
                else
                {
                    (void)ChargerComm_ClearChargerFaultWithIndex(CHARGERCOMM_CHR_OVER_TEMPERATURE_FAULT_INDEX);
                }
                // 充电机输入电压异常
                if (temp & 0x04U)
                {
                    (void)ChargerComm_SetChargeFaultWithIndex(CHARGERCOMM_CHR_INPUT_VOLT_FAULT_INDEX, 0x01U);
                }
                else
                {
                    (void)ChargerComm_ClearChargerFaultWithIndex(CHARGERCOMM_CHR_INPUT_VOLT_FAULT_INDEX);
                }
                // 充电机运行（启动）状态
                if ((temp & 0x08U))
                {
                    (void)ChargerComm_SetChargeFaultWithIndex(CHARGERCOMM_CHR_RUN_STATUS_INDEX, 0x01U);
                }
                else
                {
                    (void)ChargerComm_ClearChargerFaultWithIndex(CHARGERCOMM_CHR_RUN_STATUS_INDEX);
                }
                // BMS通信故障
                if ((temp & 0x10U))
                {
                    (void)ChargerComm_SetChargeFaultWithIndex(CHARGERCOMM_CHR_COMM_ABORT_WITH_BMS_INDEX, 0x01U);
                }
                else
                {
                    (void)ChargerComm_ClearChargerFaultWithIndex(CHARGERCOMM_CHR_COMM_ABORT_WITH_BMS_INDEX);
                }
            }
        }
    }
#if ( CHARGERCOMMUSER_DEV_ERROR_DETECT == STD_ON )
cleanup:
    return;
#endif
}

static void getTCChgCtlData(uint8 *Buffer, uint16 *Length)
{
    Std_ReturnType flag;
    uint16 temp;
    uint16 index = 0U;
    App_Tv100mvType Volt;
    Current_CurrentType current, current_max;
    RelayM_FunctionType relayType;
    Charge_ChargeType type = ChargeConnectM_GetConnectType();

    if (type == CHARGE_TYPE_DC)
    {
        relayType = ChargerComm_ConfigInfo.DC_RelayType;
    }
    else
    {
        relayType = ChargerComm_ConfigInfo.AC_RelayType;
    }
    flag = ChargerCommUser_ChargeIsAllowed();
    if (flag == E_OK)
    {
        if (ChargeConnectM_ELIsNeeding())
        {
            if (EL_GetDriveState(0U) != EL_LOCK)
            {
                flag = E_NOT_OK;
            }
            else if (EL_GetActualState(0U) != EL_LOCK)
            {
                flag = E_NOT_OK;
            }
            else
            {
            }
        }
    }
    // 充电需求总压
    Volt = UserStrategy_GetChargeVoltMax();
    // 充电需求电流
    if (flag == E_NOT_OK)
    {
        current = 0;
        Volt = 0U;
    }
    else
    {
        current = UserStrategy_GetChargeCurrentMax();
    }
    if (current < 0)
    {
        current = 0;
    }
    current_max = ChargerCommUser_GetGBSignalCurrentMax();
    if(current > current_max)
    {
        current = current_max;
    }
    if (!UserStartegy_GBChargeRelayIsReady(relayType))
    {
        current = 0;
        Volt = 0U;
    }
    ChargerComm_SetChargeVoltMax(Volt);
    WRITE_LT_UINT16(Buffer, index, Volt);
    ChargerComm_SetChargeCurrentMax(current);
    WRITE_LT_UINT16(Buffer, index, current);
    // // 充电允许
    // if (flag == E_NOT_OK)
    // {
    //     flag = 1U;
    //     ChargerCommUser_MsgInnerData.isNeedToSendStop = FALSE;
    // }
    // else
    // {
    //     flag = 0U;
    //     ChargerCommUser_MsgInnerData.isNeedToSendStop = TRUE;
    // }
    // WRITE_LT_UINT8(Buffer, index,  flag);
    // 状态标志0
    temp = 0U;
    if (flag == E_NOT_OK)
    {
        flag = 1U;
        temp |= (uint16)1U << 7;
        ChargerCommUser_MsgInnerData.isNeedToSendStop = FALSE;
    }
    else
    {
        flag = 0U;
        ChargerCommUser_MsgInnerData.isNeedToSendStop = TRUE;
    }
    flag = Diagnosis_StartDiagIsFinish();
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_CHG_HT) == CHARGEM_CHARGE_DISABLE)
    {
        temp |= (uint16)1U << 1;
    }
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_CHG_LT) == CHARGEM_CHARGE_DISABLE)
    {
        temp |= (uint16)1U << 2;
    }
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_AC_CHG_OC) == CHARGEM_CHARGE_DISABLE)
    {
        temp |= (uint16)1U << 3;
    }
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_LEAK) == CHARGEM_CHARGE_DISABLE)
    {
        temp |= (uint16)1U << 4;
    }
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_CHARGER_COMM) == CHARGEM_CHARGE_DISABLE)
    {
        temp |= (uint16)1U << 5;
    }
    if (ChargeM_StartDiagIsNormal() == E_NOT_OK && flag == TRUE)
    {
        temp |= (uint16)1U << 6;
    }
    else if (ChargeM_DiagnosisIsFaultActionExcludeItem(DIAGNOSIS_ITEM_FULL_CHARGE) == E_OK)
    {
        temp |= (uint16)1U << 6;
    }
    else if (ChargeM_OthersFaultIsNormal() == E_NOT_OK)
    {
        temp |= (uint16)1U << 6;
    }
    else
    {
    }
    WRITE_LT_UINT8(Buffer, index,  temp);
    // 状态标志1
    temp = 0U;
    if (Soc_Get() == 1000U)
    {
        temp = 1U;
    }
    WRITE_LT_UINT8(Buffer, index,  temp);
    // 电池容量
    temp = (uint16)CAP_TO_AH(Soc_GetHighPrecisionLeftCap());
    temp = temp * 45U / 44U;
    WRITE_LT_UINT16(Buffer, index, temp);
    *Length = index;
#if ( CHARGERCOMMUSER_DEV_ERROR_DETECT == STD_ON )
cleanup:
#endif
    return;
}

static void getTCChgStopData(uint8 *Buffer, uint16 *Length)
{
    uint16 flag;
    uint16 index = 0U;

    ChargerCommUser_MsgInnerData.isNeedToSendStop = FALSE;

    // 充电需求总压
    ChargerComm_SetChargeVoltMax(0U);
    WRITE_LT_UINT16(Buffer, index, 0U);
    // 充电需求电流
    ChargerComm_SetChargeCurrentMax(0);
    WRITE_LT_UINT16(Buffer, index, 0U);
    // 状态标志
    flag = 0U;
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_CHG_HT) == CHARGEM_CHARGE_DISABLE)
    {
        flag |= (uint16)1U << 1;
    }
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_CHG_LT) == CHARGEM_CHARGE_DISABLE)
    {
        flag |= (uint16)1U << 2;
    }
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_AC_CHG_OC) == CHARGEM_CHARGE_DISABLE)
    {
        flag |= (uint16)1U << 3;
    }
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_LEAK) == CHARGEM_CHARGE_DISABLE)
    {
        flag |= (uint16)1U << 4;
    }
    if (ChargeM_GetDiagnosisChargeCtlFlag(DIAGNOSIS_ITEM_CHARGER_COMM) == CHARGEM_CHARGE_DISABLE)
    {
        flag |= (uint16)1U << 5;
    }
    if (ChargeM_DiagnosisIsFaultActionExcludeItem(DIAGNOSIS_ITEM_FULL_CHARGE) == E_OK)
    {
        flag |= (uint16)1U << 6;
    }
    else if (ChargeM_StartDiagIsNormal() == E_NOT_OK && ChargerCommUser_MsgInnerData.isNeedToSendStop == TRUE)
    {
        flag |= (uint16)1U << 6;
    }
    else if (ChargeM_OthersFaultIsNormal() == E_NOT_OK)
    {
        flag |= (uint16)1U << 6;
    }
    else
    {
    }
    flag |= (uint16)1U << 7;
    WRITE_LT_UINT8(Buffer, index,  flag);
    // 状态标志1
    flag = 0U;
    if (Soc_Get() == 1000U)
    {
        flag = 1U;
    }
    WRITE_LT_UINT8(Buffer, index,  flag);
    // 电池容量
    flag = (uint16)CAP_TO_AH(Soc_GetHighPrecisionLeftCap());
    WRITE_LT_UINT16(Buffer, index, flag);
    *Length = index;
#if ( CHARGERCOMMUSER_DEV_ERROR_DETECT == STD_ON )
cleanup:
#endif
    return;
}

void ChargerCommUser_GetTC1DataCbk(uint8 *Buffer, uint16 *Length)
{
    uint16 index = 0U;
    uint8 soc;
    uint16 temp;
    uint32 time;
    Current_CurrentType current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);

    // 总电压
    temp = Statistic_GetBcu100mvTotalVoltage();
    WRITE_LT_UINT16(Buffer, index, temp);
    // 总电流
    temp = (uint16)(4000 - current);
    WRITE_LT_UINT16(Buffer, index, temp);
    // 剩余容量
    temp = Soc_Get();
    soc = (uint8)SOC_TO_UINT8(temp);
    WRITE_LT_UINT8(Buffer, index, soc);
    // 预留
    WRITE_LT_UINT8(Buffer, index, 0xFFU);
    // 放电计时
    time = Statistic_GetCumuDchgTime();
    temp = (uint16)(time / 3600U);
    WRITE_LT_UINT16(Buffer, index, temp);

    *Length = index;
}

void ChargerCommUser_GetTC2DataCbk(uint8 *Buffer, uint16 *Length)
{
    (void)memcpy(Buffer, ChargerCommUser_MsgInnerData.faultBuff, 8U);
    *Length = 8U;
}

void ChargerCommUser_RecTCTimeoutCbk(void)
{
    ChargerCommUser_SetCommunication(FALSE);
    ChargerComm_ClrChargerStatus();
    if (ChargerConnectM_GetConnectMode(ChargerCommUser_GetCurrentChargeType()) == CHARGECONNECTM_CONNECT_COMMUNICATION)
    {
        // ChargerCommUser_CommStop();
    }
    ChargerComm_SetCommunicationStatus(FALSE);
}
