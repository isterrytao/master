/**
 * \file HvProcess_Chg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 高压充电流程控制文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170320 |
 */
#include "Cpu.h"
#include "HvProcess_Dchg.h"
#include "HvProcess_Chg.h"
#include "ChargeConnectM.h"
#include "ChargeM.h"
#include "DischargeM.h"
#include "RelayM.h"
#include "RelayM_Lcfg.h"
#include "ChargerComm.h"
#include "RuntimeM.h"
#include "Stdlib.h"
#include "UserStrategy.h"
#include "Statistic.h"
#include "AppParameterM.h"
#include "CellDataM.h"
#include "PowerM.h"
#include "UserStrategy_Cfg.h"
#include "BridgeInsu_Cfg.h"
#include "VcuComm_Messages.h"
#include "DatetimeM.h"
#include "ParameterM.h"

static HvProcess_ChgInnerDataType HvProcess_ChgInnerData;

void HvProcess_ChgInit(Async_LooperType *looper)
{
    (void)looper;
    HvProcess_ChgInnerData.State = HVPROCESS_CHG_START;
}

HvProcess_ChgStateType HvProcess_GetChgState(void)
{
    HvProcess_ChgStateType state;
    imask_t mask;

    Irq_Save(mask);
    state = HvProcess_ChgInnerData.State;
    Irq_Restore(mask);

    return state;
}

void HvProcess_SetChgState(HvProcess_ChgStateType state)
{
    imask_t mask;

    Irq_Save(mask);
    HvProcess_ChgInnerData.State = state;
    Irq_Restore(mask);
}

void HvProcess_ChgPoll(void)
{

}

boolean HvProcess_ChgStateStartCond(void)
{
    boolean res = FALSE;
    uint8 chargerIsComm;
    Std_ReturnType chargeReady;
    uint32 nowTime = OSTimeGet();
    HvProcess_DchgStateType dchgState;
    uint32 delay = 2500U;
#if defined(UPA530)||defined(UPA630)||defined(UPA640)
    delay = 500U;
#else
#if BRIDGEINSU_TYPE == BRIDGEINSU_MOS_OFF
    delay = 500U;
#endif
#endif

    dchgState = HvProcess_GetDchgState();
    if (CHARGECONNECTM_IS_CONNECT() && dchgState == HVPROCESS_DCHG_START && nowTime >= delay)
    {
        HvProcess_ChgSetHeatStateNone();

        if (!HvProcess_CheckRelayAdhesFlag())
        {
            HvProcess_SetRelayAdhesFlag();
            // HvProcess_ChgInnerData.RelayAdhesCheckFlag = TRUE;
            UserStrategy_ChgHvProcessAdhesiveDetect();
        }
        else
        {
            chargeReady = ChargeM_ChargeIsReady();
            chargerIsComm = ChargerComm_GetCommunicationStatus();
            if (UserStrategy_ChgHvProcessRelayIsNormal() &&
                chargeReady == E_OK &&
                chargerIsComm)
            {
                if (TemperatureM_GetHeatState() != TEMPERATUREM_HEAT_STATE_FAULT)
                {
                    res = TRUE;
                }
            }
        }
    }
    return res;
}

void HvProcess_ChgStateStartAction(void)
{
#ifdef XUGONG_VCU_FLAG
    uint32 time;
    uint16 temp;
    if (DatetimeM_GetDatetime(&time) == DATETIME_TRUSTY)
    {
        temp = (uint16)time;
        (void)ParameterM_EeepWrite(PARAMETERM_EEEP_START_CHG_TIME_L_INDEX, temp);
        temp = time >> 16;
        (void)ParameterM_EeepWrite(PARAMETERM_EEEP_START_CHG_TIME_H_INDEX, temp);
        HvProcess_ChgInnerData.IsCharging = TRUE;
    }
#endif
    HvProcess_ChgInnerData.chgRelay = RELAYM_FN_NONE;
    HvProcess_ChgInnerData.HeatRelayFaultCheckFlag = FALSE;
#ifdef RELAYM_FN_NEGATIVE_MAIN
    (void)RelayM_Control(RELAYM_FN_NEGATIVE_MAIN, RELAYM_CONTROL_ON);
#endif
#ifdef RELAYM_FN_CHARGE
        (void)RelayM_Control(RELAYM_FN_CHARGE, RELAYM_CONTROL_ON);
        HvProcess_ChgInnerData.chgRelay = RELAYM_FN_CHARGE;
#endif
    if (ChargerComm_GetChargeType() == CHARGE_TYPE_AC)
    {
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
        (void)RelayM_Control(RELAYM_FN_POSITIVE_AC_CHARGE, RELAYM_CONTROL_ON);
        HvProcess_ChgInnerData.chgRelay = RELAYM_FN_POSITIVE_AC_CHARGE;
#endif
    }
    else
    {
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
        (void)RelayM_Control(RELAYM_FN_POSITIVE_DC_CHARGE, RELAYM_CONTROL_ON);
        HvProcess_ChgInnerData.chgRelay = RELAYM_FN_POSITIVE_DC_CHARGE;
#endif
    }
}

boolean HvProcess_ChgHeaterRelayIsNormal(void)
{
    boolean res = FALSE;

    if (RelayM_GetActualStatus(HvProcess_ChgInnerData.chgRelay) == RELAYM_ACTUAL_ON && HvProcess_ChgInnerData.chgRelay != (RelayM_FunctionType)RELAYM_FN_NONE && !HvProcess_ChgInnerData.HeatRelayFaultCheckFlag)
    {
#ifdef RELAYM_FN_HEATER
        HvProcess_ChgInnerData.HeatRelayFaultCheckFlag = TRUE;
        if (RelayMConfigData[RELAYM_FN_HEATER].GetInstantVoltage != NULL)
        {
            if (RelayM_GetControlStatus(RELAYM_FN_HEATER) == RELAYM_CONTROL_OFF)
            {
                if (RelayM_GetActualStatus(RELAYM_FN_HEATER) == RELAYM_ACTUAL_OFF)
                {
                    (void)RelayM_StartAdhesiveDetect(RELAYM_FN_HEATER, NULL);
                }
            }
        }
        else
        {
            res = TRUE;
        }
#else
        res = TRUE;
#endif
    }
    if (HvProcess_ChgInnerData.HeatRelayFaultCheckFlag)
    {
#ifdef RELAYM_FN_HEATER
        if (RELAYM_DIAGNOSIS_IS_NORMAL(RelayM_GetDiagnosisStatus(RELAYM_FN_HEATER)))
        {
            res = TRUE;
        }
#endif
    }
    return res;
}

void HvProcess_ChgHeaterRelayNormalAction(void)
{
    HvProcess_ChgInnerData.HeatRelayFaultCheckFlag = FALSE;
}

boolean HvProcess_ChgStartHeatTempCond(void)
{
    boolean res = FALSE;
    App_TemperatureType minTemp;
    uint8 heatStartTemp;
    uint8 valid;
    static uint32 lastTime = 0UL;
    uint32 nowTime = OSTimeGet();
    static uint32 monitorTime = 0UL;

    /** 监控 */
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 300UL){
        lastTime = 0UL;
    }
    monitorTime = nowTime;

    heatStartTemp = HvProcess_ChgGetStartToHeatTemp();
    if (CellDataM_TemperatureIsValid((uint16)heatStartTemp))
    {
        minTemp = Statistic_GetBcuLt(0U);
        valid = CellDataM_TemperatureIsValid((uint16)minTemp);
        if (valid && minTemp <= heatStartTemp)
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
                {
                    res = TRUE;
                }
            }
        }
        else
        {
            lastTime = 0U;
        }
    }

    return res;
}

void HvProcess_ChgStartHeatTempAction(void)
{
    if (HvProcess_ChgInnerData.chgRelay != RELAYM_FN_NONE)
    {
        (void)RelayM_Control(HvProcess_ChgInnerData.chgRelay, RELAYM_CONTROL_ON);
    }
    // TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_LT);
    HvProcess_ChgInnerData.HeatIsFinish = FALSE;
}

boolean HvProcess_ChgStartHeatAndChargeTempCond(void)
{
    boolean res = FALSE;
    App_TemperatureType minTemp;
    uint8 heatTemp, chargeTemp;
    uint8 valid;

    heatTemp = HvProcess_ChgGetStartToHeatTemp();
    chargeTemp = HvProcess_ChgGetStartToChargeTemp();
    if (CellDataM_TemperatureIsValid((uint16)heatTemp))
    {
        if (CellDataM_TemperatureIsValid((uint16)chargeTemp))
        {
            minTemp = Statistic_GetBcuLt(0U);
            valid = CellDataM_TemperatureIsValid((uint16)minTemp);
            if (valid && minTemp > heatTemp && minTemp < chargeTemp)
            {
                res = TRUE;
            }
        }
    }

    return res;
}

void HvProcess_ChgStartHeatAndChargeTempAction(void)
{
    // do nothing
}

boolean HvProcess_ChgStartChargeTempCond(void)
{
    boolean res = FALSE;
    App_TemperatureType minTemp;
    uint8 chargeTemp;
    uint8 valid;

    chargeTemp = HvProcess_ChgGetStartToChargeTemp();
    if (CellDataM_TemperatureIsValid((uint16)chargeTemp))
    {
        minTemp = Statistic_GetBcuLt(0U);
        valid = CellDataM_TemperatureIsValid((uint16)minTemp);
        if (valid && minTemp >= chargeTemp)
        {
            res = TRUE;
        }
    }
    return res;
}

void HvProcess_ChgStartChargeTempAction(void)
{
    // do nothing
}

boolean HvProcess_ChgPowerOnCurrIsValidCond(void)
{
    boolean res = FALSE;
#ifdef RELAYM_FN_HEATER
    static uint32 lastTime = 0UL,monitorTime = 0UL;
    uint32 nowTime = OSTimeGet();
    uint16 current = (uint16)ChargerComm_GetChargerOutputCurrent();
    // uint16 mainCur = (uint16)CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    uint16 heatCurrent = (uint16)USERSTRATEGY_START_HEAT_CURRENT;
    RelayM_ActualStatusType heatRelay = RelayM_GetActualStatus(RELAYM_FN_HEATER);

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 300UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    heatCurrent += CURRENT_100MA_FROM_A(10U);
    if (CURRENT_IS_VALID(current) && /*current > 0U &&*/ current < heatCurrent && heatRelay == RELAYM_ACTUAL_OFF)
    {
        if (lastTime == 0UL)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
        {
            res = TRUE;
            lastTime = 0UL;
        }
    }
    // else if (CURRENT_IS_VALID(mainCur) && mainCur > 0U && mainCur < heatCurrent && heatRelay == RELAYM_ACTUAL_OFF)
    // {
    //     if (lastTime == 0UL)
    //     {
    //         lastTime = nowTime;
    //     }
    //     if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
    //     {
    //         res = TRUE;
    //         lastTime = 0UL;
    //     }
    // }
    else
    {
        lastTime = 0UL;
    }
#endif
    return res;
}

void HvProcess_ChgPowerOnCurrIsValidAction(void)
{
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_LT);
    (void)RelayM_Control(RELAYM_FN_HEATER, RELAYM_CONTROL_ON);
    HvProcess_ChgInnerData.startToHeatRelayActionTime = OSTimeGet();
}

boolean HvProcess_ChgPowerOnCurrInValidCond(void)
{
    boolean res = FALSE;
    uint32 delay = S_TO_MS(300UL);
    static uint32 lastTime = 0U;
    uint32 nowTime = OSTimeGet();
    static uint32 monitorTime = 0UL;

    /** 监控 */
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 300UL){
        lastTime = 0U;
    }
    monitorTime = nowTime;

    if (lastTime == 0U)
    {
        lastTime = nowTime;
    }
    else
    {
        if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
        {
            res = TRUE;
            lastTime = 0U;
        }
    }
    return res;
}

void HvProcess_ChgPowerOnCurrInValidAction(void)
{
    HvProcess_ChgFaultAction();
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_FAULT);
}

boolean HvProcess_ChgPowerOnToHeatRelayDelayActionCond(void)
{
    boolean res = FALSE;
    uint32 nowTime = OSTimeGet();
    uint32 delay = 5000UL; // 协议规定延时5S

    if (MS_GET_INTERNAL(HvProcess_ChgInnerData.startToHeatRelayActionTime, nowTime) >= delay)
    {
        res = TRUE;
    }
    return res;
}

void HvProcess_ChgPowerOnToHeatRelayDelayActionAction(void)
{
    (void)RelayM_Control(HvProcess_ChgInnerData.chgRelay, RELAYM_CONTROL_OFF);
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_LT);
}

