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
#include "stdlib.h"
#include "App_Types.h"
#include "Cpu.h"
#include "Det.h"
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
#include "SystemConnection_Lcfg.h"
#include "VcuComm_Types.h"
#include "PduR_Lcfg.h"
#include "UserStrategy.h"
#include "PowerM.h"
#include "RelayM_Lcfg.h"
#include "ChargeM.h"
#include "ParameterM.h"
#include "AppParameterM.h"
#include "Datetime.h"
#include "DatetimeM.h"
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

void VcuComm_GetStatusMsg_0x1E1(uint8 *buf, uint16 *Length) {
    sint16 sval;
    uint16 uval;
    uint16 index = 0U;

    uval = Statistic_GetBcu100mvTotalVoltage();
    WRITE_BT_UINT16(buf, index, uval);
    sval = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    if (sval == CURRENT_INVALID_VALUE) {
        uval = 0xFFFFU;
    } else if (sval == CURRENT_SHORT_CIRCUIT_VALUE) {
        uval = 0xFFFEU;
    } else if (sval == CURRENT_OPEN_CIRCUIT_VALUE) {
        uval = 0xFFFDU;
    } else if (sval > 10000 || sval < -10000) {
        uval = 0xFFFCU;
    } else {
        uval = (uint16)sval + 10000U;
    }
    WRITE_BT_UINT16(buf, index, uval);
    uval = Soc_Get();
    WRITE_BT_UINT16(buf, index, uval);
    WRITE_BT_UINT16(buf, index, uval);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x1E4(uint8 *buf, uint16 *Length) {
    uint8 val8;
    uint16 index = 0U;
    uint16 val16;

    val8 = 0U;
    val8 |= (uint8)((uint8)ChargeConnectM_GetConnectType() << 4);
    if (ChargerComm_GetChargingStatus())
    {
        if (ChargeM_BatteryChargeIsFinish())
        {
            val8 |= (uint8)((uint8)2U << 6);
        }
        else
        {
            val8 |= (uint8)((uint8)1U << 6);
        }
    }
    // val8 |= (uint8)((uint8)ChargerComm_GetChargingStatus() << 6);
    WRITE_BT_UINT8(buf, index, val8);

    val8 = 0U;
    if (PrechargeM_IsFailure()) {
        val8 |= 0x03U;
    } else if (PrechargeM_IsFinish()) {
        val8 |= 0x02U;
    } else if (PrechargeM_IsStart()) {
        val8 |= 0x01U;
    } else {
    }

    if (Diagnosis_StartDiagIsFinish()) {
        if (Diagnosis_StartDiagIsAllNormal()) {
            val8 |= (uint8)((uint8)1 << 2);
        } else {
            val8 |= (uint8)((uint8)2 << 2);
        }
    }

    if (BalanceM_IsBalance()) {
        val8 |= (uint8)((uint8)1 << 4);
    }
    if (TemperatureM_IsHeating()) {
        val8 |= (uint8)((uint8)1 << 5);
    }
    if (TemperatureM_IsRefrigeration()) {
        val8 |= (uint8)((uint8)1 << 6);
    }

    //高压互锁状态
    if (!UserStrategy_IsDiagHvilAbnormal())
    {
        val8 |= (uint8)((uint8)1 << 7);
    }
    WRITE_BT_UINT8(buf, index, val8);

    val16 = Insu_GetPositive();
    WRITE_BT_UINT16(buf, index, val16);
    val16 = Insu_GetNegative();
    WRITE_BT_UINT16(buf, index, val16);
    val8 = Diagnosis_GetDiagLevelMax();
    WRITE_BT_UINT8(buf, index, val8);
    val8 = 0U;
    if (UserStrategy_DchgIsReady())
    {
        val8 |= (uint8)((uint8)1U << 0);
    }
    // 主从模式
#if SYSTEM_BMU_NUM > 1U
    {
        val8 |= (uint8)((uint8)1U << 2);
    }
#endif
    WRITE_BT_UINT8(buf, index, val8);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x1F5(uint8 *buf, uint16 *Length) {
    static uint8 this_index = 0U;
    uint8 val, goto_flag = 0U;
    uint16 index = 0U;
    Diagnosis_ItemType item;

    if (this_index > DIAGNOSIS_ITEM_DEFAULT_NUM) {
        this_index = 0U;
    }

    item = Diagnosis_IndexToItem(this_index);
    item = Diagnosis_GetNextAlarmingItem(item);
    if (item == DIAGNOSIS_ITEM_INVALID_INDEX) {
        if (this_index == 0U) {
            WRITE_BT_UINT8(buf, index, 0xFFU);
            WRITE_BT_UINT8(buf, index, 0xFFU);
            goto_flag = 1U;
        } else {
            item = Diagnosis_IndexToItem(0U);
            item = Diagnosis_GetNextAlarmingItem(item);
            if (item == DIAGNOSIS_ITEM_INVALID_INDEX) {
                WRITE_BT_UINT8(buf, index, 0xFFU);
                WRITE_BT_UINT8(buf, index, 0xFFU);
                this_index = 0U;
                goto_flag = 1U;
            }
        }
    }
    if (goto_flag == 0U) {
        val = Diagnosis_GetLevel(item);
        WRITE_BT_UINT8(buf, index, item);
        WRITE_BT_UINT8(buf, index, val);
        this_index = Diagnosis_ItemToIndex(item) + 1U;
    }
    for (; index < 8U;) {
        WRITE_BT_UINT8(buf, index, 0xFFU);
    }
    *Length = index;
}

void VcuComm_GetStatusMsg_0x1F3(uint8 *buf, uint16 *Length)
{
    uint16 u16val;
    uint16 index = 0U;

    u16val = (uint16)PowerM_GetCurrent(POWERM_CUR_DCHARGE_PEAK);
    WRITE_BT_UINT16(buf, index, u16val);
    u16val = (uint16)PowerM_GetCurrent(POWERM_CUR_DCHARGE_CONTINUE);
    WRITE_BT_UINT16(buf, index, u16val);
    u16val = (uint16)PowerM_GetCurrent(POWERM_CUR_DCHARGE_FEEDBACK);
    WRITE_BT_UINT16(buf, index, u16val);
    u16val = (uint16)PowerM_GetCurrent(POWERM_CUR_CHARGE_CONTINUE);
    WRITE_BT_UINT16(buf, index, u16val);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x211(uint8 *buf, uint16 *Length)
{
    uint16 u16val;
    uint32 remainchgtime;
    uint16 index = 0U;
    uint16 totalvoltage;
    sint16 current;
    sint32 power = 0;

    /**< 绝缘电阻 */
    u16val = Insu_GetSystem();
    WRITE_BT_UINT16(buf, index, u16val);
    /**< 剩余充电时间 */
    remainchgtime = Statistic_GetRequireChargeTime();
    remainchgtime = S_TO_MIN(remainchgtime);
    if (remainchgtime > 0xFFFFU)
    {
        u16val = 0xFFFFU;
    }
    else
    {
        u16val = (uint16)remainchgtime;
    }
    WRITE_BT_UINT16(buf, index, u16val);
    /**< 保留 */
    //WRITE_BT_UINT8(buf, index, 0xFFU);
    /**< 电池功率 = 总压 * 电流*/
    totalvoltage = Statistic_GetBcu100mvTotalVoltage();
    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    if (CurrentM_IsValidCurrent(current))
    {
        if (Statistic_TotalVoltageIsValid(totalvoltage))
        {
            power = current;
            power *= (sint32)totalvoltage;
            power /= 1000;
            power += 10000;
            if (power < 0)
            {
                power = 0;
            }
        }
    }
    if (power > 65535)
    {
        power = 65535;
    }
    u16val = (uint16)power;
    WRITE_BT_UINT16(buf, index, u16val);
    //保留
    WRITE_BT_UINT16(buf, index, 0xFFFFU);

    *Length = index;
}

void VcuComm_GetStatusMsg_0x352(uint8 *buf, uint16 *Length) {
    App_VoltageType val16;
    uint16 index = 0U;

    val16 = Statistic_GetBcuHv(0U);
    WRITE_BT_UINT16(buf, index, val16);
    val16 = Statistic_GetBcuHvLogicIndex(0U) + 1U;
    WRITE_BT_UINT16(buf, index, val16);
    val16 = Statistic_GetBcuLv(0U);
    WRITE_BT_UINT16(buf, index, val16);
    val16 = Statistic_GetBcuLvLogicIndex(0U) + 1U;
    WRITE_BT_UINT16(buf, index, val16);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x353(uint8 *buf, uint16 *Length) {
    uint16 val16;
    uint8 val8;
    uint16 index = 0U;

    val8 = Statistic_GetBcuHt(0U);
    WRITE_BT_UINT8(buf, index, val8);
    val16 = Statistic_GetBcuHtLogicIndex(0U) + 1U;
    WRITE_BT_UINT16(buf, index, val16);
    val8 = Statistic_GetBcuLt(0U);
    WRITE_BT_UINT8(buf, index, val8);
    val16 = Statistic_GetBcuLtLogicIndex(0U) + 1U;
    WRITE_BT_UINT16(buf, index, val16);
    WRITE_BT_UINT16(buf, index, 0x0000U);
    *Length = index;
}


void VcuComm_GetStatusMsg_0x354(uint8 *buf, uint16 *Length) {
    uint16 val16;
    uint8 val8;
    uint16 index = 0U;

    val8 = (uint8)Statistic_GetBcuHeatHtMax();
    WRITE_BT_UINT8(buf, index, val8);
    val16 = 0U;
    WRITE_BT_UINT16(buf, index, val16);

    val8 = (uint8)Statistic_GetBcuHeatLtMax();
    WRITE_BT_UINT8(buf, index, val8);
    val16 = 0U;
    WRITE_BT_UINT16(buf, index, val16);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x355(uint8 *buf, uint16 *Length) {
    uint16 val16;
    uint8 val8;
    uint16 index = 0U;

    val8 = (uint8)Statistic_GetBcuPoleHtMax();
    WRITE_BT_UINT8(buf, index, val8);
    val16 = 0U;
    WRITE_BT_UINT16(buf, index, val16);

    val8 = (uint8)Statistic_GetBcuPoleLtMax();
    WRITE_BT_UINT8(buf, index, val8);
    val16 = 0U;
    WRITE_BT_UINT16(buf, index, val16);
    *Length = index;
}


void VcuComm_GetStatusMsg_0x356(uint8 *buf, uint16 *Length) {
    static uint16 start_num = 0U;
    uint16 val16;
    uint16 index = 0U;

    WRITE_BT_UINT16(buf, index, start_num);
    for (; index < 8U && start_num < SYSTEM_BATTERY_CELL_NUM; start_num++) {
        val16 = CellDataM_GetVoltage(start_num);
        WRITE_BT_UINT16(buf, index, val16);
    }
    for (; index < 8U;) {
        WRITE_BT_UINT16(buf, index, 0xFFFFU);
    }
    if (start_num >= SYSTEM_BATTERY_CELL_NUM) {
        start_num = 0U;
    }
    *Length = index;
}

void VcuComm_GetStatusMsg_0x357(uint8 *buf, uint16 *Length) {
    static uint16 start_num = 0U;
    uint8 val8;
    uint16 index = 0U;

    WRITE_BT_UINT16(buf, index, start_num);
    for (; index < 8U && start_num < SYSTEM_TEMP_CELL_NUM; start_num++) {
        val8 = CellDataM_GetTemperature(start_num);
        WRITE_BT_UINT8(buf, index, val8);
    }
    for (; index < 8U;) {
        WRITE_BT_UINT8(buf, index, 0xFFU);
    }
    if (start_num >= SYSTEM_TEMP_CELL_NUM) {
        start_num = 0U;
    }
    *Length = index;
}

void VcuComm_GetStatusMsg_0x3F3(uint8 *buf, uint16 *Length) {
    static uint16 start_num = 0U;
    uint8 val8;
    uint16 index = 0U;

    WRITE_BT_UINT16(buf, index, start_num);
#if SYSTEM_HEAT_TEMP_NUM > 0U
    for (; index < 8U && start_num < SYSTEM_HEAT_TEMP_NUM; start_num++) {
        val8 = CellDataM_GetHeatTemperature(start_num);
        WRITE_BT_UINT8(buf, index, val8);
    }
    if (start_num >= SYSTEM_HEAT_TEMP_NUM) {
        start_num = 0U;
    }
#endif
    val8 = 0xFFU;
    for (; index < 8U;) {
        WRITE_BT_UINT8(buf, index, val8);
    }
    *Length = index;
}
/*
void VcuComm_GetStatusMsg_0x3F4(uint8 *buf, uint16 *Length) {
    static uint16 start_num = 0U;
    uint8 val8;
    uint16 index = 0U;

    WRITE_BT_UINT16(buf, index, start_num);
    for (; index < 8U && start_num < SYSTEM_POLE_TEMP_NUM; start_num++) {
        val8 = CellDataM_GetPoleTemperature(start_num);
        WRITE_BT_UINT8(buf, index, val8);
    }
    for (; index < 8U;) {
        WRITE_BT_UINT8(buf, index, 0xFFU);
    }
    if (start_num >= SYSTEM_POLE_TEMP_NUM) {
        start_num = 0U;
    }
    *Length = index;
}
*/


void VcuComm_GetStatusMsg_0x3F5(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    uint16 val16;

    (void)ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_DCHG_CAP_H_INDEX, &val16);
    WRITE_BT_UINT16(buf, index, val16);
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_DCHG_CAP_L_INDEX, &val16);
    WRITE_BT_UINT16(buf, index, val16);
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_CHG_CAP_H_INDEX, &val16);
    WRITE_BT_UINT16(buf, index, val16);
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_CHG_CAP_L_INDEX, &val16);
    WRITE_BT_UINT16(buf, index, val16);

    *Length = index;
}

void VcuComm_GetStatusMsg_0x3F6(uint8 *buf, uint16 *Length) {
    uint8 val8;
    uint16 index = 0U;
    uint16 val16;

    val16 = Statistic_GetBcuAverageVoltage();
    WRITE_BT_UINT16(buf, index, val16);
    val8 = Statistic_GetBcuAverageTemperature();
    WRITE_BT_UINT8(buf, index, val8);
    val16 = Statistic_GetBcuDeltaVoltage();
    WRITE_BT_UINT16(buf, index, val16);
    val8 = Statistic_GetBcuDeltaTemperature();
    WRITE_BT_UINT8(buf, index, val8);
    val16 = Statistic_GetBcuBatteryDischargeCount();
    WRITE_BT_UINT16(buf, index, val16);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x3FF(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    WRITE_BT_UINT16(buf, index, SYSTEM_BATTERY_CELL_NUM);
    WRITE_BT_UINT16(buf, index, SYSTEM_TEMP_CELL_NUM);
    WRITE_BT_UINT16(buf, index, SYSTEM_POLE_TEMP_NUM);
    WRITE_BT_UINT16(buf, index, SYSTEM_HEAT_TEMP_NUM);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x1F4(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U;
    uint16 temp;
    uint16 u16val;
    uint8 u8val;
    uint8 chgIsAllow = ChargeM_ChargeIsAllowed();
    Current_CurrentType current;

    VALIDATE_PTR(buf, VCUCOMM_MESSAGES_API_ID_GetMsgData0x1F4Cbk);
    VALIDATE_PTR(Length, VCUCOMM_MESSAGES_API_ID_GetMsgData0x1F4Cbk);

    /**< 最大允许充电电压 */
    if (CHARGECONNECTM_IS_CONNECT() && chgIsAllow == E_OK)
    {
        temp = ChargeM_GetChargeDisableTriggerPara(DIAGNOSIS_ITEM_CHG_HTV, 0U);
        if (Statistic_TotalVoltageIsValid(temp))
        {
            u16val = temp;
        }
        else
        {
            u16val = 0U;
        }
    }
    else
    {
        u16val = 0U;
    }
    WRITE_BT_UINT16(buf, index, u16val);
    /**< 最大允许充电电流 */
    if (CHARGECONNECTM_IS_CONNECT())
    {
        if (chgIsAllow == E_OK)
        {
            /*current = abs(PowerM_GetCurrent(POWERM_CUR_CHARGE_AC));
            current_max = (Current_CurrentType)ChargerCommUser_GetGBSignalCurrentMax();
            if(current > current_max)
            {
                current = current_max;
            }*/
            current = UserStrategy_GetChargeCurrentMax();
            if (!CurrentM_IsValidCurrent(current))
            {
                u16val = 0U;
            }
            else
            {
                u16val = (uint16)abs(current);
            }
        }
        else
        {
            u16val = 0U;
        }
    }
    else
    {
        u16val = 0U;
    }
    WRITE_BT_UINT16(buf, index, u16val);
    /**< 充电控制 */
    if (chgIsAllow == E_OK)
    {
        u8val = 0U;
    }
    else
    {
        u8val = 1U;
    }
    WRITE_BT_UINT8(buf, index, u8val);

    //保留
    WRITE_BT_UINT8(buf, index, 0U);
    WRITE_BT_UINT16(buf, index, 0U);

    *Length = index;
}

//新增报文
void VcuComm_GetStatusMsg_0x201(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U;
    uint16 u16val;
    uint8 temp,fault;
    Current_CurrentType current;

    //当前充电电压
    u16val = ChargerComm_GetChargerOutputHV();
    if (!CellDataM_VoltageIsValid(u16val))
    {
        u16val = 0xFFFFU;
    }
    WRITE_BT_UINT16(buf, index, u16val);

    //当前充电电流
    current = ChargerComm_GetChargerOutputCurrent();
    if (!CurrentM_IsValidCurrent(current))
    {
        current = 0;
    }
    WRITE_BT_UINT16(buf, index, current);
    temp = 0U;
    //充电机硬件故障
    fault = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_HARDWARE_FAULT_INDEX);
    if (fault == 1U || fault == 2U)
    {
        temp = 1U;
    }
    //充电机温度异常
    fault = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_OVER_TEMPERATURE_FAULT_INDEX);
    if(fault == 1U || fault == 2U)
    {
        temp |= (uint8)(1U << 1);
    }
    //充电机输入电压异常
    fault = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_INPUT_VOLT_FAULT_INDEX);
    if(fault == 1U || fault == 2U)
    {
        temp |= (uint8)(1U << 2);
    }
    // 充电机运行（启动）状态
    fault = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_RUN_STATUS_INDEX);
    if(fault == 1U || fault == 2U)
    {
        temp |= (uint8)(1U << 3);
    }
    //充电机通信异常
    fault = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_COMM_ABORT_WITH_BMS_INDEX);
    if(fault == 1U || fault == 2U)
    {
        temp |= (uint8)(1U << 4);
    }
    WRITE_BT_UINT8(buf, index, temp);
    WRITE_BT_UINT16(buf, index, 0U);
    WRITE_BT_UINT8(buf, index, 0U);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x360(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U;
    uint8 u8val;
    uint8 diag;
    uint8 temp;

    // 继电器数量
    WRITE_BT_UINT8(buf, index, RELAYM_FN_NUM);
    // 继电器状态
    for (temp = 0U; temp + 1U <= RELAYM_FN_NUM && temp <= 6U; temp++)
    {
        u8val = 0U;
        // 继电器状态
        u8val |= (RelayM_GetActualStatus(temp) == RELAYM_ACTUAL_ON ? 1U : 0U);
        // 继电器故障状态
        diag = RelayM_GetDiagnosisStatus(temp);
        if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
        {
            u8val |= (uint8)(1U << 1);
        }
        else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
        {
            u8val |= (uint8)(2U << 1);
        }
        else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
        {
            u8val |= (uint8)(3U << 1);
        }
        else
        {

        }
        // 继电器索引号
#ifdef RELAYM_FN_POSITIVE_MAIN
        if (RELAYM_FN_POSITIVE_MAIN == temp)
        {
            //u8val |= (uint8)(0U << 3);
        }
#endif
#ifdef RELAYM_FN_NEGATIVE_MAIN
        if (RELAYM_FN_NEGATIVE_MAIN == temp)
        {
            u8val |= (uint8)(1U << 3);
        }
#endif
#ifdef RELAYM_FN_PRECHARGE
        if (RELAYM_FN_PRECHARGE == temp)
        {
            u8val |= (uint8)(3U << 3);
        }
#endif
#ifdef RELAYM_FN_CHARGE
        if (RELAYM_FN_CHARGE == temp)
        {
            u8val |= (uint8)(4U << 3);
        }
#endif
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
        if (RELAYM_FN_POSITIVE_AC_CHARGE == temp)
        {
            u8val |= (uint8)(5U << 3);
        }
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
        if (RELAYM_FN_POSITIVE_DC_CHARGE == temp)
        {
            u8val |= (uint8)(6U << 3);
        }
#endif
#ifdef RELAYM_FN_HEATER
        if (RELAYM_FN_HEATER == temp)
        {
            u8val |= (uint8)(7U << 3);
        }
#endif
#ifdef RELAYM_FN_SELF_LOCK
        if (RELAYM_FN_SELF_LOCK == temp)
        {
            u8val |= (uint8)(10U << 3);
        }
#endif
#ifdef RELAYM_FN_BUZZER
        if (RELAYM_FN_BUZZER == temp)
        {
            u8val |= (uint8)(11U << 3);
        }
#endif
#ifdef RELAYM_FN_COOLER
        if (RELAYM_FN_COOLER == temp)
        {
            u8val |= (uint8)(16U << 3);
        }
#endif
#ifdef RELAYM_FN_HMI
        if (RELAYM_FN_HMI == temp)
        {
            u8val |= (uint8)(12U << 3);
        }
#endif
        WRITE_BT_UINT8(buf, index, u8val);
    }

    for (; index < 8U; )
    {
        WRITE_BT_UINT8(buf, index, 0U);
    }

    *Length = index;
}

