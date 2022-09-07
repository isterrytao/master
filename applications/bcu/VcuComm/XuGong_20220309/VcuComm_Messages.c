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
#include <String.h>
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
#include "DisChargeM.h"
#include "PowerM.h"
#include "RelayM_Lcfg.h"
#include "ChargeM.h"
#include "DatetimeM.h"
#include "HvProcess_Chg.h"
#include "HvProcess_Dchg.h"
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

void VcuComm_GetStatusMsg_0x2F0(uint8 *buf, uint16 *Length) {
    sint16 sval;
    uint16 uval;
    uint16 index = 0U;

    uval = Statistic_GetBcu100mvTotalVoltage();
    WRITE_LT_UINT16(buf, index, uval);
    sval = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    if (CurrentM_IsValidCurrent(sval))
    {
        if (sval > 6000)
        {
            sval = 6000;
        }
        else if (sval < -6000)
        {
            sval = -6000;
        }
        else
        {

        }
        sval = sval + 6000;
        uval = (uint16)sval;
    }
    else
    {
        uval = 0xFFFFU;
    }
    WRITE_LT_UINT16(buf, index, uval);
    uval = Soh_Get();
    uval = (uint16)SOC_TO_PERCENT(uval);
    WRITE_LT_UINT8(buf, index, uval);
    WRITE_LT_UINT8(buf, index, 0xFFU);
    WRITE_LT_UINT16(buf, index, 0xFFFFU);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x2F1(uint8 *buf, uint16 *Length) {
    uint16 index = 0U, temp = 0U;
    Diagnosis_LevelType level;

    // 最大持续放电电流 发送额定放电电流
    if (!CHARGECONNECTM_IS_CONNECT())
    {
        if (DischargeM_DischargeIsAllowed() == E_OK)
        {
            temp = BatteryInfo_BaseConfigInfo.NominalDischargeCurrent;
        }
    }
    WRITE_LT_UINT16(buf, index, temp);
    temp = 0U;
    // 最大脉冲放电电流 发送瞬时放电最高故障等级触发阈值
    level = DischargeM_GetDischargeDisableLevel(DIAGNOSIS_ITEM_SP_OC, FALSE);
    if (level >= 1U)
    {
        level -= 1U;
    }
    if (!CHARGECONNECTM_IS_CONNECT())
    {
        if (DischargeM_DischargeIsAllowed() == E_OK)
        {
            temp = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_SP_OC, level, 0U);
        }
    }
    WRITE_LT_UINT16(buf, index, temp);
    // 最大回馈电流值 根据当前充电连接状态发送快慢充额定电流
    if (ChargeConnectM_GetConnectType() == CHARGE_TYPE_AC)
    {
        temp = BatteryInfo_BaseConfigInfo.NominalACCurrent;
    }
    else
    {
        temp = BatteryInfo_BaseConfigInfo.NominalDCCurrent;
    }
    WRITE_LT_UINT16(buf, index, temp);
    temp = 0U;
    if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_MAIN) == RELAYM_ACTUAL_ON)
    {
        temp |= (uint16)0x1U << 0;
    }
#ifdef RELAYM_FN_SELF_LOCK
    if (RelayM_GetActualStatus(RELAYM_FN_SELF_LOCK) == RELAYM_ACTUAL_ON)
    {
        temp |= (uint16)0x1U << 1;
    }
#endif
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_AC_CHARGE) == RELAYM_ACTUAL_ON)
    {
        temp |= (uint16)0x1U << 2;
    }
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
    if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_DC_CHARGE) == RELAYM_ACTUAL_ON)
    {
        temp |= (uint16)0x1U << 2;
    }
#endif
#ifdef RELAYM_FN_CHARGE
    if (RelayM_GetActualStatus(RELAYM_FN_CHARGE) == RELAYM_ACTUAL_ON)
    {
        temp |= (uint16)0x1U << 2;
    }
#endif
#ifdef RELAYM_FN_PRECHARGE
    if (RelayM_GetActualStatus(RELAYM_FN_PRECHARGE) == RELAYM_ACTUAL_ON)
    {
        temp |= (uint16)0x1U << 3;
    }
