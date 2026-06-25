/**
  ******************************************************************************
  * @file    bsp_spi_flash.h
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SPI Flash驱动头文件（存根，不使用外部Flash时）
  ******************************************************************************
  */

#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f4xx.h"

/* 函数声明（存根） */
void SPI_FLASH_Init(void);
uint16_t SPI_FLASH_ReadID(void);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);

#endif /* __SPI_FLASH_H */