boolean HvProcess_ChgHeatAndChargeToHeatPrepareCond(void)
{
    boolean res = FALSE;
    uint32 delay = 1000UL;
    static uint32 overTick = 0UL;
    uint32 overTimeDelay = 5000UL; // 转化阶段最多等待5S
    static uint32 lastTime = 0UL;
    uint32 nowTime = OSTimeGet();
    static uint32 monitorTime = 0UL;
    Current_CurrentType current, maxCurrLimit;

    /** 监控 */
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 300UL){
        lastTime = 0UL;
        overTick = 0UL;
    }
    monitorTime = nowTime;

    maxCurrLimit = USERSTRATEGY_START_HEAT_CURRENT + 100; /** 加热请求电流+10A */
    current = ChargerComm_GetChargerOutputCurrent();
    if (CurrentM_IsValidCurrent(current))
    {
        if (current <= maxCurrLimit)
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
                {
                    res = TRUE;
                    lastTime = 0U;
                    overTick = 0U;
                }
            }
        }
        else
        {
            lastTime = 0U;
        }
    }
    else
    {
        lastTime = 0U;
    }

    if (overTick == 0U)
    {
        overTick = nowTime;
    }
    else
    {
        if (MS_GET_INTERNAL(overTick, nowTime) >= overTimeDelay)
        {
            res = TRUE;
            lastTime = 0UL;
            overTick = 0UL;
        }
    }

    return res;
}

void HvProcess_ChgHeatAndChargeToHeatPrepareAction(void)
{
    (void)RelayM_Control(HvProcess_ChgInnerData.chgRelay, RELAYM_CONTROL_OFF);
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_LT);
}

boolean HvProcess_ChgStopHeatTempCond(void)
{
    boolean res = FALSE;
    App_TemperatureType minTemp;
    uint8 heatStopTemp;
    uint8 valid;
    uint32 delay = 1000UL;
    static uint32 lastTime = 0UL;
    uint32 nowTime = OSTimeGet();
    static uint32 monitorTime = 0UL;

    /** 监控 */
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 300UL){
        lastTime = 0UL;
    }
    monitorTime = nowTime;

    heatStopTemp = HvProcess_ChgGetHeatToHeatAndChargeTemp();
    if (CellDataM_TemperatureIsValid((uint16)heatStopTemp))
    {
        minTemp = Statistic_GetBcuLt(0U);
        valid = CellDataM_TemperatureIsValid((uint16)minTemp);
        if (valid && minTemp >= heatStopTemp)
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
                {
                    res = TRUE;
                    lastTime = 0U;
                }
            }
        }
        else
        {
            lastTime = 0U;
        }
    }

    return res;
}

void HvProcess_ChgStopHeatTempAction(void)
{
    // do nothing
}

boolean HvProcess_ChgStartHeatAndChargeCurrIsValidCond(void)
{
    boolean res = FALSE;
#ifdef RELAYM_FN_HEATER
    static uint32 lastTime = 0UL,monitorTime = 0UL;
    uint32 nowTime = OSTimeGet();
    uint16 current = (uint16)ChargerComm_GetChargerOutputCurrent();
    // uint16 mainCur = (uint16)CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    uint16 heatCurrent = (uint16)USERSTRATEGY_START_HEAT_CURRENT;
    RelayM_ActualStatusType heatRelay = RelayM_GetActualStatus(RELAYM_FN_HEATER);

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 300UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    heatCurrent += CURRENT_100MA_FROM_A(10U);
    if (CURRENT_IS_VALID(current) && /*current > 0U &&*/ current < heatCurrent && heatRelay == RELAYM_ACTUAL_OFF)
    {
        if (lastTime == 0UL)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
        {
            res = TRUE;
            lastTime = 0UL;
        }
    }
    // else if (CURRENT_IS_VALID(mainCur) && mainCur > 0U && mainCur < heatCurrent && heatRelay == RELAYM_ACTUAL_OFF)
    // {
    //     if (lastTime == 0UL)
    //     {
    //         lastTime = nowTime;
    //     }
    //     if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
    //     {
    //         res = TRUE;
    //         lastTime = 0UL;
    //     }
    // }
    else
    {
        lastTime = 0UL;
    }
#endif
    return res;
}

void HvProcess_ChgStartHeatAndChargeCurrIsValidAction(void)
{
    (void)RelayM_Control(RELAYM_FN_HEATER, RELAYM_CONTROL_ON);
}

boolean HvProcess_ChgStartHeatAndChargeCurrInValidCond(void)
{
    boolean res = FALSE;
    uint32 delay = S_TO_MS(300UL);
    static uint32 lastTime = 0U;
    uint32 nowTime = OSTimeGet();
    static uint32 monitorTime = 0UL;

    /** 监控 */
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 300UL){
        lastTime = 0UL;
    }
    monitorTime = nowTime;

    if (lastTime == 0U)
    {
        lastTime = nowTime;
    }
    else
    {
        if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
        {
            res = TRUE;
        }
    }
    return res;
}

void HvProcess_ChgStartHeatAndChargeCurrInValidAction(void)
{
    HvProcess_ChgFaultAction();
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_FAULT);
}

boolean HvProcess_ChgHeatToHeatAndChargePrepareCond(void)
{
    return TRUE;
}

void HvProcess_ChgHeatToHeatAndChargePrepareAction(void)
{
    (void)RelayM_Control(HvProcess_ChgInnerData.chgRelay, RELAYM_CONTROL_ON);
    HvProcess_ChgSetHeatStateNone();
}

boolean HvProcess_ChgChargeToHeatAndChargePrepareCond(void)
{
    return TRUE;
}

void HvProcess_ChgChargeToHeatAndChargePrepareAction(void)
{
    (void)RelayM_Control(RELAYM_FN_HEATER, RELAYM_CONTROL_ON);
}

boolean HvProcess_ChgHeatAndChargeToHeatTempCond(void)
{
    boolean res = FALSE;
    App_TemperatureType minTemp;
    uint8 setTemp;
    uint8 valid;
    uint32 delay = 1000UL;
    static uint32 lastTime = 0UL;
    uint32 nowTime = OSTimeGet();
    static uint32 monitorTime = 0UL;

    /** 监控 */
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 300UL){
        lastTime = 0UL;
    }
    monitorTime = nowTime;

    setTemp = HvProcess_ChgGetHeatAndChargeToHeatTemp();
    if (CellDataM_TemperatureIsValid((uint16)setTemp))
    {
        minTemp = Statistic_GetBcuLt(0U);
        valid = CellDataM_TemperatureIsValid((uint16)minTemp);
        if (valid && minTemp <= setTemp)
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
                {
                    res = TRUE;
                    lastTime = 0U;
                }
            }
        }
        else
        {
            lastTime = 0U;
        }
    }

    return res;
}

void HvProcess_ChgHeatAndChargeToHeatTempAction(void)
{
    // do nothing
}

boolean HvProcess_ChgHeatAndChargeToChargeTempCond(void)
{
    boolean res = FALSE;
    App_TemperatureType minTemp;
    uint8 setTemp;
    uint8 valid;
    uint32 delay = 1000UL;
    static uint32 lastTime = 0UL;
    uint32 nowTime = OSTimeGet();
    static uint32 monitorTime = 0UL;

    /** 监控 */
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 300UL){
        lastTime = 0UL;
    }
    monitorTime = nowTime;

    setTemp = HvProcess_ChgGetHeatAndChargeToChargeTemp();
    if (CellDataM_TemperatureIsValid((uint16)setTemp))
    {
        minTemp = Statistic_GetBcuLt(0U);
        valid = CellDataM_TemperatureIsValid((uint16)minTemp);
        if (valid && minTemp >= setTemp)
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
                {
                    res = TRUE;
                    lastTime = 0U;
                }
            }
        }
        else
        {
            lastTime = 0U;
        }
    }

    return res;
}
/**
 * \file UserStrategy.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 用户策略定义文件.
 * 小旦编程益佳通分支
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170322 |
 */
#include <string.h>
#include <stdlib.h>
#include "UserStrategy.h"
#include "Statistic.h"
#include "Util.h"
#include "Hv.h"
#include "RelayM.h"
#include "PowerM.h"
#include "TemperatureM.h"
#include "ChargeConnectM.h"
#include "SocCalib_Cbk.h"
#include "ChargeM.h"
#include "DischargeM.h"
#include "DigitalInput.h"
#include "RelayM_Lcfg.h"
#include "ChargerComm.h"
#include "ParameterM.h"
#include "HvProcess_Chg.h"
#include "HvProcess_Dchg.h"
#include "ChargeM_Cfg.h"
#include "DischargeM_Cfg.h"
#include "PrechargeM_Lcfg.h"
#include "ChargerComm_LCfg.h"
#include "ChgSckTmpM.h"
#include "Insu.h"
#include "BridgeInsu.h"

#if ( USERSTRATEGY_DEV_ERROR_DETECT == STD_ON )
#define VALIDATE_PTR(_ptr, _api) \
    if(_ptr == NULL) { \
        Det_ReportError(MODULE_ID_USERSTRATEGY, 0U, _api, USERSTRATEGY_E_PARAM_INVALID_PTR); \
        goto cleanup; \
    }
#else
#define VALIDATE_RPTR(_ptr, _api)
#endif

UserStrategy_InnerDataType UserStrategy_innerData;


static Async_EvnetCbkReturnType UserStrategy_Poll(Async_EventType *event, uint8 byWhat);
static void safeCurrentCheck(void);
static void UserStrategy_AllRlyOff(void);
#if USERSTRATEGY_RESET_TO_OTA_EN == STD_ON
static void UserStrategy_ResetToOTA(void);
#endif

#ifdef RELAYM_FN_SELF_LOCK
static void UserStrategy_SelfLockRelayControl(void);
#endif

#if USERSTRATEGY_AUTO_POWER_DOWN_EN == STD_ON
static void UserStrategy_AutoLostPower(void);
#endif

#if USERSTRATEGY_MANUAL_POWER_DOWN_EN == STD_ON
static void UserStrategy_ManualPowerDownCheck(void);
#endif

#if USERSTRATEGY_BUZZER_ALARM_EN == STD_ON
static void UserStrategy_BuzzerControl(void);
#endif

#if USERSTRATEGY_DCHG_LV_PROTECT_WITH_CURRENT_EN == STD_ON
static void dchgLvProtectForCurrentCheck(void);
#endif

#if USERSTRATEGY_DCHG_LTV_PROTECT_WITH_CURRENT_EN == STD_ON
static void dchgLtvProtectForCurrentCheck(void);
#endif

#if USERSTRATEGY_LV_POWER_DOWN_EN == STD_ON
static void UserStrategy_LvPowerDown(void);
#endif

#if USERSTRATEGY_BUZZER_ALARM_EN == STD_ON
static const UserStrategy_BuzzerAlarmType Alarm_type_Lever1[] = {
    {DIAGNOSIS_ITEM_DCHG_LTV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_LV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_HTV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_HV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_DV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_HT, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_LT, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_DT, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_SP_OC, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_FB_OC, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_OC, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_LSOC, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_VOLT_LINE, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_TEMP_LINE, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_SUPPLY_VOL_LOW, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_LEAK, DIAGNOSIS_LEVEL_SECOND},
};

static const UserStrategy_BuzzerAlarmType Alarm_type_Lever2[] = {
    {DIAGNOSIS_ITEM_DCHG_LTV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_LV, DIAGNOSIS_LEVEL_SECOND},
};

static const UserStrategy_BuzzerAlarmType Alarm_type_Lever3[] = {
    {DIAGNOSIS_ITEM_DCHG_LTV, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DCHG_LV, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DCHG_HTV, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DCHG_HV, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DCHG_DV, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DCHG_HT, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DCHG_LT, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DCHG_DT, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_SP_OC, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_FB_OC, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DCHG_OC, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_CHG_LTV, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_CHG_LV, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_CHG_DV, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_CHG_HT, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_CHG_LT, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_CHG_DT, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_AC_CHG_OC, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DC_CHG_OC, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_BMS_INIT_FAILURE, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_VOLT_LINE, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_TEMP_LINE, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_RELAY_ABNORMAL, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_LSOC, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CURRENT_ABNORMAL, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_SUPPLY_VOL_LOW, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_DOUBLE_CHG_CONNECT_FAULT, DIAGNOSIS_LEVEL_THIRD},
    {DIAGNOSIS_ITEM_LEAK, DIAGNOSIS_LEVEL_THIRD},
};
#endif

