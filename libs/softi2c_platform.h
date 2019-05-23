#ifndef __SOFT_I2C_PLATFORM_H__
#define __SOFT_I2C_PLATFORM_H__

#include "softi2c.h"
#include "Dio.h"

struct softi2c_platform {
    Dio_ChannelType scl;
    Dio_ChannelType sda;
};

extern const struct i2c_operations softi2c_ops;

#endif

