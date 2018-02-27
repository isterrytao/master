#ifndef PWMCAPTURE_CFG_H__
#define PWMCAPTURE_CFG_H__

#define PWM_CAPTURE_CHANNEL_NUM        3
#define PWM_CAPTURE_CP_CHANNEL         0
#define PWM_CAPTURE_SIGIN1_CHANNEL     1
#define PWM_CAPTURE_SIGIN2_CHANNEL     2

extern void UserStrategy_CheckSRS(const PwmCapture_DataType *dat);

#define PWM_CAPTURE_SIGIN1_RAISING_FUNC   UserStrategy_CheckSRS
//#define PWM_CAPTURE_SIGIN1_FALLING_FUNC   UserStrategy_CheckSRS
//#define PWM_CAPTURE_SIGIN1_FALLING_FUNC


#endif