void VcuComm_GetStatusMsg_0x361(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U;
#if (RELAYM_FN_NUM > 7U)
    uint8 u8val;
    uint8 diag;
    uint8 temp;

  // 继电器状态
    for (temp = 7U; temp + 1U <= RELAYM_FN_NUM && temp <= 15U; temp++)
    {
        u8val = 0U;
        // 继电器状态
        u8val |= (RelayM_GetActualStatus(temp) == RELAYM_ACTUAL_ON ? 1U : 0U);
        // 继电器故障状态
        diag = RelayM_GetDiagnosisStatus(temp);
        if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
        {
            u8val |= (uint8)(1U << 1);
        }
        else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
        {
            u8val |= (uint8)(2U << 1);
        }
        else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
        {
            u8val |= (uint8)(3U << 1);
        }
        else
        {

        }
        // 继电器索引号（暂无）
        WRITE_BT_UINT8(buf, index, u8val);
    }
#endif
    for (; index < 8U; )
    {
        WRITE_BT_UINT8(buf, index, 0U);
    }

    *Length = index;
}

void VcuComm_GetStatusMsg_0x358(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    App_SlaveIdType uval;
    uint8 temp;

    // 电池最高单体电压从机号
    uval = Statistic_GetBcuHvSlaveNum(0U) + 1U;
    WRITE_BT_UINT8(buf, index, uval);

    // 电池最高单体电压从机串号
    temp = Statistic_GetBcuHvIndex(0U) + 1U;
    WRITE_BT_UINT8(buf, index, temp);

    // 电池最低单体电压从机号
    uval = Statistic_GetBcuLvSlaveNum(0U) + 1U;
    WRITE_BT_UINT8(buf, index, uval);

    // 电池最低单体电压从机串号
    temp = Statistic_GetBcuLvIndex(0U) + 1U;
    WRITE_BT_UINT8(buf, index, temp);

    // 电池最高温度从机号
    uval = Statistic_GetBcuHtSlaveNum(0U) + 1U;
    WRITE_BT_UINT8(buf, index, uval);

    // 电池最高温度从机串号
    temp = Statistic_GetBcuHtIndex(0U) + 1U;
    WRITE_BT_UINT8(buf, index, temp);

    // 电池最低温度从机号
    uval = Statistic_GetBcuLtSlaveNum(0U) + 1U;
    WRITE_BT_UINT8(buf, index, uval);

    // 电池最低温度从机串号
    temp = Statistic_GetBcuLtIndex(0U) + 1U;
    WRITE_BT_UINT8(buf, index, temp);

    *Length = index;
}

