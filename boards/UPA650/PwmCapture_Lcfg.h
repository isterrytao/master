#ifndef PWM_CAPTURE_LCFG_H
#define PWM_CAPTURE_LCFG_H

#include "PwmCapture.h"

#define CP_GetDuty() PwmCapture_GetDuty(&PwmCapture_DataList[PWM_CAPTURE_CP_CHANNEL])


#pragma push
#pragma DATA_SEG __GPAGE_SEG XGATE_DATA
#pragma CONST_SEG __GPAGE_SEG XGATE_CONST

#pragma push
#pragma DATA_SEG __GPAGE_SEG XGATE_DATA
extern PwmCapture_DataType PwmCapture_DataList[PWM_CAPTURE_CHANNEL_NUM];
#pragma pop

#pragma pop


void PwmCapture_Start(void);

uint16 CP_GetFrequence(void);


#endif