void UserStrategy_Init(Async_LooperType *looper)
{
#ifdef RELAYM_FN_HMI
    RuntimeM_RunModeType mode = RuntimeM_GetMode();
#endif
    UserStrategy_innerData.currentIsAllowToPowerOff = TRUE;

#ifdef RELAYM_FN_HMI
    if(mode == RUNTIMEM_RUNMODE_NORMAL || mode == RUNTIMEM_RUNMODE_DATA || mode == RUNTIMEM_RUNMODE_CALIBRATE){
        HLSS_Drive(RelayMConfigData[RELAYM_FN_HMI].DriveHSS, HLSS_DRIVE_ON);
    }
    (void)RelayM_Control(RELAYM_FN_HMI, RELAYM_CONTROL_ON);
#endif

    if (looper != NULL)
    {
        UserCurrentSensor_Init(looper);
        if (E_OK == Async_EventInit(&UserStrategy_innerData.event, looper, UserStrategy_Poll, 100UL))
        {
            (void)Async_EventRegisterToLooper(&UserStrategy_innerData.event);
        }
    }
}

#if defined(UPA530)||defined(UPA630)||defined(UPA640)
#if (KEY_TYPE == KEY_TYPE_IS_SELFRESET)
static void pollPowerKeyState()
{
    uint32 now = OSTimeGet();
    static uint32 tick = 0U;
    static uint8 status = 0U;
    Dio_LevelType keyStatus = Dio_ReadChannel(DIO_CHANNEL_KEY_ON);

    if (status == 0U && now > 10000U)
    {
        if (keyStatus == STD_LOW)
        {
            if (now - tick >= 3000U)
            {
                status = 1U;
                tick = now;
            }
        }
        else
        {
            tick = now;
        }
    }
    else if (status == 1U)
    {
        if (keyStatus == STD_HIGH)
        {
            if (now - tick >= 1000U)
            {
                status = 2U;
                tick = now;
            }
        }
        else
        {
            tick = now;
        }
    }
    else if (status == 2U)
    {
        status = 0U;
        UserStrategy_AllRlyOff();
        RuntimeM_RequestPowerDown();
    }
    else
    {}
}
#endif
#endif

#if (USERSTRATEGY_CHG_TRATEGY_EN == STD_ON)
static void UserStrategy_ChgRelayOff(void)
{
    static boolean flag = FALSE;
#ifdef RELAYM_FN_CHARGE
    (void)RelayM_Control(RELAYM_FN_CHARGE, RELAYM_CONTROL_OFF);
    flag = TRUE;
#endif
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    (void)RelayM_Control(RELAYM_FN_POSITIVE_AC_CHARGE, RELAYM_CONTROL_OFF);
    flag = TRUE;
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
    (void)RelayM_Control(RELAYM_FN_POSITIVE_DC_CHARGE, RELAYM_CONTROL_OFF);
    flag = TRUE;
#endif
    if (!flag)
    {
#ifdef RELAYM_FN_POSITIVE_MAIN
        (void)RelayM_Control(RELAYM_FN_POSITIVE_MAIN, RELAYM_CONTROL_OFF);
#endif
    }
}

static void UserStrategy_FullChgCheck(void)
{
    uint32 nowTime = OSTimeGet();
    static uint32 lastTime = 0U;
    boolean isConnected = CHARGECONNECTM_IS_CONNECT();
    Std_ReturnType chgIsFault = ChargeM_ChargeIsFault();
    uint16 totalVol = Statistic_GetBcu100mvTotalVoltage();
    uint16 hv = Statistic_GetBcuHvMax();
    uint16 fullHtV = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_CHG_HTV, 2U, 0U);
    uint16 fullHv = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_CHG_HV, 2U, 0U);
    boolean fullHvIsValid,hvIsValid;

    fullHvIsValid = Statistic_TotalVoltageIsValid(totalVol);
    hvIsValid = CellDataM_VoltageIsValid(hv);
    if (isConnected == TRUE && chgIsFault == E_NOT_OK)
    {
        if ((fullHvIsValid && totalVol >= fullHtV) || (hvIsValid && hv >= fullHv))
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            if (MS_GET_INTERNAL(lastTime, nowTime) >= 3000U)
            {
                SocDiagCalib_FullCalibCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_ASSERT);
                ChargeM_DiagnosisCtlDisableEventCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_ASSERT);
                UserStrategy_ChgRelayOff();
            }
        }
        else
        {
            lastTime = 0U;
        }
    }
    else
    {
        if (!isConnected)
        {
            SocDiagCalib_FullCalibRelCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_DEASSERT);
            ChargeM_DiagnosisCtlEnableEventCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_DEASSERT);
        }
        lastTime = 0U;
    }
}

static void UserStrategy_FullCharge(void)
{
    SocDiagCalib_FullCalibCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_ASSERT);
    ChargeM_DiagnosisCtlDisableEventCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_ASSERT);
    UserStrategy_ChgRelayOff();
}

static void UserStrategy_StopChargeCheck(void)
{
    uint16 hv = Statistic_GetBcuHvMax();
    boolean flg = FALSE;
    boolean stopFlag = FALSE;
    Current_CurrentType chgCurrent = ChargerComm_GetChargeCurrentMax();
    uint16 mainCurrent = CurrentM_GetMainDiagChgCurrent();
    static uint32 lastTime = 0U, lastTime1 = 0U, lastTime2 = 0U;
    uint32 nowTime = OSTimeGet();
    Std_ReturnType allow = ChargeM_ChargeIsAllowed();

    if (CellDataM_VoltageIsValid(hv))
    {
        if (hv >= USERSTRATEGY_FULL_CHG_VOLT)
        {
            flg = TRUE;
            if (hv >= USERSTRATEGY_CHG_STOP_VOLT)
            {
                if (lastTime == 0U)
                {
                    lastTime = nowTime;
                }
                else
                {
                    if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
                    {
                        UserStrategy_FullCharge();
                        lastTime = 0U;
                        stopFlag = TRUE;
                    }
                }
            }
        }
        else
        {
            lastTime = 0U;
        }
    }
    else
    {
        lastTime = 0U;
    }
    if (ChargerCommUser_IsCommunication() && flg && allow == E_OK && !stopFlag)
    {
        if (CurrentM_IsValidCurrent(chgCurrent))
        {
            if (chgCurrent <= USERSTRATEGY_CHG_END_CURRENT && chgCurrent > 0)
            {
                if (lastTime2 == 0U)
                {
                    lastTime2 = nowTime;
                }
                if (MS_GET_INTERNAL(lastTime2, nowTime) >= USERSTRATEGY_CHG_END_TIME)
                {
                    UserStrategy_FullCharge();
                    lastTime2 = 0U;
                }
            }
            else
            {
                lastTime2 = 0U;
            }
        }
        else
        {
            lastTime2 = 0U;
        }
        if (CurrentM_DiagIsValidCurrent(mainCurrent))
        {
            if (mainCurrent < (uint16)USERSTRATEGY_CHG_STOP_CURRENT)
            {
                if (lastTime1 == 0U)
                {
                    lastTime1 = nowTime;
                }
                if (MS_GET_INTERNAL(lastTime1, nowTime) >= 10000UL)
                {
                    UserStrategy_FullCharge();
                    lastTime1 = 0U;
                }
            }
            else
            {
                lastTime1 = 0U;
            }
        }
        else
        {
            lastTime1 = 0U;
        }
    }
    else
    {
        lastTime = 0U;
        lastTime1 = 0U;
        lastTime2 = 0U;
    }
}
#endif

static Async_EvnetCbkReturnType UserStrategy_Poll(Async_EventType *event, uint8 byWhat)
{
#if defined(UPA550)||defined(UPA650)
    static boolean flag = FALSE;
#endif
    (void)event;
    (void)byWhat;
#if USERSTRATEGY_RESET_TO_OTA_EN == STD_ON
    UserStrategy_ResetToOTA();
#endif

#if defined(UPA530)||defined(UPA630)||defined(UPA640)
#if (KEY_TYPE == KEY_TYPE_IS_SELFRESET)
    if (RuntimeM_GetMode() != RUNTIMEM_RUNMODE_TEST) {
        pollPowerKeyState();
    }
#endif
#endif
    safeCurrentCheck();

#ifdef RELAYM_FN_SELF_LOCK
    UserStrategy_SelfLockRelayControl();
#endif

#if USERSTRATEGY_BUZZER_ALARM_EN == STD_ON
    UserStrategy_BuzzerControl();
#endif

#if USERSTRATEGY_AUTO_POWER_DOWN_EN == STD_ON
    UserStrategy_AutoLostPower();
#endif

#if USERSTRATEGY_MANUAL_POWER_DOWN_EN == STD_ON
    UserStrategy_ManualPowerDownCheck();
#endif

#if USERSTRATEGY_DCHG_LV_PROTECT_WITH_CURRENT_EN == STD_ON
    dchgLvProtectForCurrentCheck();
#endif

#if USERSTRATEGY_DCHG_LTV_PROTECT_WITH_CURRENT_EN == STD_ON
    dchgLtvProtectForCurrentCheck();
#endif

#if USERSTRATEGY_CHG_TRATEGY_EN == STD_ON
    UserStrategy_FullChgCheck();
    UserStrategy_StopChargeCheck();
#endif

#if USERSTRATEGY_LV_POWER_DOWN_EN == STD_ON
    UserStrategy_LvPowerDown();
#endif

#if defined(UPA550)||defined(UPA650)
    if (OSTimeGet() >= 2500UL && !flag)
    {
        BridgeInsu_Stop();
        flag = TRUE;
    }
#endif

    return ASYNC_EVENT_CBK_RETURN_OK;
}

static void safeCurrentCheck(void)
{
    boolean flag = FALSE;
    uint32 nowTick;
    Current_CurrentType current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);

    if (!CURRENT_IS_VALID(current)) {
        flag = TRUE;
    }
    else if ((uint16)abs(current) < CURRENT_100MA_FROM_A(5U)) {
        flag = TRUE;
    }
    else {
    }

    nowTick = OSTimeGet();
    if (flag) {
        if (UserStrategy_innerData.powerOffTick == 0UL) {
            UserStrategy_innerData.powerOffTick = nowTick;
        }
        else {
            if (MS_GET_INTERNAL(UserStrategy_innerData.powerOffTick, nowTick) >= 1000UL) {
                UserStrategy_innerData.currentIsAllowToPowerOff = TRUE;
            }
        }
    }
    else {
        UserStrategy_innerData.powerOffTick = 0UL;
        UserStrategy_innerData.currentIsAllowToPowerOff = FALSE;
    }
}

//此函数底层仅调用一次，需要循环调用请在UserStrategy_SaftyOff函数中实现
boolean UserStrategy_IsSafeToOff(void) {
    boolean res = FALSE;

    if (UserStrategy_innerData.currentIsAllowToPowerOff) {
        res = TRUE;
    }
    return res;
}

boolean UserStartegy_ChargeIsAllow(void)
{
    boolean res = FALSE;

    if (Diagnosis_StartDiagIsFinish())
    {
        res = TRUE;
    }
    return res;
}

App_Tv100mvType UserStrategy_GetChargeVoltMax(void)
{
    Charge_ChargeType type = ChargeConnectM_GetConnectType();
    App_Tv100mvType volt = PowerM_GetChargeVoltage(type);
#ifdef RELAYM_FN_HEATER
    boolean isHeat = HvProcess_ChargerIsHeatMode();
    boolean isJump = HvProcess_IsJumpMode();
    if (isJump)
    {
        volt = Statistic_GetBcu100mvTotalVoltage();
        if (Statistic_TotalVoltageIsValid(volt))
        {
            volt += V_TO_100MV(2U);
        }
    }
    else if (isHeat)
    {
        volt = USERSTRATEGY_START_HEAT_VOLT;
    }
    else
    {
        // volt = PowerM_GetChargeVoltage(type);
    }
#endif
    if (!Statistic_TotalVoltageIsValid(volt))
    {
        volt = 0U;
    }

    return volt;
}

