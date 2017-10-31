/**
 * \file FullCharge_Lcfg.c
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 满充策略链接配置文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170524 |
 */

#include "FullCharge.h"

const FullCharge_ConfigInfoType FullCharge_ConfigInfo = {
    TRUE, /**< currentDecreaseEndFlag: 充电降流末端检查标志 */
};
