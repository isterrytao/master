#ifndef __ADS1018_SPI_H__
#define __ADS1018_SPI_H__

void ads1018_spi_init(void);
uint8 ads1018_spi_rw(uint8 data);
void ads1018_spi_select(uint8 index, uint8 is_select);

#endif
