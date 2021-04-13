/**
 * \file HvProcess_Dchg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 高压放电流程控制文件.
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
#include "Statistic.h"
#include "Stdlib.h"
#include "UserStrategy.h"
#include "ChargerComm_LCfg.h"
#include "PrechargeM.h"

static HvProcess_DchgInnerDataType HvProcess_DchgInnerData;

void HvProcess_DchgInit(Async_LooperType *looper)
{
    (void)looper;
    HvProcess_DchgInnerData.State = HVPROCESS_DCHG_START;
}

HvProcess_DchgStateType HvProcess_GetDchgState(void)
{
    HvProcess_DchgStateType state;
    imask_t mask;

    Irq_Save(mask);
    state = HvProcess_DchgInnerData.State;
    Irq_Restore(mask);

    return state;
}

void HvProcess_SetDchgState(HvProcess_DchgStateType state)
{
    imask_t mask;

    Irq_Save(mask);
    HvProcess_DchgInnerData.State = state;
    Irq_Restore(mask);
}

void HvProcess_DchgPoll(void)
{

}

static boolean WakeupSignalIsOk(void)
{
    boolean res = TRUE;
#if defined(A640)||defined(A641)||defined(A630)||defined(A635)
#else
    boolean flag = FALSE;
    uint8 wakeup;

    wakeup = RuntimeM_GetWakeSignal();
    if (ChargerComm_ConfigInfo.AC_Protocol != CHARGERCOMM_PROTOCOL_NONE)
    {
        if (ChargeConnectM_ConfigInfo.AC_Para.Wakeup != RUNTIMEM_WAKEUP_SIGNAL_BIT_NONE &&
            ChargeConnectM_ConfigInfo.AC_Para.Wakeup != RUNTIMEM_WAKEUP_SIGNAL_BIT_KEY_ON &&
            (ChargeConnectM_ConfigInfo.AC_Para.Wakeup & wakeup) != 0U)
        {
            flag = TRUE;
        }
    }
    if (ChargerComm_ConfigInfo.DC_Protocol != CHARGERCOMM_PROTOCOL_NONE)
    {
        if (ChargeConnectM_ConfigInfo.DC_Para.Wakeup != RUNTIMEM_WAKEUP_SIGNAL_BIT_NONE &&
            ChargeConnectM_ConfigInfo.DC_Para.Wakeup != RUNTIMEM_WAKEUP_SIGNAL_BIT_KEY_ON &&
            (ChargeConnectM_ConfigInfo.DC_Para.Wakeup & wakeup) != 0U)
        {
            flag = TRUE;
        }
    }
    if ((wakeup & RUNTIMEM_WAKEUP_SIGNAL_BIT_KEY_ON) == 0U)
    {
        flag = TRUE;
    }
    if (flag)
    {
        res = FALSE;
    }
#endif
    return res;
}

static boolean waitChgCommIsOkForPowerOn(void)
{
    boolean flag = FALSE, res = TRUE;

    if (ChargerComm_ConfigInfo.AC_Protocol != CHARGERCOMM_PROTOCOL_NONE)
    {
        if (ChargeConnectM_ConfigInfo.AC_Para.type == CHARGECONNECTM_CONNECT_COMMUNICATION &&
            ChargeConnectM_ConfigInfo.AC_Para.Wakeup != RUNTIMEM_WAKEUP_SIGNAL_BIT_NONE &&
            ChargeConnectM_ConfigInfo.AC_Para.Wakeup != RUNTIMEM_WAKEUP_SIGNAL_BIT_KEY_ON)
        {
            flag = TRUE;
        }
    }
    if (ChargerComm_ConfigInfo.DC_Protocol != CHARGERCOMM_PROTOCOL_NONE)
    {
        if (ChargeConnectM_ConfigInfo.DC_Para.type == CHARGECONNECTM_CONNECT_COMMUNICATION &&
            ChargeConnectM_ConfigInfo.DC_Para.Wakeup != RUNTIMEM_WAKEUP_SIGNAL_BIT_NONE &&
            ChargeConnectM_ConfigInfo.DC_Para.Wakeup != RUNTIMEM_WAKEUP_SIGNAL_BIT_KEY_ON)
        {
            flag = TRUE;
        }
    }
    if (flag)
    {
        if (OSTimeGet() < 1500U)
        {
            res = FALSE;
        }
    }
    return res;
}

boolean HvProcess_DchgStateStartCond(void)
{
    boolean res = FALSE;
    Std_ReturnType allow;
    uint32 nowtime = OSTimeGet();
    HvProcess_ChgStateType chgState;

    if (WakeupSignalIsOk())
    {
        chgState = HvProcess_GetChgState();
        if ((!CHARGECONNECTM_IS_CONNECT()) && chgState == HVPROCESS_CHG_START && nowtime >= 300U)
        {
            if (!HvProcess_DchgInnerData.RelayAdhesCheckFlag)
            {
                HvProcess_DchgInnerData.RelayAdhesCheckFlag = TRUE;
                UserStrategy_DchgHvProcessAdhesiveDetect();
            }
            else
            {
                if (waitChgCommIsOkForPowerOn())
                {
                    allow = DischargeM_DischargeIsAllowed();
                    if (UserStrategy_DchgHvProcessRelayIsNormal() && allow == E_OK)
                    {
                        res = TRUE;
                    }
                }
            }
        }
        else
        {
            HvProcess_DchgInnerData.RelayAdhesCheckFlag = FALSE;
        }
    }
    return res;
}

void HvProcess_DchgStateStartAction(void)
{
#ifdef RELAYM_FN_NEGATIVE_MAIN
    (void)RelayM_Control(RELAYM_FN_NEGATIVE_MAIN, RELAYM_CONTROL_ON);
#endif
#ifdef RELAYM_FN_PRECHARGE
    PrechargeM_Start();
#endif
}

boolean HvProcess_DchgStatePrechargeCond(void)
{
    boolean res = FALSE;

#ifdef RELAYM_FN_PRECHARGE
    if (PrechargeM_IsFinish() == TRUE)
    {
        res = TRUE;
    }
#else
    res = TRUE;
#endif
    return res;
}

void HvProcess_DchgStatePrechargeAction(void)
{
    (void)RelayM_Control(RELAYM_FN_POSITIVE_MAIN, RELAYM_CONTROL_ON);
}

boolean HvProcess_DchgChargeConnectionCond(void)
{
    boolean res = FALSE;
    uint32 nowTime = OSTimeGet();
    static uint32 lastTime = 0UL,monitorTime = 0U;

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 500UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    if (!WakeupSignalIsOk())
    {
        if (lastTime == 0UL)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) >= 500U)
        {
            lastTime = 0UL;
            res = TRUE;
        }
    }
    else if (CHARGECONNECTM_IS_CONNECT())
    {
        lastTime = 0UL;
        res = TRUE;
    }
    else
    {
        lastTime = 0UL;
    }
    return res;
}

void HvProcess_DchgChargeConnectionAction(void)
{
    HvProcess_DchgInnerData.RelayOffTick = OSTimeGet();
#ifdef RELAYM_FN_PRECHARGE
    PrechargeM_Stop();
#endif
}

boolean HvProcess_DchgFaultCond(void)
{
    boolean res = FALSE;

    if (DischargeM_DischargeIsFault() == E_OK)
    {
        res = TRUE;
    }
    return res;
}

void HvProcess_DchgFaultAction(void)
{
    HvProcess_DchgInnerData.RelayOffTick = OSTimeGet();
#ifdef RELAYM_FN_PRECHARGE
    PrechargeM_Stop();
#endif
}

boolean HvProcess_DchgRelayOffDelayCond(void)
{
    boolean res = FALSE;
    uint32 nowTime = OSTimeGet();
    uint32 delay = 5000U;
    Current_CurrentType current;

    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    if (CurrentM_IsValidCurrent(current))
    {
        if (abs(current) <= CURRENT_S_100MA_FROM_A(5))
        {
            delay = 0U;
        }
    }
    if (MS_GET_INTERNAL(HvProcess_DchgInnerData.RelayOffTick, nowTime) >= delay)
    {
        res = TRUE;
        HvProcess_DchgInnerData.RelayOffTick = nowTime;
    }
    return res;
}

void HvProcess_DchgRelayOffDelayAction(void)
{
    (void)RelayM_Control(RELAYM_FN_POSITIVE_MAIN, RELAYM_CONTROL_OFF);
#ifdef RELAYM_FN_NEGATIVE_MAIN
    (void)RelayM_Control(RELAYM_FN_NEGATIVE_MAIN, RELAYM_CONTROL_OFF);
#endif
}

boolean HvProcess_DchgReStartJudgeCond(void)
{
    boolean res = FALSE;
    uint8 state = 0U;
    App_Tv100mvType bat_tv, hv1, hv2 = 0U, hv3 = 0U, temp;
    uint32 delay = 30000UL, nowTime = OSTimeGet();
    static uint32 lastTime = 0UL;

#if defined(A640)||defined(A641)||defined(A630)||defined(A635)
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

void HvProcess_DchgReStartJudgeAction(void)
{
    HvProcess_DchgInnerData.RelayAdhesCheckFlag = FALSE;
}
