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
#include "ChgSckTmpM.h"

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
static const UserStrategy_BuzzerAlarmType alarmLevel1Cfg[] = {
    {DIAGNOSIS_ITEM_DCHG_LTV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_LV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_HTV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_HV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_DV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_HT, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_LT, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_DT, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_SP_OC, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_DCHG_OC, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_LSOC, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_CHG_LTV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_CHG_LV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_CHG_DV, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_CHG_HT, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_CHG_DT, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_AC_CHG_OC, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_VOLT_LINE, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_TEMP_LINE, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_SUPPLY_VOL_LOW, DIAGNOSIS_LEVEL_FIRST},
    {DIAGNOSIS_ITEM_LEAK, DIAGNOSIS_LEVEL_FIRST},
};
static const UserStrategy_BuzzerAlarmType alarmLevel2Cfg[] = {
    {DIAGNOSIS_ITEM_DCHG_LTV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_LV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_HTV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_HV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_DV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_HT, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_LT, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_DT, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_SP_OC, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_DCHG_OC, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CHG_HTV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CHG_HV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CHG_LTV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CHG_LV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CHG_DV, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CHG_HT, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CHG_LT, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CHG_DT, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_AC_CHG_OC, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_BMS_INIT_FAILURE, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_VOLT_LINE, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_TEMP_LINE, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_RELAY_ABNORMAL, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_LSOC, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_CURRENT_ABNORMAL, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_SUPPLY_VOL_LOW, DIAGNOSIS_LEVEL_SECOND},
    {DIAGNOSIS_ITEM_LEAK, DIAGNOSIS_LEVEL_SECOND},
};
#endif

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

#if defined(A640)||defined(A641)
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
        RuntimeM_RequestPowerDown();
    }
    else
    {}
}
#endif
#endif

static Async_EvnetCbkReturnType UserStrategy_Poll(Async_EventType *event, uint8 byWhat)
{
    (void)event;
    (void)byWhat;
#if USERSTRATEGY_RESET_TO_OTA_EN == STD_ON
    UserStrategy_ResetToOTA();
#endif

#if defined(A640)||defined(A641)
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

#if USERSTRATEGY_LV_POWER_DOWN_EN == STD_ON
    UserStrategy_LvPowerDown();
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
    boolean res = FALSE;

    if (Diagnosis_StartDiagIsFinish())
    {
        res = TRUE;
    }
    return res;
}

App_Tv100mvType UserStrategy_GetChargeVoltMax(void)
{
    App_Tv100mvType volt;
    Charge_ChargeType type = ChargeConnectM_GetConnectType();

    volt = PowerM_GetChargeVoltage(type);
#ifdef RELAYM_FN_HEATER
    if (HvProcess_ChargerIsHeadMode() == TRUE)
    {
        if (HvProcess_HeatIsJump() == TRUE)
        {
            volt = Statistic_GetBcu100mvTotalVoltage();
            if (Statistic_TotalVoltageIsValid(volt))
            {
                volt += V_TO_100MV(5U);
            }
        }
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
#ifdef RELAYM_FN_HEATER
    if (HvProcess_ChargerIsHeadMode() == TRUE)
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
    }
    return res;
}

App_Tv100mvType UserStrategy_GetMcuVoltage(void)
{
    return HV_GetVoltage(GETMCUVOLTAGE_CHANNEL);
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
    SocDiagCalib_FullCalibRelCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_DEASSERT);
    ChargeM_DiagnosisCtlEnableEventCbk(DIAGNOSIS_ITEM_FULL_CHARGE, DIAGNOSIS_LEVEL_FIRST, DIAGNOSIS_EVENT_DEASSERT);
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
        current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
        if (CurrentM_IsValidCurrent(current))
        {
            if (abs(current) < CURRENT_S_100MA_FROM_A(5))
            {
                flag = TRUE;
                if (MS_GET_INTERNAL(lastTime, nowTime) >= 2000U)
                {
                    (void)RuntimeM_RequestToDtuMode();
                }
            }
        }
    }
    if (!flag)
    {
        lastTime = nowTime;
    }
}

