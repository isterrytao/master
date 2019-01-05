#ifndef _VNQ7140_H_
#define _VNQ7140_H_
#include "Std_Types.h"

#define VNQ7140CHANNELVCC  0U
#define VNQ7140CHANNELTEMP 1U
#define VNQ7140CHANNEL0    2U
#define VNQ7140CHANNEL1    3U
#define VNQ7140CHANNEL2    4U
#define VNQ7140CHANNEL3    5U
#define VNQ7140MAXCHANNEL  6U

#define R_SENSE 1000U /*MultiSense脚对地电阻，单位欧姆*/
#define K0 550U
#define K1 520U
#define K2 475U
#define K3 470U


void dealHLSSVnq7140Data(uint16 adc_value);
void setVnq7140MultiSenseChannel(uint8 channel);

#endif
