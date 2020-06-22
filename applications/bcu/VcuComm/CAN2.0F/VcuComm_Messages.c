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
#include "RelayM_Lcfg.h"
#include "DischargeM.h"
#include "ChargeM.h"
#include "PowerM.h"
#include "PduR_Types.h"
#include "TemperatureM.h"
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

#define VCUCOMM_CURRENT_OFFSET      (30000)
#define VCUCOMM_VOLT_4_DISPLAY(volt)    (VOLT_4_DISPLAY(volt) < 10000U ? VOLT_4_DISPLAY(volt) : 10000U)
#define VCUCOMM_RESERVE_UINT8 0U
#define VCUCOMM_RESERVE_UINT16 0U

VcuComm_MsgDataType VcuComm_MsgData;

static uint16 VcuComm_bms2VcuCurrent(Current_CurrentType current);

void VcuComm_MessageInit(Async_LooperType *looper)
{
    (void)looper;
    (void)VcuComm_SendIPduRegister(&VcuComm_IPduSendConfigInfo[0], VcuComm_SendIPduNum);
}

void VcuComm_GetStatusMsg_0x200(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    uint16 u16val;
    Current_CurrentType current;
    static uint8 life = 0U;

    /**< 总电压 */
    /**< 电池组总电压 */
    u16val = Statistic_GetBcu100mvTotalVoltage();
    if (!Statistic_TotalVoltageIsValid(u16val)){
        u16val = 0U;
    }
    WRITE_BT_UINT16(buf, index, u16val);
    /**< 总电流 */
    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    u16val = VcuComm_bms2VcuCurrent(current);
    WRITE_BT_UINT16(buf, index, u16val);
    /**< SOC */
    u16val = (uint16)SOC_TO_PERCENT(Soc_Get());
    WRITE_BT_UINT8(buf, index, (uint8)u16val);
    /**< 预留 */
    WRITE_BT_UINT16(buf, index, VCUCOMM_RESERVE_UINT16);
    /**< LIFE */
    WRITE_BT_UINT8(buf, index, life);
    if (life >= 255U) {
        life = 0U;
    } else {
        life++;
    }

    *Length = index;
}

void VcuComm_GetStatusMsg_0x201(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    uint8 u8val;
    uint16 u16val;
    RelayM_FunctionType relay;
    static uint8 life = 0U;
    Charge_ChargeType type = ChargeConnectM_GetConnectType();

    /**< 最高单体电压 */
    u16val = Statistic_GetBcuHv(0U);
    if (!CellDataM_VoltageIsValid(u16val)){
        u16val = 0U;
    }
    WRITE_BT_UINT16(buf, index, u16val);
    /**< 最高单体电池位置 */
    u16val = Statistic_GetBcuHvLogicIndex(0U) + 1U;
    WRITE_BT_UINT8(buf, index, (uint8)u16val);
    /**< 最低单体电压 */
    u16val = Statistic_GetBcuLv(0U);
    if (!CellDataM_VoltageIsValid(u16val)){
        u16val = 0U;
    }
    WRITE_BT_UINT16(buf, index, u16val);
    /**< 最低单体电池位置 */
    u16val = Statistic_GetBcuLvLogicIndex(0U) + 1U;
    WRITE_BT_UINT8(buf, index, (uint8)u16val);

    u8val = 0U;
    /**< 电池组当前状态 */
    // bit1: 放电状态
    if (!CHARGECONNECTM_IS_CONNECT()) {
#ifdef RELAYM_FN_POSITIVE_MAIN
        relay = RELAYM_FN_POSITIVE_MAIN;
#else
#ifdef RELAYM_FN_DISCHARGE
        relay = RELAYM_FN_DISCHARGE;
#endif
#endif
        if (RelayM_GetActualStatus(relay) == RELAYM_ACTUAL_ON) {
            u8val |= (uint8)((uint8)1U << 0);
        }
    } else {
        if (type == CHARGE_TYPE_DC) {
            relay = ChargerComm_ConfigInfo.DC_RelayType;
        } else if (type == CHARGE_TYPE_AC) {
            relay = ChargerComm_ConfigInfo.AC_RelayType;
        } else {
            relay = RELAYM_FN_NONE;
        }
        if (RelayM_GetActualStatus(relay) == RELAYM_ACTUAL_ON) {
            u8val |= (uint8)((uint8)1U << 1);
        }
    }
    // bit2: 充电状态
    // bit3: 加热状态
    if (TemperatureM_GetHeatState() == TEMPERATUREM_HEAT_STATE_LT)
    {
        u8val |= (uint8)((uint8)1U << 2);
    }
    WRITE_BT_UINT8(buf, index, u8val);
    /**< LIFE */
    WRITE_BT_UINT8(buf, index, life);
    if (life >= 255U) {
        life = 0U;
    } else {
        life++;
    }

    *Length = index;
}

