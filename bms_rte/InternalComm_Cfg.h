/**
 * \file InternalComm_Cfg.h
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 内网通信配置文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初版本, 完成讨论部分的定义. | UD00004 | 20161216 |
 */

/**
 * \addtogroup InternalComm 内网通信
 */
#ifndef INTERNALCOMM_CFG_H_
#define INTERNALCOMM_CFG_H_

#include "CanTp_Cfg.h"

/**
 * \brief 内网通信DID请求使能控制
 */
#define INTERNALCOMM_REQUIRE_SLAVE_BALANCE_STATUS_EN    STD_ON
#define INTERNALCOMM_REQUIRE_SLAVE_HSS_STATUS_EN        STD_ON
#define INTERNALCOMM_REQUIRE_SLAVE_HSS_VOLTAGE_EN       STD_ON
#define INTERNALCOMM_REUQIRE_SLAVE_HSS_CURRENT_EN       STD_ON
#define INTERNALCOMM_REUQIRE_SLAVE_DIGITAL_INPUT_EN     STD_ON
#define INTERNALCOMM_REUQIRE_SLAVE_ANALOG_INPUT_EN      STD_ON
#define INTERNALCOMM_REUQIRE_SLAVE_BOARD_VOLTAGE_EN     STD_ON
#define INTERNALCOMM_REUQIRE_SLAVE_BOARD_TEMPERATURE_EN STD_ON
#define INTERNALCOMM_REQUIRE_SLAVE_FW_VERSION_EN        STD_ON
#define INTERNALCOMM_REUQIRE_SLAVE_HWID_EN              STD_ON

/**
 * \brief 内网通信发送缓存大小定义
 */
#define INTERNALCOMM_TX_BUFF_SIZE               (410U)

/**
 * \brief 内网通信接收缓存大小定义
 */
#define INTERNALCOMM_RX_BUFF_SIZE               (150U)

/**
 * \brief 内网通信从机请求DID个数
 */
#define INTERNALCOMM_SLAVE_DID_NUM              (0x0EU)

/**
 * \内网通信请求通道个数，代表主机请求从机数据并发能力
 */
#define INTERNALCOMM_REQUIRE_CHANNEL_NUM        (4U)

/**
 * \brief 内网通信从机响应超时时间
 */
#define INTERNALCOMM_P2CAN_CLIENT               (50U)
#define INTERNALCOMM_P2CAN_CLIENT_EXT           (2000U)
/**
 * \brief 内网通信从机数据请求间隔
 */
#define INTERNALCOMM_SLAVE_REQUIRE_INTERNAL     (0x02U)
#define INTERNALCOMM_RESTART_REURIRE_INTERNAL   (0x0AU)

/**
 * \brief 内网通信从机启动请求超时时间
 */
#define INTERNALCOMM_START_SLAVE_REQUIRE_TIMEOUT    (1000U)

/**
 * \brief 主从通信超时时间定义
 */
#define INTERNALCOMM_SLAVE_COMMABORT_TIME       (5000U)

/**
 * \brief 内网通信错误检查开关
 */
#define INTERNALCOMM_DEV_ERROR_DETECT           STD_OFF

/**
 * \brief 内网通信使用到的XGATE软件中断最大通道号
 */
#define INTERNALCOMM_USE_XGATE_SW_ISR_MAX       3U

#endif