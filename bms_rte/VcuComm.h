/**
 * \file VcuComm.h
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 整车通信头文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20161209 |
 */

/**
 * \addtogroup VcuComm 整车通信
 * \{
 */


#ifndef VCUCOMM_H_
#define VCUCOMM_H_

#include "VcuComm_Internal.h"

/**
 * \brief 整车状态类型定义
 */
typedef struct{
    boolean CommunicationStatus; /**< 整车通信状态 */
    uint32 CommAbortStatus; /**< 通信中断状态 */
}VcuComm_VcuDataType;

/**
 * \brief 整车通信初始化
 *
 * \param pri 任务优先级
 */
void VcuComm_Init(uint8 pri);

/**
 * \brief 整车通信是否已启动
 * \return FALSE-未启动 TRUE-已启动
 */
boolean VcuComm_IsStart(void);

/**
 * \brief 设置当前整车通信状态
 * \param status TRUE:通信中 FALSE:未通信
 */
void VcuComm_SetCommunicationStatus(boolean status);

/**
 * \brief 获取当前整车通信状态
 * \return TRUE:通信中 FALSE:未通信
 */
boolean VcuComm_GetCommunicationStatus(void);

/**
 * \brief 置位IPdu接收报文通信超时标志
 * \note  此接口仅置位对应IPdu的通信超时标志位
 *
 * \param IPdu 接收报文IPdu值
 */
void VcuComm_SetCommAbortMessageFlag(uint16 IPdu);

/**
 * \brief 清除IPdu接收报文通信超时标志
 * \note  此接口仅清除对应IPdu的通信超时标志位
 *
 * \param IPdu 接收报文IPdu值
 */
void VcuComm_ClrCommAbortMessageFlag(uint16 IPdu);

/**
 * \brief 清除整车状态信息
 * \note  此接口会将VcuComm_VcuDataType变量中所有值清零
 */
void VcuComm_ClrVcuData(void);

/**
 * \brief 增加VCU数据指针
 * \return 详见VcuComm_VcuData定义
 */
VcuComm_VcuDataType* VcuComm_GetVcuDataPtr(void);

/**
 * \brief 诊断用当前整车通信中断状态
 * \note  受VcuComm_SetCommAbortMessageFlag和VcuComm_ClrCommAbortMessageFlag接口调用影响,非0则返回TRUE
 *
 * \return TRUE:通信中断 FALSE:通信正常
 */
uint16 VcuComm_DiagIsCommAbort(void);

/**
 * \brief 获取整车通信状态
 * \note 受VcuComm_SetCommunicationStatus接口调用影响
 *
 * \return E_NOT_OK:未通信 E_OK:正在通信
 */
Std_ReturnType VcuComm_VcuIsCommunication(void);



#endif

/**
 * \}
 */