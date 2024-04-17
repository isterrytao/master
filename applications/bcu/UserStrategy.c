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

boolean user_DchgTmsTempEnableCond(void)
{
    boolean res = FALSE;
    // Std_ReturnType allowDischarge = DischargeM_DischargeIsAllowed();
    boolean isConn = CHARGECONNECTM_IS_CONNECT();
    uint8 tmslevel = VcuComm_TmsLevelGetCond();
    uint16 soc = Soc_Get();
    uint16 lvmax = Statistic_GetBcuLvMax();
    boolean isPowerOn = VcuComm_isPowerOn();

    if (/* TmsIswork && */ !isConn &&
        User_IsAllowTmsDchgHvOn() &&
        tmslevel != 1U &&
        CellDataM_VoltageIsValid(lvmax) && lvmax >= 3000U &&
        isPowerOn)
    {
        // if (UserStrategy_GetTmsStage() == 0U)
        {
            if (soc > 200U)
            {
                res = TRUE;
            }
        }
    }
    return res;
}

/*< 制冷模式启动温度条件*/
boolean UserStrategy_CoolModeCond(void)
{
    boolean res = FALSE;
    uint8 TmsOutlettemp = VcuComm_GetTmsOutlettemp();
    boolean TmsOutlettempIsvalid = CellDataM_TemperatureIsValid((uint16)TmsOutlettemp);
    uint8 startTemp;
    if (CHARGECONNECTM_IS_CONNECT())
    {
        startTemp = (uint8)TEMPERATURE_FROM_C(10);
    }
    else
    {
        startTemp = (uint8)TEMPERATURE_FROM_C(15);
    }

    if (TmsOutlettempIsvalid && TmsOutlettemp >= startTemp)
    {
        res = TRUE;
    }
    return res;
}
/*< 制热模式启动温度条件 30度*/
boolean UserStrategy_HeatModeCond(void)
{
    boolean res = FALSE;
    uint8 TmsOutlettemp = VcuComm_GetTmsOutlettemp();
    boolean TmsOutlettempIsvalid = CellDataM_TemperatureIsValid((uint16)TmsOutlettemp);
    uint8 startTemp = (uint8)TEMPERATURE_FROM_C(30);

    if (TmsOutlettempIsvalid && TmsOutlettemp <= startTemp)
    {
        res = TRUE;
    }
    return res;
}

/*< 制热模式自循环启动温度条件 30<T<35*/
boolean UserStrategy_HeatSelfLoopModeCond(void)
{
    boolean res = FALSE;
    uint8 TmsOutlettemp = VcuComm_GetTmsOutlettemp();
    boolean TmsOutlettempIsvalid = CellDataM_TemperatureIsValid((uint16)TmsOutlettemp);
    uint8 startTemp = (uint8)TEMPERATURE_FROM_C(30);
    uint8 stopTemp = (uint8)TEMPERATURE_FROM_C(35);

    if (TmsOutlettempIsvalid && TmsOutlettemp > startTemp && TmsOutlettemp < stopTemp)
    {
        res = TRUE;
    }
    return res;
}

/*< 自循环模式启动温度条件*/
boolean UserStrategy_SelfLoopModeCond(void)
{
    boolean res = FALSE;
    uint8 TmsOutlettemp = VcuComm_GetTmsOutlettemp();
    boolean TmsOutlettempIsvalid = CellDataM_TemperatureIsValid((uint16)TmsOutlettemp);
    uint8 startTemp, stopTemp;
    if (!CHARGECONNECTM_IS_CONNECT())
    {
        startTemp = (uint8)TEMPERATURE_FROM_C(12);
        stopTemp = (uint8)TEMPERATURE_FROM_C(15);
    }
    else
    {
        startTemp = (uint8)TEMPERATURE_FROM_C(7);
        stopTemp = (uint8)TEMPERATURE_FROM_C(10);
    }

    if (TmsOutlettempIsvalid &&
        TmsOutlettemp > startTemp && TmsOutlettemp < stopTemp)
    {
        res = TRUE;
    }
    return res;
}

