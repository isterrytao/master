/**
 * \file VcuComm_Types.h
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 整车通信类型定义头文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170316 |
 */

/**
 * \addtogroup VcuComm 整车通信
 * \{
 */


#ifndef VCUCOMM_TYPES_H_
#define VCUCOMM_TYPES_H_

/**
 * \brief 整车通信IPdu类型定义
 * \notes 发送报文优先
 */


typedef enum _VcuComm_StageType{
    VCUCOMM_STAGE_IDLE = 0, /**< 空闲状态 */

    VCUCOMM_STAGE_START = 1,
    VCUCOMM_STAGE_STAGE1 = VCUCOMM_STAGE_START, /**< 整车通信 */
    VCUCOMM_STAGE_STOP = VCUCOMM_STAGE_STAGE1,

    VCUCOMM_STAGE_MAX = 2
}VcuComm_StageType;


#endif



/**
 * \}
 */