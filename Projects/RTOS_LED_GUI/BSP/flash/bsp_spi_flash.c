/**
  ******************************************************************************
  * @file    bsp_spi_flash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   SPI Flash驱动（存根，不使用外部Flash时）
  ******************************************************************************
  */

#include "bsp_spi_flash.h"

/**
  * @brief  初始化SPI Flash（存根）
  * @param  无
  * @retval 无
  */
void SPI_FLASH_Init(void)
{
    /* 不使用外部Flash，空实现 */
}

/**
  * @brief  读取SPI Flash ID（存根）
  * @param  无
  * @retval 返回0表示无设备
  */
uint16_t SPI_FLASH_ReadID(void)
{
    return 0; /* 无设备 */
}

/**
  * @brief  写SPI Flash（存根）
  * @param  pBuffer: 数据缓冲区
  * @param  WriteAddr: 写入地址
  * @param  NumByteToWrite: 写入字节数
  * @retval 无
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    /* 不使用外部Flash，空实现 */
    (void)pBuffer;
    (void)WriteAddr;
    (void)NumByteToWrite;
}

/**
  * @brief  读SPI Flash（存根）
  * @param  pBuffer: 数据缓冲区
  * @param  ReadAddr: 读取地址
  * @param  NumByteToRead: 读取字节数
  * @retval 无
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    /* 不使用外部Flash，空实现 */
    (void)pBuffer;
    (void)ReadAddr;
    (void)NumByteToRead;
}

/**
  * @brief  擦除SPI Flash扇区（存根）
  * @param  SectorAddr: 扇区地址
  * @retval 无
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
    /* 不使用外部Flash，空实现 */
    (void)SectorAddr;
}