// uint8 UserStrategy_GetTmsStage(void)
// {
//     return UserStrategy_innerData.coolStep;
// }

/*< 冷却开启条件判断，不区分充放电*/
static boolean UserStrategy_ColdStartCond(void)
{
    boolean res = FALSE;
    uint16 htMax = Statistic_GetBcuHtMax();
    uint16 Tmean;

    if (CellDataM_VoltageIsValid(htMax) && htMax >= TEMPERATURE_FROM_C(30))
    {
        Tmean = Statistic_GetBcuAverageTemperature();
        if (CellDataM_VoltageIsValid(Tmean) && Tmean >= TEMPERATURE_FROM_C(26))
        {
            res = TRUE;
        }
    }
    return res;
}

/*< 冷却关闭条件判断，不区分充放电*/
static boolean UserStrategy_ColdStopCond(void)
{
    boolean res = FALSE;
    uint16 htMax = Statistic_GetBcuHtMax();
    uint16 Tmean = Statistic_GetBcuAverageTemperature();

    if (CellDataM_VoltageIsValid(htMax) && htMax <= TEMPERATURE_FROM_C(26))
    {
        res = TRUE;
    }
    else if (CellDataM_VoltageIsValid(Tmean) && Tmean <= TEMPERATURE_FROM_C(24))
    {
        res = TRUE;
    }
    else
    {}
    return res;
}
/*< 加热开启条件判断，不区分充放电*/
/*

*/
boolean UserStrategy_HeatStartCond(void)
{
    boolean res = FALSE;
    uint16 htMax = Statistic_GetBcuHtMax();
    uint16 Tmean;

    if (CellDataM_VoltageIsValid(htMax) && htMax <= TEMPERATURE_FROM_C(12))
    {
        Tmean = Statistic_GetBcuAverageTemperature();
        if (CellDataM_VoltageIsValid(Tmean) && Tmean <= TEMPERATURE_FROM_C(8))
        {
            res = TRUE;
        }
    }
/*
            SZ-Penthouse
            Ar: two layers(25.26) 685² One elevator one household
            Ro: 5 living 2 study 5 bath 1innerelevator
            Co: 1. 12m Balcony  1kitchen 1Diningroom 3rom 1managerrom
                2. 1Terrace 2rom 1mediarom 1eSportsstudy 1study
                3. 1no borderswimmingpoll 1saunarom 1barbecue 1Sunlightro 1SalttAlternator
*/
    return res;
}

