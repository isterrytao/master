#ifndef __PWMCAPURE_H__
#define __PWMCAPURE_H__

#include "Std_Types.h"
#include "XGate.hx"

#pragma push
#pragma align on
typedef struct {
    uint16 *near ect2_tc;
    uint8 port_t_bit;
    uint8 revertflag;
    uint8 edgesToTriggerHcs12;
    uint8 overflowIgnore;
    uint32 highLevelCounter;
    uint32 lowLevelCounter;
    uint32 counter;
    uint16 last_capture;
} PwmCapture_DataType;

#define PWM_TRIGGER_HCS12_RAISING 0x01U
#define PWM_TRIGGER_HCS12_FALLING 0x02U
#define PWM_TRIGGER_HCS12_ALL     0x03U
#define PWM_TRIGGER_HCS12_NONE    0x00U

#define PWM_RAISING_EDGE_TRIGGERED  0x04U

#pragma pop


void PwmCapture_Init(void);
uint16 PwmCapture_GetDuty(const PwmCapture_DataType *pwm_data);
uint32 PwmCapture_GetCycle(const PwmCapture_DataType *pwm_data);
uint32 PwmCapture_GetFrequence(const PwmCapture_DataType *pwm_data);
boolean PwmCapture_IsTriggerRising(const PwmCapture_DataType *pwm_data);
uint32 PwmCapture_GetLowTime(const PwmCapture_DataType *pwm_data);
uint32 PwmCapture_GetHighTime(const PwmCapture_DataType *pwm_data);

#include "PwmCapture_Cfg.h"

#endif