void VcuComm_GetStatusMsg_0x202(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    uint8 u8val;
    uint16 u16val;
    static uint8 life = 0U;
    RelayM_FunctionType relay;
    RelayM_ActualStatusType status;
    Charge_ChargeType type = ChargeConnectM_GetConnectType();

    /**< 最高温度点温度 */
    u8val = Statistic_GetBcuHt(0U);
    WRITE_BT_UINT8(buf, index, u8val);
    /**< 最高温度点位置 */
    u16val = Statistic_GetBcuHtLogicIndex(0U) + 1U;
    WRITE_BT_UINT8(buf, index, (uint8)u16val);
    /**< 最低温度点温度 */
    u8val = Statistic_GetBcuLt(0U);
    WRITE_BT_UINT8(buf, index, u8val);
    /**< 最低温度点位置 */
    u16val = Statistic_GetBcuLtLogicIndex(0U) + 1U;
    WRITE_BT_UINT8(buf, index, (uint8)u16val);
    /**< 继电器状态 */
    if (CHARGECONNECTM_IS_CONNECT()) {
#ifdef RELAYM_FN_POSITIVE_MAIN
        relay = RELAYM_FN_POSITIVE_MAIN;
#else
#ifdef RELAYM_FN_DISCHARGE
        relay = RELAYM_FN_DISCHARGE;
#endif
#endif
    } else {
        if (type == CHARGE_TYPE_DC) {
            relay = ChargerComm_ConfigInfo.DC_RelayType;
        } else if (type == CHARGE_TYPE_AC) {
            relay = ChargerComm_ConfigInfo.AC_RelayType;
        } else {
            relay = RELAYM_FN_NONE;
        }
    }
    status = RelayM_GetActualStatus(relay);
    if (status == RELAYM_ACTUAL_ON) {
        u8val = 1U;
    } else {
        u8val = 0U;
    }
    WRITE_BT_UINT8(buf, index, u8val);
    /**< 预留 */
    WRITE_BT_UINT16(buf, index, VCUCOMM_RESERVE_UINT16);
    /**< LIFE */
    WRITE_BT_UINT8(buf, index, life);
    if (life >= 255U) {
        life = 0U;
    } else {
        life++;
    }

    *Length = index;
}

/*
void VcuComm_GetStatusMsg_0x1F3(uint8 *buf, uint16 *Length) {
}

void VcuComm_GetStatusMsg_0x211(uint8 *buf, uint16 *Length) {
}
*/

