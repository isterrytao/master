#include "CurrentM.h"
#include "Hv.h"
#include "SystemConnection_Lcfg.h"
#include "Soc.h"
#include "CellDataM.h"
#include "statistic.h"
#include "Hmi.h"
#include "ChargeConnectM.h"
#include "ChargerComm.h"
#include "RelayM_Lcfg.h"
#include "RelayM.h"
#include "ParameterM.h"
#include "Soh.h"
#include "Datetime.h"
#include "DatetimeM.h"
#include "DischargeM.h"
#include "ChargeM.h"
#include "Insu.h"
#include "UserStrategy.h"
#include "TemperatureM.h"

static uint16 MDBSgetTotalVoltage(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void)addr;
    (void)p;
    rc = Statistic_GetBcu100mvTotalVoltage();
    rc = (rc + 5U) / 10U;
    return rc;
}

static uint16 MDBSgetTotalCurrent(const Modbus_ReadRegionType *p, uint16 addr) {
    sint16 rc;
    (void) addr;
    (void)p;
    rc =  (sint16)CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    if (rc > (-32766L)) {
        if (rc < 0) {
            rc = (rc - 5) / 10;
        } else {
            rc = (rc + 5) / 10;
        }
    } else {
    }
    return (uint16)rc;
}
static uint16 MDBSgetSOC(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Soc_Get();
    rc = (uint16)((rc + 5U) / 10U);
    return rc;
}
static uint16 MDBSgetCellNum(const Modbus_ReadRegionType *p, uint16 addr) {
    (void) addr;
    (void)p;
    return (uint16)SYSTEM_BATTERY_CELL_NUM;
}
static uint16 MDBSgetStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    (void) addr;
    (void)p;
    if (CHARGECONNECTM_IS_CONNECT())
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetVoltMax(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Statistic_GetBcuHvMax();
    return rc;
}
static uint16 MDBSgetVoltMaxIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = CellDataM_VoltageIndexToLogic(Statistic_GetBcuHvSlaveNum(0U), Statistic_GetBcuHvIndex(0U)) + 1U;
    if (rc == 0xFFFFU)
    {
        rc = 0U;
    }
    return rc;
}
static uint16 MDBSgetVoltMaxBmu(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)Statistic_GetBcuHvSlaveNum(0U) + 1U;
    if (rc == 0xFFFFU)
    {
        rc = 0U;
    }
    return rc;
}
static uint16 MDBSgetVoltMaxBmuSn(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)Statistic_GetBcuHvIndex(0U) + 1U;
    if (rc == 0xFFFFU)
    {
        rc = 0U;
    }
    return rc;
}
static uint16 MDBSgetVoltMin(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Statistic_GetBcuLvMax();
    return rc;
}
static uint16 MDBSgetVoltMinIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = CellDataM_VoltageIndexToLogic(Statistic_GetBcuLvSlaveNum(0U), Statistic_GetBcuLvIndex(0U)) + 1U;
    if (rc == 0xFFFFU)
    {
        rc = 0U;
    }
    return rc;
}
static uint16 MDBSgetVoltMinBmu(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)Statistic_GetBcuLvSlaveNum(0U) + 1U;
    if (rc == 0xFFU)
    {
        rc = 0U;
    }
    return rc;
}
static uint16 MDBSgetVoltMinBmuSn(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)Statistic_GetBcuLvIndex(0U) + 1U;
    if (rc == 0xFFU)
    {
        rc = 0U;
    }
    return rc;
}
static uint16 MDBSgetTempMax(const Modbus_ReadRegionType *p, uint16 addr) {
    sint16 rc;
    (void) addr;
    (void)p;
    rc = (sint)Statistic_GetBcuHtMax();
    if ((rc >= 0) && (rc <= 175)) {
        rc -= 50;
    } else {
    }
    return (uint16)rc;
}
static uint16 MDBSgetTempMaxIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = CellDataM_TemperatureIndexToLogic(Statistic_GetBcuHtSlaveNum(0U), Statistic_GetBcuHtIndex(0U)) + 1U;
    return rc;
}
static uint16 MDBSgetTempMaxBmu(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)Statistic_GetBcuHtSlaveNum(0U) + 1U;
    return rc;
}
static uint16 MDBSgetTempMaxBmuSn(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)Statistic_GetBcuHtIndex(0U) + 1U;
    return rc;
}
static uint16 MDBSgetTempMin(const Modbus_ReadRegionType *p, uint16 addr) {
    sint16 rc;
    (void) addr;
    (void)p;
    rc = (sint)Statistic_GetBcuLtMax();
    if ((rc >= 0) && (rc <= 175)) {
        rc -= 50;
    } else {
    }
    return (uint16)rc;
}
static uint16 MDBSgetTempMinIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = CellDataM_TemperatureIndexToLogic(Statistic_GetBcuLtSlaveNum(0U), Statistic_GetBcuLtIndex(0U)) + 1U;
    return rc;
}
static uint16 MDBSgetTempMinBmu(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)Statistic_GetBcuLtSlaveNum(0U) + 1U;
    return rc;
}
static uint16 MDBSgetTempMinBmuSn(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)Statistic_GetBcuLtIndex(0U) + 1U;
    return rc;
}
static uint16 MDBSgetChgRequestVolt(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    (void) addr;
    (void)p;
    if (CHARGECONNECTM_IS_CONNECT())
    {
        if (ChargeM_ChargeIsFault() == E_NOT_OK)
        {
            rc = ChargerComm_GetChargeVoltMax();
        }
    }
    return rc;
}
static uint16 MDBSgetChgRequestCur(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    (void) addr;
    (void)p;
    if (CHARGECONNECTM_IS_CONNECT())
    {
        if (ChargeM_ChargeIsFault() == E_NOT_OK)
        {
            rc = (uint16)ChargerComm_GetChargeCurrentMax();
        }
    }
    return rc;
}
static uint16 MDBSgetChgRequestOrder(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    (void) addr;
    (void)p;
    if (CHARGECONNECTM_IS_CONNECT())
    {
        if (ChargeM_ChargeIsFault() == E_OK)
        {
            rc = 1U;
        }
    }
    else
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetChargerOutVolt(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = ChargerComm_GetChargerOutputHV();
    return rc;
}
static uint16 MDBSgetChargerOutCur(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)ChargerComm_GetChargerOutputCurrent();
    return rc;
}
static uint16 MDBSgetChargerStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sta;
    (void) addr;
    (void)p;
    if (CHARGECONNECTM_IS_CONNECT())
    {
        //充电机硬件故障
        sta = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_HARDWARE_FAULT_INDEX);
        if (sta != 0U &&
            sta != 0xFFU)
        {
            rc = 1U;
        }
        //充电机温度异常
        sta = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_OVER_TEMPERATURE_FAULT_INDEX);
        if (sta != 0U &&
           sta != 0xFFU)
        {
            rc |= ((uint16)1U << 1);
        }
        //充电机输入电压异常
        sta = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_INPUT_VOLT_FAULT_INDEX);
        if (sta != 0U &&
           sta != 0xFFU)
        {
            rc |= ((uint16)1U << 2);
        }
        // 充电机运行（启动）状态
        sta = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_RUN_STATUS_INDEX);
        if (sta != 0U &&
            sta != 0xFFU)
        {
            rc |= ((uint16)1U << 3);
        }
        //充电机通信异常
        sta = ChargerComm_GetChargerFaultWithIndex(CHARGERCOMM_CHR_COMM_ABORT_WITH_BMS_INDEX);
        if (sta != 0U &&
            sta != 0xFFU)
        {
            rc |= ((uint16)1U << 4);
        }
    }
    else
    {
        rc |= ((uint16)1U << 3);
    }
    return rc;
}
// 继电器相关状态上报，序号与RelayM_Lcfg.h中一致，继电器索引号参考协议中继电器定义部分
static uint16 getRlyIndex(uint8 num) {
    uint16 rc = 29U;

#ifdef RELAYM_FN_POSITIVE_MAIN
    if (RELAYM_FN_POSITIVE_MAIN == num)
    {
        rc = 0U;
    }
#endif
#ifdef RELAYM_FN_NEGATIVE_MAIN
    if (RELAYM_FN_NEGATIVE_MAIN == num)
    {
        rc = 1U;
    }
#endif
#ifdef RELAYM_FN_DISCHARGE
    if (RELAYM_FN_DISCHARGE == num)
    {
        rc = 2U;
    }
#endif
#ifdef RELAYM_FN_PRECHARGE
    if (RELAYM_FN_PRECHARGE == num)
    {
        rc = 3U;
    }
#endif
#ifdef RELAYM_FN_CHARGE
    if (RELAYM_FN_CHARGE == num)
    {
        rc = 4U;
    }
#endif
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    if (RELAYM_FN_POSITIVE_AC_CHARGE == num)
    {
        rc = 5U;
    }
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
    if (RELAYM_FN_POSITIVE_DC_CHARGE == num)
    {
        rc = 6U;
    }
#endif
#ifdef RELAYM_FN_HEATER
    if (RELAYM_FN_HEATER == num)
    {
        rc = 7U;
    }
#endif
#ifdef RELAYM_FN_SELF_LOCK
    if (RELAYM_FN_SELF_LOCK == num)
    {
        rc = 10U;
    }
#endif
#ifdef RELAYM_FN_BUZZER
    if (RELAYM_FN_BUZZER == num)
    {
        rc = 11U;
    }
#endif
#ifdef RELAYM_FN_COOLER
    if (RELAYM_FN_COOLER == num)
    {
        rc = 16U;
    }
#endif
#ifdef RELAYM_FN_HMI
    if (RELAYM_FN_HMI == num)
    {
        rc = 12U;
    }
#endif
#ifdef RELAYM_FN_ALARM
    if (RELAYM_FN_ALARM == num)
    {
        rc = 28U;
    }
#endif
#ifdef RELAYM_FN_POSITIVE_MAIN_LED
    if (RELAYM_FN_POSITIVE_MAIN_LED == num)
    {
        rc = 30U;
    }
#endif
#ifdef RELAYM_FN_SELF_LOCK_LED
    if (RELAYM_FN_SELF_LOCK_LED == num)
    {
        rc = 31U;
    }
#endif

    return rc;
}

