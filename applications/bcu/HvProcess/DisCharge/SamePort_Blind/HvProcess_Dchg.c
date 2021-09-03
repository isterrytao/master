/**
 * \file HvProcess_Dchg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 高压放电流程控制文件.
 *
 * \note 充放电同口无预充无加热高压流程
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
#include "DischargeM_Cfg.h"
#include "ChargerComm_LCfg.h"
#include "BridgeInsu_Cfg.h"

static HvProcess_DchgInnerDataType HvProcess_DchgInnerData;
static boolean HvProcess_DchgIsFaultDirectRelayOff(void);
static boolean HvProcess_DchgIsFault17sOff(void);
static boolean HvProcess_DchgIsFault27sOff(void);

void HvProcess_DchgInit(Async_LooperType *looper)
{
    (void)looper;
    HvProcess_DchgInnerData.State = HVPROCESS_DCHG_START;
    HvProcess_DchgInnerData.chgFinishFlag = FALSE;
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

boolean HvProcess_DchgStateStartCond(void)
{
    boolean res = FALSE;
    uint32 delay = 5000U;
#if defined(UPA530)||defined(UPA630)||defined(UPA640)
    delay = 500U;
#else
#if BRIDGEINSU_TYPE == BRIDGEINSU_MOS_OFF
    delay = 500U;
#endif
#endif

    if (OSTimeGet() >= delay)
    {
        if (!HvProcess_DchgInnerData.RelayAdhesCheckFlag)
        {
            HvProcess_DchgInnerData.RelayAdhesCheckFlag = TRUE;
            UserStrategy_DchgHvProcessAdhesiveDetect();
        }
        else
        {
            if (UserStrategy_DchgHvProcessRelayIsNormal())
            {
                if (CHARGECONNECTM_IS_CONNECT())
                {
                    if (ChargeM_ChargeIsReady() == E_OK)
                    {
                        res = TRUE;
                    }
                }
                else
                {
                    if (DischargeM_DischargeIsAllowed() == E_OK)
                    {
                        res = TRUE;
                    }
                }
            }
        }
    }
    return res;
}

void HvProcess_DchgStateStartAction(void)
{
#ifdef RELAYM_FN_NEGATIVE_MAIN
    (void)RelayM_Control(RELAYM_FN_NEGATIVE_MAIN, RELAYM_CONTROL_ON);
#endif
    (void)RelayM_Control(RELAYM_FN_POSITIVE_MAIN, RELAYM_CONTROL_ON);
}

boolean HvProcess_DchgFaultCond(void)
{
    boolean res = FALSE;

    if (CHARGECONNECTM_IS_CONNECT())
    {
        if (ChargeM_ChargeIsReady() == E_OK)
        {
            if (ChargeM_ChargeIsFault() == E_OK)
            {
                res = TRUE;
                if (HvProcess_ChgIsFinishCond())
                {
                    HvProcess_DchgInnerData.chgFinishFlag = TRUE;
                }
            }
        }
        else if (ChargeM_ChargeIsAllowed() == E_NOT_OK)
        {
            res = TRUE;
        }
        else
        {}
    }
    else
    {
        UserStrategy_FullChargeReleaseHook();
        if (DischargeM_DischargeIsFault() == E_OK)
        {
            if (HvProcess_DchgIsFaultDirectRelayOff())
            {
                res = TRUE;
            }
            else if (HvProcess_DchgIsFault17sOff())
            {
                res = TRUE;
            }
            else if (HvProcess_DchgIsFault27sOff())
            {
                res = TRUE;
            }
            else
            {}
        }
    }
    return res;
}

void HvProcess_DchgFaultAction(void)
{
    HvProcess_DchgInnerData.RelayOffTick = OSTimeGet();
}

boolean HvProcess_DchgRelayOffDelayCond(void)
{
    // boolean res = FALSE;
    // uint32 nowTime = OSTimeGet();
    // uint32 delay = 5000U;
    // Current_CurrentType current;

    // current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    // if (CurrentM_IsValidCurrent(current))
    // {
    //     if (abs(current) <= CURRENT_S_100MA_FROM_A(5))
    //     {
    //         delay = 0U;
    //     }
    // }
    // if (MS_GET_INTERNAL(HvProcess_DchgInnerData.RelayOffTick, nowTime) >= delay)
    // {
    //     res = TRUE;
    //     HvProcess_DchgInnerData.RelayOffTick = nowTime;
    // }
    return TRUE;
}

void HvProcess_DchgRelayOffDelayAction(void)
{
    (void)RelayM_Control(RELAYM_FN_POSITIVE_MAIN, RELAYM_CONTROL_OFF);
#ifdef RELAYM_FN_NEGATIVE_MAIN
    (void)RelayM_Control(RELAYM_FN_NEGATIVE_MAIN, RELAYM_CONTROL_OFF);
#endif
    if (HvProcess_DchgInnerData.chgFinishFlag == TRUE)
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

boolean HvProcess_DchgReStartJudgeCond(void)
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

void HvProcess_DchgReStartJudgeAction(void)
{
    HvProcess_DchgInnerData.chgFinishFlag = FALSE;
    HvProcess_DchgInnerData.RelayAdhesCheckFlag = FALSE;
}

static boolean HvProcess_DchgIsFaultDirectRelayOff(void)
{
    boolean res = FALSE;
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV) == DIAGNOSIS_LEVEL_FOURTH)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) == DIAGNOSIS_LEVEL_FOURTH)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_DCHG_HTV) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_DCHG_HV) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_FB_OC) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_DCHG_HT) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_DCHG_LT) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_DCHG_DT) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_SP_OC) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_DCHG_OC) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_BMS_INIT_FAILURE) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_VOLT_LINE) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_TEMP_LINE) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_LEAK) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_RELAY_ABNORMAL) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetDiagnosisDchargeCtlAction(DIAGNOSIS_ITEM_CURRENT_ABNORMAL) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else if (DischargeM_GetOthersFaultDchargeCtl(DISCHARGEM_OTHERS_FAULT_OVER_DCHG) == DISCHARGEM_DISCHARGE_DISABLE)
    {
        res = TRUE;
    }
    else
    {
    }
    return res;
}

static boolean HvProcess_DchgIsFault17sOff(void)
{
    boolean res = FALSE;
    static uint32 lastTime = 0UL, monitorTime = 0U;
    uint32 nowTime = OSTimeGet();

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 500UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV) == DIAGNOSIS_LEVEL_THIRD)
    {
        res = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) == DIAGNOSIS_LEVEL_THIRD)
    {
        res = TRUE;
    }
    else
    {
    }
    if (res == TRUE)
    {
        if (lastTime == 0UL)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) < 17000UL)
        {
            res = FALSE;
        }
    }
    else
    {
        lastTime = 0UL;
    }
    return res;
}

static boolean HvProcess_DchgIsFault27sOff(void)
{
    boolean res = FALSE;
    static uint32 lastTime = 0UL, monitorTime = 0U;
    uint32 nowTime = OSTimeGet();

    if (MS_GET_INTERNAL(monitorTime, nowTime) > 500UL)
    {
        lastTime = 0UL;
    }
    monitorTime = nowTime;
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        res = TRUE;
    }
    if (res == TRUE)
    {
        if (lastTime == 0UL)
        {
            lastTime = nowTime;
        }
        if (MS_GET_INTERNAL(lastTime, nowTime) < 27000UL)
        {
            res = FALSE;
        }
    }
    else
    {
        lastTime = 0UL;
    }
    return res;
}
