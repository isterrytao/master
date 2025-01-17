/**
 * \file HvProcess_Chg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 高压充电流程控制文件.
 *
 * \note 充放电异口无预充无加热高压流程
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170320 |
 */
#include "stdlib.h"
#include "Cpu.h"
#include "HvProcess_Dchg.h"
#include "HvProcess_Chg.h"
#include "ChargeConnectM.h"
#include "ChargeM.h"
#include "Statistic.h"
#include "DischargeM.h"
#include "RelayM.h"
#include "RelayM_Lcfg.h"
#include "ChargerComm.h"
#include "RuntimeM.h"
#include "ChargerCommUser_Messages.h"
#include "ChargerComm_LCfg.h"
#include "UserStrategy.h"
#include "BridgeInsu_Cfg.h"
#include "VcuComm_Messages.h"
#include "DatetimeM.h"
#include "ParameterM.h"

static HvProcess_ChgInnerDataType HvProcess_ChgInnerData;


void HvProcess_ChgInit(Async_LooperType *looper)
{
    (void)looper;
    HvProcess_ChgInnerData.State = HVPROCESS_CHG_START;
    HvProcess_ChgInnerData.OnChgRly = RELAYM_FN_NONE;
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
    uint32 nowTime = OSTimeGet();
    uint32 delay = 2500U;
#if defined(UPA530)||defined(UPA630)||defined(UPA640)
    delay = 500U;
#else
#if BRIDGEINSU_TYPE == BRIDGEINSU_MOS_OFF
    delay = 500U;
#endif
#endif
    if (nowTime >= delay)
    {
        if (!HvProcess_ChgInnerData.RelayAdhesCheckFlag && nowTime >= 500UL)
        {
            HvProcess_ChgInnerData.RelayAdhesCheckFlag = TRUE;
            UserStrategy_ChgHvProcessAdhesiveDetect();
        }
        if (HvProcess_ChgInnerData.RelayAdhesCheckFlag)
        {
            if (UserStrategy_ChgHvProcessRelayIsNormal())
            {
                if (ChargeM_ChargeIsReady() == E_OK)
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
#ifdef RELAYM_FN_NEGATIVE_MAIN
    (void)RelayM_Control(RELAYM_FN_NEGATIVE_MAIN, RELAYM_CONTROL_ON);
#endif
#ifdef RELAYM_FN_CHARGE
        (void)RelayM_Control(RELAYM_FN_CHARGE, RELAYM_CONTROL_ON);
        HvProcess_ChgInnerData.OnChgRly = RELAYM_FN_CHARGE;
#endif
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
        (void)RelayM_Control(RELAYM_FN_POSITIVE_AC_CHARGE, RELAYM_CONTROL_ON);
        HvProcess_ChgInnerData.OnChgRly = RELAYM_FN_POSITIVE_AC_CHARGE;
#endif
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
        UserStrategy_FullChargeReleaseHook();
    }
    return res;
}

void HvProcess_ChgChargeConnectionAction(void)
{
    HvProcess_ChgInnerData.RelayOffTick = OSTimeGet();
}

boolean HvProcess_ChgFaultCond(void)
{
    boolean res = FALSE;
#ifdef XUGONG_VCU_FLAG
    uint32 time;
    uint16 temp;
#endif

    if (ChargeM_ChargeIsFault() == E_OK)
    {
        if (HvProcess_ChgIsFinishCond())
        {
            HvProcess_ChgFinishAction();
        }
        res = TRUE;
    }
    else
    {
#ifdef XUGONG_VCU_FLAG
        if (CHARGECONNECTM_IS_CONNECT())
        {
            if (DatetimeM_GetDatetime(&time) == DATETIME_TRUSTY && !HvProcess_ChgInnerData.IsCharging)
            {
                temp = (uint16)time;
                (void)ParameterM_EeepWrite(PARAMETERM_EEEP_START_CHG_TIME_L_INDEX, temp);
                temp = time >> 16;
                (void)ParameterM_EeepWrite(PARAMETERM_EEEP_START_CHG_TIME_H_INDEX, temp);
                HvProcess_ChgInnerData.IsCharging = TRUE;
            }
        }
        else
        {
            if (DatetimeM_GetDatetime(&time) == DATETIME_TRUSTY && HvProcess_ChgInnerData.IsCharging)
            {
                temp = (uint16)time;
                (void)ParameterM_EeepWrite(PARAMETERM_EEEP_STOP_CHG_TIME_L_INDEX, temp);
                temp = time >> 16;
                (void)ParameterM_EeepWrite(PARAMETERM_EEEP_STOP_CHG_TIME_H_INDEX, temp);
                HvProcess_ChgInnerData.IsCharging = FALSE;
            }
        }
#endif
    }
    return res;
}

void HvProcess_ChgFaultAction(void)
{
    HvProcess_ChgInnerData.RelayOffTick = OSTimeGet();
}

boolean HvProcess_ChgRelayOffDelayCond(void)
{
    return TRUE;
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
    HvProcess_ChgInnerData.RelayAdhesCheckFlag = FALSE;
}

boolean HvProcess_ChgRestartAllowedCond(void)
{
    boolean res = FALSE;
    uint8 state = 0U;
    App_Tv100mvType bat_tv, hv1, hv2 = 0U, hv3 = 0U, temp;
    uint32 delay = 30000UL, nowTime = OSTimeGet();
    // Charge_ChargeType type = HvProcess_ChgInnerData.ChgType;
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

boolean HvProcess_ChargerIsHeatMode(void)
{
    boolean res = FALSE;
    return res;
}

boolean HvProcess_HeatIsJump(void)
{
    boolean res = FALSE;
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