Current_CurrentType UserStrategy_GetChargeCurrentMax(void)
{
    Current_CurrentType current;
    Charge_ChargeType type = ChargeConnectM_GetConnectType();
#if USERSTRATEGY_CHG_TRATEGY_EN == STD_ON
    static uint32 lastTime = 0U, monitorTime = 0U;
    static uint8 flag = 0U;
    uint32 nowTime = OSTimeGet();
    uint16 lt = Statistic_GetBcuLtMax();
#endif
#ifdef RELAYM_FN_HEATER
    boolean isHeat = HvProcess_ChargerIsHeatMode();
    boolean isJump = HvProcess_IsJumpMode();
    boolean isHeatCharge = HvProcess_IsHeatAndChargeMode();

    if (isJump)
    {
        current = USERSTRATEGY_START_HEAT_CURRENT;
    }
    else if (isHeat)
    {
        current = USERSTRATEGY_START_HEAT_CURRENT;
        // current = PowerM_GetCurrent(POWERM_CUR_CHARGE_HEATER);
    }
    else if (isHeatCharge)
    {
        if (type == CHARGE_TYPE_DC)
        {
            current = PowerM_GetCurrent(POWERM_CUR_CHARGE_DC);
        }
        else
        {
            current = PowerM_GetCurrent(POWERM_CUR_CHARGE_AC);
        }
#if USERSTRATEGY_CHG_TRATEGY_EN == STD_ON
        if (MS_GET_INTERNAL(monitorTime, nowTime) >= 3000U)
        {
            lastTime = 0U;
        }
        monitorTime = nowTime;
        if (lt <= (uint8)USERSTRATEGY_LOW_TEMP_STOP &&
            lt > (uint8)USERSTRATEGY_LOW_TEMP_START)
        {
            if (flag == 0U)
            {
                flag = 1U;
            }
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) <= USERSTRATEGY_LOW_TEMP_CHG_TIME &&
                    flag == 1U)
                {
                    if (current > USERSTRATEGY_LOW_TEMP_CHG_CURRENT)
                    {
                        current = USERSTRATEGY_LOW_TEMP_CHG_CURRENT;
                    }
                }
            }
        }
        else
        {
            if (flag == 1U)
            {
                flag = 2U;
            }
        }
#endif
        current += USERSTRATEGY_START_HEAT_CURRENT;
    }
    else
    {
        if (type == CHARGE_TYPE_DC)
        {
            current = PowerM_GetCurrent(POWERM_CUR_CHARGE_DC);
        }
        else
        {
            current = PowerM_GetCurrent(POWERM_CUR_CHARGE_AC);
        }
#if USERSTRATEGY_CHG_TRATEGY_EN == STD_ON
        if (MS_GET_INTERNAL(monitorTime, nowTime) >= 3000U)
        {
            lastTime = 0U;
        }
        monitorTime = nowTime;
        if (lt <= (uint8)USERSTRATEGY_LOW_TEMP_STOP &&
            lt > (uint8)USERSTRATEGY_LOW_TEMP_START)
        {
            if (flag == 0U)
            {
                flag = 1U;
            }
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) <= USERSTRATEGY_LOW_TEMP_CHG_TIME &&
                    flag == 1U)
                {
                    if (current > USERSTRATEGY_LOW_TEMP_CHG_CURRENT)
                    {
                        current = USERSTRATEGY_LOW_TEMP_CHG_CURRENT;
                    }
                }
            }
        }
        else
        {
            if (flag == 1U)
            {
                flag = 2U;
            }
        }
#endif
    }
#else
    if (TemperatureM_GetHeatState() != TEMPERATUREM_HEAT_STATE_NONE)
    {
        current = PowerM_GetCurrent(POWERM_CUR_CHARGE_HEATER);
    }
    else if (type == CHARGE_TYPE_DC)
    {
        current = PowerM_GetCurrent(POWERM_CUR_CHARGE_DC);
    }
    else
    {
        current = PowerM_GetCurrent(POWERM_CUR_CHARGE_AC);
    }
#if USERSTRATEGY_CHG_TRATEGY_EN == STD_ON
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 3000U)
    {
        lastTime = 0U;
    }
    monitorTime = nowTime;
    if (lt <= (uint8)USERSTRATEGY_LOW_TEMP_STOP &&
        lt > (uint8)USERSTRATEGY_LOW_TEMP_START)
    {
        if (flag == 0U)
        {
            flag = 1U;
        }
        if (lastTime == 0U)
        {
            lastTime = nowTime;
        }
        else
        {
            if (MS_GET_INTERNAL(lastTime, nowTime) <= USERSTRATEGY_LOW_TEMP_CHG_TIME &&
                flag == 1U)
            {
                if (current > USERSTRATEGY_LOW_TEMP_CHG_CURRENT)
                {
                    current = USERSTRATEGY_LOW_TEMP_CHG_CURRENT;
                }
            }
        }
    }
    else
    {
        if (flag == 1U)
        {
            flag = 2U;
        }
    }
#endif
#endif

    return current;
}

boolean UserStartegy_GBChargeRelayIsReady(RelayM_FunctionType relayType)
{
    boolean res = FALSE;

    if (relayType == RELAYM_FN_NONE)
    {
        res = TRUE;
    }
    else if (RelayM_FunctionRelayIsEnable(relayType) == E_NOT_OK)
    {
        res = TRUE;
    }
    else if (RelayM_GetActualStatus(relayType) == RELAYM_ACTUAL_ON)
    {
        res = TRUE;
    }
#ifdef RELAYM_FN_HEATER
    else if (RelayM_GetActualStatus(RELAYM_FN_HEATER) == RELAYM_ACTUAL_ON)
    {
        res = TRUE;
    }
#endif
    else
    {
        /*
            all: 365*24*60*60 = 0.31T   7427.6T
            count: 0.17T     3.1     101.9   789.2   1481.8  2202.2  2849.3
            1789 - 130 - 450 - 1000 - 200 = 9

            22.10.01 - 22.12.31 self count:794w, comm count:1000w
            loan fees: 50w + 80 = 130w
            taycan: 200
            Paramera:300
            Continental GT:0.06
            bently_bentaga: 550W
            *hf: 1500

        */
    }
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    if (RelayMConfigData[RELAYM_FN_POSITIVE_AC_CHARGE].GetInstantVoltage != NULL)
    {
        if (ChargerComm_ConfigInfo.AC_Blind_En != STD_ON)
        {
            if (RelayM_GetControlStatus(RELAYM_FN_POSITIVE_AC_CHARGE) == RELAYM_CONTROL_OFF)
            {
                if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_AC_CHARGE) == RELAYM_ACTUAL_OFF)
                {
                    (void)RelayM_StartAdhesiveDetect(RELAYM_FN_POSITIVE_AC_CHARGE, NULL);
                }
            }
        }
    }
#endif
    return res;
}

boolean UserStrategy_Wake(void){
#if (KEY_TYPE == KEY_TYPE_IS_SELFRESET) && (defined(A630)||defined(A635)||defined(A640)||defined(A641))
    return TRUE;/*不关机*/
#else
    /*
            3.1
            ros_currelan: 0.1T refitver
            lanb: 0.1
            *shanghai:0.5
            peking:0.5
            spring eu north fourth country 0.1T
            winter disea, wotaihua malaisia, tailand 0.1T
            1000*30*12 = 36w
            36*6=216w
    */
    return FALSE;/*关机*/
#endif
}

boolean UserStrategy_SaftyOff(RuntimeM_SignalType signal) {
    (void) signal;
    return UserStrategy_IsSafeToOff();
}


boolean UserStrategy_Wakeup(void){
#if (KEY_TYPE == KEY_TYPE_IS_SELFRESET) && (defined(A630)||defined(A635)||defined(A640)||defined(A641))
    return TRUE;/*不关机*/
#else
    /*
            4.1 Purchase property
            bently_flyspur -W12 : 600W
            ros_:0.1
            new york :1
            Canberra:1
            Finland:1
            malaisia:0.1
    */
    return FALSE;/*关机*/
#endif
}

boolean UserStrategy_Saft(RuntimeM_SignalType signal) {
    (void) signal;
    return UserStrategy_IsSafeToOff();
}

App_Tv100mvType UserStrategy_GetMcuVoltage(void)
{
    /*
            5.1 Expansion
            Personal Jet 757：280.8
            //Continental GT:0.06

            Shanghai: 5
            peking:5
            Ottawa Villa House: 7
            Langkawi Villa+yacht: 5
            Gold Coast piece of land: 8
    */
    return HV_GetVoltage(GETMCUVOLTAGE_CHANNEL);
}

uint16 UserStrategy_IsDiagHvilAbnormal(void)
{
    /*
            6.1 consumption
            liner: 462
            Lightning jet 18*10
                :1.6 mahe = 1944km /h
            sorge 0.1*300

            7.1 military affairs
            aircraft carrier : 1846
            sorge 0.1*300
    */
    return 0U;
}

boolean UserStrategy_SaftyOff(RuntimeM_SignalType signal) {
    (void) signal;
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    if (ChargerComm_ConfigInfo.AC_Blind_En != STD_ON)
    {
        if (RelayM_GetControlStatus(RELAYM_FN_POSITIVE_AC_CHARGE) == RELAYM_CONTROL_OFF)
        {
            if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_AC_CHARGE) == RELAYM_ACTUAL_OFF)
            {
                (void)RelayM_StartAdhesiveDetect(RELAYM_FN_POSITIVE_AC_CHARGE, NULL);
            }
        }
    }
#endif
    return UserStrategy_IsSafeToOff();
}

void UserStrategy_CheckSRS(const PwmCapture_DataType *dat)
{
    (void)dat;
    // uint8 i;
    // uint32 lowtime = PwmCapture_GetLowTime(dat);
    // if (lowtime >= 1800 && lowtime <= 2200) {
    //     ChargeM_SetOthersFaultDchargeCtl(, DIS);
    //     DischargeM_SetOthersFaultDchargeCtl();
    //     RelayM_ForceControl(RELAYM_FN_NEGTIVE_MAIN, RELAYM_FORCE_OFF); //优化断开主负
    //     for (i = 0; i < RELAYM_FN_NUM; ++i) {
    //         RelayM_ForceControl(i, RELAYM_FORCE_OFF);
    //     }
    // }
            /*
            1.China Coreelec (CNCE科力) new energy R&D co.ltd
            Sault new energy infinit machine energy NaCl battery density, balance
            Lithium iron hydrochloride  double one mattle density battery
            21-25 first generation 300Wh/kg, 26-30 second generation 400Wh/kg, 30-35 third generation 500Wh/kg
            before nindeshidai two and harf genration winter 85% Integration rate:90%
            China Coreelec new energy R&D sh company, China Coreelec new energy R&D Vietnam company
            count:
            2,      3,      4,      5,      6       7       8
            8%      10%     16%     20%     24%     25%     26%
            5       20      500     1500    2000    2200    2400
            0.4     2       80      300     480     554     624
            */
}

boolean UserStrategy_SaftyOff(RuntimeM_SignalType signal) {
    (void) signal;
    return UserStrategy_IsSafeToOff();
}

#if USERSTRATEGY_RESET_TO_OTA_EN == STD_ON
static void UserStrategy_ResetToOTA(void)
{
    boolean flag = FALSE;
    Current_CurrentType current;
    uint32 nowTime = OSTimeGet();
    static uint32 lastTime = 0U;

    if (RuntimeM_HasOtaRequest())
    {
            /*
            2.Finland Dosson Photolithography co.ltd
            Dosson Photolithography peking Headquarters
            Dosson Photolithography Serbia Belgrade Branch
            ceo 1,saler : 2,tech + 2 : 3, Administrative par-time Recruiting :1
            financial staff:1, 8 person: 12+40+10+12 = 74w /year
            rent fi: 20  register 100w + 100w; increaseing:1000w
            first generation:5nm workmanship top of world.
            second generation:3nm workmanship, third generation:1nm workmanship
            profit margin:
            3,      4,      5,      6       7       8
            16.6%   24.6%   31%     36%     39%     41%
            5       76      891     1367    1958    2358
            0.83    18.7    276.2   92.1    763.6   966.8
            */
        current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    }
    if (!flag)
    {
        lastTime = nowTime;
    }
#if USERSTRATEGY_AC_CHG_SELF_LOCK_EN == STD_ON
            if (!is_on)
            {
                is_on = TRUE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_ON);
            }
#endif
}

#endif

boolean UserStartegy_AcChargeRelayIsReady(void)
{
    boolean res = FALSE;
    RelayM_FunctionType relayType = ChargerComm_ConfigInfo.AC_RelayType;
    if (relayType == RELAYM_FN_NONE)
    {
            /*
            3.USA New Jersey Darcyhua Biotechnology new tech co.ltd
            Darcyhua Group has a diversified business portfolio, covering innovative patented drugs,
            eye care, non patented drugs, consumer health care, vaccines and diagnostics and other fields,
            products:
            1.Health drugs:
            2.Anticancer drugs: gen.1: Target cell, gen.2: Target cell, gen.3: Target cell
            3.Genetic engineering & transgenosis projec:

            profit margin: 46.5%,
            4,      5,      6       7       8
            17.5%   23.5%   1.5%    40.5%   46.5%
            18      906.4   618     2194    2706.4
            3.5     213.0   509.7   888.7   1258.5
            */
    }
    else if (RelayM_FunctionRelayIsEnable(relayType) == E_NOT_OK)
    {
        if (OSTimeGet() >= 1000U)
        {
            res = TRUE;
        }
    }
    return res;
}

