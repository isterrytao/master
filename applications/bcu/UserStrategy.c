/**
 * \file UserStrategy.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 用户策略定义文件.
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
#include "ChargeM_Cfg.h"
#include "DischargeM.h"
#include "DischargeM_Cfg.h"
#include "RelayM_Lcfg.h"

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
static void UserStrategy_Beep(void);
static void UserStrategy_AutoLostPower(void);
static void UserStrategy_ResetToOTA(void);

void UserStrategy_Init(Async_LooperType *looper)
{
    UserStrategy_innerData.currentIsAllowToPowerOff = TRUE;
    if (looper != NULL)
    {
        UserCurrentSensor_Init(looper);
        if (E_OK == Async_EventInit(&UserStrategy_innerData.event, looper, UserStrategy_Poll, 100UL))
        {
            (void)Async_EventRegisterToLooper(&UserStrategy_innerData.event);
        }
    }
}

static Async_EvnetCbkReturnType UserStrategy_Poll(Async_EventType *event, uint8 byWhat)
{
    (void)event;
    (void)byWhat;

    safeCurrentCheck();
    UserStrategy_Beep();
    UserStrategy_AutoLostPower();
    UserStrategy_ResetToOTA();

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
    else if ((uint16)abs(current) < CURRENT_100MA_FROM_A(3U)) {
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
    return TRUE;
}

App_Tv100mvType UserStrategy_GetChargeVoltMax(void)
{
    App_Tv100mvType volt;
    Charge_ChargeType type = ChargeConnectM_GetConnectType();

    volt = PowerM_GetChargeVoltage(type);

    return volt;
}

Current_CurrentType UserStrategy_GetChargeCurrentMax(void)
{
    Current_CurrentType current;
    uint32 nowTime;
    static uint8 state = 0U;
    static uint32 lastTime = 0UL;
    Charge_ChargeType type = ChargeConnectM_GetConnectType();

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
        nowTime = OSTimeGet();
        current = PowerM_GetCurrent(POWERM_CUR_CHARGE_AC);
        if (current == CURRENT_S_100MA_FROM_A(150))
        {
            if (state == 0U)
            {
                if (MS_GET_INTERNAL(lastTime, nowTime) >= 10UL * 60UL * 1000UL)
                {
                    state = 1U;
                    lastTime = nowTime;
                }
            }
            else
            {
                current = CURRENT_S_100MA_FROM_A(100);
                if (MS_GET_INTERNAL(lastTime, nowTime) >= 5UL * 60UL * 1000UL)
                {
                    state = 0U;
                    lastTime = nowTime;
                }
            }
        }
        else
        {
            lastTime = nowTime;
        }
    }
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
    else
    {
    }
    return res;
}

App_Tv100mvType UserStrategy_GetMcuVoltage(void)
{
    return HV_GetVoltage(HV_CHANNEL_HV1);
}

uint16 UserStrategy_IsDiagHvilAbnormal(void)
{
    return 0U;
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
}

boolean UserStrategy_SaftyOff(RuntimeM_SignalType signal) {
    (void) signal;
    return UserStrategy_IsSafeToOff();
}

/**< 建议由连接到未连接变化时增加适当延时，保证连接状态不会突变而导致异常 */
Std_ReturnType UserStrategy_ACConnected(void)
{
    return E_NOT_OK;
}

/**< 建议由连接到未连接变化时增加适当延时，保证连接状态不会突变而导致异常 */
Std_ReturnType UserStrategy_DCConnected(void)
{
    return E_NOT_OK;
}

uint16 UserStrategy_GetCrashFault(void)
{
    return 0U;
}

void UserStrategy_FullChargeHook(void)
{
    SocDiagCalib_FullCalibCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_ASSERT);
    ChargeM_DiagnosisCtlDisableEventCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_ASSERT);
}

void UserStrategy_FullChargeReleaseHook(void)
{
    SocDiagCalib_FullCalibCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_NONE, DIAGNOSIS_EVENT_DEASSERT);
    ChargeM_DiagnosisCtlEnableEventCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_NONE, DIAGNOSIS_EVENT_DEASSERT);
}

static void UserStrategy_Beep(void)
{
    boolean is_alarm = FALSE, limit = FALSE;
    uint32 nowTime;
    static boolean is_on = FALSE, limit_is_start = FALSE;
    static uint32 lastTime = 0UL, limitLastTime = 0UL;

    nowTime = OSTimeGet();
    if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_DV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_HT) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_HT) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LT) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_LT) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_CHG_DT) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_LTV) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_INTER_COMM) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_VOLT_LINE) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_TEMP_LINE) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_DCHG_OC) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_RELAY_ABNORMAL) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if (Diagnosis_GetLevel(DIAGNOSIS_ITEM_LEAK) >= DIAGNOSIS_LEVEL_THIRD)
    {
        is_alarm = TRUE;
    }
    else if((Diagnosis_GetLevel(DIAGNOSIS_ITEM_LSOC) > DIAGNOSIS_LEVEL_FIRST))
    {
        is_alarm = TRUE;
    }
    else if((Diagnosis_GetLevel(DIAGNOSIS_ITEM_LSOC) == DIAGNOSIS_LEVEL_FIRST))
    {
        limit = TRUE;
    }
    else
    {
    }

    if (limit)
    {
        if (!limit_is_start)
        {
            limit_is_start = TRUE;
            limitLastTime = nowTime;
        }
        if (limit_is_start)
        {
            if (MS_GET_INTERNAL(limitLastTime, nowTime) <= 15000UL)
            {
                is_alarm = TRUE;
            }
        }
    }
    else
    {
        limit_is_start = FALSE;
    }

    if (is_on)
    {
        if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000UL || !is_alarm)
        {
            is_on = FALSE;
            lastTime = nowTime;
            (void)RelayM_Control(RELAYM_FN_BEEP, RELAYM_CONTROL_OFF);
        }
    }
    else
    {
        if (is_alarm)
        {
            if (MS_GET_INTERNAL(lastTime, nowTime) >= 1000UL)
            {
                is_on = TRUE;
                lastTime = nowTime;
                (void)RelayM_Control(RELAYM_FN_BEEP, RELAYM_CONTROL_ON);
            }
        }
        else
        {
            lastTime = nowTime;
        }
    }
}

static void UserStrategy_AutoLostPower(void)
{
    sint16 current;
    uint32 nowTime;
    static uint32 preTime = 0UL;
    current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    nowTime = OSTimeGet();

    if (CurrentM_IsValidCurrent(current))
    {
        if (current < CURRENT_S_100MA_FROM_A(3) && current > CURRENT_S_100MA_FROM_A(-3))
        {
            if (MS_GET_INTERNAL(preTime, nowTime) >= 2UL * 60UL * 60UL * 1000UL)
            {
                DischargeM_SetOthersFaultDchargeCtl(DISCHARGEM_OTHERS_FAULT_SELF_LOCK, DISCHARGEM_DISCHARGE_DISABLE);
                ChargeM_SetOthersFaultChargeCtl(CHARGEM_OTHERS_FAULT_SELF_LOCK, CHARGEM_CHARGE_DISABLE);
            }
        }
        else
        {
            preTime = nowTime;
        }
    }
    else
    {
        preTime = nowTime;
    }
}

static void UserStrategy_ResetToOTA(void)
{
    Current_CurrentType current;

    if (RuntimeM_HasOtaRequest())
    {
        current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
        if (CurrentM_IsValidCurrent(current))
        {
            if (abs(current) < CURRENT_S_100MA_FROM_A(5))
            {
                (void)RuntimeM_RequestToDtuMode();
            }
        }
    }
}

