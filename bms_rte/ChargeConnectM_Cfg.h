/**
 * \file ChargeConnectM_Cfg.h
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 充电连接管理头文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170113 |
 */

/**
 * \addtogroup ChargeConnectM 充电连接管理
 * \{
 */

#ifndef CHARGECONNECTM_CFG_H_
#define CHARGECONNECTM_CFG_H_

#include "Charge_Types.h"

#define CHARGECONNECTM_DEV_ERROR_DETECT     STD_OFF

/**
 * \brief 充电电流连接模式参数
 */
#define CHARGECONNECTM_CONNECTION_CHG_CURRENT_MIN       CURRENT_S_100MA_FROM_A(3) /**< 最小有效充电电流值 */
#define CHARGECONNECTM_CHG_CURRENT_CONNECTION_DELAY     5000U // ms/bit 确认充电模式延时
#define CHARGECONNECTM_CHG_CURRENT_UNCONNECTION_DELAY   3000U // ms/bit 确认退出充电模式延时

#if 0
#define CHARGECONNECTM_CONNECT_TYPE_UPDATE_CYCLE    25U
#define CHARGECONNECTM_CP_CONNECT_DELAY             50U
#define CHARGECONNECTM_CP_UNCONCECT_DELAY           50U
#else
#define CHARGECONNECTM_CONNECT_TYPE_UPDATE_CYCLE    50U
#define CHARGECONNECTM_CP_CONNECT_DELAY             100U
#define CHARGECONNECTM_CP_UNCONNECT_DELAY           300U
#endif

/**
 * \brief 国标充电连接CC CC2通道定义
 */
#define CHARGECONNECTM_ADC_CHANNEL_MAX      2U

#define CHARGECONNECTM_ADC_CC_CHANNEL       0U
#define CHARGECONNECTM_ADC_CC2_CHANNEL      1U


#endif

/**
 * \}
 */