void VcuComm_GetStatusMsg_0x202(uint8 *buf, uint16 *Length)
{
    uint16 temp;
    uint16 index = 0U;
    uint32 time;
    Datetime_DatetimeType date;

    /**< 年 */
    if (DatetimeM_GetDatetime(&time) == DATETIME_TRUSTY)
    {
        if (Datetime_FromSecond(&date, time) == E_OK)
        {
            temp = date.year;
            temp += 2000U;
            WRITE_BT_UINT16(buf, index,  temp);
            /**< 月 */
            temp = (uint16)date.month;
            WRITE_BT_UINT8(buf, index,  temp);
            /**< 日 */
            temp = (uint16)date.day;
            WRITE_BT_UINT8(buf, index,  temp);
            //时
            temp = (uint16)date.hour;
            WRITE_BT_UINT8(buf, index,  temp);
            //分
            temp = (uint16)date.minute;
            WRITE_BT_UINT8(buf, index,  temp);
            //秒
            temp = (uint16)date.second;
            WRITE_BT_UINT8(buf, index,  temp);
        }
    }
    //保留
    WRITE_BT_UINT8(buf, index,  0U);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x203(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U;
    uint32 temp1,temp2;
    uint16 val1,val2,val3,val4;
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_CHG_TIME_L_INDEX, &val1);
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_CHG_TIME_H_INDEX, &val2);
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_DCHG_TIME_L_INDEX, &val3);
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_CUMULATIVE_DCHG_TIME_H_INDEX, &val4);
    temp1 = (uint32)val2;
    temp1 = temp1 << 16U;
    temp1 += val1;
    temp2 = (uint32)val4;
    temp2 = temp2 << 16U;
    temp2 += val3;
    temp1 = DIVISION(temp1, 360U);
    temp2 = DIVISION(temp2, 360U);
    VcuComm_MsgData.CmuTotalTime = temp1 + temp2;
    WRITE_BT_UINT32(buf, index, temp2);
    WRITE_BT_UINT32(buf, index, temp1);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x204(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U;
    // uint32 temp,temp1,temp2;
    // uint16 val1,val2,val3,val4;
    // (void)ParameterM_EeepRead(PARAMETERM_EEEP_CHARGE_TIME_LOW_INDEX, &val1);
    // (void)ParameterM_EeepRead(PARAMETERM_EEEP_CHARGE_TIME_HIGH_INDEX, &val2);
    // (void)ParameterM_EeepRead(PARAMETERM_EEEP_DISCHARGE_TIME_LOW_INDEX, &val3);
    // (void)ParameterM_EeepRead(PARAMETERM_EEEP_DISCHARGE_TIME_HIGH_INDEX, &val4);
    // temp1 = (uint32)val2;
    // temp1 = temp1 << 16U;
    // temp2 = (uint32)val4;
    // temp2 = temp2 << 16U;
    // temp = val1 + temp1 + val2 + temp2;
    // temp = DIVISION(temp, 360U);
    WRITE_BT_UINT32(buf, index, VcuComm_MsgData.CmuTotalTime);
    WRITE_BT_UINT16(buf, index, 0xFFFFU);
    WRITE_BT_UINT8(buf, index,  0xFFU);
    WRITE_BT_UINT8(buf, index, 0xFFU);
    *Length = index;
}

