/*
 * mflash.h
 *
 *  Created on: 2018Äê4ÔÂ12ÈÕ
 *      Author: Administrator
 */

#ifndef MFLASH_H_
#define MFLASH_H_
#include "S32K144.h"
#include "flash0.h"
#include "clockMan1.h"

#define FLASH_TARGET

extern flash_ssd_config_t flashSSDConfig;

status_t flash_pflash_init(void);
status_t flash_EEPROM_init(void);
status_t flash_pflash_erase_sectors(uint32_t sector_index, uint32_t sector_num);
status_t flash_write_PFLASH(uint32_t address, uint32_t size, uint8_t *sourceBuffer, uint32_t *failAddr);
status_t flash_write_EEPROM(uint32_t index, uint8_t *sourceBuffer, uint32_t len);

/* Function declarations */
void CCIF_Handler(void);
/* If target is flash, insert this macro to locate callback function into RAM */
START_FUNCTION_DECLARATION_RAMSECTION
void CCIF_Callback(void)
END_FUNCTION_DECLARATION_RAMSECTION

#endif /* MFLASH_H_ */