#ifdef RELAYM_FN_SELF_LOCK
static void UserStrategy_SelfLockRelayControl(void)
{
#if USERSTRATEGY_AC_CHG_SELF_LOCK_EN == STD_ON
            if (!is_on)
            {
                is_on = TRUE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_ON);
            }
#endif
#if USERSTRATEGY_AC_CHG_SELF_LOCK_EN == STD_ON || USERSTRATEGY_DC_CHG_SELF_LOCK_EN == STD_ON || USERSTRATEGY_DCHG_SELF_LOCK_EN == STD_ON
    static boolean is_on = FALSE;
#endif
    Charge_ChargeType type = ChargeConnectM_GetConnectType();
    if (OSTimeGet() >= 300U)
    {
            /*
            4. Australia Gaussian AI communication tech co.ltd
            Gaussian Group has a 6.0G VR Comm & UAV business portfolio, covering innovative communication tec
            and devices, artificial intelligence, virtual reality, UAV, Supersonic stealth drone and other fields,
            Gaussian communication is a new form of communication that uses a Gaussian communication model.
            No longer based on universal satellite and network communication, will use quantum communication
            to improve communication efficiency and security.
            profit margin: 50.1%,
            6       7         8         9         10
            38.9%   47.5%     49.5%     54.5%     65.1%
            1346    2194      3306.4    4606.4    6506.4
            523.6   1042.2    1636.7    2510.5    4235.7
            COUNT : 9948.5 - 7060 = 2888.5
            */
        if (type == CHARGE_TYPE_AC)
        {
#if USERSTRATEGY_AC_CHG_SELF_LOCK_EN == STD_ON
            if (!is_on)
            {
                is_on = TRUE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_ON);
            }
#endif
}

void UserStrategy_ChgAcOverCurrentParaGet(uint8 level, Diagnosis_LevelParaType *para)
{
#if USERSTRATEGH_AC_CHG_OC_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
    uint32 temp;
#endif
    uint16 triggerThreshold = 0x8000U, releaseThreshold = 0x8000U;
    Current_CurrentType current;

    current = ChargerComm_GetChargeCurrentMax();
    if (para != NULL)
    {

            /*
            5.China New Fuel Energy tech co.ltd
            Gaussian Group has a 6.0G VR Comm & UAV business portfolio, covering innovative communication tec
            and devices, artificial intelligence, virtual reality, UAV, Supersonic stealth drone and other fields,
            6      7       8        9        20
            1756    2603     3025   3240    4236
            42%     53%      64%    75%     86%
            737.5   1379.6   1936   2430    3642.9

            */
        if (CurrentM_IsValidCurrent(current))
        {
            triggerThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_AC_CHG_OC, level, 0U);
            releaseThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_AC_CHG_OC, level, 1U);

#if USERSTRATEGH_AC_CHG_OC_TYPE == USERSTRATEGY_OC_TYPE_CURRENT_OFFSET
            triggerThreshold += (uint16)current;
            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                releaseThreshold += (uint16)current;
            }
            else
            {
                releaseThreshold = 0U;
            }
#elif USERSTRATEGH_AC_CHG_OC_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
            temp = (uint16)current;
            temp = temp * triggerThreshold;
            temp = DIVISION(temp, 1000U);
            if (temp > 0x7FFFU)
            {
                temp = 0x7FFFU;
            }
            if (temp == 0U)
            {
                temp = CURRENT_100MA_FROM_A(2U);
            }
            triggerThreshold = (uint16)temp;

            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                temp = (uint16)current;
                temp = temp * releaseThreshold;
                temp = DIVISION(temp, 1000U);
                if (temp > 0x7FFFU)
                {
                    temp = 0x7FFFU;
                }
                if (temp == 0U)
                {
                    temp = CURRENT_100MA_FROM_A(2U);
                }
            }
            else
            {
                temp = 0U;
            }
            releaseThreshold = (uint16)temp;
#else
            // do nothing
#endif
        }
        para->triggerThreshold = triggerThreshold;
        para->releaseThreshold = releaseThreshold;
        para->triggerConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_AC_CHG_OC, level, 2U);
        para->releaseConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_AC_CHG_OC, level, 3U);
    }
}
#endif

boolean UserStrategy_Wakeup(void){
#if (KEY_TYPE == KEY_TYPE_IS_SELFRESET) && (defined(A630)||defined(A635)||defined(A640)||defined(A641))
    return TRUE;/*不关机*/
#else
    return FALSE;/*关机*/
#endif
}

boolean UserStrategy_RemoteBalanceConditionCheck(void)
{
    uint8 ret = TRUE;
    return ret;
}
/**< 建议由连接到未连接变化时增加适当延时，保证连接状态不会突变而导致异常 */
Std_ReturnType UserStrategy_ACConnected(void)
{
    Std_ReturnType res = E_NOT_OK;
    if (ChargeConnectM_GetCc2ConnectStatus())
    {
        res = E_OK;
    }
    return res;
}

/**< 建议由连接到未连接变化时增加适当延时，保证连接状态不会突变而导致异常 */
Std_ReturnType UserStrategy_DCConnected(void)
{
    Std_ReturnType res = E_NOT_OK;
    if (ChargeConnectM_GetCc2ConnectStatus())
    {
        res = E_OK;
    }
    return res;
}

uint16 UserStrategy_GetCrashFault(void)
{
    return 0U;
}

void UserStrategy_FullChargeHook(void)
{
    SocDiagCalib_FullCalibCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_ASSERT);
#if USERSTRATEGY_CHG_TRATEGY_EN == STD_OFF
    ChargeM_DiagnosisCtlDisableEventCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_ASSERT);
#endif
}

void UserStrategy_FullChargeReleaseHook(void)
{
    SocDiagCalib_FullCalibRelCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_DEASSERT);
    ChargeM_DiagnosisCtlEnableEventCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_DEASSERT);
}
/**
 * \brief OTA实时升级
 * \details [long description]
 */
#if USERSTRATEGY_MANUAL_POWER_DOWN_EN == STD_ON
static void UserStrategy_ManualPowerDownCheck(void)
{
    static uint32 lastTime = 0U;
    static boolean is_manual = FALSE;
    uint32 nowTime = OSTimeGet();
    RuntimeM_RunModeType mode = RuntimeM_GetMode();

    if (nowTime >= 10000U && (mode == RUNTIMEM_RUNMODE_CALIBRATE || mode == RUNTIMEM_RUNMODE_NORMAL))
    {
        if (USERSTRATEGY_IS_MANUAL_POWER_DOWN())
        {
            if (MS_GET_INTERNAL(lastTime, nowTime) >= USERSTRATEGY_MANUAL_POWER_DOWN_DELAY)
            {
                is_manual = TRUE;
                lastTime = nowTime;
                ChargeM_SetOthersFaultChargeCtl(CHARGEM_OTHERS_FAULT_POWER_OFF, CHARGEM_CHARGE_DISABLE);
                DischargeM_SetOthersFaultDchargeCtl(DISCHARGEM_OTHERS_FAULT_POWER_OFF, DISCHARGEM_DISCHARGE_DISABLE);
                UserStrategy_AllRlyOff();
            }
        }
        else
        {
            if (is_manual)
            {
                RuntimeM_RequestPowerDown();
            }
            else
            {
                lastTime = nowTime;
            }
        }
    }
    else
    {
        lastTime = nowTime;
    }
}
#endif

#if USERSTRATEGY_AUTO_POWER_DOWN_EN == STD_ON
static void UserStrategy_AutoLostPower(void)
{
    Current_CurrentType current;
    RuntimeM_RunModeType mode;
    static uint32 lastTime = 0U;
    uint32 nowTime = OSTimeGet();
    uint32 delay = USERSTRATEGY_AUTO_POWER_DOWN_TIME;
    boolean isChgHeat = HvProcess_IsHeatAndChargeMode();

    mode = RuntimeM_GetMode();
    if (!HvProcess_ChargerIsHeatMode() &&
        !isChgHeat)
    {
        if (mode == RUNTIMEM_RUNMODE_CALIBRATE || mode == RUNTIMEM_RUNMODE_NORMAL)
        {
            current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
            if (CurrentM_IsValidCurrent(current))
            {
                current = abs(current);
                if (current <= CURRENT_S_100MA_FROM_A(USERSTRATEGY_AUTO_POWER_DOWN_CURRENT))
                {
                    if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
                    {
                        lastTime = nowTime;
                        ChargeM_SetOthersFaultChargeCtl(CHARGEM_OTHERS_FAULT_POWER_OFF, CHARGEM_CHARGE_DISABLE);
                        DischargeM_SetOthersFaultDchargeCtl(DISCHARGEM_OTHERS_FAULT_POWER_OFF, DISCHARGEM_DISCHARGE_DISABLE);
                        UserStrategy_AllRlyOff();
                        RuntimeM_RequestPowerDown();
                    }
                }
                else
                {
                    lastTime = nowTime;
                }
            }
            else
            {
                lastTime = nowTime;
            }
        }
        else
        {
            lastTime = nowTime;
        }
    }
    else
    {
        lastTime = nowTime;
    }
}
#endif

#if USERSTRATEGY_BUZZER_ALARM_EN == STD_ON
static void UserStrategy_BuzzerControl(void)
{
// #ifdef RELAYM_FN_BUZZER
    uint8 AlarmLevel = 0U;
    uint32 nowTime = OSTimeGet();
    static uint32 onTime = 0U, offTime = 0U;
    static uint32 lastTime = 0U;
    static boolean is_on = FALSE;
    uint8 i;

    for (i = 0U; i < (uint8)ARRAY_SIZE(Alarm_type_Lever3); i ++)
    {
        if (Diagnosis_GetLevel(Alarm_type_Lever3[i].diagItem) >= Alarm_type_Lever3[i].level)
        {
            AlarmLevel = 3U;
            break;
        }
    }
    if (!AlarmLevel)
    {
        for (i = 0U; i < (uint8)ARRAY_SIZE(Alarm_type_Lever2); i ++)
        {
            if (Diagnosis_GetLevel(Alarm_type_Lever2[i].diagItem) >= Alarm_type_Lever2[i].level)
            {
                AlarmLevel = 2U;
                break;
            }
        }
    }
    if (!AlarmLevel)
    {
        for (i = 0U; i < (uint8)ARRAY_SIZE(Alarm_type_Lever1); i ++)
        {
            if (Diagnosis_GetLevel(Alarm_type_Lever1[i].diagItem) >= Alarm_type_Lever1[i].level)
            {
                AlarmLevel = 1U;
                break;
            }
        }
    }
    if (AlarmLevel)
    {
        if (is_on)
        {
            if (MS_GET_INTERNAL(lastTime, nowTime) >= onTime)
            {
                is_on = FALSE;
                lastTime = nowTime;
#ifdef RELAYM_FN_BUZZER
                (void)RelayM_Control(RELAYM_FN_BUZZER, RELAYM_CONTROL_OFF);
#endif
#if USERSTRATEGY_HMI_BUZZER_ALARM_EN == STD_ON
                UserStrategy_innerData.isBuzzer = FALSE;
#endif
            }
        }
        else
        {
            if (MS_GET_INTERNAL(lastTime, nowTime) >= offTime)
            {
                is_on = TRUE;
                lastTime = nowTime;
#ifdef RELAYM_FN_BUZZER
                (void)RelayM_Control(RELAYM_FN_BUZZER, RELAYM_CONTROL_ON);
#endif
#if USERSTRATEGY_HMI_BUZZER_ALARM_EN == STD_ON
                UserStrategy_innerData.isBuzzer = TRUE;
#endif
            }
        }
        if (AlarmLevel == 1U)
        {
            onTime = USERSTRATEGY_BUZZER_LEVEL1_ON_TIME;
            offTime = USERSTRATEGY_BUZZER_LEVEL1_OFF_TIME;
        }
        else if (AlarmLevel == 2U)
        {
            onTime = USERSTRATEGY_BUZZER_LEVEL2_ON_TIME;
            offTime = USERSTRATEGY_BUZZER_LEVEL2_OFF_TIME;
        }
        else if (AlarmLevel == 3U)
        {
            onTime = USERSTRATEGY_BUZZER_LEVEL3_ON_TIME;
            offTime = USERSTRATEGY_BUZZER_LEVEL3_OFF_TIME;
        }
        else
        {}
    }
    else
    {
        is_on = FALSE;
        lastTime = nowTime;
        onTime = 0U;
        offTime = 0U;
#ifdef RELAYM_FN_BUZZER
        (void)RelayM_Control(RELAYM_FN_BUZZER, RELAYM_CONTROL_OFF);
#endif
#if USERSTRATEGY_HMI_BUZZER_ALARM_EN == STD_ON
        UserStrategy_innerData.isBuzzer = FALSE;
#endif
    }
// #endif
}
#endif