void VcuComm_GetStatusMsg_0x203(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    uint16 u16val;
    uint32 u32val;
    uint32 cumuTime, cumuChgTime, cumuDchgTime;
    static uint8 life = 0U;

    /**< 电池容量规格:和售前确认使用额定容量 */
    if (ParameterM_EeepRead(PARAMETERM_EEEP_TOTAL_CAP_INDEX, &u16val) == E_OK){
        u16val = u16val / 10U;
    } else {
        u16val = 0U;
    }
    WRITE_BT_UINT16(buf, index, u16val);

    cumuChgTime = Statistic_GetCumuChgTime();
    cumuDchgTime = Statistic_GetCumuDchgTime();
    cumuTime = cumuChgTime + cumuDchgTime;
    /**< 电池使用时间A (0.1h,则认为将累计时间换算为小时的第一位小数位) */
    u32val = cumuTime % 3600U;
    u32val = u32val * 10UL;
    u32val = DIVISION(u32val, 3600UL);
    WRITE_BT_UINT8(buf, index, (uint8)u32val);
    /**< 电池使用时间B (1h,则认为将累计时间换算成小时的个位数部分) */
    u32val = cumuTime / 3600U;
    u32val = u32val % 100UL;
    WRITE_BT_UINT8(buf, index, (uint8)u32val);
    /**< 电池使用时间C */
    u32val = cumuTime / 3600U;
    u32val = u32val / 100UL;
    WRITE_BT_UINT16(buf, index, (uint16)u32val);
    /**< 预留 */
    WRITE_BT_UINT8(buf, index, VCUCOMM_RESERVE_UINT8);
    /**< LIFE */
    WRITE_BT_UINT8(buf, index, life);
    if (life >= 255U) {
        life = 0U;
    } else {
        life++;
    }

    *Length = index;
}

