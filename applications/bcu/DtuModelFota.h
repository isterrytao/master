#ifndef _DTUMODELFOTA_H
#define _DTUMODELFOTA_H

#include "Async_Event.h"

typedef enum {
	DTU_MODEL_FOTA_INIT = 0,
	DTU_MODEL_FOTA_START = 1,
	DTU_MODEL_CLOSE_TCP = 2,
	DTU_MODEL_FOTA_STATE_CHECK = 3,
	DTU_MODEL_FOTA_FINISH = 4,
	DTU_MODEL_FOTA_ERROR = 5,
}DtuModelFota_StateType;

typedef struct {
	Async_EventType event;
	DtuModelFota_StateType State;
	uint8 FotaFaultNum;
	uint32 StartOtaTimeTick;
}DtuModelFota_InnerDataType;


void DtuModelFota_Init(Async_LooperType *looper);

#endif