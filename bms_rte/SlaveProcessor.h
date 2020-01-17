#ifndef SLAVEPROCESSOR_H__
#define SLAVEPROCESSOR_H__

#include "Async_Event.h"
#include "ComStack_Types.h"
#include "Uart.h"


Std_ReturnType SlaveProcessor_Init(Async_LooperType *looper, uint8 * fw_data, uint16 fw_size, uint32 fw_crc);

#endif