#endif
    WRITE_LT_UINT8(buf, index, temp);
    WRITE_LT_UINT8(buf, index, 0xFFU);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x2F2(uint8 *buf, uint16 *Length) {
    uint16 index = 0U, temp;
    uint32 cap;
    cap = Soc_GetDchgPower();
    cap *= 10U;
    WRITE_LT_UINT16(buf, index, cap);
    cap = Soc_GetCumuChgPower();
    WRITE_LT_UINT32(buf, index, cap);
    temp = Statistic_GetBcuBatteryDischargeCount();
    WRITE_LT_UINT16(buf, index, temp);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x2F3(uint8 *buf, uint16 *Length) {
    App_VoltageType val16;
    uint8 val8;
    uint16 index = 0U;
    val16 = Statistic_GetBcuHv(0U);
    WRITE_LT_UINT16(buf, index, val16);
    val16 = Statistic_GetBcuLv(0U);
    WRITE_LT_UINT16(buf, index, val16);
    val8 = Statistic_GetBcuHt(0U);
    if (CellDataM_TemperatureIsValid((uint16)val8))
    {
        val8 = (uint8)TEMP_TO_40_OFFSET(val8);
    }
    else
    {
        val8 = 0xFFU;
    }
    WRITE_LT_UINT8(buf, index, val8);
    val8 = Statistic_GetBcuLt(0U);
    if (CellDataM_TemperatureIsValid((uint16)val8))
    {
        val8 = (uint8)TEMP_TO_40_OFFSET(val8);
    }
    else
    {
        val8 = 0xFFU;
    }
    WRITE_LT_UINT8(buf, index, val8);
    WRITE_LT_UINT16(buf, index, 0xFFFFU);
    *Length = index;
}

static boolean VcuComm_IsOrtherFault(void)
{
    boolean res = FALSE;

    if (CHARGECONNECTM_IS_CONNECT())
    {
        if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LV) >= DIAGNOSIS_LEVEL_SECOND)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LTV) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DV) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DT) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_AC_CHG_OC) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DC_CHG_OC) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHARGER_COMM) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHGSCKTMP_DC_NEGATIVE) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHGSCKTMP_DC_POSITIVE) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHGSCKTMP_AC_AL) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHGSCKTMP_AC_BN) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHGSCKTMP_AC_C) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHGSCKTMP_ABNORMAL) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_HEAT_FAULT) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else
        {

        }
    }
    else
    {
        if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DV) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DT) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_LSOC) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else
        {

        }
    }
    if (!res)
    {
        if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_VOLT_LINE) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_TEMP_LINE) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_BMS_INIT_FAILURE) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_BMS_INIT_FAILURE) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_VCU_COMM) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_RELAY_ABNORMAL) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CURRENT_ABNORMAL) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_PRECHARGE_FAILURE) >= DIAGNOSIS_LEVEL_THIRD)
        {
            res = TRUE;
        }
        else
        {}
    }

    return res;
}