static uint16 MDBSgetFirstRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;

    (void) addr;
    (void)p;
    rc = getRlyIndex(0U);

    return rc;
}
static uint16 MDBSgetFirstRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 0U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetFirstRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 0U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetSecondRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;

    (void) addr;
    (void)p;
    rc = getRlyIndex(1U);

    return rc;
}
static uint16 MDBSgetSecondRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 1U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetSecondRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 1U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetThirdRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;

    (void) addr;
    (void)p;
    rc = getRlyIndex(2U);

    return rc;
}
static uint16 MDBSgetThirdRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 2U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetThirdRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 2U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetFourthRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;

    (void) addr;
    (void)p;
    rc = getRlyIndex(3U);

    return rc;
}
static uint16 MDBSgetFourthRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 3U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetFourthRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 3U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetFifthRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;

    (void) addr;
    (void)p;
    rc = getRlyIndex(4U);

    return rc;
}
static uint16 MDBSgetFifthRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 4U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetFifthRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 4U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetSixthRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = getRlyIndex(5U);

    return rc;
}
static uint16 MDBSgetSixthRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 5U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetSixthRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 5U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetSeventhRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = getRlyIndex(6U);

    return rc;
}
static uint16 MDBSgetSeventhRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 6U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetSeventhRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 6U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetEighthRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    // 预留
    rc = 31U;
    return rc;
}
static uint16 MDBSgetEighthRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 7U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetEighthRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 7U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetNinthRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    // 预留
    rc = 31U;
    return rc;
}
static uint16 MDBSgetNinthRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 8U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetNinthRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 8U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetTenthRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    // 预留
    rc = 31U;
    return rc;
}
static uint16 MDBSgetTenthRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 9U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetTenthRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 9U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetEleventhRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    // 预留
    rc = 31U;
    return rc;
}
static uint16 MDBSgetEleventhRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 10U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetEleventhRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 10U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetTwelfthRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    // 预留
    rc = 31U;
    return rc;
}
static uint16 MDBSgetTwelfthRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 11U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetTwelfthRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 11U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetThirteenRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    // 预留
    rc = 31U;
    return rc;
}
static uint16 MDBSgetThirteenRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 12U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetThirteenRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 12U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetFourteenRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    // 预留
    rc = 31U;
    return rc;
}
static uint16 MDBSgetFourteenRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 13U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetFourteenRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 13U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetFifteenRlyIndex(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    // 预留
    rc = 31U;
    return rc;
}
static uint16 MDBSgetFifteenRlyStatus(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 sn = 14U;
    (void) addr;
    (void)p;
    if (RelayM_GetActualStatus(sn) == RELAYM_ACTUAL_ON)
    {
        rc = 1U;
    }
    return rc;
}
static uint16 MDBSgetFifteenRlyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    uint8 diag;
    uint8 sn = 14U;
    (void) addr;
    (void)p;
    diag = RelayM_GetDiagnosisStatus(sn);
    if (RELAYM_DIAGNOSIS_IS_ADHESIVE(diag))
    {
        rc |= ((uint16)1U << 0);
    }
    else if (RELAYM_DIAGNOSIS_IS_OPEN(diag))
    {
        rc |= ((uint16)2U << 0);
    }
    else if (((diag) & RELAYM_DIAGNOSIS_DRIVER_STATE_MASK) != 0U)
    {
        rc |= ((uint16)3U << 0);
    }
    else
    {
    }
    return rc;
}
static uint16 MDBSgetRelayNum(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) p;
    (void) addr;
    rc = RELAYM_FN_NUM;
    return rc;
}
static uint16 MDBSgetChargeCapHigh(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 cap;
    uint16 rc;
    (void) addr;
    (void)p;
    cap = Soc_GetCumuChgPower();
    rc = (uint16)(cap >> 16);
    return rc;
}
static uint16 MDBSgetChargeCapLow(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 cap;
    uint16 rc;
    (void) addr;
    (void)p;
    cap = Soc_GetCumuChgPower();
    rc = (uint16)(cap);
    return rc;
}
static uint16 MDBSgetDischargeCapHigh(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 cap;
    uint16 rc;
    (void) addr;
    (void)p;
    cap = Soc_GetCumuDchgPower();
    rc = (uint16)(cap >> 16);
    return rc;
}
static uint16 MDBSgetDischargeCapLow(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 cap;
    uint16 rc;
    (void) addr;
    (void)p;
    cap = Soc_GetCumuDchgPower();
    rc = (uint16)cap;
    return rc;
}
static uint16 MDBSgetTempSensorNum(const Modbus_ReadRegionType *p, uint16 addr) {
    (void) addr;
    (void)p;
    return (uint16)SYSTEM_TEMP_CELL_NUM;
}
static uint16 MDBSgetHeatSensorNum(const Modbus_ReadRegionType *p, uint16 addr) {
    (void) addr;
    (void)p;
    return (uint16)SYSTEM_HEAT_TEMP_NUM;
}
static uint16 MDBSgetPoleTempNum(const Modbus_ReadRegionType *p, uint16 addr) {
    (void) addr;
    (void)p;
    return (uint16)SYSTEM_POLE_TEMP_NUM;
}
/*2000-2399*/
static uint16 MDBSget_CellDataM_GetVoltage(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    rc = CellDataM_GetVoltage(addr - p->startaddr);
    return rc;
}
/*3000,3199*/
static uint16 MDBSget_CellDataM_GetTemperature(const Modbus_ReadRegionType *p, uint16 addr) {
    sint16 rc;
    rc = (sint16)CellDataM_GetTemperature(addr - p->startaddr);
    if ((rc >= 0) && (rc <= 175)) {
        rc -= 50;
    } else {
    }
    return (uint16)rc;
}
/*3500,3549,*/
static uint16 MDBSget_CellDataM_GetHeatTemperature(const Modbus_ReadRegionType *p, uint16 addr) {
    sint16 rc;
    rc = (sint16)CellDataM_GetHeatTemperature(addr - p->startaddr);
    if ((rc >= 0) && (rc <= 175)) {
        rc -= 50;
    } else {
    }
    return (uint16)rc;
}
/*3600,3649,*/
static uint16 MDBSget_CellDataM_GettPoleTemperature(const Modbus_ReadRegionType *p, uint16 addr) {
    sint16 rc;
    rc = (sint16)CellDataM_GetPoleTemperature(addr - p->startaddr);
    if ((rc >= 0) && (rc <= 175)) {
        rc -= 50;
    } else {
    }
    return (uint16)rc;
}
static uint16 MDBSgetalarm(const Modbus_ReadRegionType *p, uint16 addr) {
    uint8 i, j;
    uint16 trc, rc = 0U;
    for (i = 0U; i < 16U; i++) {
        if (Diagnosis_GetLevel((Diagnosis_ItemType)(i + addr - p->startaddr))) {
            rc = (rc | ((uint16)0x0001U << i));
        }
    }
    trc = rc & 0x00ffU;
    j = (uint8)(rc >> 8U);
    rc = (trc << 8U) | j;
    return rc;
}

