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
#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
#include "Modules.h"
#endif

#define VCUCOMM_MESSAGES_E_PARAM_INVALID_PTR            0U

#define VCUCOMM_MESSAGES_API_ID_ReceiveCbk              0U
#define VCUCOMM_MESSAGES_API_ID_GetMsgData0x000002F4Cbk 1U
#define VCUCOMM_MESSAGES_API_ID_GetMsgData0x000004F4Cbk 2U
#define VCUCOMM_MESSAGES_API_ID_GetMsgData0x000005F4Cbk 3U
#define VCUCOMM_MESSAGES_API_ID_GetMsgData0x000007F4Cbk 4U


#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
#define VALIDATE_PTR(_ptr, _api) \
    if (_ptr == NULL) { \
        Det_ReportError(MODULE_ID_VCUCOMM_MESSAGES, 0U, _api, VCUCOMM_MESSAGES_E_PARAM_INVALID_PTR); \
        goto cleanup; \
    }
#else
#define VALIDATE_PTR(_ptr, _api)
#endif

#define VCUCOMM_CURRENT_OFFSET      (4000)
#define VCUCOMM_VOLT_4_DISPLAY(volt)    (VOLT_4_DISPLAY(volt) < 10000U ? VOLT_4_DISPLAY(volt) : 10000U)

VcuComm_MsgDataType VcuComm_MsgData;

static uint16 VcuComm_bms2VcuCurrent(Current_CurrentType current);

void VcuComm_MessageInit(Async_LooperType *looper)
{
    (void)looper;
    (void)VcuComm_SendIPduRegister(&VcuComm_IPduSendConfigInfo[0], VcuComm_SendIPduNum);
}

void VcuComm_GetMsgData0x000002F4Cbk(uint8 *Buffer, uint16 *Length)
{
    Current_CurrentType current;
    uint16 index = 0U, temp;

    VALIDATE_PTR(Buffer, VCUCOMM_MESSAGES_API_ID_GetMsgData0x000002F4Cbk);
    VALIDATE_PTR(Length, VCUCOMM_MESSAGES_API_ID_GetMsgData0x000002F4Cbk);
    /**< 总压 */
    temp = Statistic_GetBcu100mvTotalVoltage();
    temp = VOLT_4_DISPLAY(temp);
    WRITE_LT_UINT16(Buffer, index, temp);
    /**< 总电流 */
    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    temp = VcuComm_bms2VcuCurrent(current);
    WRITE_LT_UINT16(Buffer, index, temp);
    /**< SOC */
    temp = Soc_Get();
    temp = (uint16)SOC_TO_PERCENT(temp);
    WRITE_LT_UINT8(Buffer, index,  temp);
    /**< 保留 */
    WRITE_LT_UINT8(Buffer, index,  0x00U);
    WRITE_LT_UINT16(Buffer, index,  0x00U);

    *Length = index;
#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
cleanup:
#endif
    return;
}

void VcuComm_GetMsgData0x000004F4Cbk(uint8 *Buffer, uint16 *Length)
{
    uint16 temp;
    uint16 index = 0U;

    VALIDATE_PTR(Buffer, VCUCOMM_MESSAGES_API_ID_GetMsgData0x000004F4Cbk);
    VALIDATE_PTR(Length, VCUCOMM_MESSAGES_API_ID_GetMsgData0x000004F4Cbk);
    /**< 最高单体电压 */
    temp = Statistic_GetBcuHvMax();
    temp = VCUCOMM_VOLT_4_DISPLAY(temp);
    WRITE_LT_UINT16(Buffer, index, temp);
    /**< 最高单体电压串号 */
    temp = CellDataM_VoltageIndexToLogic(Statistic_GetBcuHvSlaveNum(0U), Statistic_GetBcuHvIndex(0U)) + 1U;
    if (temp > 0xFFU)
    {
        temp = 0xFFU;
    }
    WRITE_LT_UINT8(Buffer, index, temp);
    /**< 最低单体电压 */
    temp = Statistic_GetBcuLvMax();
    temp = VCUCOMM_VOLT_4_DISPLAY(temp);
    WRITE_LT_UINT16(Buffer, index, temp);
    /**< 最低单体电压串号 */
    temp = CellDataM_VoltageIndexToLogic(Statistic_GetBcuLvSlaveNum(0U), Statistic_GetBcuLvIndex(0U)) + 1U;
    if (temp > 0xFFU)
    {
        temp = 0xFFU;
    }
    WRITE_LT_UINT8(Buffer, index, temp);
    /**< 保留 */
    WRITE_LT_UINT16(Buffer, index,  0x00U);

    *Length = index;
#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
cleanup:
#endif
    return;
}

void VcuComm_GetMsgData0x000005F4Cbk(uint8 *Buffer, uint16 *Length)
{
    uint16 temp;
    uint16 index = 0U;

    VALIDATE_PTR(Buffer, VCUCOMM_MESSAGES_API_ID_GetMsgData0x00005F4Cbk);
    VALIDATE_PTR(Length, VCUCOMM_MESSAGES_API_ID_GetMsgData0x00005F4Cbk);

    /**< 最高温度 */
    temp = Statistic_GetBcuHtMax();
    temp = TEMP_4_DISPLAY(temp);
    WRITE_LT_UINT8(Buffer, index, temp);
    /**< 最高温度串号 */
    temp = CellDataM_TemperatureIndexToLogic(Statistic_GetBcuHtSlaveNum(0U), Statistic_GetBcuHtIndex(0U)) + 1U;
    if (temp > 0xFFU)
    {
        temp = 0xFFU;
    }
    WRITE_LT_UINT8(Buffer, index, temp);
    /**< 最低温度 */
    temp = Statistic_GetBcuLtMax();
    temp = TEMP_4_DISPLAY(temp);
    WRITE_LT_UINT8(Buffer, index, temp);
    /**< 最低温度串号 */
    temp = CellDataM_TemperatureIndexToLogic(Statistic_GetBcuLtSlaveNum(0U), Statistic_GetBcuLtIndex(0U)) + 1U;
    if (temp > 0xFFU)
    {
        temp = 0xFFU;
    }
    WRITE_LT_UINT8(Buffer, index, temp);
    /**< 平均温度 */
    temp = Statistic_GetBcuAverageTemperature();
    temp = TEMP_4_DISPLAY(temp);
    WRITE_LT_UINT8(Buffer, index, temp);
    /**< 保留 */
    WRITE_LT_UINT8(Buffer, index,  0x00U);
    WRITE_LT_UINT16(Buffer, index,  0x00U);

    *Length = index;
#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
cleanup:
#endif
    return;
}

