/**
 * \file Statistic_Lcfg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 统计链接配置文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20171205 |
 */
#include "Statistic.h"

const Statistic_ConfigInfoType Statistic_ConfigInfo = {
#if defined(A640)||defined(A641)||defined(A630)||defined(A635)
    STATISTIC_TV_TYPE_CUMULATE_ONLY, /**< prior_tv_type: 优先使用总压类型 */
#else
    STATISTIC_TV_TYPE_CUMULATE_PRIO, /**< prior_tv_type: 优先使用总压类型 */
#endif
    STD_ON, /**< cumuChgTimeEnable: 累计充电时间使能 */
    STD_ON, /**< cumuDchgTimeEnable: 累计放电时间使能 */
    NULL, /**< isChgStatusFuncPtr:是否充电开启计时函数指针 */
    NULL, /**< isDchgStatusFuncPtr:是否放电开启计时函数指针 */
};