void UserStrategy_ChgDcOverCurrentParaGet(uint8 level, Diagnosis_LevelParaType *para)
{
#if USERSTRATEGH_DC_CHG_OC_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
    uint32 temp;
#endif
    uint16 triggerThreshold = 0x8000U, releaseThreshold = 0x8000U;
    Current_CurrentType current;

    current = ChargerComm_GetChargeCurrentMax();
    if (para != NULL)
    {
        if (CurrentM_IsValidCurrent(current))
        {
            triggerThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_DC_CHG_OC, level, 0U);
            releaseThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_DC_CHG_OC, level, 1U);

#if USERSTRATEGH_DC_CHG_OC_TYPE == USERSTRATEGY_OC_TYPE_CURRENT_OFFSET
            triggerThreshold += (uint16)current;
            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                releaseThreshold += (uint16)current;
            }
            else
            {
                releaseThreshold = 0U;
            }
#elif USERSTRATEGH_DC_CHG_OC_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
            temp = (uint16)current;
            temp = temp * triggerThreshold;
            temp = DIVISION(temp, 1000U);
            if (temp > 0x7FFFU)
            {
                temp = 0x7FFFU;
            }
            if (temp == 0U)
            {
                temp = CURRENT_100MA_FROM_A(2U);
            }
            triggerThreshold = (uint16)temp;
            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                temp = (uint16)current;
                temp = temp * releaseThreshold;
                temp = DIVISION(temp, 1000U);
                if (temp > 0x7FFFU)
                {
                    temp = 0x7FFFU;
                }
                if (temp == 0U)
                {
                    temp = CURRENT_100MA_FROM_A(2U);
                }
            }
            else
            {
                temp = 0U;
            }
            releaseThreshold = (uint16)temp;
#else
            // do nothing
#endif
        }
        para->triggerThreshold = triggerThreshold;
        para->releaseThreshold = releaseThreshold;
        para->triggerConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_DC_CHG_OC, level, 2U);
        para->releaseConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_DC_CHG_OC, level, 3U);
    }
}

void UserStrategy_DchgOverCurrentParaGet(uint8 level, Diagnosis_LevelParaType *para)
{
#if USERSTRATEGH_DCHG_OC_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
    uint32 temp;
#endif
    uint16 triggerThreshold = 0x8000U, releaseThreshold = 0x8000U;
    Current_CurrentType current;

    current = PowerM_GetCurrent(POWERM_CUR_DCHARGE_CONTINUE);
    if (para != NULL)
    {
        if (CurrentM_IsValidCurrent(current))
        {
            triggerThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_DCHG_OC, level, 0U);
            releaseThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_DCHG_OC, level, 1U);

#if USERSTRATEGH_DCHG_OC_TYPE == USERSTRATEGY_OC_TYPE_CURRENT_OFFSET
            triggerThreshold += (uint16)current;
            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                releaseThreshold += (uint16)current;
            }
            else
            {
                releaseThreshold = 0U;
            }
#elif USERSTRATEGH_DCHG_OC_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
            temp = (uint16)current;
            temp = temp * triggerThreshold;
            temp = DIVISION(temp, 1000U);
            if (temp > 0x7FFFU)
            {
                temp = 0x7FFFU;
            }
            if (temp == 0U)
            {
                temp = CURRENT_100MA_FROM_A(2U);
            }
            triggerThreshold = (uint16)temp;

            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                temp = (uint16)current;
                temp = temp * releaseThreshold;
                temp = DIVISION(temp, 1000U);
                if (temp > 0x7FFFU)
                {
                    temp = 0x7FFFU;
                }
                if (temp == 0U)
                {
                    temp = CURRENT_100MA_FROM_A(2U);
                }
            }
            else
            {
                temp = 0U;
            }
            releaseThreshold = (uint16)temp;
#else
            // do nothing
#endif
        }
        para->triggerThreshold = triggerThreshold;
        para->releaseThreshold = releaseThreshold;
        para->triggerConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_DCHG_OC, level, 2U);
        para->releaseConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_DCHG_OC, level, 3U);
    }
}

void UserStrategy_DchgSpOverCurrentParaGet(uint8 level, Diagnosis_LevelParaType *para)
{
#if USERSTRATEGH_DCHG_SP_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
    uint32 temp;
#endif
    uint16 triggerThreshold = 0x8000U, releaseThreshold = 0x8000U;
    Current_CurrentType current;

    current = PowerM_GetCurrent(POWERM_CUR_DCHARGE_PEAK);
    if (para != NULL)
    {
        if (CurrentM_IsValidCurrent(current))
        {
            triggerThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_SP_OC, level, 0U);
            releaseThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_SP_OC, level, 1U);

#if USERSTRATEGH_DCHG_SP_TYPE == USERSTRATEGY_OC_TYPE_CURRENT_OFFSET
            triggerThreshold += (uint16)current;
            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                releaseThreshold += (uint16)current;
            }
            else
            {
                releaseThreshold = 0U;
            }
#elif USERSTRATEGH_DCHG_SP_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
            temp = (uint16)current;
            temp = temp * triggerThreshold;
            temp = DIVISION(temp, 1000U);
            if (temp > 0x7FFFU)
            {
                temp = 0x7FFFU;
            }
            if (temp == 0U)
            {
                temp = CURRENT_100MA_FROM_A(2U);
            }
            triggerThreshold = (uint16)temp;

            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                temp = (uint16)current;
                temp = temp * releaseThreshold;
                temp = DIVISION(temp, 1000U);
                if (temp > 0x7FFFU)
                {
                    temp = 0x7FFFU;
                }
                if (temp == 0U)
                {
                    temp = CURRENT_100MA_FROM_A(2U);
                }
            }
            else
            {
                temp = 0U;
            }
            releaseThreshold = (uint16)temp;
#else
            // do nothing
#endif
        }
        para->triggerThreshold = triggerThreshold;
        para->releaseThreshold = releaseThreshold;
        para->triggerConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_SP_OC, level, 2U);
        para->releaseConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_SP_OC, level, 3U);
    }
}

void UserStrategy_DchgFeedBackCurrentParaGet(uint8 level, Diagnosis_LevelParaType *para)
{
#if USERSTRATEGH_DCHG_FB_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
    uint32 temp;
#endif
    uint16 triggerThreshold = 0x8000U, releaseThreshold = 0x8000U;
    Current_CurrentType current;

    current = PowerM_GetCurrent(POWERM_CUR_DCHARGE_FEEDBACK);
    if (para != NULL)
    {
        if (CurrentM_IsValidCurrent(current))
        {
            triggerThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_FB_OC, level, 0U);
            releaseThreshold = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_FB_OC, level, 1U);

#if USERSTRATEGH_DCHG_FB_TYPE == USERSTRATEGY_OC_TYPE_CURRENT_OFFSET
            triggerThreshold += (uint16)current;
            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                releaseThreshold += (uint16)current;
            }
            else
            {
                releaseThreshold = 0U;
            }
#elif USERSTRATEGH_DCHG_FB_TYPE == USERSTRATEGY_OC_TYPE_PERCENT
            temp = (uint16)current;
            temp = temp * triggerThreshold;
            temp = DIVISION(temp, 1000U);
            if (temp > 0x7FFFU)
            {
                temp = 0x7FFFU;
            }
            if (temp == 0U)
            {
                temp = CURRENT_100MA_FROM_A(2U);
            }
            triggerThreshold = (uint16)temp;

            if (CurrentM_DiagIsValidCurrent(releaseThreshold))
            {
                temp = (uint16)current;
                temp = temp * releaseThreshold;
                temp = DIVISION(temp, 1000U);
                if (temp > 0x7FFFU)
                {
                    temp = 0x7FFFU;
                }
                if (temp == 0U)
                {
                    temp = CURRENT_100MA_FROM_A(2U);
                }
            }
            else
            {
                temp = 0U;
            }
            releaseThreshold = (uint16)temp;
#else
            // do nothing
#endif
        }
        para->triggerThreshold = triggerThreshold;
        para->releaseThreshold = releaseThreshold;
        para->triggerConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_FB_OC, level, 2U);
        para->releaseConfirmTime = ParameterM_DiagCalibRead(DIAGNOSIS_ITEM_FB_OC, level, 3U);
    }
}

void UserStrategy_DchgHvProcessAdhesiveDetect(void)
{
    if (RelayMConfigData[RELAYM_FN_POSITIVE_MAIN].GetInstantVoltage != NULL)
    {
        if (RelayM_GetControlStatus(RELAYM_FN_POSITIVE_MAIN) == RELAYM_CONTROL_OFF)
        {
            if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_MAIN) == RELAYM_ACTUAL_OFF)
            {
                (void)RelayM_StartAdhesiveDetect(RELAYM_FN_POSITIVE_MAIN, NULL);
            }
        }
    }
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    if (RelayMConfigData[RELAYM_FN_POSITIVE_AC_CHARGE].GetInstantVoltage != NULL)
    {
        if (ChargerComm_ConfigInfo.AC_Blind_En != STD_ON)
        {
            if (RelayM_GetControlStatus(RELAYM_FN_POSITIVE_AC_CHARGE) == RELAYM_CONTROL_OFF)
            {
                if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_AC_CHARGE) == RELAYM_ACTUAL_OFF)
                {
                    (void)RelayM_StartAdhesiveDetect(RELAYM_FN_POSITIVE_AC_CHARGE, NULL);
                }
            }
        }
    }
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
    if (RelayMConfigData[RELAYM_FN_POSITIVE_DC_CHARGE].GetInstantVoltage != NULL)
    {
        if (ChargerComm_ConfigInfo.DC_Blind_En != STD_ON)
        {
            if (RelayM_GetControlStatus(RELAYM_FN_POSITIVE_DC_CHARGE) == RELAYM_CONTROL_OFF)
            {
                if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_DC_CHARGE) == RELAYM_ACTUAL_OFF)
                {
                    (void)RelayM_StartAdhesiveDetect(RELAYM_FN_POSITIVE_DC_CHARGE, NULL);
                }
            }
        }
    }
#endif
#ifdef RELAYM_FN_CHARGE
    if (RelayMConfigData[RELAYM_FN_CHARGE].GetInstantVoltage != NULL)
    {
        if (ChargerComm_ConfigInfo.DC_Blind_En == STD_OFF  &&
            ChargerComm_ConfigInfo.AC_Blind_En == STD_OFF)
        {
            (void)RelayM_StartAdhesiveDetect(RELAYM_FN_CHARGE, NULL);
        }
        else
        {
            if (ChargeConnectM_DinIsConnect(ChargeConnectM_ConfigInfo.DC_Para.DinPara.type, ChargeConnectM_ConfigInfo.DC_Para.DinPara.channel) == E_NOT_OK &&
                ChargerComm_ConfigInfo.DC_Blind_En == STD_ON)
            {
                if (RelayM_GetControlStatus(RELAYM_FN_CHARGE) == RELAYM_CONTROL_OFF)
                {
                    if (RelayM_GetActualStatus(RELAYM_FN_CHARGE) == RELAYM_ACTUAL_OFF)
                    {
                        (void)RelayM_StartAdhesiveDetect(RELAYM_FN_CHARGE, NULL);
                    }
                }
            }
            else if (ChargeConnectM_DinIsConnect(ChargeConnectM_ConfigInfo.AC_Para.DinPara.type, ChargeConnectM_ConfigInfo.AC_Para.DinPara.channel) == E_NOT_OK &&
                ChargerComm_ConfigInfo.AC_Blind_En == STD_ON)
            {
                if (RelayM_GetControlStatus(RELAYM_FN_CHARGE) == RELAYM_CONTROL_OFF)
                {
                    if (RelayM_GetActualStatus(RELAYM_FN_CHARGE) == RELAYM_ACTUAL_OFF)
                    {
                        (void)RelayM_StartAdhesiveDetect(RELAYM_FN_CHARGE, NULL);
                    }
                }
            }
            else
            {

            }
        }
    }
