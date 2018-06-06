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
#include <stdlib.h>
#include "Cpu.h"
#include "HvProcess_Dchg.h"
#include "HvProcess_Chg.h"
#include "ChargeConnectM.h"
#include "ChargeM.h"
#include "DischargeM.h"
#include "RelayM.h"
#include "RelayM_Lcfg.h"
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

boolean HvProcess_DchgStateStartCond(void)
{
    boolean res = FALSE;
    uint32 nowTime = OSTimeGet();
    Dio_LevelType powerOn = Dio_ReadChannel(DIO_CHANNEL_KEY_ON);

    if (!HvProcess_DchgInnerData.RelayFaultCheckFlag && nowTime >= 300UL)
    {
        HvProcess_DchgInnerData.RelayFaultCheckFlag = TRUE;
        (void)RelayM_StartAdhesiveDetect(RELAYM_FN_POSITIVE_MAIN, NULL);
    }
    if (RELAYM_DIAGNOSIS_IS_NORMAL(RelayM_GetDiagnosisStatus(RELAYM_FN_POSITIVE_MAIN)) &&
        HvProcess_DchgInnerData.RelayFaultCheckFlag&&
        powerOn == STD_LOW)
    {
        if (CHARGECONNECTM_IS_CONNECT())
        {
            if (ChargeM_ChargeIsAllowed() == E_OK)
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
    return res;
}

void HvProcess_DchgStateStartAction(void)
{
    (void)RelayM_Control(RELAYM_FN_POSITIVE_MAIN, RELAYM_CONTROL_ON);
}

boolean HvProcess_DchgConnectionCond(void)
{
    boolean res = FALSE;

    if (!CHARGECONNECTM_IS_CONNECT())
    {
        res = TRUE;
    }
    return res;
}

void HvProcess_DchgConnectionAction(void)
{
    // HvProcess_DchgInnerData.RelayOffTick = OSTimeGet();
}

boolean HvProcess_DchgFaultCond(void)
{
    boolean res = FALSE;
    Dio_LevelType powerOn = Dio_ReadChannel(DIO_CHANNEL_KEY_ON);

    if (CHARGECONNECTM_IS_CONNECT())
    {
        if (ChargeM_ChargeIsFault() == E_OK ||
            powerOn == STD_HIGH)
        {
            if (HvProcess_ChgIsFinishCond())
            {
                HvProcess_ChgFinishAction();
            }
            res = TRUE;
        }
    }
    else
    {
        if (DischargeM_DischargeIsFault() == E_OK ||
            powerOn == STD_HIGH)
        {
            res = TRUE;
        }
    }
    return res;
}

void HvProcess_DchgFaultAction(void)
{
    HvProcess_DchgInnerData.RelayOffTick = OSTimeGet();
}

boolean HvProcess_DchgChrIsStopCond(void)
{
    boolean res = FALSE;
    Current_CurrentType current;

    if (CHARGECONNECTM_IS_CONNECT())
    {
        current = CurrentM_GetCurrent(CURRENTM_CHANNEL_MAIN);
        current = abs(current);
        if (current < CURRENT_S_100MA_FROM_A(1))
        {
            res = TRUE;
        }
    }
    return res;
}

boolean HvProcess_DchgChgOhvCond(void)
{
    boolean res = FALSE;

    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HV) == DIAGNOSIS_LEVEL_FOURTH)
    {
        res = TRUE;
    }
    return res;
}

boolean HvProcess_DchgRelayOffDelayCond(void)
{
    boolean res = FALSE;
    uint32 delay = 5000UL, nowTime = OSTimeGet();
    Current_CurrentType current;

    if (CHARGECONNECTM_IS_CONNECT())
    {
        current = CurrentM_GetCurrent(CURRENTM_CHANNEL_MAIN);
        current = abs(current);
        if (current > CURRENT_S_100MA_FROM_A(0))
        {
            if (current >= CURRENT_S_100MA_FROM_A(20))
            {
                delay = 5000UL;
            }
            else
            {
                delay = 20000UL;
            }
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
}

//HVPROCESS_DCHG_ALLOW_RESTART_CHECK
boolean HvProcess_DchgIsAllowRestartCond(void)
{
    boolean res = FALSE;
    static uint32 lastTime = 0UL;
    uint32 nowTime = OSTimeGet();

    if (lastTime == 0UL)
    {
        lastTime = nowTime;
    }
    if (MS_GET_INTERNAL(lastTime, nowTime) >= 3000UL)
    {
        lastTime = 0UL;
        if (CHARGECONNECTM_IS_CONNECT())
        {
            if (ChargeM_ChargeIsAllowed() == E_OK)
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
    return res;
}

void HvProcess_DchgIsAllowRestartAction(void)
{
    HvProcess_DchgInnerData.RelayFaultCheckFlag = FALSE;
}
