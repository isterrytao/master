#ifndef HMI_H__
#define HMI_H__

#include "Async_Event.h"

Std_ReturnType Hmi_Init(Async_LooperType *looper, uint32 baud);



typedef struct Modbus_RegistersRegion {
	uint16 startaddr; /*收到的寄存器地址*/
	uint16 endaddr;
	uint16 (*getRegisterValue)(const struct Modbus_RegistersRegion *regs, uint16 addr);
} Modbus_RegistersRegionType;

typedef struct {
	const Modbus_RegistersRegionType *regRegions;
	uint8 regionsNum;
	uint8 addrIncEachWord;
} Modbus_RegistersTableType;


extern const Modbus_RegistersTableType Modbus_X02registerTable;
extern const Modbus_RegistersTableType Modbus_X04registerTable;


#endif
