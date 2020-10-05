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
status_t updating(instance) {
    status_t ret = STATUS_ERROR;
    while(1) {
        if (xmodem_is_active(instance)) {
            ret = xmodem_download(instance);                 /* 串口收到数据,使用串口升级程序 */
            break;
        }
        OSIF_TimeDelay(190);
    }
    return ret;
}

/**
 * \brief   初始化升级程序所需的外设
 */
void update_init(uint8_t instance) {
    flash_pflash_init();
//    flexcan_init();
    LPUART_DRV_Init(instance, &lpuart0_State, &lpuart0_InitConfig0);
    LPUART_DRV_InstallRxCallback(instance, xmodem_uart_handler, NULL);
    LPUART_DRV_ReceiveData(instance, lpuart0_default_rx_buffer, 8);
    xmodem_init();
}

/**
 * \brief   升级程序
 */
status_t update(uint8_t instance)
{
    status_t ret;
    update_init(instance);
    ret = updating(instance);
//    FLEXCAN_DRV_Deinit(INST_CANCOM0);
    LPUART_DRV_Deinit(instance);
    return ret;
}
void software_reset(void)
{
    __asm volatile ("dsb");
    S32_SCB->AIRCR = ((0x5FA << S32_SCB_AIRCR_VECTKEY_SHIFT)      |
            S32_SCB_AIRCR_SYSRESETREQ_MASK);
    __asm volatile ("dsb");
    while(1);                     /* wait until reset */
}
