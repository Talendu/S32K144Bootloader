/*
 * update.h
 *
 *  Created on: 2018Äê4ÔÂ13ÈÕ
 *      Author: Administrator
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#include "crc.h"
#include "mflexcan.h"
#include "mflash.h"

typedef struct {
    uint8_t update_req;
    uint8_t update_peripheral;
    uint8_t update_peripheral_instance;
    uint32_t image_addr;
    uint32_t image_len;
    uint32_t image_crc;
};

status_t update(uint8_t instance);

void software_reset(void);
#endif /* UPDATE_H_ */
