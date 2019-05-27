#ifndef __SOFT_I2C_H__
#define __SOFT_I2C_H__

#include "Std_Types.h"
#include "i2c.h"

/// \name  SoftI2C IO模拟软件实现I2C
/// @{

typedef const struct softi2c_platform *softi2c_t;


/// \brief softi2c_init 初始化.
///
/// \param i2c 对应的IO底层接口.
uint8 softi2c_init(softi2c_t i2c);

void softi2c_deinit(const struct softi2c_platform *i2c);
uint8 softi2c_write(const struct softi2c_platform *i2c,
                      uint8 addr,
                      const uint8 *dat,
                      uint8 len);

uint8 softi2c_read(const struct softi2c_platform *i2c,
                     uint8 addr,
                     uint8 *dat,
                     uint8 len);
/// \brief softi2c_write_then_read 先写后读.
///
/// \param i2c 对应的IO底层接口.
/// \param addr SLAVE地址, 7位, 不包含读写位, 最终总线上的第一个8位周期为:((addr << 1) + RW).
/// \param w 需要写入的数据.
/// \param wlen 需要写入的数据的长度.
/// \param r 读取的数据存放的地址.
/// \param rlen 期望读取的数据长度.
///
/// \return 写入和读取的数据的字节数相加.
uint8 softi2c_write_then_read(softi2c_t i2c,
                                uint8 addr,
                                const uint8 *w,
                                uint8 wlen,
                                uint8 *r,
                                uint8 rlen);

///  @}
#endif
