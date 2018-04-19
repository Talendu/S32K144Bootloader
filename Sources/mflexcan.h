/*
 * flexcan.h
 *
 *  Created on: 2018年4月8日
 *      Author: Administrator
 */

#ifndef MFLEXCAN_H_
#define MFLEXCAN_H_

#include "S32K144.h"
#include "clockMan1.h"
#include "canCom0.h"
#include "pin_mux.h"
#include "dmaController1.h"
#include "osif.h"
#include "xmodem.h"

#define RECEIVE_STD_MB  8               /**< \brief CAN标准帧接收邮箱号 */
#define RECEIVE_EXT_MB  9               /**< \brief CAN扩展帧接收邮箱号 */

#define TRANSMIT_STD_MB 10              /**< \brief CAN标准帧发送邮箱号 */
#define TRANSMIT_EXT_MB 11              /**< \brief CAN扩展帧发送邮箱号 */

//#define RXMB_GLOBALMASK 0X7FFFFFFF
#define RXMB_GLOBALMASK 0x00000000      /**< CAN接收邮箱全局ID掩码 */
#define RXID_UPDATE     0x00000555      /**< CAN升级程序ID */

extern flexcan_msgbuff_t   g_can_receive_buff;  /**< can接收到的最后一个数据包*/

/**
 * \brief   接收邮箱信息
 */
extern flexcan_data_info_t rx_info;

/**
 * \brief   发送邮箱信息
 */
extern flexcan_data_info_t tx_info;

/**
 * \brief   初始化CAN
 */
void init_flexcan(void);

/**
 * \brief       获取can0时钟频率
 * \param[out]  flexcanSourceClock  时钟频率
 */
void flexcan_get_source_clock(uint32_t *flexcanSourceClock);



#endif /* MFLEXCAN_H_ */
