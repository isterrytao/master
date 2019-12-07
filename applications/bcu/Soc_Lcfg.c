/**
 * \file Soc_Lcfg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief SOC配置文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170501 |
 */
#include "Soc.h"
#include "ChargeM.h"
#include "TemperatureM.h"
#include "UserStrategy_Cfg.h"
#include "HvProcess_Chg.h"
#include "RelayM_Lcfg.h"


const Soc_ConfigInfoType Soc_ConfigInfo = {
    /**< ChgCumuInfo: 充电累计配置信息 */
    {
        Soc_CumuTypeCap, /**< type: 累计类型 */
        100U, /**< resoltion: 存储分辨率(仅用于能量累计类型) 精度: 瓦时(wh) 范围: 100~1000 */
        FALSE, /**< isContainFeedback: 是否包含回馈电流 */
    },
    /**< DchgCumuInfo: 放电累计配置信息 */
    {
        Soc_CumuTypeCap, /**< type: 累计类型 */
        100U, /**< resoltion: 存储分辨率(仅用于能量累计类型) 精度: 瓦时(wh) 范围: 100~1000 */
    },
    {
        PERCENT_TO_SOC(10U), /**< socJumpMax: SOC最大跳变值，注：此值需要不大于SOC跳变缓存大小SOC_JUMP_BUFF_SIZE */
        NULL, /**< 获取最大跳变SOC值，包含释放值，此函数有效时 socJumpMax 和 socJumpRel不启作用 */
        MIN_TO_S(1U), /**< socJumpTime: SOC跳变时间 单位：S */
    },
};

Std_ReturnType Soc_IsChargeFinish(void)
{
	return ChargeM_ChargeIsAllowed();
}

Current_CurrentType Soc_CurrentHook(Current_CurrentType current)
{
#ifdef RELAYM_FN_HEATER
#if USERSTRATEGY_CURRENT_FLAG == TRUE
    if (CurrentM_IsValidCurrent(current))
    {
        if (HvProcess_IsHeatAndChargeMode())
        {
            current = current - USERSTRATEGY_START_HEAT_CURRENT;
        }
    }
#endif
#endif
    return current;
}

boolean Soc_IsBatteryCurrent(void)
{
    boolean res;

#if USERSTRATEGY_CURRENT_FLAG == FALSE
    res = TRUE;
#else
    if (!HvProcess_ChargerIsHeatMode())
    {
        res = TRUE;
    }
    else
    {
        res = FALSE;
    }
#endif

    return res;
}