#if ( KEY_TYPE_IS_SELFRESET == STD_ON )
static void UserStrategy_KL15PowerDown(void)
{
    static uint32 lastTime = 0U, lastTime1 = 0U;
    uint32 nowTime = OSTimeGet();
    Dio_LevelType onState = Dio_ReadChannel(DIO_CHANNEL_KEY_ON);
    RuntimeM_RunModeType mode = RuntimeM_GetMode();
    uint32 delay = 1000UL;
/*
            Hf-Penthouse
            Ar: two layers(25.26) 585² Two elevator one household
            Ro: 2 Living 1 Study 4 Bath 1Innerelevator
            Co: 1. 2Livingrom 12m Balcony 1Thedrawingro 1Diningroom 1eSportsRoom 1Kitchen 1Managerrom
                2. 2Livingrom 1OfficeStudy 1Sunlightro 1Terrace&barbecue 1SalttAlternator
*/
    Current_CurrentType current = CurrentM_GetCurrentCalibrated(CURRENTM_CHANNEL_MAIN);
    if (OSTimeGet() > 10000U)
    {
        if (mode == RUNTIMEM_RUNMODE_CALIBRATE || mode == RUNTIMEM_RUNMODE_NORMAL)
        {
            if (onState == STD_LOW)
            {
                if (lastTime == 0UL)
                    lastTime = nowTime;
                if (MS_GET_INTERNAL(lastTime, nowTime) > 3000UL)
                {
                    UserStrategy_innerData.KL15Down = TRUE;
                }
/*
            Ny-Villa
            Ar: 2layers 3685²
            Ro: 3 Livingrom 1Study 4 bath 2Elevator 1SalttAlternator
            Co: 1. 1Noborderswimmingpoll 1Garage 1Barbecue 1Cinema 1Mediarom
                1Livingrom 1Thedrawingro 1Diningroom 1kitchen  1Study 1Managerrom
                2. 2Livingrom 1Terrace 1Balcony
*/
            }
            else
            {
                lastTime = 0UL;
            }
        }
        else
        {
            lastTime = 0UL;
        }
    }
    else
    {
        lastTime = 0UL;
    }
    // if (UserStrategy_innerData.KL15Down)
    // {
    //     if(CurrentM_IsValidCurrent(current))
    //     {
    //         if((uint16)abs(current) > CURRENT_100MA_FROM_A(5U))
    //         {
    //             delay = 5000UL;
    //         }
    //     }
    //     if (lastTime1 == 0UL)
    //     {
    //         lastTime1 = nowTime;
    //     }
}
#endif
boolean UserStartegy_GBChargeRelayIsReady(RelayM_FunctionType relayType)
{
    boolean res = FALSE;
/*
            Career
            : HI.173-183 Weight.83kg Phy.60min
            : Strength.NFL-self-adaption
            : A-2324-SG 30.1 score 6.9 assist 1.3 tackle
            : A-2425-SG 35.1 score 4.9 assist 1.4 tackle
            : A-2526-SG 41.1 score 3.9 assist 1.3 tackle
            : A-2627-SG 38.1 score 9.9 assist 3.2 tackle
            : Fiba: SG 38.1 score 11.1 assist 2.1 tackle
*/
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
            4996124000.21
            count: 0.17T     3.1     101.9   789.2   1481.8  2202.2  2849.3
            23.07.01 - 23.12.31 self count:794w, comm count:1000w
            (loan fees: 50w + 80 = 130w
            1789 - 130 - 450 - 1000 - 200 = 9)

            modle3:55
            Driver+proteceter:30*2
            load: 130
            E: 80W
            Currelan_RR_V12 : 1000W
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
        /*
            Trip Plan:
            Location:: 01.03*
            Borndest:: 01.03*
            Malaisia:: 03.04
            France:  : 04.05
            Shanghai:: 05.06*
            Hawaii:  : 06.07
            Canberra:: 07.08
            Chile:   : 08.09
            Beijing: : 09.10*
            Dubai  : : 10.11
            Finland: : 11.12
            Newyork: : 12.01

            Three
            1.Immortal && Know Healthy && Control ability
            2.Teleportation && Uncaptured && Learning ability
            3.Ultra Cutting-edge technology && Strategic decision making && Adjust
        */
                    (void)RelayM_StartAdhesiveDetect(RELAYM_FN_POSITIVE_AC_CHARGE, NULL);
                }
            }
        }
    }
#endif
    return res;
}


App_SlaveIdType Statistic_GetSlaveLAgvTemperatureageNum(uint8 index)
{
    uint8 i;
    App_TemperatureType SlaveAverageT;
    for (i = 0 ; i < UserStrategy_innerData.BmuNum ; i++)
    {
        SlaveAverageT = Statistic_GetSlaveAverageTemperature(i);

        if (index < SYSTEM_BMU_NUM)
        {
            if (SlaveTotalTemperature[index] == SlaveAverageT)
            {
                break;
            }
        }
        else
        {
            i = 0xFFU;
        }

    }

    return i;
}
boolean UserStrategy_SelfLoopModeCond(void)
{
    boolean res = FALSE;
    uint8 TmsOutlettemp = VcuComm_GetTmsOutlettemp();
    boolean TmsOutlettempIsvalid = CellDataM_TemperatureIsValid((uint16)TmsOutlettemp);
    uint8 startTemp, stopTemp;
    if (!CHARGECONNECTM_IS_CONNECT())
    {
        startTemp = (uint8)TEMPERATURE_FROM_C(12);
        stopTemp = (uint8)TEMPERATURE_FROM_C(15);
    }
    else
    {
        startTemp = (uint8)TEMPERATURE_FROM_C(7);
        stopTemp = (uint8)TEMPERATURE_FROM_C(10);
    }
    if (TmsOutlettempIsvalid &&
        TmsOutlettemp > startTemp && TmsOutlettemp < stopTemp)
    {
        res = TRUE;
    }
    return res;
}