void VcuComm_GetStatusMsg_0x205(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U;
    uint32 temp;
    uint16 val16;

    // 电池总容量
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_TOTAL_CAP_INDEX, &val16);
    WRITE_BT_UINT16(buf, index, val16);

    // 电池组剩余容量
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_LEFT_CAP_INDEX, &val16);
    WRITE_BT_UINT16(buf, index, val16);

    // 已充电时间
    temp = Statistic_GetEclipseChargeTime();
    temp = DIVISION(temp , 60U);
    WRITE_BT_UINT16(buf, index, (uint16)temp);

    //Soh
    WRITE_BT_UINT16(buf, index, Soh_Get());
    *Length = index;
}

void VcuComm_GetStatusMsg_0x206(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U;
    SocDiagCalib_NoCalibReasonType val;
    Diagnosis_ItemType temp;
    DischargeM_OthersFaultIndexType uval;
    // SOC满诊断校准自诊断
    val = SocDiagCalib_GetFullReason();
    WRITE_BT_UINT8(buf, index, val);

    // SOC空诊断校准自诊断
    val = SocDiagCalib_GetEmptyReason();
    WRITE_BT_UINT8(buf, index, val);

    // 放电状态自检故障码
    temp = DischargeM_GetStartDiagFault();
    WRITE_BT_UINT8(buf, index, temp);

    // 放电诊断故障标志
    temp = DischargeM_GetDiagFaultFlag();
    WRITE_BT_UINT8(buf, index, temp);

    // 放电诊断故障动作
    temp = DischargeM_GetDiagFaultAction();
    WRITE_BT_UINT8(buf, index, temp);

    //放电其他故障
    uval = DischargeM_GetOthersFault();
    WRITE_BT_UINT8(buf, index, uval);

    //保留
    WRITE_BT_UINT16(buf, index, 0U);

    *Length = index;
}