#endif
}

boolean UserStrategy_DchgHvProcessRelayIsNormal(void)
{
    boolean res = TRUE;

    if (!RELAYM_DIAGNOSIS_IS_NORMAL(RelayM_GetDiagnosisStatus(RELAYM_FN_POSITIVE_MAIN)))
    {
        res = FALSE;
    }
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    else if (!RELAYM_DIAGNOSIS_IS_NORMAL(RelayM_GetDiagnosisStatus(RELAYM_FN_POSITIVE_AC_CHARGE)))
    {
        res = FALSE;
    }
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
    else if (!RELAYM_DIAGNOSIS_IS_NORMAL(RelayM_GetDiagnosisStatus(RELAYM_FN_POSITIVE_DC_CHARGE)))
    {
        res = FALSE;
    }
#endif
#ifdef RELAYM_FN_CHARGE
    else if (!RELAYM_DIAGNOSIS_IS_NORMAL(RelayM_GetDiagnosisStatus(RELAYM_FN_CHARGE)))
    {
        res = FALSE;
    }
#endif
    else
    {
    }
    return res;
}

void UserStrategy_ChgHvProcessAdhesiveDetect(void)
{
    UserStrategy_DchgHvProcessAdhesiveDetect();
}

boolean UserStrategy_ChgHvProcessRelayIsNormal(void)
{
    return UserStrategy_DchgHvProcessRelayIsNormal();
}

#if USERSTRATEGY_DCHG_LV_PROTECT_WITH_CURRENT_EN == STD_ON
static void dchgLvProtectForCurrentCheck(void)
{
    Current_CurrentType current;
    uint32 nowTime = OSTimeGet();
    Diagnosis_LevelType level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV);
    static uint32 lastTime = 0U;

    if (level >= USERSTRATEGY_DCHG_LV_PROTECT_LEVEL)
    {
        current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
        if (!CurrentM_IsValidCurrent(current) || current < CURRENT_S_100MA_FROM_A(-5))
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
            {
                lastTime = nowTime;
                DischargeM_SetDiagnosisDchargeCtl(DIAGNOSIS_ITEM_DCHG_LV, DISCHARGEM_DISCHARGE_DISABLE);
                DischargeM_SetOthersFaultDchargeCtl(DISCHARGEM_OTHERS_FAULT_OVER_DCHG, DISCHARGEM_DISCHARGE_DISABLE);
            }
        }
        else
        {
            lastTime = nowTime;
        }
    }
    else
    {
        lastTime = nowTime;
    }
}
#endif

#if USERSTRATEGY_DCHG_LTV_PROTECT_WITH_CURRENT_EN == STD_ON
static void dchgLtvProtectForCurrentCheck(void)
{
    Current_CurrentType current;
    uint32 nowTime = OSTimeGet();
    Diagnosis_LevelType level = Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV);
    static uint32 lastTime = 0U;

    if (level >= USERSTRATEGY_DCHG_LTV_PROTECT_LEVEL)
    {
        current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
        if (!CurrentM_IsValidCurrent(current) || current < CURRENT_S_100MA_FROM_A(-5))
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
            {
                lastTime = nowTime;
                DischargeM_SetDiagnosisDchargeCtl(DIAGNOSIS_ITEM_DCHG_LTV, DISCHARGEM_DISCHARGE_DISABLE);
                DischargeM_SetOthersFaultDchargeCtl(DISCHARGEM_OTHERS_FAULT_OVER_DCHG, DISCHARGEM_DISCHARGE_DISABLE);
            }
        }
        else
        {
            lastTime = nowTime;
        }
    }
    else
    {
        lastTime = nowTime;
    }
}
#endif

boolean UserStrategy_DchgIsReady(void)
{
    boolean res = FALSE;

    if (HvProcess_GetDchgState() == HVPROCESS_DCHG_HV_ON)
    {
        res = TRUE;
    }
    return res;
}

static void UserStrategy_AllRlyOff(void)
{
    uint8 i;

    for (i = 0U; i < RELAYM_FN_NUM; i++)
    {
        (void)RelayM_Control(i, RELAYM_CONTROL_OFF);
    }
#ifdef RELAYM_FN_HMI
    HLSS_Drive(RelayMConfigData[RELAYM_FN_HMI].DriveHSS, HLSS_DRIVE_OFF);
#endif
}

void UserStrategy_DischargeM_DiagnosisCtlEnableEventCbk(Diagnosis_ItemType item, Diagnosis_LevelType level, Diagnosis_EventType event)
{
    Diagnosis_LevelType nowLevel = DischargeM_GetDischargeDisableLevel(item,FALSE);
    if (Diagnosis_GetLevel(item) < nowLevel)
    {
        DischargeM_DiagnosisCtlEnableEventCbk(item, level, event);
    }
}

#if USERSTRATEGY_LV_POWER_DOWN_EN == STD_ON
static void UserStrategy_LvPowerDown(void)
{
    uint16 lv = Statistic_GetBcuLvMax();
    uint32 nowTime = OSTimeGet();
    uint32 delay = USERSTRATEGY_LV_POWER_DOWN_TIME;
    static uint32 lastTime = 0U;
    RuntimeM_RunModeType mode = RuntimeM_GetMode();

    if (!CHARGECONNECTM_IS_CONNECT() && (mode == RUNTIMEM_RUNMODE_CALIBRATE || mode == RUNTIMEM_RUNMODE_NORMAL))
    {
        if (CellDataM_VoltageIsValid(lv) && lv <= (uint16)USERSTRATEGY_LV_POWER_DOWN_VOLT)
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
                {
                    RuntimeM_RequestPowerDown();
                }
            }
        }
        else
        {
            lastTime = 0U;
        }
    }
    else
    {
        lastTime = 0U;
    }
}
#endif

boolean UserStrategy_GetBuzzerSta(void)
{
    return UserStrategy_innerData.isBuzzer;
}

uint16 UserStrategy_ChgSckTmpM_GetAbnormalTemperatureNum(void)
{
    uint8 temperature;
    uint16 i = 0U, num = 0U, count = 0U;
    imask_t mask;
    Charge_ChargeType type;

    // if (SystemConnection_ConfigInfo.BcuType >= DEVICE_TYPE_A650 && SystemConnection_ConfigInfo.BcuType <= DEVICE_TYPE_A653)
    // {
    //     i = 0U;
    //     num = i + 2U;
    // }
    // else
    // {
    //     type = ChargeConnectM_GetConnectType();
    //     if (type == CHARGE_TYPE_DC)
    //     {
    //         i = 0U;
    //         num = i + 2U;
    //     }
    //     else if (type == CHARGE_TYPE_AC)
    //     {
    //         i = 2U;
    //         num = i + 3U;
    //     }
    //     else
    //     {
    //     }
    // }
    type = ChargeConnectM_GetConnectType();
    if (type == CHARGE_TYPE_DC)
    {
        i = 0U;
        num = i + 2U;
    }
    else if (type == CHARGE_TYPE_AC)
    {
        i = 2U;
        num = i + 3U;
    }
    else
    {
    }
    for (; i < num; i++)
    {
        if (ChgSckTmpM_ConfigInfo.temperature_config[i].enable == STD_ON)
        {
            Irq_Save(mask);
            temperature = ChgSckTmpM_InternalStatusInfo[i].temperature;
            Irq_Restore(mask);
            if (CellDataM_TemperatureIsValid((uint16)temperature) == 0U)
            {
                count++;
            }
        }
    }
    return count;
}

uint16 UserStrategy_GetInsuLeak(void)
{
    uint16 leak = 0xFFFFU;
#if defined(UPA550)||defined(UPA650)
    uint16 totalVol = Statistic_GetBcu100mvTotalVoltage();
    uint16 insu = Insu_GetPositive();
    uint32 insuFact;

    if (Insu_ResIsValid(insu))
    {
        insuFact = (uint32)insu * 1000U;
        if (Statistic_TotalVoltageIsValid(totalVol) && totalVol != 0U)
        {
            totalVol = (uint16)V_FROM_100MV(totalVol);
            insuFact = DIVISION(insuFact, (uint32)totalVol);
            leak = (uint16)insuFact;
        }
    }
#endif

    return leak;
}

void HvProcess_ChgHeatAndChargeToChargeTempAction(void)
{
    // do nothing
}

boolean HvProcess_ChgHeatAndChargeToChargePrepareCond(void)
{
    boolean res = FALSE;
    uint32 delay = 1000UL;
    static uint32 overTick = 0UL;
    uint32 overTimeDelay = 5000UL; // 转化阶段最多等待5S
    static uint32 lastTime = 0UL;
    uint32 nowTime = OSTimeGet();
    static uint32 monitorTime = 0UL;
    Current_CurrentType current, maxCurrLimit;

    /** 监控 */
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 300UL){
        lastTime = 0UL;
        overTick = 0UL;
    }
    monitorTime = nowTime;

    maxCurrLimit = USERSTRATEGY_START_HEAT_CURRENT + 100; /** 加热请求电流+10A */
    current = ChargerComm_GetChargerOutputCurrent();
    if (CurrentM_IsValidCurrent(current))
    {
        if (current <= maxCurrLimit)
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
                {
                    res = TRUE;
                    lastTime = 0U;
                    overTick = 0U;
                }
            }
        }
        else
        {
            lastTime = 0U;
        }
    }
    else
    {
        lastTime = 0U;
    }

    if (overTick == 0U)
    {
        overTick = nowTime;
    }
    else
    {
        if (MS_GET_INTERNAL(overTick, nowTime) >= overTimeDelay)
        {
            res = TRUE;
            lastTime = 0UL;
            overTick = 0UL;
        }
    }

    return res;
}

void HvProcess_ChgHeatAndChargeToChargePrepareAction(void)
{
    (void)RelayM_Control(RELAYM_FN_HEATER, RELAYM_CONTROL_OFF);
    HvProcess_ChgSetHeatStateNone();
}

boolean HvProcess_ChgHvOnToHeatAndChargeTempCond(void)
{
    boolean res = FALSE;
    App_TemperatureType minTemp;
    uint8 setTemp;
    uint8 valid;
    uint32 delay = 1000UL;
    static uint32 lastTime = 0UL;
    uint32 nowTime = OSTimeGet();
    static uint32 monitorTime = 0UL;

    /** 监控 */
    if (MS_GET_INTERNAL(monitorTime, nowTime) >= 300UL){
        lastTime = 0UL;
    }
    monitorTime = nowTime;

    setTemp = HvProcess_ChgGetChargeToHeatAndChargeTemp();
    if (CellDataM_TemperatureIsValid((uint16)setTemp))
    {
        minTemp = Statistic_GetBcuLt(0U);
        valid = CellDataM_TemperatureIsValid((uint16)minTemp);
        if (valid && minTemp < setTemp)
        {
            if (lastTime == 0U)
            {
                lastTime = nowTime;
            }
            else
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
                {
                    res = TRUE;
                    lastTime = 0U;
                }
            }
        }
        else
        {
            lastTime = 0U;
        }
    }

    return res;
}

void HvProcess_ChgHvOnToHeatAndChargeTempAction(void)
{
    // do nothing
}

boolean HvProcess_ChgIsFinishCond(void)
{
    boolean res = FALSE;

    if (ChargeM_BatteryChargeIsFinish() == TRUE)
    {
        res = TRUE;
    }
    return res;
}

void HvProcess_ChgFinishAction(void)
{
    HvProcess_ChgInnerData.RelayOffTick = OSTimeGet();
    HvProcess_ChgInnerData.chgFinishFlag = TRUE;
}

boolean HvProcess_ChgChargeConnectionCond(void)
{
    boolean res = FALSE;

    if (!CHARGECONNECTM_IS_CONNECT())
    {
        res = TRUE;
    }
    return res;
}

void HvProcess_ChgChargeConnectionAction(void)
{
    HvProcess_ChgInnerData.RelayOffTick = OSTimeGet();
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_NONE);
}

boolean HvProcess_ChgFaultCond(void)
{
    boolean res = FALSE;

    if (ChargeM_ChargeIsFault() == E_OK)
    {
        if (HvProcess_ChgIsFinishCond())
        {
            HvProcess_ChgFinishAction();
        }
        res = TRUE;
    }
    return res;
}

void HvProcess_ChgFaultAction(void)
{
    HvProcess_ChgInnerData.RelayOffTick = OSTimeGet();
}

