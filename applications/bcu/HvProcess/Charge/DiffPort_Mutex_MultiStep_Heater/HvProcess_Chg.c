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

    dchgState = HvProcess_GetDchgState();
    if (CHARGECONNECTM_IS_CONNECT() && dchgState == HVPROCESS_DCHG_START && nowTime >= 300U)
    {
        HvProcess_ChgSetHeatStateNone();
        if (!HvProcess_ChgInnerData.RelayAdhesCheckFlag)
        {
            HvProcess_ChgInnerData.RelayAdhesCheckFlag = TRUE;
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
    HvProcess_ChgInnerData.chgRelay = RELAYM_FN_NONE;
    HvProcess_ChgInnerData.HeatRelayFaultCheckFlag = FALSE;
#ifdef RELAYM_FN_NEGATIVE_MAIN
    (void)RelayM_Control(RELAYM_FN_NEGATIVE_MAIN, RELAYM_CONTROL_ON);
#endif
#ifdef RELAYM_FN_CHARGE
        (void)RelayM_Control(RELAYM_FN_CHARGE, RELAYM_CONTROL_ON);
#endif
    if (ChargerComm_GetChargeType() == CHARGE_TYPE_AC)
    {
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
        (void)RelayM_Control(RELAYM_FN_POSITIVE_AC_CHARGE, RELAYM_CONTROL_ON);
#endif
    }
    else
    {
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
        (void)RelayM_Control(RELAYM_FN_POSITIVE_DC_CHARGE, RELAYM_CONTROL_ON);
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
    uint32 delay = 30000UL, nowTime = OSTimeGet();
    static uint32 lastTime = 0UL;

#if defined(A640)||defined(A641)
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
            if (RelayMConfigData[ChargerComm_ConfigInfo.AC_RelayType].GetInstantVoltage != NULL)
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
            if (RelayMConfigData[ChargerComm_ConfigInfo.DC_RelayType].GetInstantVoltage != NULL)
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
    HvProcess_ChgInnerData.RelayAdhesCheckFlag = FALSE;
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
