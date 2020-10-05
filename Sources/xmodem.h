#ifndef __XMODEM_H__
#define __XMODEM_H__

#include <stdbool.h>
#include "S32K144.h"
#include "status.h"
#include "mflexcan.h"

#define APP_IMAGE_START     0xA000

/*
 * \brief   XMODEM模式下载初始化
 * \details 对应用程序首地址, 串口接收队列初始化
 */
void xmodem_init(void);

/*
 * \brief   XMODEM接收数据,并且烧录到
 * \retval  STATUS_SUCCESS  接收成功
 * \        STATUS_ERROR    接收错误
 */
status_t xmodem_download(uint8_t instance);

/*
 * \brief   向上位机请求数据
 * \details XMODEM下载模式需要单片机向上位机发起传输请求,
 *          上位机接收到请求后开始发送数据,该请求发送20ms
 *          之后才检测上位机是否响应
 * \retval  ture    上位机有响应
 *          false   上位机没有响应
 */
bool xmodem_is_active(uint8_t instance);

/*
 * \brief   向串口接收缓冲区末尾添加一字节数据
 * \param   byte    添加的数据
 */
void xmodem_queue_byte(uint8_t byte);

/*
 * \brief   向串口发送一字节数据
 * \param   x   发送的数据
 */
void xmodem_putchar(uint8_t instance, uint8_t byte);

/*
 * \brief   判断数据缓冲区书否为有数据
 * \retval  true    数据缓冲区中有数据
 *          false   数据缓冲区为空
 */
bool xmodem_getchar_present(void);

/*
 * \brief   串口接收中断回掉函数
 * \param   instance    串口序号
 * \param   lpuartState 串口状态
 */
void xmodem_uart_handler(uint32_t instance, void * lpuartState);

/*
 * \brief   CAN接收中断回掉函数
 * \param   instance    CAN序号
 * \param   eventType   中断事件类型
 * \param   state       CAN状态
 */
void xmodem_can_handler(uint8_t instance, flexcan_event_type_t eventType,
        struct FlexCANState * state);
#endif
