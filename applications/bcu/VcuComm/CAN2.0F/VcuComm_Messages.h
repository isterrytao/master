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

#define VCUCOMM_READ_EEPROM_COMMAND    0x42U
#define VCUCOMM_WRITE_EEPROM_COMMAND   0x22U
#define VCUCOMM_EEPROM_OPERATE_OEM_MIN 1U
#define VCUCOMM_EEPROM_OPERATE_OEM_MAX 8U


/**
 * \brief 整车通信高压上下电命令类型定义
 */
typedef enum{
    VCUCOMM_POWER_OFF, /**< 高压下电命令 */
    VCUCOMM_POWER_ON /**< 高压上电命令 */
}VcuComm_PowerCmdType;

typedef enum {
    VCUCOMM_EEPROM_OPERATE_CMD_NONE = 0,
    VCUCOMM_EEPROM_OPERATE_CMD_WIRTE,
    VCUCOMM_EEPROM_OPERATE_CMD_READ,
    VCUCOMM_EEPROM_OPERATE_CMD_ERROR,
}VcuComm_EepromOperateCmdType;

typedef enum {
    VCUCOMM_EEPROM_WRITE_NONE,
    VCUCOMM_EEPROM_WRITE_NORMAL,
    VCUCOMM_EEPROM_WRITE_OEM_ERROR,
    VCUCOMM_EEPROM_WRITE_CRC_ERROR,
}VcuComm_EepromWriteCmdType;

typedef enum {
    VCUCOMM_EEPROM_READ_NONE,
    VCUCOMM_EEPROM_READ_NORMAL,
    VCUCOMM_EEPROM_READ_OEM_ERROR,
}VcuComm_EepromReadCmdType;

/**
 * \整车通信报文数据
 */
typedef struct{
    VcuComm_PowerCmdType PowerCommand; /**< 高压上下电命令 */
    VcuComm_EepromOperateCmdType eepromCommand;
    VcuComm_EepromWriteCmdType eepWriteCmd;
    VcuComm_EepromReadCmdType eepReadCmd;
    uint16 eepromWriteData1;
    uint16 eepromWriteData2;
    uint8 sid;
    uint8 writeOem;
    uint8 readOem;
}VcuComm_MsgDataType;


extern VcuComm_MsgDataType VcuComm_MsgData;

/**
 * \brief 整车通信报文初始化
 */
void VcuComm_MessageInit(Async_LooperType *looper);

/**
 * \brief 放电CAN2.0F协议
 */
void VcuComm_GetStatusMsg_0x200(uint8 *buf, uint16 *Length);
/**
 * \brief 放电CAN2.0F协议
 */
void VcuComm_GetStatusMsg_0x201(uint8 *buf, uint16 *Length);
/**
 * \brief 放电CAN2.0F协议
 */
void VcuComm_GetStatusMsg_0x202(uint8 *buf, uint16 *Length);
/**
 * \brief 放电CAN2.0F协议
 */
void VcuComm_GetStatusMsg_0x203(uint8 *buf, uint16 *Length);
/**
 * \brief 放电CAN2.0F协议
 */
void VcuComm_GetStatusMsg_0x190(uint8 *buf, uint16 *Length);
/**
 * \brief 放电CAN2.0F协议
 */
void VcuComm_GetStatusMsg_0x5F2(uint8 *buf, uint16 *Length);

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
void VcuComm_ReceiveCbk(const uint8 *Buffer, uint16 Length);

uint8 VcuComm_Bms2VcuSendLevel(uint8 level);


#endif