void VcuComm_GetStatusMsg_0x190(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    uint8 u8val;
    static uint8 life = 0U;
    Current_CurrentType tableCurrent, destCurrent;
    Diagnosis_LevelType level, level1;

    u8val = 0U;
    /**< BMS ERROR1 */
    // 单体充电温度低故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LT);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 0);
    // 单体充电温度高故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 2);
    // 单体放电温度低故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 4);
    // 单体放电温度高故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 6);
    WRITE_BT_UINT8(buf, index, u8val);

    u8val = 0U;
    /**< BMS ERROR2 */
    // 单体电压过低故障
    if (CHARGECONNECTM_IS_CONNECT()) {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 0);
    } else {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 0);
    }
    // 单体电压过高故障
    if (CHARGECONNECTM_IS_CONNECT()) {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 2);
    } else {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 2);
    }
    // 单体压差故障
    if (CHARGECONNECTM_IS_CONNECT()) {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 4);
    } else {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 4);
    }
    // 单体温差故障
    if (CHARGECONNECTM_IS_CONNECT()) {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DT);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 6);
    } else {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DT);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 6);
    }
    WRITE_BT_UINT8(buf, index, u8val);

    u8val = 0U;
    /**< BMS ERROR3 */
    // SOC过低故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LSOC);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 0);
    // 快充电流过大故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_AC_CHG_OC);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 2);
    // 回馈电流过大故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_FB_OC);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 4);
    // 放电电流过大故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC);
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_SP_OC) >= level)
    {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_SP_OC);
    }
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 6);
    WRITE_BT_UINT8(buf, index, u8val);

    u8val = 0U;
    /**< BMS ERROR4 */
    // 模块失联故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_VOLT_LINE);
    level1 = Diagnosis_GetLevel(DIAGNOSIS_ITEM_TEMP_LINE);
    if (level < level1) {
        level = level1;
    }
    level1 = Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK);
    if (level < level1) {
        level = level1;
    }
    level1 = Diagnosis_GetLevel(DIAGNOSIS_ITEM_RELAY_ABNORMAL);
    if (level < level1) {
        level = level1;
    }
    level1 = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CURRENT_ABNORMAL);
    if (level < level1) {
        level = level1;
    }
    level1 = Diagnosis_GetLevel(DIAGNOSIS_ITEM_BMS_INIT_FAILURE);
    if (level < level1) {
        level = level1;
    }
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 0);
    // 单体温度传感器故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_TEMP_LINE);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 2);
    // 总压过低故障
    if (CHARGECONNECTM_IS_CONNECT()) {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LTV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 4);
    } else {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 4);
    }
    // 总压过高故障
    if (CHARGECONNECTM_IS_CONNECT()) {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HTV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 6);
    } else {
        level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HTV);
        level = VcuComm_Bms2VcuSendLevel(level);
        u8val |= (uint8)((uint8)level << 6);
    }
    WRITE_BT_UINT8(buf, index, u8val);

    u8val = 0U;
    /**< BMS ERROR5 */
    // 放电继电器故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_RELAY_ABNORMAL);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 0);
    // 充电座温度过高故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHGSCKTMP_DC_POSITIVE);
    level1 = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHGSCKTMP_DC_NEGATIVE);
    if (level < level1) {
        level = level1;
    }
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 2);
    // 充电座温度传感器故障
    level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHGSCKTMP_ABNORMAL);
    level = VcuComm_Bms2VcuSendLevel(level);
    u8val |= (uint8)((uint8)level << 4);
    WRITE_BT_UINT8(buf, index, u8val);

    /**< BMS ERROR6 */
    WRITE_BT_UINT8(buf, index, VCUCOMM_RESERVE_UINT8);

    u8val = 0U;
    /**< 故障处理请求 */
    // 停车
    /*if (CHARGECONNECTM_IS_CONNECT()) {
        u8val |= (uint8)((uint8)1U << 0);
    } else */if (DischargeM_DischargeIsAllowed() == E_NOT_OK) {
        u8val |= (uint8)((uint8)1U << 0);
    } else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC) >= DIAGNOSIS_LEVEL_THIRD) {
        u8val |= (uint8)((uint8)1U << 0);
    } else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_LSOC) >= DIAGNOSIS_LEVEL_THIRD) {
        u8val |= (uint8)((uint8)1U << 0);
    } else {

    }
    // 放电限流50%
    tableCurrent = PowerM_GetCurrentWithoutDiag(POWERM_CUR_DCHARGE_CONTINUE);
    destCurrent = PowerM_GetDestCurrent(POWERM_CUR_DCHARGE_CONTINUE);
    tableCurrent /= 2;
    if (tableCurrent == destCurrent) {
        u8val |= (uint8)((uint8)1U << 1);
        u8val |= (uint8)((uint8)1U << 3);
    }

    if (DischargeM_DischargeIsAllowed() == E_NOT_OK) {
        u8val |= (uint8)((uint8)1U << 1);
        u8val |= (uint8)((uint8)1U << 3);
    }
    // 反馈限流50%
    // 限速
    // 警告
    if (Diagnosis_GetDiagLevelMax() != DIAGNOSIS_LEVEL_NONE) {
        u8val |= (uint8)((uint8)1U << 4);
    }
    // 预留
    WRITE_BT_UINT8(buf, index, u8val);

    /**< LIFE */
    WRITE_BT_UINT8(buf, index, life);
    if (life >= 255U) {
        life = 0U;
    } else {
        life++;
    }

    *Length = index;
}

