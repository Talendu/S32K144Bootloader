/*
 * update.c
 *
 *  Created on: 2018年4月13日
 *      Author: Administrator
 */
#include "update.h"
#include "lpuart0.h"
#include "xmodem.h"
#include "mflash.h"

uint32_t package_size = 0;

uint8_t lpuart0_default_rx_buffer[8];

/**
 * \brief   程序更新
 * \note    在该函数中,会以约5次每秒的速度向上位机发出升级请求
 *          可以通过按键退出升级程序
 */
void updating(void) {
    while(1) {
        if (xmodem_is_active()) {
            xmodem_download();                 /* 串口收到数据,使用串口升级程序 */
            break;
        } else if ((GPIO_HAL_ReadPins(PTE) & (1<<7)) == 0) {
            OSIF_TimeDelay(10);
            while ((GPIO_HAL_ReadPins(PTE) & (1<<7)) == 0);
            break;
        }
        OSIF_TimeDelay(190);
    }
}

/**
 * \brief   初始化升级程序所需的外设
 */
void update_init(void) {
    flash_pflash_init();
    flexcan_init();
    LPUART_DRV_Init(INST_LPUART0, &lpuart0_State, &lpuart0_InitConfig0);
    LPUART_DRV_InstallRxCallback(INST_LPUART0, xmodem_uart_handler, NULL);
    LPUART_DRV_ReceiveData(INST_LPUART0, lpuart0_default_rx_buffer, 8);
    xmodem_init();
}

/**
 * \brief   升级程序
 */
void update(void) {
    update_init();
    updating();
    FLEXCAN_DRV_Deinit(INST_CANCOM0);
    LPUART_DRV_Init(INST_LPUART0, &lpuart0_State, &lpuart0_InitConfig0);
}