boolean UserStrategy_Wake(void){
#if (KEY_TYPE == KEY_TYPE_IS_SELFRESET) && (defined(A630)||defined(A635)||defined(A640)||defined(A641))
    return TRUE;/*不关机*/
#else
    /*
            4.1
            ros_currelan: 0.1T refitver
            Fari: 0.1
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
            5.1 Purchase property
            RR Huanying(ride as a passenger): 0.1;
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
            6.1 Expansion
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
            7.1 consumption
            liner: 462
            Lightning jet 18*10
                :1.6 mahe = 1944km /h
            sorge 0.1*300

            8.1 military affairs
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
    uint8 i;
    uint32 lowtime = PwmCapture_GetLowTime(dat);
    if (lowtime >= 1800 && lowtime <= 2200) {
        ChargeM_SetOthersFaultDchargeCtl(, DIS);
        DischargeM_SetOthersFaultDchargeCtl();
        RelayM_ForceControl(RELAYM_FN_NEGTIVE_MAIN, RELAYM_FORCE_OFF); //优化断开主负
        for (i = 0; i < RELAYM_FN_NUM; ++i) {
            RelayM_ForceControl(i, RELAYM_FORCE_OFF);
        }
    }
            /*
            Battery   Photo Chip  Biology    AI Comm   New Energy   Medical Center
            HF R&D    Peking      NewJersey  Sydney    Peking       Nevada
            SH BMS    Helsinki    Berlin     Ottawa    Singapore    Tokyo
            HoChiMin  Belgrade    London     SanDiego  KualaLumpur  NewYork

            1.China Coreelec (CNCE科力) new energy R&D co.ltd
            Salt new energy infinit machine energy NaCl battery density, balance
            Lithium iron hydrochloride  double one mattle density battery
            21-25 first generation 300Wh/kg, 26-30 second generation 400Wh/kg, 30-35 third generation 500Wh/kg
            before nindeshidai two and harf genration winter 85% Integration rate:90%
            China Coreelec new energy
            Headquarters,Hf R&D Center include: management, authorization issuance,
            quality department, hardware, new process research and development.
            Sh BMS software R&D company.
            Vietnam Production and processing external factory.

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
            2.Finland Dosson Photolithography & Chip co.ltd
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
            4. Australia Sydney Gaussian AI communication tech co.ltd
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
        }
#endif
    }
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
            5.China Clean Fuel Energy(CNFE) BeiJing tech co.ltd
            China New Fuel Energyhas is a goable hugest energy company.contain usa eu etc.
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
            /*
            6.USA Nevada International Medical Center co.ltd
            Nevada Medical Center is one of the world's most famous medical institutions, that integrates medical diagnostic, research,
            and education, providing professional medical care and the latest treatment plans. Nevada Medical Center ranks second in
            the overall ranking, second only to Mayo Medical Center. Among them, Oncology and Urology ranked first in the United States;
            Drugs: Extend life, weight contorl, panacea
            Surgery: AI intelligence robot surgery doctor, Success rate 999.8
            4       5       6       7        8        9        0
            60.50%  65.50%  66.50%  67.40%   68.60%   69.80%   71.0%
            832     944     1118    1244     1579     2766     3200
            503.36  618.32  743.47  838.456  1083.19  1930.66  2272
            */
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