static Diagnosis_LevelType alarmLevel2VcuLevel(Diagnosis_LevelType level)
{
    Diagnosis_LevelType vcu_level;

    if (level >= DIAGNOSIS_LEVEL_THIRD)
    {
        vcu_level = 1U;
    }
    else if (level == DIAGNOSIS_LEVEL_FIRST)
    {
        vcu_level = 3U;
    }
    else
    {
        vcu_level = level;
    }
    return vcu_level;
}

void VcuComm_GetMsgData0x000007F4Cbk(uint8 *Buffer, uint16 *Length)
{
    uint16 temp;
    Diagnosis_LevelType dchgLevel, chgLevel;
    uint16 index = 0U;

    VALIDATE_PTR(Buffer, VCUCOMM_MESSAGES_API_ID_GetMsgData0x00007F4Cbk);
    VALIDATE_PTR(Length, VCUCOMM_MESSAGES_API_ID_GetMsgData0x00007F4Cbk);

    temp = 0U;
    /**< 单体过充 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HV);
    chgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HV);
    if (chgLevel > dchgLevel)
    {
        dchgLevel = chgLevel;
    }
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 0);

    /**< 单体过放 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV);
    chgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LV);
    if (chgLevel > dchgLevel)
    {
        dchgLevel = chgLevel;
    }
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 2);

    /**< 总压过高 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HTV);
    chgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HTV);
    if (chgLevel > dchgLevel)
    {
        dchgLevel = chgLevel;
    }
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 4);

    /**< 总压过低 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV);
    chgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LTV);
    if (chgLevel > dchgLevel)
    {
        dchgLevel = chgLevel;
    }
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 6);

    /**< 压差过大 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DV);
    chgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DV);
    if (chgLevel > dchgLevel)
    {
        dchgLevel = chgLevel;
    }
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 8);

    /**< 放电过流 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC);
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 10);

    /**< 充电过流 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_AC_CHG_OC);
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 12);

    /**< 温度过高 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT);
    chgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT);
    if (chgLevel > dchgLevel)
    {
        dchgLevel = chgLevel;
    }
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 14);
    WRITE_LT_UINT16(Buffer, index, temp);

    temp = 0U;
    /**< 温度过低 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT);
    chgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LT);
    if (chgLevel > dchgLevel)
    {
        dchgLevel = chgLevel;
    }
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 0);

    /**< 温差过大 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DT);
    chgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DT);
    if (chgLevel > dchgLevel)
    {
        dchgLevel = chgLevel;
    }
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 2);

    /**< SOC低 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LSOC);
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 4);

    /**< 绝缘过低 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK);
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 6);

    /**< 高压互锁故障 */
    /**< 外部通信故障 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_VCU_COMM);
    chgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHARGER_COMM);
    if (chgLevel > dchgLevel)
    {
        dchgLevel = chgLevel;
    }
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 10);

    /**< 内部通信故障 */
    dchgLevel = Diagnosis_GetLevel(DIAGNOSIS_ITEM_INTER_COMM);
    dchgLevel = alarmLevel2VcuLevel(dchgLevel);
    temp |= (uint16)((uint16)dchgLevel << 12);

    WRITE_LT_UINT16(Buffer, index, temp);

    /**< 保留 */
    WRITE_LT_UINT16(Buffer, index, 0x00U);
    WRITE_LT_UINT16(Buffer, index, 0x00U);

    *Length = index;
#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
cleanup:
#endif
    return;
}


void VcuComm_ReceiveCbk(uint8 *Buffer, uint16 Length)
{
    VALIDATE_PTR(Buffer, VCUCOMM_MESSAGES_API_ID_ReceiveCbk);

    VcuComm_SetCommunicationStatus(TRUE);
    VcuComm_ClrCommAbortMessageFlag((uint16)VCUCOMM_RX_IPDU_MSG_0x12345678);
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
    VcuComm_SetCommAbortMessageFlag((uint16)VCUCOMM_RX_IPDU_MSG_0x12345678);
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

static uint16 VcuComm_bms2VcuCurrent(Current_CurrentType current)
{
    uint16 vcu_current;
    Current_CurrentType cur_offset = VCUCOMM_CURRENT_OFFSET;

    if (!CurrentM_IsValidCurrent(current))
    {
        current = 0;
    }
    else
    {
        current *= -1;
    }
    if (current < (-1) * cur_offset)
    {
        vcu_current = 0U;
    }
    else if (current > CURRENT_S_100MA_FROM_A(1000))
    {
        vcu_current = CURRENT_100MA_FROM_A(1400U);
    }
    else
    {
        vcu_current = (uint16)current + (uint16)cur_offset;
    }
    return vcu_current;
}