static uint16 MDBSgetSysMode(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    (void)addr;
    (void)p;

#if SYSTEM_BMU_NUM > 1U
    {
        rc = 1U;
    }
#endif

    return rc;
}

static uint16 MDBSgetBatDeltVolt(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Statistic_GetBcuDeltaVoltage();
    return rc;
}
static uint16 MDBSgetBatDeltTemp(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Statistic_GetBcuDeltaTemperature();
    rc -= 50U;
    return rc;
}
static uint16 MDBSgetBatCap(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_TOTAL_CAP_INDEX, &rc);
    return rc;
}
static uint16 MDBSgetBatLeftCap(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    (void)ParameterM_EeepRead(PARAMETERM_EEEP_LEFT_CAP_INDEX, &rc);
    return rc;
}
static uint16 MDBSgetPoInsu(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Insu_GetPositive();
    return rc;
}
static uint16 MDBSgetNeInsu(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Insu_GetNegative();
    return rc;
}
static uint16 MDBSgetTotalVoltage100mV(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Statistic_GetBcu100mvTotalVoltage();
    return rc;
}
static uint16 MDBSgetTotalCurrent100mA(const Modbus_ReadRegionType *p, uint16 addr) {
    sint16 rc;
    (void) addr;
    (void)p;
    rc = (sint16)CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    return (uint16)rc;
}
static uint16 MDBSgetSOCWithResolution(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Soc_Get();
    return rc;
}
static uint16 MDBSgetSOH(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = Soh_Get();
    return rc;
}
static uint16 MDBSgetHeatCur(const Modbus_ReadRegionType *p, uint16 addr) {
    sint16 rc;
    (void) addr;
    (void)p;
    rc = (sint16)(CurrentM_GetCurrent(CURRENTM_CHANNEL_HEATER));
    return (uint16)rc;
}
static uint16 MDBSgetLevelMax(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)Diagnosis_GetDiagLevelMax();
    return rc;
}
static uint16 MDBSgetChgTime(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 rc;
    (void) addr;
    (void)p;
    rc = Statistic_GetEclipseChargeTime();
    rc = DIVISION(rc , 60U);
    return (uint16)rc;
}
static uint16 MDBSgetYear(const Modbus_ReadRegionType *p, uint16 addr) {
    Datetime_DatetimeType date;
    uint32 rc;
    (void) addr;
    (void)p;
    if (DatetimeM_GetDatetime(&rc) == DATETIME_TRUSTY)
    {
        if (Datetime_FromSecond(&date, rc) == E_OK)
        {
            rc = date.year;
            rc += 2000U;
        }
    }
    return (uint16)rc;
}
static uint16 MDBSgetMonth(const Modbus_ReadRegionType *p, uint16 addr) {
    Datetime_DatetimeType date;
    uint32 rc;
    (void) addr;
    (void)p;
    if (DatetimeM_GetDatetime(&rc) == DATETIME_TRUSTY)
    {
        if (Datetime_FromSecond(&date, rc) == E_OK)
        {
            rc = date.month;
        }
    }
    return (uint16)rc;
}
static uint16 MDBSgetDay(const Modbus_ReadRegionType *p, uint16 addr) {
    Datetime_DatetimeType date;
    uint32 rc;
    (void) addr;
    (void)p;
    if (DatetimeM_GetDatetime(&rc) == DATETIME_TRUSTY)
    {
        if (Datetime_FromSecond(&date, rc) == E_OK)
        {
            rc = date.day;
        }
    }
    return (uint16)rc;
}
static uint16 MDBSgetHour(const Modbus_ReadRegionType *p, uint16 addr) {
    Datetime_DatetimeType date;
    uint32 rc;
    (void) addr;
    (void)p;
    if (DatetimeM_GetDatetime(&rc) == DATETIME_TRUSTY)
    {
        if (Datetime_FromSecond(&date, rc) == E_OK)
        {
            rc = date.hour;
        }
    }
    return (uint16)rc;
}
static uint16 MDBSgetMinute(const Modbus_ReadRegionType *p, uint16 addr) {
    Datetime_DatetimeType date;
    uint32 rc;
    (void) addr;
    (void)p;
    if (DatetimeM_GetDatetime(&rc) == DATETIME_TRUSTY)
    {
        if (Datetime_FromSecond(&date, rc) == E_OK)
        {
            rc = date.minute;
        }
    }
    return (uint16)rc;
}
static uint16 MDBSgetSeconds(const Modbus_ReadRegionType *p, uint16 addr) {
    Datetime_DatetimeType date;
    uint32 rc;
    (void) addr;
    (void)p;
    if (DatetimeM_GetDatetime(&rc) == DATETIME_TRUSTY)
    {
        if (Datetime_FromSecond(&date, rc) == E_OK)
        {
            rc = date.second;
        }
    }
    return (uint16)rc;
}
static uint16 MDBSgetCmuDchgTimeHigh(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 rc;
    uint32 temp;
    (void) addr;
    (void)p;

    temp = Statistic_GetCumuDchgTime();
    rc = temp / 360U;

    return (uint16)(rc >> 16);
}
static uint16 MDBSgetCmuDchgTimeLow(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 rc;
    uint32 temp;
    (void) addr;
    (void)p;

    temp = Statistic_GetCumuDchgTime();
    rc = temp / 360U;
    return (uint16)(rc);
}
static uint16 MDBSgetCmuChgTimeHigh(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 rc;
    uint32 temp;
    (void) addr;
    (void)p;

    temp = Statistic_GetCumuChgTime();
    rc = temp / 360U;

    return (uint16)(rc >> 16);
}
static uint16 MDBSgetCmuChgTimeLow(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 rc;
    uint32 temp;
    (void) addr;
    (void)p;

    temp = Statistic_GetCumuChgTime();
    rc = temp / 360U;

    return (uint16)(rc);
}
static uint16 MDBSgetTotalTimeHigh(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 rc;
    uint32 temp;
    (void) addr;
    (void)p;

    temp = Statistic_GetCumuTime();
    rc = temp / 360U;

    return (uint16)(rc >> 16);
}
static uint16 MDBSgetTotalTimeLow(const Modbus_ReadRegionType *p, uint16 addr) {
    uint32 rc;
    uint32 temp;
    (void) addr;
    (void)p;

    temp = Statistic_GetCumuTime();
    rc = temp / 360U;

    return (uint16)(rc);
}
static uint16 MDBSgetSocFullDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)SocDiagCalib_GetFullReason();
    return rc;
}
static uint16 MDBSgetSocEmptyDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)SocDiagCalib_GetEmptyReason();
    return rc;
}
static uint16 MDBSgetDchgStartDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)DischargeM_GetStartDiagFault();
    return rc;
}
static uint16 MDBSgetDchgFlagDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)DischargeM_GetDiagFaultFlag();
    return rc;
}
static uint16 MDBSgetDchgActionDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)DischargeM_GetDiagFaultAction();
    return rc;
}
static uint16 MDBSgetDchgOtherDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)DischargeM_GetOthersFault();
    return rc;
}
static uint16 MDBSgetChgStartDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)ChargeM_GetStartDiagFault();
    return rc;
}
static uint16 MDBSgetChgFlagDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)ChargeM_GetDiagFaultFlag();
    return rc;
}
static uint16 MDBSgetChgActionDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)ChargeM_GetDiagFaultAction();
    return rc;
}
static uint16 MDBSgetChgOtherDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)ChargeM_GetOthersFault();
    return rc;
}
static uint16 MDBSgetGBDia(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc;
    (void) addr;
    (void)p;
    rc = (uint16)ChargeM_GetChargerGBReadyFault();
    return rc;
}
static uint16 MDBSgetBuzzerSta(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    (void) addr;
    (void)p;
    if (UserStrategy_GetBuzzerSta())
    {
        rc = 1U;
    }

    return rc;
}