void VcuComm_GetStatusMsg_0x244(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U, temp = 0U;
    if (!CHARGECONNECTM_IS_CONNECT())
    {
        if (DischargeM_DischargeIsAllowed() == E_OK)
        {
            temp |= (uint16)0x1U << 0;
        }
    }
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 1;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 1;
    }
    else
    {}
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 2;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_SP_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 2;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_FB_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 2;
    }
    else
    {

    }
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x2U << 3;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_FB_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x2U << 3;
    }
    // else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK) >= DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= (uint16)0x1U << 3;
    // }
    else
    {}
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 5;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HTV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 5;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 5;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HTV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 5;
    }
    else
    {}
    if (VcuComm_IsOrtherFault())
    {
        temp |= (uint16)0x1U << 6;
    }
    WRITE_LT_UINT8(buf, index, temp);
    temp = 0U;
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 0;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LT) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x1U << 0;
    }
    else
    {}
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x2U << 1;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT) >= DIAGNOSIS_LEVEL_THIRD)
    {
        temp |= (uint16)0x2U << 1;
    }
    // else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT) >= DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= (uint16)0x1U << 1;
    // }
    // else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT) >= DIAGNOSIS_LEVEL_FIRST)
    // {
    //     temp |= (uint16)0x1U << 1;
    // }
    else
    {}
    WRITE_LT_UINT8(buf, index, temp);
    WRITE_LT_UINT16(buf, index, 0U);
    WRITE_LT_UINT32(buf, index, 0U);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x444(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U, temp ;
    WRITE_LT_UINT32(buf, index, 0U);
    WRITE_LT_UINT16(buf, index, 0U);
    temp = Soc_Get();
    temp = (uint16)SOC_TO_PERCENT(temp);
    WRITE_LT_UINT8(buf, index, temp);
    WRITE_LT_UINT8(buf, index, 0U);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x445(uint8 *buf, uint16 *Length)
{
    uint16 temp, temp1;
    uint16 index = 0U;
    uint32 time;
    Datetime_DatetimeType date;

    (void)ParameterM_EeepRead(PARAMETERM_EEEP_START_CHG_TIME_L_INDEX, &temp);
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_START_CHG_TIME_H_INDEX, &temp1);
    time = (uint32)temp1;
    time = time << 16U;
    time += temp;
    if (Datetime_FromSecond(&date, time) == E_OK && time != 0xFFFFFFFFUL)
    {
        temp = (uint16)date.year;
        temp += 2000U;
        WRITE_LT_UINT16(buf, index,  temp);
        /**< 月 */
        temp = (uint16)date.month;
        WRITE_LT_UINT8(buf, index,  temp);
        /**< 日 */
        temp = (uint16)date.day;
        WRITE_LT_UINT8(buf, index,  temp);
        //时
        temp = (uint16)date.hour;
        WRITE_LT_UINT8(buf, index,  temp);
        //分
        temp = (uint16)date.minute;
        WRITE_LT_UINT8(buf, index,  temp);
        //秒
        temp = (uint16)date.second;
        WRITE_LT_UINT8(buf, index,  temp);
    }
    else
    {
        WRITE_LT_UINT16(buf, index,  0xFFFFU);
        WRITE_LT_UINT16(buf, index,  0xFFFFU);
        WRITE_LT_UINT16(buf, index,  0xFFFFU);
        WRITE_LT_UINT8(buf, index,  0xFFU);
    }
    //保留
    WRITE_LT_UINT8(buf, index,  0U);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x446(uint8 *buf, uint16 *Length)
{
    uint16 temp, temp1;
    uint16 index = 0U;
    uint32 time;
    Datetime_DatetimeType date;

    (void)ParameterM_EeepRead(PARAMETERM_EEEP_STOP_CHG_TIME_L_INDEX, &temp);
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_STOP_CHG_TIME_H_INDEX, &temp1);
    time = (uint32)temp1;
    time = time << 16U;
    time += temp;
    if (Datetime_FromSecond(&date, time) == E_OK && time != 0xFFFFFFFFUL)
    {
        if (HvProcess_IsCharging())
        {
            WRITE_LT_UINT16(buf, index,  0xFFFFU);
            WRITE_LT_UINT16(buf, index,  0xFFFFU);
            WRITE_LT_UINT16(buf, index,  0xFFFFU);
            WRITE_LT_UINT8(buf, index,  0xFFU);
        }
        else
        {
            temp = (uint16)date.year;
            temp += 2000U;
            WRITE_LT_UINT16(buf, index,  temp);
            /**< 月 */
            temp = (uint16)date.month;
            WRITE_LT_UINT8(buf, index,  temp);
            /**< 日 */
            temp = (uint16)date.day;
            WRITE_LT_UINT8(buf, index,  temp);
            //时
            temp = (uint16)date.hour;
            WRITE_LT_UINT8(buf, index,  temp);
            //分
            temp = (uint16)date.minute;
            WRITE_LT_UINT8(buf, index,  temp);
            //秒
            temp = (uint16)date.second;
            WRITE_LT_UINT8(buf, index,  temp);
        }
    }
    else
    {
        WRITE_LT_UINT16(buf, index,  0xFFFFU);
        WRITE_LT_UINT16(buf, index,  0xFFFFU);
        WRITE_LT_UINT16(buf, index,  0xFFFFU);
        WRITE_LT_UINT8(buf, index,  0xFFU);
    }
    //保留
    WRITE_LT_UINT8(buf, index,  0U);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x447(uint8 *buf, uint16 *Length)
{
    uint16 capL, capH;
    uint16 index = 0U;
    uint32 cumuCap;
    uint16 volt = BatteryInfo_BaseConfigInfo.NominalTotalVolt;
    //累积放电电量，KWH
    cumuCap = 0U;
    if (E_OK == ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_DCHG_CAP_L_INDEX, &capL)) {
        if (E_OK == ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_DCHG_CAP_H_INDEX, &capH)) {
            cumuCap = ((uint32)capH << 16) + capL;
        }
    }
    // cumuCap = (cumuCap + 5) / 10;
    cumuCap *= (uint32)BatteryInfo_BaseConfigInfo.NominalTotalVolt;
    if (cumuCap != 0U)
    {
        cumuCap = DIVISION(cumuCap, 10000UL);
    }
    WRITE_LT_UINT32(buf, index, cumuCap);
    //累积充电电量，KWH
    cumuCap = 0U;
    if (E_OK == ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_CHG_CAP_L_INDEX, &capL)) {
        if (E_OK == ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_CHG_CAP_H_INDEX, &capH)) {
            cumuCap = ((uint32)capH << 16) + capL;
        }
    }
    // cumuCap = (cumuCap + 5) / 10;
    cumuCap *= (uint32)BatteryInfo_BaseConfigInfo.NominalTotalVolt;
    if (cumuCap != 0U)
    {
        cumuCap = DIVISION(cumuCap, 10000UL);
    }
    WRITE_LT_UINT32(buf, index, cumuCap);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x448(uint8 *buf, uint16 *Length)
{
    // uint16 timeL, timeH;
    uint16 index = 0U;
    uint32 cumuTime;
    //累积放电时间，单位：小时
    cumuTime = Statistic_GetCumuChgTime();
    // if (E_OK == ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_DCHG_TIME_L_INDEX, &timeL)) {
    //     if (E_OK == ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_DCHG_TIME_H_INDEX, &timeH)) {
    //         cumuTime = ((uint32)timeH << 16) + timeL;
    //     }
    // }
    cumuTime = (cumuTime + 1800) / 3600;
    WRITE_LT_UINT32(buf, index, cumuTime);
    //累积充电时间，单位：小时
    cumuTime = Statistic_GetCumuDchgTime();
    // if (E_OK == ParameterM_EeepRead(PARAMETERM_EEEP_USER_CUMULATIVE_CHG_TIME_L_INDEX, &timeL)) {
    //     if (E_OK == ParameterM_EeepRead(PARAMETERM_EEEP_USER_CUMULATIVE_CHG_TIME_H_INDEX, &timeH)) {
    //         cumuTime = ((uint32)timeH << 16) + timeL;
    //     }
    // }
    cumuTime = (cumuTime + 1800) / 3600;
    WRITE_LT_UINT32(buf, index, cumuTime);
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
