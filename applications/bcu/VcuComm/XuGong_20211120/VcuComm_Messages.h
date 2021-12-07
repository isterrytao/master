/**
 * \file VcuComm_messages.h
 * \copyright UDAN Co.,Ltd. *
 *
 * \brief 整车通信报文头文件.
 *
 * * \par 修订历史:
 * | 版本号 | 修订日志 | 修改人 | 修订时间 |
 * | :--- | :--- | :--- | :--- |
 * | 0.1 | 初始版本, 完成讨论部分的定义. | UD00004 | 20170316 |
 */
#ifndef VCUCOMM_MESSAGE_H_
#define VCUCOMM_MESSAGE_H_

#include "VcuComm.h"


/**
 * \brief 整车通信高压上下电命令类型定义
 */
typedef enum{
    VCUCOMM_POWER_OFF, /**< 高压下电命令 */
    VCUCOMM_POWER_ON /**< 高压上电命令 */
}VcuComm_PowerCmdType;

/**
 * \整车通信报文数据
 */
typedef struct{
    VcuComm_PowerCmdType PowerCommand; /**< 高压上下电命令 */
    uint8 faultBuff[8];
}VcuComm_MsgDataType;


extern VcuComm_MsgDataType VcuComm_MsgData;

/**
 * \brief 整车通信报文初始化
 */
void VcuComm_MessageInit(Async_LooperType *looper);

/**
 * \brief 参考BMS-CAN标准帧通信协议V1.01_20171202协议
 */
void VcuComm_GetStatusMsg_0x2F0(uint8 *buf, uint16 *Length);
/**
 * \brief 参考BMS-CAN标准帧通信协议V1.01_20171202协议
 */
void VcuComm_GetStatusMsg_0x2F1(uint8 *buf, uint16 *Length);
/**
 * \brief 参考BMS-CAN标准帧通信协议V1.01_20171202协议
 */
void VcuComm_GetStatusMsg_0x2F2(uint8 *buf, uint16 *Length);
/**
 * \brief 参考BMS-CAN标准帧通信协议V1.01_20171202协议
 */
void VcuComm_GetStatusMsg_0x2F3(uint8 *buf, uint16 *Length);
void VcuComm_GetStatusMsg_0x244(uint8 *buf, uint16 *Length);
void VcuComm_GetStatusMsg_0x444(uint8 *buf, uint16 *Length);

/**
 * \brief 接收超时处理回调函数
 */
void VcuComm_RecTimeoutCbk(void);


/**
 * \brief 发送整车报文1条件检查
 *
 * \return E_OK: 允许发送 E_NOT_OK: 不允许发送
 */
Std_ReturnType VcuComm_SendConditionCheck(uint16 IPdu);

/**
 * \brief 接收整车报文条件检查
 *
 * \return E_OK: 允许接收 E_NOT_OK: 不允许接收
 */
Std_ReturnType VcuComm_RecConditionCheck(void);

/**
 * \brief 整车接收报文处理
 *
 * \param Buffer 数据缓存
 * \param Length 数据长度
 */
void VcuComm_ReceiveCbk(uint8 *Buffer, uint16 Length);

/**
 * \brief 7F4报文发送条件检查
 *
 * \return E_OK: 允许发送 E_NOT_OK: 不允许发送
 */
Std_ReturnType VcuComm_SendMsgCond_0x7F4(uint16 IPdu);


#endif