static uint16 MDBSgetHeatSta(const Modbus_ReadRegionType *p, uint16 addr) {
    uint16 rc = 0U;
    (void) addr;
    (void)p;
    if (TemperatureM_GetHeatState() == TEMPERATUREM_HEAT_STATE_LT)
    {
        rc = 1U;
    }

    return rc;
}

static uint16 test_modbus_x01(const Modbus_ReadRegionType *p, uint16 addr) {
    (void) p;
    (void) addr;
    return 0x1234U;
}

static void modbus_ctrl_mos(const struct Modbus_WriteRegion *regs, uint16 addr, const uint8 *dat, uint16 len) {
    (void) regs;
    (void) addr;
    (void) dat;
    (void) len;
}

static const Modbus_ReadRegionType x01registerTable[] = {
    {2000U, 2399U, test_modbus_x01},
};
static const Modbus_ReadRegionType x02registerTable[] = {
    {2000U, 2399U, MDBSgetalarm},
    {1000U, 1000U, MDBSgetSysMode},
};
static const Modbus_ReadRegionType x03registerTable[] = {
    {2000U, 2399U, test_modbus_x01},
};
// static uint16 MDBSgetTestValue(const Modbus_ReadRegionType *p, uint16 addr) {
//     static uint16 rc = 4U;
//     uint32 nowTime = OSTimeGet();
//     static uint32 lastTime = 0U;
//     (void) p;
//     (void) addr;
//     if (rc > 11U)
//     {
//         rc = 4U;
//         lastTime = 0U;
//     }
//     if (lastTime == 0U)
//     {
//         lastTime = nowTime;
//     }
//     else
//     {
//         if (MS_GET_INTERNAL(lastTime, nowTime) >= 30000UL)
//         {
//             lastTime = 0U;
//             rc ++;
//         }
//     }
//     return rc;
// }
static const Modbus_ReadRegionType x04registerTable[] = {
    {2000U, 2399U, MDBSget_CellDataM_GetVoltage},
    /*  {2400, 2999, get2},*/
    {3000U, 3199U, MDBSget_CellDataM_GetTemperature},
    /*  {3200,3499, getCellVolVisable},*/
    {3500U, 3549U, MDBSget_CellDataM_GetHeatTemperature},
    /*  {3550,3599, getCellVolVisable},*/
    {3600U, 3649U, MDBSget_CellDataM_GettPoleTemperature},
    /*  {3650,3699, getCellVolVisable},*/
    {1000U, 1000U, MDBSgetTotalVoltage},
    {1001U, 1001U, MDBSgetTotalCurrent},
    {1002U, 1002U, MDBSgetSOC},
    {1003U, 1003U, MDBSgetCellNum},
    {1004U, 1004U, MDBSgetTempSensorNum},
    {1005U, 1005U, MDBSgetHeatSensorNum},
    {1006U, 1006U, MDBSgetPoleTempNum},
    {1007U, 1007U, MDBSgetRelayNum},
    {1008U, 1008U, MDBSgetBatDeltVolt},
    {1009U, 1009U, MDBSgetBatDeltTemp},
    {101U, 101U, MDBSgetStatus},
    {1010U, 1010U, MDBSgetChargeCapLow},
    {1011U, 1011U, MDBSgetChargeCapHigh},
    {1012U, 1012U, MDBSgetDischargeCapLow},
    {1013U, 1013U, MDBSgetDischargeCapHigh},
    {1014U, 1014U, MDBSgetBatCap},
    {1015U, 1015U, MDBSgetBatLeftCap},
    {1030U, 1030U, MDBSgetPoInsu},
    {1031U, 1031U, MDBSgetNeInsu},
    {1050U, 1050U, MDBSgetTotalVoltage100mV},
    {1051U, 1051U, MDBSgetTotalCurrent100mA},
    {1052U, 1052U, MDBSgetSOCWithResolution},
    {1053U, 1053U, MDBSgetSOH},
    {1054U, 1054U, MDBSgetHeatCur},
    {1055U, 1055U, MDBSgetLevelMax},
    {1100U, 1100U, MDBSgetVoltMax},
    {1101U, 1101U, MDBSgetVoltMaxIndex},
    {1102U, 1102U, MDBSgetVoltMaxBmu},
    {1103U, 1103U, MDBSgetVoltMaxBmuSn},
    {1150U, 1150U, MDBSgetVoltMin},
    {1151U, 1151U, MDBSgetVoltMinIndex},
    {1152U, 1152U, MDBSgetVoltMinBmu},
    {1153U, 1153U, MDBSgetVoltMinBmuSn},
    {1200U, 1200U, MDBSgetTempMax},
    {1201U, 1201U, MDBSgetTempMaxIndex},
    {1202U, 1202U, MDBSgetTempMaxBmu},
    {1203U, 1203U, MDBSgetTempMaxBmuSn},
    {1250U, 1250U, MDBSgetTempMin},
    {1251U, 1251U, MDBSgetTempMinIndex},
    {1252U, 1252U, MDBSgetTempMinBmu},
    {1253U, 1253U, MDBSgetTempMinBmuSn},
    {1500U, 1500U, MDBSgetChgRequestVolt},
    {1501U, 1501U, MDBSgetChgRequestCur},
    {1502U, 1502U, MDBSgetChgRequestOrder},
    {1503U, 1503U, MDBSgetChgTime},
    {1550U, 1550U, MDBSgetChargerOutVolt},
    {1551U, 1551U, MDBSgetChargerOutCur},
    {1552U, 1552U, MDBSgetChargerStatus},
    {1600U, 1600U, MDBSgetFirstRlyIndex},
    {1601U, 1601U, MDBSgetSecondRlyIndex},
    {1602U, 1602U, MDBSgetThirdRlyIndex},
    {1603U, 1603U, MDBSgetFourthRlyIndex},
    {1604U, 1604U, MDBSgetFifthRlyIndex},
    {1605U, 1605U, MDBSgetSixthRlyIndex},
    {1606U, 1606U, MDBSgetSeventhRlyIndex},
    {1607U, 1607U, MDBSgetEighthRlyIndex},
    {1608U, 1608U, MDBSgetNinthRlyIndex},
    {1609U, 1609U, MDBSgetTenthRlyIndex},
    {1610U, 1610U, MDBSgetEleventhRlyIndex},
    {1611U, 1611U, MDBSgetTwelfthRlyIndex},
    {1612U, 1612U, MDBSgetThirteenRlyIndex},
    {1613U, 1613U, MDBSgetFourteenRlyIndex},
    {1614U, 1614U, MDBSgetFifteenRlyIndex},
    {1650U, 1650U, MDBSgetFirstRlyStatus},
    {1651U, 1651U, MDBSgetSecondRlyStatus},
    {1652U, 1652U, MDBSgetThirdRlyStatus},
    {1653U, 1653U, MDBSgetFourthRlyStatus},
    {1654U, 1654U, MDBSgetFifthRlyStatus},
    {1655U, 1655U, MDBSgetSixthRlyStatus},
    {1656U, 1656U, MDBSgetSeventhRlyStatus},
    {1657U, 1657U, MDBSgetEighthRlyStatus},
    {1658U, 1658U, MDBSgetNinthRlyStatus},
    {1659U, 1659U, MDBSgetTenthRlyStatus},
    {1660U, 1660U, MDBSgetEleventhRlyStatus},
    {1661U, 1661U, MDBSgetTwelfthRlyStatus},
    {1662U, 1662U, MDBSgetThirteenRlyStatus},
    {1663U, 1663U, MDBSgetFourteenRlyStatus},
    {1664U, 1664U, MDBSgetFifteenRlyStatus},
    {1700U, 1700U, MDBSgetFirstRlyDia},
    {1701U, 1701U, MDBSgetSecondRlyDia},
    {1702U, 1702U, MDBSgetThirdRlyDia},
    {1703U, 1703U, MDBSgetFourthRlyDia},
    {1704U, 1704U, MDBSgetFifthRlyDia},
    {1705U, 1705U, MDBSgetSixthRlyDia},
    {1706U, 1706U, MDBSgetSeventhRlyDia},
    {1707U, 1707U, MDBSgetEighthRlyDia},
    {1708U, 1708U, MDBSgetNinthRlyDia},
    {1709U, 1709U, MDBSgetTenthRlyDia},
    {1710U, 1710U, MDBSgetEleventhRlyDia},
    {1711U, 1711U, MDBSgetTwelfthRlyDia},
    {1712U, 1712U, MDBSgetThirteenRlyDia},
    {1713U, 1713U, MDBSgetFourteenRlyDia},
    {1714U, 1714U, MDBSgetFifteenRlyDia},
    {1750U, 1750U, MDBSgetYear},
    {1751U, 1751U, MDBSgetMonth},
    {1752U, 1752U, MDBSgetDay},
    {1753U, 1753U, MDBSgetHour},
    {1754U, 1754U, MDBSgetMinute},
    {1755U, 1755U, MDBSgetSeconds},
    {1800U, 1800U, MDBSgetCmuDchgTimeLow},
    {1801U, 1801U, MDBSgetCmuDchgTimeHigh},
    {1802U, 1802U, MDBSgetCmuChgTimeLow},
    {1803U, 1803U, MDBSgetCmuChgTimeHigh},
    {1804U, 1804U, MDBSgetTotalTimeLow},
    {1805U, 1805U, MDBSgetTotalTimeHigh},
    {1820U, 1820U, MDBSgetSocFullDia},
    {1821U, 1821U, MDBSgetSocEmptyDia},
    {1830U, 1830U, MDBSgetDchgStartDia},
    {1831U, 1831U, MDBSgetDchgFlagDia},
    {1832U, 1832U, MDBSgetDchgActionDia},
    {1833U, 1833U, MDBSgetDchgOtherDia},
    {1850U, 1850U, MDBSgetChgStartDia},
    {1851U, 1851U, MDBSgetChgFlagDia},
    {1852U, 1852U, MDBSgetChgActionDia},
    {1853U, 1853U, MDBSgetChgOtherDia},
    {1854U, 1854U, MDBSgetGBDia},
    {102U, 102U, MDBSgetBuzzerSta},
    {103U, 103U, MDBSgetHeatSta},
};