boolean HvProcess_ChgRelayOffDelayCond(void)
{
    boolean res = FALSE;
    uint32 nowTime = OSTimeGet();
    uint32 delay = S_TO_MS(12U);
    Current_CurrentType current;

    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);

    if (CurrentM_IsValidCurrent(current))
    {
        if (abs(current) <= CURRENT_S_100MA_FROM_A(3))
        {
            delay = 0U;
        }
    }
    if (MS_GET_INTERNAL(HvProcess_ChgInnerData.RelayOffTick, nowTime) >= delay)
    {
        res = TRUE;
        HvProcess_ChgInnerData.RelayOffTick = nowTime;
    }
    return res;
}

void HvProcess_ChgRelayOffDelayAction(void)
{
#ifdef XUGONG_VCU_FLAG
    uint32 time;
    uint16 temp;
    if (DatetimeM_GetDatetime(&time) == DATETIME_TRUSTY)
    {
        temp = (uint16)time;
        (void)ParameterM_EeepWrite(PARAMETERM_EEEP_STOP_CHG_TIME_L_INDEX, temp);
        temp = time >> 16;
        (void)ParameterM_EeepWrite(PARAMETERM_EEEP_STOP_CHG_TIME_H_INDEX, temp);
        HvProcess_ChgInnerData.IsCharging = FALSE;
    }
#endif
#ifdef RELAYM_FN_HEATER
    (void)RelayM_Control(RELAYM_FN_HEATER, RELAYM_CONTROL_OFF);
#endif
#ifdef RELAYM_FN_CHARGE
    (void)RelayM_Control(RELAYM_FN_CHARGE, RELAYM_CONTROL_OFF);
#endif
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    (void)RelayM_Control(RELAYM_FN_POSITIVE_AC_CHARGE, RELAYM_CONTROL_OFF);
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
    (void)RelayM_Control(RELAYM_FN_POSITIVE_DC_CHARGE, RELAYM_CONTROL_OFF);
#endif
#ifdef RELAYM_FN_NEGATIVE_MAIN
    (void)RelayM_Control(RELAYM_FN_NEGATIVE_MAIN, RELAYM_CONTROL_OFF);
#endif
    if (HvProcess_ChgInnerData.chgFinishFlag == TRUE)
    {
#if USERSTRATEGY_RESET_TO_OTA_EN == STD_OFF
        if (RuntimeM_ConfigData.chargeEndOtaSupport)
        {
#ifdef RELAYM_FN_SELF_LOCK
            (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
#else
            RuntimeM_RequestPowerDown();
#endif
        }
#endif
    }
}

boolean HvProcess_ChgReStartJudgeCond(void)
{
    boolean res = FALSE;
    uint8 state = 0U;
    App_Tv100mvType bat_tv, hv1, hv2 = 0U, hv3 = 0U, temp;
    uint32 delay = 3000UL, nowTime = OSTimeGet();
    static uint32 lastTime = 0UL;

#if defined(UPA530)||defined(UPA630)||defined(UPA640)
    bat_tv = Statistic_GetBcu100mvTotalVoltage();
#else
    bat_tv = HV_GetVoltage(HV_CHANNEL_BPOS);
#endif
    if (Statistic_TotalVoltageIsValid(bat_tv))
    {
        if (RelayMConfigData[RELAYM_FN_POSITIVE_MAIN].GetInstantVoltage != NULL)
        {
            hv1 = (App_Tv100mvType)RelayMConfigData[RELAYM_FN_POSITIVE_MAIN].GetInstantVoltage();
            if (Statistic_TotalVoltageIsValid(hv1))
            {
                temp = (App_Tv100mvType)((uint32)bat_tv * (uint32)RelayMConfigData[RELAYM_FN_POSITIVE_MAIN].totalPercent / 100UL);
                if (hv1 > temp)
                {
                    state |= 1U;
                }
            }
        }
        if (state == 0U)
        {
            if (ChargerComm_ConfigInfo.AC_RelayType != 0xFFU &&
                RelayMConfigData[ChargerComm_ConfigInfo.AC_RelayType].GetInstantVoltage != NULL)
            {
                hv2 = (App_Tv100mvType)RelayMConfigData[ChargerComm_ConfigInfo.AC_RelayType].GetInstantVoltage();
                if (Statistic_TotalVoltageIsValid(hv2))
                {
                    temp = (App_Tv100mvType)((uint32)bat_tv * (uint32)RelayMConfigData[ChargerComm_ConfigInfo.AC_RelayType].totalPercent / 100UL);
                    if (hv2 > temp)
                    {
                        state |= (uint8)((uint8)1U << 1);
                    }
                }
            }
        }
        if (state ==0U)
        {
            if (ChargerComm_ConfigInfo.DC_RelayType != 0xFFU &&
                RelayMConfigData[ChargerComm_ConfigInfo.DC_RelayType].GetInstantVoltage != NULL)
            {
                hv3 = (App_Tv100mvType)RelayMConfigData[ChargerComm_ConfigInfo.DC_RelayType].GetInstantVoltage();
                {
                    if (Statistic_TotalVoltageIsValid(hv3))
                    {
                        temp = (App_Tv100mvType)((uint32)bat_tv * (uint32)RelayMConfigData[ChargerComm_ConfigInfo.DC_RelayType].totalPercent / 100UL);
                        if (hv3 > temp)
                        {
                            state |= (uint8)((uint8)1U << 2);
                        }
                    }
                }
            }
        }
        if (state == 0U)
        {
            delay = 3000U;
        }
    }
    if (lastTime == 0UL)
    {
        lastTime = nowTime;
    }
    if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
    {
        lastTime = 0UL;
        res = TRUE;
    }
    return res;
}

void HvProcess_ChgReStartJudgeAction(void)
{
    // HvProcess_ChgInnerData.RelayAdhesCheckFlag = FALSE;
    HvProcess_ChgSetHeatStateNone();
}

void HvProcess_ChgSetHeatStateNone(void)
{
    if (TemperatureM_GetHeatState() != TEMPERATUREM_HEAT_STATE_FAULT) //加热故障不可恢复
    {
        TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_NONE);
    }
}

/**************************************************
1. 上电起始start状态
temp <= USERSTRATEGY_START_HEAT_TEMP                                          next-state: 加热
temp > USERSTRATEGY_START_HEAT_TEMP && temp < USERSTRATEGY_STOP_HEAT_TEMP     next-state: 边充电边加热
temp >= USERSTRATEGY_STOP_HEAT_TEMP                                           next-state: 充电

2. 加热状态
temp >= (USERSTRATEGY_START_HEAT_TEMP + USERSTRATEGY_REVERT_TEMP)             next-state: 边充电边加热

3. 边充电边加热状态
temp <= USERSTRATEGY_START_HEAT_TEMP                                          next-state: 加热
temp >= (USERSTRATEGU_STOP_HEAT_TEMP + USERSTRATEGY_REVERT_TEMP)              next-state: 充电

4. 充电状态
temp < USERSTRATEGY_STOP_HEAT_TEMP                                            next-state: 边充电边加热
***************************************************/

// 上电进入加热的温度
uint8 HvProcess_ChgGetStartToHeatTemp(void)
{
#ifdef RELAYM_FN_HEATER
    uint8 temp = (uint8)USERSTRATEGY_START_HEAT_TEMP;
#else
    uint8 temp = CELL_TEMPERATURE_INVALID_VALUE;
#endif

    return temp;
}

// 上电进入充电的温度
uint8 HvProcess_ChgGetStartToChargeTemp(void)
{
#ifdef RELAYM_FN_HEATER
    uint8 temp = (uint8)USERSTRATEGY_STOP_HEAT_TEMP;
#else
    uint8 temp = CELL_TEMPERATURE_INVALID_VALUE
#endif

    return temp;
}

// 加热转边充电边加热的温度
uint8 HvProcess_ChgGetHeatToHeatAndChargeTemp(void)
{
#ifdef RELAYM_FN_HEATER
    uint8 temp = (uint8)USERSTRATEGY_START_HEAT_TEMP + (uint8)USERSTRATEGY_REVERT_TEMP;
#else
    uint8 temp = CELL_TEMPERATURE_INVALID_VALUE;
#endif

    return temp;
}

// 边充电边加热转加热的温度
uint8 HvProcess_ChgGetHeatAndChargeToHeatTemp(void)
{
#ifdef RELAYM_FN_HEATER
    uint8 temp = (uint8)USERSTRATEGY_START_HEAT_TEMP;
#else
    uint8 temp = CELL_TEMPERATURE_INVALID_VALUE;
#endif

    return temp;
}

// 边充电边加热转充电的温度
uint8 HvProcess_ChgGetHeatAndChargeToChargeTemp(void)
{
#ifdef RELAYM_FN_HEATER
    uint8 temp = (uint8)USERSTRATEGY_STOP_HEAT_TEMP + (uint8)USERSTRATEGY_REVERT_TEMP;
#else
    uint8 temp = CELL_TEMPERATURE_INVALID_VALUE;
#endif

    return temp;
}

// 充电转边充电边加热的温度
uint8 HvProcess_ChgGetChargeToHeatAndChargeTemp(void)
{
#ifdef RELAYM_FN_HEATER
    uint8 temp = (uint8)USERSTRATEGY_STOP_HEAT_TEMP;
#else
    uint8 temp = CELL_TEMPERATURE_INVALID_VALUE;
#endif

    return temp;
}

boolean HvProcess_ChargerIsHeatMode(void)
{
    boolean res = FALSE;
    HvProcess_ChgStateType chgStatus = HvProcess_GetChgState();

    if (chgStatus == HVPROCESS_CHG_HEAT_ON)
    {
        res = TRUE;
    }
    return res;
}

boolean HvProcess_IsJumpMode(void)
{
    boolean res = FALSE;
    HvProcess_ChgStateType chgStatus = HvProcess_GetChgState();

    if (chgStatus == HVPROCESS_CHG_START_TO_HEAT_CURR_VALID_JUDGE ||
        chgStatus == HVPROCESS_CHG_START_TO_HEAT_RELAY_DELAY_ACTION ||
        chgStatus == HVPROCESS_CHG_HEAT_AND_CHARGE_TO_HEAT_PREPARE ||
        chgStatus == HVPROCESS_CHG_START_TO_HEAT_AND_CHARGE_CURR_VALID_JUDGE ||
        chgStatus == HVPROCESS_CHG_HEAT_TO_HEAT_AND_CHARGE_PREPARE ||
        chgStatus == HVPROCESS_CHG_HEAT_AND_CHARGE_TO_CHARGE_PREPARE)
    {
        res = TRUE;
    }

    return res;
}

boolean HvProcess_IsHeatAndChargeMode(void)
{
    boolean res = FALSE;
    HvProcess_ChgStateType chgStatus = HvProcess_GetChgState();

    if (chgStatus == HVPROCESS_CHG_HEAT_AND_CHARGE_ON)
    {
        res = TRUE;
    }

    return res;
}

boolean HvProcess_ChgHeatPowerFaultCond(void)
{
    boolean res = FALSE, flag = FALSE;
    uint32 nowTime = OSTimeGet(),delay = 60000UL;
    uint16 current = (uint16)ChargerComm_GetChargerOutputCurrent();
    static uint32 lastTime = 0UL,monitorTime = 0U;
    uint16 heatCurrent = (uint16)USERSTRATEGY_START_HEAT_CURRENT;

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 300UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    heatCurrent += CURRENT_100MA_FROM_A(10U);
    if (current < CURRENT_100MA_FROM_A(1U))
    {
        flag = TRUE;
    }
    else if (current >= heatCurrent)
    {
        flag = TRUE;
        delay = 5000UL;
    }
    else
    {}
    if (flag && HvProcess_ChgInnerData.HeatIsFinish == FALSE)
    {
        if (lastTime == 0UL)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
        {
            res = TRUE;
            lastTime = 0UL;
        }
    }
    else
    {
        lastTime = 0UL;
    }
    return res;
}

void HvProcess_ChgHeatFaultAction(void)
{
    HvProcess_ChgInnerData.RelayOffTick = OSTimeGet();
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_FAULT);
}

boolean HvProcess_IsCharging(void)
{
    return HvProcess_ChgInnerData.IsCharging;
}

void HvProcess_SetRelayAdhesFlag(void)
{
    HvProcess_ChgInnerData.RelayAdhesCheckFlag = TRUE;
}

boolean HvProcess_CheckRelayAdhesFlag(void)
{
    return HvProcess_ChgInnerData.RelayAdhesCheckFlag;
}