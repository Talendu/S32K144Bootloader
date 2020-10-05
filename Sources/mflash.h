/*
 * mflash.h
 *
 *  Created on: 2018年4月12日
 *      Author: Administrator
 */

#ifndef MFLASH_H_
#define MFLASH_H_
#include "S32K144.h"
#include "flash0.h"
#include "clockMan1.h"

#define FLASH_TARGET

extern flash_ssd_config_t g_flashSSDConfig;

/**
 * \brief   写入pflash前的初始化操作
 */
status_t flash_pflash_init(void);

/**
 * \brief   将DFlash初始化为EEPROM
 */
status_t flash_EEPROM_init(void);

/*
 * \brief   擦出PFlash扇区
 * \param   sector_index    扇区指标,及从第几个扇区开始擦出,
 *                          取值范围0~127(4kByte一个扇区,共512kByte)
 * \param   sector_num      擦出的扇区个数
 * \note    sector_index + sector_num <= 127
 */
status_t flash_pflash_erase_sectors(uint32_t sector_index, uint32_t sector_num);

/*
 * \brief   向PFlash写入数据
 * \param   address             写入数据的地址,必须是8的倍数
 * \param   size                写入数据的字节数
 * \param   sourceBuffer[in]    要写入的数据
 * \param   failAddr[out]       返回写入数据失败的地址
 * \retval
 */
status_t flash_write_PFLASH(uint32_t address, uint32_t size, uint8_t *sourceBuffer, uint32_t *failAddr);

/**
 * \brief   向EEPROM中写入数据
 *
 * \param   offset          写入数据地址与EEPROM首地址的偏移量
 * \param   sourceBuffer    要写入的数据
 * \param   len             写入数据的长度
 *
 * \retval  STATUS_SUCCESS  写入成功
 *          STATUS_ERROR    写入失败
 */
status_t flash_write_EEPROM(uint32_t index, uint8_t *sourceBuffer, uint32_t len);

START_FUNCTION_DECLARATION_RAMSECTION
/**
 * \brief   写入flash前的回掉函数
 * \details 在向FALSH写入数据前,会先调用该函数
 */
void CCIF_Callback(void)
END_FUNCTION_DECLARATION_RAMSECTION

#endif /* MFLASH_H_ */
