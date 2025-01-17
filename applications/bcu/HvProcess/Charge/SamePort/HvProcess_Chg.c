/**
 * \file HvProcess_Chg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 高压充电流程控制文件.
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
#include "ChargerComm.h"
#include "RuntimeM.h"
#include "Stdlib.h"
#include "UserStrategy.h"
#include "Statistic.h"

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
    return FALSE;
}

void HvProcess_ChgStateStartAction(void)
{
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    (void)RelayM_Control(RELAYM_FN_POSITIVE_AC_CHARGE, RELAYM_CONTROL_ON);
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
    (void)RelayM_Control(RELAYM_FN_POSITIVE_DC_CHARGE, RELAYM_CONTROL_ON);
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
    uint32 delay = S_TO_MS(5U);
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
#ifdef RELAYM_FN_POSITIVE_AC_CHARGE
    (void)RelayM_Control(RELAYM_FN_POSITIVE_AC_CHARGE, RELAYM_CONTROL_OFF);
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
    (void)RelayM_Control(RELAYM_FN_POSITIVE_DC_CHARGE, RELAYM_CONTROL_OFF);
#endif
    if (HvProcess_ChgInnerData.chgFinishFlag == TRUE)
    {
        // RuntimeM_RequestPowerDown();
    }
}

boolean HvProcess_ChgReStartJudgeCond(void)
{
    boolean res = FALSE;
    App_Tv100mvType bat_tv, hv1, temp;
    uint32 delay = 30000UL, nowTime = OSTimeGet();
    static uint32 lastTime = 0UL;

    if (RelayMConfigData[RELAYM_FN_POSITIVE_MAIN].GetInstantVoltage != NULL)
    {
        bat_tv = HV_GetVoltage(HV_CHANNEL_BPOS);
        hv1 = HV_GetVoltage(HV_CHANNEL_HV1);
        if (Statistic_TotalVoltageIsValid(bat_tv))
        {
            if (Statistic_TotalVoltageIsValid(hv1))
            {
                 temp = (App_Tv100mvType)((uint32)bat_tv * (uint32)RelayMConfigData[RELAYM_FN_POSITIVE_MAIN].totalPercent / 100UL);
                 if (hv1 <= temp)  // 判断HV1是否低于粘连检测阈值
                 {
                     delay = 0U;
                 }
            }
        }
    }
    else
    {
        delay = 3000U;
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