void VcuComm_GetStatusMsg_0x5F2(uint8 *buf, uint16 *Length) {
    uint16 index = 0U;
    boolean operateResult = FALSE;
    uint16 eepromIndex;
    uint16 readData1 = 0U, readData2 = 0U;

    switch (VcuComm_MsgData.eepromCommand)
    {
        case VCUCOMM_EEPROM_OPERATE_CMD_WIRTE:
            if (VcuComm_MsgData.eepWriteCmd == VCUCOMM_EEPROM_WRITE_NORMAL) {
                if (VcuComm_MsgData.writeOem > 0U) {
                    eepromIndex = (uint16)((uint16)VcuComm_MsgData.writeOem - 1U);
                    eepromIndex *= 2U;
                    eepromIndex += (uint16)PARAMETERM_EEEP_VCU_EEPROM_GROUP1_DATA1;

                    if (ParameterM_EeepWrite((ParameterM_EeepParaIndexType)eepromIndex, VcuComm_MsgData.eepromWriteData1) == E_OK) {
                        eepromIndex += 1U;
                        if (ParameterM_EeepWrite((ParameterM_EeepParaIndexType)eepromIndex, VcuComm_MsgData.eepromWriteData2) == E_OK) {
                            operateResult = TRUE;
                        }
                    }
                }
            }
            // 0x80
            WRITE_LT_UINT8(buf, index, 0x80U);
            // 0x00
            WRITE_LT_UINT8(buf, index, 0U);
            // 0x22
            WRITE_LT_UINT8(buf, index, 0x22U);
            // 数据组编号
            WRITE_LT_UINT8(buf, index, VcuComm_MsgData.writeOem);
            // 操作结果(0写失败, 1写成功)
            WRITE_LT_UINT8(buf, index, operateResult);
            // 保留
            WRITE_LT_UINT8(buf, index, VCUCOMM_RESERVE_UINT8);
            WRITE_LT_UINT8(buf, index, VCUCOMM_RESERVE_UINT8);
            WRITE_LT_UINT8(buf, index, VCUCOMM_RESERVE_UINT8);
            // 清除指令缓存
            VcuComm_MsgData.eepWriteCmd = VCUCOMM_EEPROM_WRITE_NONE;
            break;
        case VCUCOMM_EEPROM_OPERATE_CMD_READ:
            if (VcuComm_MsgData.eepReadCmd == VCUCOMM_EEPROM_READ_NORMAL) {
                if (VcuComm_MsgData.readOem > 0U) {
                    eepromIndex = (uint16)((uint16)VcuComm_MsgData.readOem - 1U);
                    eepromIndex *= 2U;
                    eepromIndex += (uint16)PARAMETERM_EEEP_VCU_EEPROM_GROUP1_DATA1;

                    if (ParameterM_EeepRead((ParameterM_EeepParaIndexType)eepromIndex, &readData1) == E_OK) {
                        eepromIndex += 1U;
                        if (ParameterM_EeepRead((ParameterM_EeepParaIndexType)eepromIndex, &readData2) == E_OK) {
                            operateResult = TRUE;
                        } else {
                            readData2 = 0U;
                        }
                    } else {
                        readData1 = 0U;
                    }
                }
            }
            (void)operateResult;
            // 0x80
            WRITE_LT_UINT8(buf, index, 0x80U);
            // 0x00
            WRITE_LT_UINT8(buf, index, 0U);
            // 0x42
            WRITE_LT_UINT8(buf, index, 0x42U);
            // 数据组编号
            WRITE_LT_UINT8(buf, index, VcuComm_MsgData.readOem);
            // 数据1
            WRITE_LT_UINT16(buf, index, readData1);
            // 数据2
            WRITE_LT_UINT16(buf, index, readData2);

            // 清除指令缓存
            VcuComm_MsgData.eepReadCmd = VCUCOMM_EEPROM_READ_NONE;
            break;
        case VCUCOMM_EEPROM_OPERATE_CMD_ERROR:
            break;
        case VCUCOMM_EEPROM_OPERATE_CMD_NONE:
            break;
        default :
            break;
    }

    // 清除指令缓存
    VcuComm_MsgData.eepromCommand = VCUCOMM_EEPROM_OPERATE_CMD_NONE;

    *Length = index;
}

