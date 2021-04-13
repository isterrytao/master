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

#define HSSVCCOFFSET 710U /*理论+实验得出的偏移值*/
#define R_SENSE 680U /*MultiSense脚对地电阻，单位欧姆*/
#define K0 500U
#define K1 500U
#define K2 500U
#define K3 500U


void dealHLSSVnq7140Data(uint16 adc_value);
void setVnq7140MultiSenseChannel(uint8 channel);

#endif
