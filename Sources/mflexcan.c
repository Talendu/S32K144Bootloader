/*
 * flexcan.c
 *
 *  Created on: 2018年4月8日
 *      Author: Administrator
 */
#include "mflexcan.h"

flexcan_msgbuff_t g_can_receive_buff;

/**
 * \brief   接收邮箱信息
 */
flexcan_data_info_t g_rx_info = {
        .data_length = 8,
        .enable_brs = 1,
        .fd_enable = 0,
        .fd_padding = 0,
        .is_remote = 0,
        .msg_id_type = FLEXCAN_MSG_ID_STD
};

/**
 * \brief   发送邮箱信息
 */
flexcan_data_info_t g_tx_info = {
        .data_length = 8,
        .enable_brs = 1,
        .fd_enable = 0,
        .fd_padding = 0,
        .is_remote = 0,
        .msg_id_type = FLEXCAN_MSG_ID_STD
};

/**
 * \brief   初始化CAN
 */
void flexcan_init(void) {
    /* 先将can复位 */
    FLEXCAN_DRV_Deinit(INST_CANCOM0);
    /* 配置CAN参数 */
    FLEXCAN_DRV_Init(0, &canCom0_State, &canCom0_InitConfig);
    /* 设置全局屏蔽码 */
    FLEXCAN_DRV_SetRxMbGlobalMask(0, FLEXCAN_MSG_ID_EXT, RXMB_GLOBALMASK);
    /* 配置接收邮箱 */
    FLEXCAN_DRV_ConfigRxMb(0, RECEIVE_STD_MB, &g_rx_info, RXID_UPDATE);
    /* 设置接收回掉函数 */
    FLEXCAN_DRV_InstallEventCallback(0, xmodem_can_handler, NULL);
    /* 开始接收 */
    FLEXCAN_DRV_Receive(0, RECEIVE_STD_MB, &g_can_receive_buff);
}


/**
 * \brief       获取can0时钟频率
 * \param[out]  flexcanSourceClock  时钟频率
 */
void flexcan_get_source_clock(uint32_t *flexcanSourceClock) {
    if (canCom0_InitConfig.pe_clock == FLEXCAN_CLK_SOURCE_SYS){
        CLOCK_SYS_GetFreq(CORE_CLOCK, flexcanSourceClock);
    } else {
        uint32_t i = (SCG->SOSCDIV & SCG_SOSCDIV_SOSCDIV2_MASK) >> SCG_SOSCDIV_SOSCDIV2_SHIFT;
        CLOCK_SYS_GetFreq(SOSC_CLOCK, flexcanSourceClock);
        for (i=i-1; i>0; i--) {
            *flexcanSourceClock >>= 1;
        }
    }
}