void VcuComm_GetStatusMsg_0x207(uint8 *buf, uint16 *Length)
{
    uint16 index = 0U;
    Diagnosis_ItemType temp;
    uint16 u16val;
    ChargeM_OthersFaultIndexType uval;

    // 充电自检故障
    temp = ChargeM_GetStartDiagFault();
    WRITE_BT_UINT8(buf, index, temp);

    // 充电诊断故障标志
    temp = ChargeM_GetDiagFaultFlag();
    WRITE_BT_UINT8(buf, index, temp);

    // 充电诊断故障动作
    temp = ChargeM_GetDiagFaultAction();
    WRITE_BT_UINT8(buf, index, temp);

    // 充电其他故障
    uval = ChargeM_GetOthersFault();
    WRITE_BT_UINT8(buf, index, uval);

    // 保留
    WRITE_BT_UINT8(buf, index, 0U);

    // 国标通信故障代码
    u16val = ChargeM_GetChargerGBReadyFault();
    WRITE_BT_UINT16(buf, index, u16val);

    // 保留
    WRITE_BT_UINT8(buf, index, 0U);

    *Length = index;
}

void VcuComm_GetStatusMsg_0x208(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    Current_CurrentType current;

    // 加热电流(采集无偏移)
    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_HEATER);
    if (!CurrentM_IsValidCurrent(current)) {
        current = 0;
    }
    WRITE_BT_UINT16(buf, index, (uint16)current);
    // 加热电流(采集有偏移 -1000)
    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_HEATER);
    if (!CurrentM_IsValidCurrent(current)) {
        current = 0;
    }
    else {
        current += 10000;
    }
    WRITE_BT_UINT16(buf, index, (uint16)current);
    // 蜂鸣控制
    if (UserStrategy_GetBuzzerSta())
    {
        WRITE_BT_UINT16(buf, index, 1U);
    }
    else
    {
        WRITE_BT_UINT16(buf, index, 0U);
    }

    // 保留
    // WRITE_BT_UINT8(buf, index, 0xFFU);
    WRITE_BT_UINT16(buf, index, 0xFFFFU);
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