void VcuComm_ReceiveCbk(const uint8 *Buffer, uint16 Length)
{
    uint16 index = 0U;
    uint8 sid, oem;
    uint16 crc;
    uint16 u16temp;

    VALIDATE_PTR(Buffer, VCUCOMM_MESSAGES_API_ID_ReceiveCbk);

    VcuComm_SetCommunicationStatus(TRUE);

    if (Length >= 1U) {
        sid = READ_LT_UINT8(Buffer, index);
        VcuComm_MsgData.sid = sid;
        if (sid == VCUCOMM_WRITE_EEPROM_COMMAND) {
            VcuComm_MsgData.eepromCommand = VCUCOMM_EEPROM_OPERATE_CMD_WIRTE;
            if (Length >= 8U) {
                oem = READ_LT_UINT8(Buffer, index);
                VcuComm_MsgData.writeOem = oem;
                if (oem < VCUCOMM_EEPROM_OPERATE_OEM_MIN || oem > VCUCOMM_EEPROM_OPERATE_OEM_MAX) {
                    VcuComm_MsgData.eepWriteCmd = VCUCOMM_EEPROM_WRITE_OEM_ERROR;
                } else {
                    VcuComm_MsgData.eepromWriteData1 = READ_LT_UINT16(Buffer, index);
                    VcuComm_MsgData.eepromWriteData2 = READ_LT_UINT16(Buffer, index);
                    crc = READ_LT_UINT16(Buffer, index);
                    u16temp = (uint16)sid + (uint16)oem + (VcuComm_MsgData.eepromWriteData1 & 0xFFU) \
                                + ((VcuComm_MsgData.eepromWriteData1 >> 8) & 0xFFU) \
                                + (VcuComm_MsgData.eepromWriteData2 & 0xFFU) \
                                + ((VcuComm_MsgData.eepromWriteData2 >> 8) & 0xFFU);
                    if (u16temp != crc) {
                        VcuComm_MsgData.eepWriteCmd = VCUCOMM_EEPROM_WRITE_CRC_ERROR;
                    } else {
                        VcuComm_MsgData.eepWriteCmd = VCUCOMM_EEPROM_WRITE_NORMAL;
                    }
                }
            }
        } else if (sid == VCUCOMM_READ_EEPROM_COMMAND) {
            VcuComm_MsgData.eepromCommand = VCUCOMM_EEPROM_OPERATE_CMD_READ;
            if (Length >= 4U) {
                index += 2U; // 加数据偏移
                oem = READ_LT_UINT8(Buffer, index);
                VcuComm_MsgData.readOem = oem;
                if (oem < VCUCOMM_EEPROM_OPERATE_OEM_MIN || oem > VCUCOMM_EEPROM_OPERATE_OEM_MAX) {
                    VcuComm_MsgData.eepReadCmd = VCUCOMM_EEPROM_READ_OEM_ERROR;
                } else {
                    VcuComm_MsgData.eepReadCmd = VCUCOMM_EEPROM_READ_NORMAL;
                }
            }
        } else {
            VcuComm_MsgData.eepromCommand = VCUCOMM_EEPROM_OPERATE_CMD_ERROR;
        }
    }

#if ( VCUCOMM_DEV_ERROR_DETECT == STD_ON )
cleanup:
#endif
    return;
}

void VcuComm_RecTimeoutCbk(void)
{
    // VcuComm_MsgData.PowerCommand = VCUCOMM_POWER_OFF;
    // VcuComm_SetCommunicationStatus(FALSE);
    // VcuComm_SetCommAbortMessageFlag((uint16)PDUR_VCUCOMM_RX_PDUID_12345678);
}



// Condition Check function define
Std_ReturnType VcuComm_SendConditionCheck(uint16 IPdu)
{
    Std_ReturnType res = E_NOT_OK;

    if (IPdu == (uint16)PDUR_VCUCOMM_TX_PDUID_000005F2) {
        if (VcuComm_MsgData.eepromCommand == VCUCOMM_EEPROM_OPERATE_CMD_WIRTE ||
            VcuComm_MsgData.eepromCommand == VCUCOMM_EEPROM_OPERATE_CMD_READ) {
            res = E_OK;
        }
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
    if (current < (-1) * cur_offset)
    {
        vcu_current = 0U;
    }
    else if (current > cur_offset)
    {
        vcu_current = (uint16)cur_offset * 2U;
    }
    else
    {
        current *= -1;
        vcu_current = (uint16)current + (uint16)cur_offset;
    }
    return vcu_current;
}

uint8 VcuComm_Bms2VcuSendLevel(uint8 level)
{
    if (level > DIAGNOSIS_LEVEL_THIRD) {
        level = DIAGNOSIS_LEVEL_THIRD;
    }
    return level;
}
