/**
 * \file HvProcess_Chg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 高压充电流程控制文件.
 *
 * \note 充放电异口带加热高压流程，充电加热互斥
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
    uint32 nowtime = OSTimeGet();
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
    if (CHARGECONNECTM_IS_CONNECT() && dchgState == HVPROCESS_DCHG_START && nowtime >= delay)
    {
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
        HvProcess_ChgInnerData.chgRelay = RELAYM_FN_CHARGE;
        (void)RelayM_Control(RELAYM_FN_CHARGE, RELAYM_CONTROL_ON);
#endif
    if (ChargerComm_GetChargeType() == CHARGE_TYPE_AC)
    {
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
        HvProcess_ChgInnerData.chgRelay = RELAYM_FN_POSITIVE_AC_CHARGE;
        (void)RelayM_Control(RELAYM_FN_POSITIVE_AC_CHARGE, RELAYM_CONTROL_ON);
#endif
    }
    else
    {
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
        HvProcess_ChgInnerData.chgRelay = RELAYM_FN_POSITIVE_DC_CHARGE;
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

boolean HvProcess_ChgStartHeatCond(void)
{
    App_TemperatureType temperature, heatOnTemp;
    boolean res = FALSE;
    static uint32 lastTime = 0U,monitorTime = 0U;
    uint32 nowTime = OSTimeGet();

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 300UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    temperature = Statistic_GetBcuLt(0U);
    heatOnTemp = (App_TemperatureType)USERSTRATEGY_START_HEAT_TEMP;
    if (CellDataM_TemperatureIsValid((uint16)temperature) != 0U)
    {
        if (temperature <= heatOnTemp)
        {
            if (lastTime == 0U){
                lastTime = nowTime;
            }
            if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
            {
                res = TRUE;
            }
        }
        else
        {
            lastTime = 0U;
        }
    }
    return res;
}

void HvProcess_ChgStartHeatAction(void)
{
    if (HvProcess_ChgInnerData.chgRelay != RELAYM_FN_NONE)
    {
        (void)RelayM_Control(HvProcess_ChgInnerData.chgRelay, RELAYM_CONTROL_ON);
    }
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_LT);
    HvProcess_ChgInnerData.HeatIsFinish = FALSE;
}

boolean HvProcess_ChgStartChargeCond(void)
{
    App_TemperatureType temperature, heatOnTemp;
    boolean res = FALSE;
    static uint32 lastTime = 0U,monitorTime = 0U;
    uint32 nowTime = OSTimeGet();

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 300UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    temperature = Statistic_GetBcuLt(0U);
    heatOnTemp = (App_TemperatureType)USERSTRATEGY_START_HEAT_TEMP;
    if (CellDataM_TemperatureIsValid((uint16)temperature) != 0U)
    {
        if (temperature > heatOnTemp)
        {
            if (lastTime == 0U){
                lastTime = nowTime;
            }
            if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000U)
            {
                res = TRUE;
            }
        }
        else
        {
            lastTime = 0U;
        }
    }

    return res;
}

void HvProcess_ChgStartChargeAction(void)
{
    if (HvProcess_ChgInnerData.chgRelay != RELAYM_FN_NONE)
    {
        (void)RelayM_Control(HvProcess_ChgInnerData.chgRelay, RELAYM_CONTROL_ON);
    }
#ifdef RELAYM_FN_HEATER
    (void)RelayM_Control(RELAYM_FN_HEATER, RELAYM_CONTROL_OFF);
#endif
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_NONE);
}

boolean HvProcess_ChgStartHeatTimeoutCond(void)
{
    boolean res = FALSE;
    static uint32 lastTime = 0U,monitorTime = 0U;
    uint32 nowTime = OSTimeGet();

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 500UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    if (lastTime == 0U)
    {
        lastTime = nowTime;
    }
    if (MS_GET_INTERNAL(lastTime, nowTime) >= 5UL * 60000UL)
    {
        res = TRUE;
    }
    return res;
}

void HvProcess_ChgStartHeatTimeoutAction(void)
{
    HvProcess_ChgFaultAction();
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_FAULT);
}

boolean HvProcess_ChgStartHeatFinishCond(void)
{
    boolean res = FALSE;
#ifdef RELAYM_FN_HEATER
    static uint32 lastTime = 0UL,monitorTime = 0UL;
    uint32 nowTime = OSTimeGet();
    RelayM_ActualStatusType heatRelay = RelayM_GetActualStatus(RELAYM_FN_HEATER);

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 300UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    if (RelayM_GetActualStatus(HvProcess_ChgInnerData.chgRelay) == RELAYM_ACTUAL_ON && HvProcess_ChgInnerData.chgRelay != (RelayM_FunctionType)RELAYM_FN_NONE && heatRelay == RELAYM_ACTUAL_ON)
    {
        if (lastTime == 0UL)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) >= 5000U)
        {
            res = TRUE;
            lastTime = 0UL;
        }
    }
    else
    {
        lastTime = 0UL;
    }
#endif
    return res;
}

void HvProcess_ChgStartHeatFinishAction(void)
{
    if (HvProcess_ChgInnerData.chgRelay != RELAYM_FN_NONE)
    {
        (void)RelayM_Control(HvProcess_ChgInnerData.chgRelay, RELAYM_CONTROL_OFF);
    }
}

boolean HvProcess_ChgHeatCurrentIsOk(void)
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

void HvProcess_ChgHeatRelayOn(void)
{
#ifdef RELAYM_FN_HEATER
    (void)RelayM_Control(RELAYM_FN_HEATER, RELAYM_CONTROL_ON);
#endif
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

boolean HvProcess_ChgHeatFinishCond(void)
{
    boolean res = FALSE;
    static uint32 lastTime = 0UL,monitorTime = 0U;
    uint32 nowTime = OSTimeGet();

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 300UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    if (HvProcess_ChgInnerData.HeatIsFinish == TRUE)
    {
        if (lastTime == 0UL)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) >= 5000UL)
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

void HvProcess_ChgHeatFinishAction(void)
{
    if (HvProcess_ChgInnerData.chgRelay != RELAYM_FN_NONE)
    {
        (void)RelayM_Control(HvProcess_ChgInnerData.chgRelay, RELAYM_CONTROL_ON);
    }
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_NONE);
    HvProcess_ChgInnerData.HeatIsFinish = FALSE;
}

boolean HvProcess_ChgHeatFinishCheckCond(void)
{
    App_TemperatureType temperature, heatOffTemp;
    boolean flag = FALSE, res = FALSE;
    static uint32 lastTime = 0UL,monitorTime = 0U;
    uint32 nowTime = OSTimeGet();

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 300UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    temperature = Statistic_GetBcuLt(0U);
    heatOffTemp = (App_TemperatureType)USERSTRATEGY_STOP_HEAT_TEMP;
    if (CellDataM_TemperatureIsValid((uint16)temperature) != 0U)
    {
        if (temperature >= heatOffTemp)
        {
            flag = TRUE;
        }
    }
    if (flag)
    {
        if (lastTime == 0UL)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) >= 3000UL)
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

void HvProcess_ChgHeatFinishCheckAction(void)
{
    HvProcess_ChgInnerData.HeatIsFinish = TRUE;
}

boolean HvProcess_ChgStartChgFinishCond(void)
{
    boolean res = FALSE;
    uint32 nowTime = OSTimeGet();
    static uint32 lastTime = 0U,monitorTime = 0U;

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 300UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    if (RelayM_GetActualStatus(HvProcess_ChgInnerData.chgRelay) != RELAYM_ACTUAL_ON && HvProcess_ChgInnerData.chgRelay != RELAYM_FN_NONE)
    {
        lastTime = 0U;
    }
    else
    {
        if (lastTime == 0U)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000UL)
        {
            res = TRUE;
        }
    }

    return res;
}

void HvProcess_ChgStartChgFinishAction(void)
{
#ifdef RELAYM_FN_HEATER
    (void)RelayM_Control(RELAYM_FN_HEATER, RELAYM_CONTROL_OFF);
#endif
    TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_NONE);
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
    uint32 delay = 30000UL, nowTime = OSTimeGet();
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
    HvProcess_ChgInnerData.RelayAdhesCheckFlag = FALSE;
    if (TemperatureM_GetHeatState() != TEMPERATUREM_HEAT_STATE_FAULT)
    {
        TemperatureM_SetHeatState(TEMPERATUREM_HEAT_STATE_NONE);
    }
}

boolean HvProcess_ChargerIsHeatMode(void)
{
    boolean res = FALSE;
    HvProcess_ChgStateType chgStatus = HvProcess_GetChgState();

    if (chgStatus == HVPROCESS_CHG_HEATING)
    {
        res = TRUE;
    }
    return res;
}

boolean HvProcess_IsJumpMode(void)
{
    boolean res = FALSE;
    HvProcess_ChgStateType chgStatus = HvProcess_GetChgState();

    if (chgStatus == HVPROCESS_CHG_START_HEAT ||
        chgStatus == HVPROCESS_CHG_START_CHARGE)
    {
        res = TRUE;
    }

    return res;
}

boolean HvProcess_IsHeatAndChargeMode(void)
{
    return FALSE;
}

boolean HvProcess_IsCharging(void)
{
    return HvProcess_ChgInnerData.IsCharging;
}