static const Modbus_WriteRegionType x06registerTable[] = {
    {3001U, 3001U, modbus_ctrl_mos},
};

static const Modbus_WriteRegionType x16registerTable[] = {
    {3001U, 3001U, modbus_ctrl_mos},
};

const Modbus_Cfg ModbusCfg = {
    0x01U, //uint8  slaveaddr;
    0x00U, //uint8  broadcastaddr;
    0xa001U, //uint16 CRCpolynomial;
    0xffffU, //uint16 CRCInit;
    100U, //uint16 FrameTimeout
    {x01registerTable, (uint8)ARRAY_SIZE(x01registerTable), 16U}, //Modbus_RegistersTableType Modbus_X01registerTable;
    {x02registerTable, (uint8)ARRAY_SIZE(x02registerTable), 16U}, //Modbus_RegistersTableType Modbus_X02registerTable;
    {x03registerTable, (uint8)ARRAY_SIZE(x03registerTable), 1U}, //Modbus_RegistersTableType odbus_X03registerTable;
    {x04registerTable, (uint8)ARRAY_SIZE(x04registerTable), 1U}, //Modbus_RegistersTableType *Modbus_X04registerTable;
    {x06registerTable, (uint8)ARRAY_SIZE(x06registerTable), 1U}, //Modbus_RegistersTableType *Modbus_X06registerTable;
    {x16registerTable, (uint8)ARRAY_SIZE(x16registerTable), 1U}, //Modbus_RegistersTableType *Modbus_X16registerTable;
};