#endif

boolean UserStrategy_Wakeup(void){
#if (KEY_TYPE == KEY_TYPE_IS_SELFRESET) && (defined(A640)||defined(A641))
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

boolean UserStartegy_AcChargeRelayIsReady(void)
{
    boolean res = FALSE;
    RelayM_FunctionType relayType = ChargerComm_ConfigInfo.AC_RelayType;

    if (relayType == RELAYM_FN_NONE)
    {
        res = TRUE;
    }
    else if (RelayM_FunctionRelayIsEnable(relayType) == E_NOT_OK)
    {
        if (OSTimeGet() >= 1000U)
        {
            res = TRUE;
        }
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

#ifdef RELAYM_FN_SELF_LOCK
static void UserStrategy_SelfLockRelayControl(void)
{
#if USERSTRATEGY_AC_CHG_SELF_LOCK_EN == STD_ON || USERSTRATEGY_DC_CHG_SELF_LOCK_EN == STD_ON || USERSTRATEGY_DCHG_SELF_LOCK_EN == STD_ON
    static boolean is_on = FALSE;
#endif
    Charge_ChargeType type = ChargeConnectM_GetConnectType();
    if (OSTimeGet() >= 300U)
    {
        if (type == CHARGE_TYPE_AC)
        {
#if USERSTRATEGY_AC_CHG_SELF_LOCK_EN == STD_ON
            if (!is_on)
            {
                is_on = TRUE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_ON);
            }
#elif !defined(A640) && !defined(A641)
            if (Dio_ReadChannel(DIO_CHANNEL_OBC_POWER) == STD_LOW)
            {
                is_on = FALSE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
            }
            else if (Dio_ReadChannel(DIO_CHANNEL_KEY_ON) == STD_LOW)
            {
                is_on = FALSE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
            }
            else
            {}
#else
            if (Dio_ReadChannel(DIO_CHANNEL_KEY_ON) == STD_LOW)
            {
                is_on = FALSE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
            }
#endif
        }
        else if (type == CHARGE_TYPE_DC)
        {
#if USERSTRATEGY_DC_CHG_SELF_LOCK_EN == STD_ON
            if (!is_on)
            {
                is_on = TRUE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_ON);
            }
#elif !defined(A640) && !defined(A641)
            if (Dio_ReadChannel(DIO_CHANNEL_CHARGER_READY) == STD_LOW)
            {
                is_on = FALSE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
            }
            else if (Dio_ReadChannel(DIO_CHANNEL_KEY_ON) == STD_LOW)
            {
                is_on = FALSE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
            }
            else
            {}
#else
            if (Dio_ReadChannel(DIO_CHANNEL_KEY_ON) == STD_LOW)
            {
                is_on = FALSE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
            }
#endif
        }
        else
        {
#if USERSTRATEGY_DCHG_SELF_LOCK_EN == STD_ON
            if (!is_on)
            {
                is_on = TRUE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_ON);
            }
#else
            if (Dio_ReadChannel(DIO_CHANNEL_KEY_ON) == STD_LOW)
            {
                is_on = FALSE;
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
            }
#endif
        }
    }
}
#endif

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

    if (nowTime >= 10000U)
    {
        if (USERSTRATEGY_IS_MANUAL_POWER_DOWN())
        {
            if (MS_GET_INTERNAL(lastTime, nowTime) >= USERSTRATEGY_MANUAL_POWER_DOWN_DELAY)
            {
                is_manual = TRUE;
                lastTime = nowTime;
                ChargeM_SetOthersFaultChargeCtl(CHARGEM_OTHERS_FAULT_POWER_OFF, CHARGEM_CHARGE_DISABLE);
                DischargeM_SetOthersFaultDchargeCtl(DISCHARGEM_OTHERS_FAULT_POWER_OFF, DISCHARGEM_DISCHARGE_DISABLE);
            }
        }
        else
        {
            if (is_manual)
            {
#ifdef RELAYM_FN_SELF_LOCK
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
                RuntimeM_RequestPowerDown();
#else
                RuntimeM_RequestPowerDown();
#endif
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

    mode = RuntimeM_GetMode();
    if (mode == RUNTIMEM_RUNMODE_CALIBRATE || mode == RUNTIMEM_RUNMODE_NORMAL)
    {
        current = abs(CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN));
        if (CurrentM_IsValidCurrent(current) && current <= CURRENT_S_100MA_FROM_A(USERSTRATEGY_AUTO_POWER_DOWN_CURRENT))
        {
            if (MS_GET_INTERNAL(lastTime, nowTime) >= delay)
            {
                lastTime = nowTime;
                ChargeM_SetOthersFaultChargeCtl(CHARGEM_OTHERS_FAULT_POWER_OFF, CHARGEM_CHARGE_DISABLE);
                DischargeM_SetOthersFaultDchargeCtl(DISCHARGEM_OTHERS_FAULT_POWER_OFF, DISCHARGEM_DISCHARGE_DISABLE);
#ifdef RELAYM_FN_SELF_LOCK
                (void)RelayM_Control(RELAYM_FN_SELF_LOCK, RELAYM_CONTROL_OFF);
                RuntimeM_RequestPowerDown();
#else
                RuntimeM_RequestPowerDown();
#endif
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
    for (i = 0U; i < (uint8)ARRAY_SIZE(alarmLevel2Cfg); i ++)
    {
        if (Diagnosis_GetLevel(alarmLevel2Cfg[i].diagItem) >= alarmLevel2Cfg[i].level)
        {
            AlarmLevel = 2U;
            break;
        }
    }
    if (AlarmLevel == 0U)
    {
        for (i = 0U; i < (uint8)ARRAY_SIZE(alarmLevel1Cfg); i ++)
        {
            if (Diagnosis_GetLevel(alarmLevel1Cfg[i].diagItem) >= alarmLevel1Cfg[i].level)
            {
                AlarmLevel = 1U;
                break;
            }
        }
    }
    if (AlarmLevel != 0U)
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
        if (RelayM_GetControlStatus(RELAYM_FN_POSITIVE_AC_CHARGE) == RELAYM_CONTROL_OFF)
        {
            if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_AC_CHARGE) == RELAYM_ACTUAL_OFF)
            {
                (void)RelayM_StartAdhesiveDetect(RELAYM_FN_POSITIVE_AC_CHARGE, NULL);
            }
        }
    }
#endif
#ifdef RELAYM_FN_POSITIVE_DC_CHARGE
    if (RelayMConfigData[RELAYM_FN_POSITIVE_DC_CHARGE].GetInstantVoltage != NULL)
    {
        if (RelayM_GetControlStatus(RELAYM_FN_POSITIVE_DC_CHARGE) == RELAYM_CONTROL_OFF)
        {
            if (RelayM_GetActualStatus(RELAYM_FN_POSITIVE_DC_CHARGE) == RELAYM_ACTUAL_OFF)
            {
                (void)RelayM_StartAdhesiveDetect(RELAYM_FN_POSITIVE_DC_CHARGE, NULL);
            }
        }
    }
#endif
#ifdef RELAYM_FN_CHARGE
    if (RelayMConfigData[RELAYM_FN_CHARGE].GetInstantVoltage != NULL)
    {
        if (RelayM_GetControlStatus(RELAYM_FN_CHARGE) == RELAYM_CONTROL_OFF)
        {
            if (RelayM_GetActualStatus(RELAYM_FN_CHARGE) == RELAYM_ACTUAL_OFF)
            {
                (void)RelayM_StartAdhesiveDetect(RELAYM_FN_CHARGE, NULL);
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

#if USERSTRATEGY_LV_POWER_DOWN_EN == STD_ON
static void UserStrategy_LvPowerDown(void)
{
    uint16 lv = Statistic_GetBcuLvMax();
    uint32 nowTime = OSTimeGet();
    uint32 delay = USERSTRATEGY_LV_POWER_DOWN_TIME;
    static uint32 lastTime = 0U;

    if (!CHARGECONNECTM_IS_CONNECT())
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
