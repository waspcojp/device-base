/**
 * @file UART2.cpp
 * @brief UART2 ドライバ
 */
#include "config.h"
#ifdef   USE_UART2
#include <hal/uart_types.h>
#include <driver/uart.h>
#include "UART2.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * 初期化
 * 
 * UART ポートの初期化とピン割り当て
 */
void uart2_init ( void )
{
        uart_config_t uart_config = {
                .baud_rate =            UART2_BAUD_RATE,
                .data_bits =            UART2_DATA_BITS,
                .parity =               UART2_PARITY,
                .stop_bits =            UART2_STOP_BITS,
                .flow_ctrl =            UART2_FLOW_CTRL,
                .rx_flow_ctrl_thresh =  UART2_RX_FLOW_CTRL_THRESH
        };

        ESP_ERROR_CHECK(uart_param_config ( UART2_UART_NUM, &uart_config));
        ESP_ERROR_CHECK(uart_set_pin(UART2_UART_NUM, UART2_TX_PORT, UART2_RX_PORT, -1, -1));
        ESP_ERROR_CHECK(
                uart_driver_install(UART2_UART_NUM, UART2_BUFFER_SIZE,
                        UART2_BUFFER_SIZE, 0, NULL, 0));
}


/**
 * UART ドライバの終了処理
 */
void uart2_fini ( void )
{
        ESP_ERROR_CHECK( uart_driver_delete ( UART2_UART_NUM ) );
}



/**
 * バイト列の送信
 * 
 * バイト列を送信バッファへ格納する．
 * 送信バッファへの格納が終了するまでブロックされる．
 * 
 * @param[in]   buf     送信バッファ
 * @param[in]   bytes   送信バイト数
 * @return 送出したバイト数．エラー時は -1
 */
size_t uart2_write ( const char *buf, size_t bytes )
{
        return uart_write_bytes ( UART2_UART_NUM, buf, bytes );
}



/**
 * バイト列の受信
 * 
 * UART から指定されたバイト数だけ受信する．
 * 指定バイト数に満たない間，ブロックされる
 * 
 * @param[out]  buf     受信バイト列が格納されるバッファ
 * @param[in]   bytes   受信バイト数
 */
size_t uart2_read ( uint8_t *buf, size_t bytes )
{
        size_t b = bytes;
        while ( b > 0 ) {
                int s = uart_read_bytes ( UART2_UART_NUM, buf, b, 10000 );
                if ( s > 0 ) {
                        b -= s;
                        buf += s;
                }
        }
        return bytes;
}



/**
 * バイト列の受信．ノンブロック版
 * 
 * UART 受信バッファにある文字列を指定されたバイト数だけ受信する．
 * 指定バイト数に満たない場合でもブロックしない
 * 
 * @param[out]  buf     受信バイト列が格納されるバッファ
 * @param[in]   bytes   バイト長
 * 
 * @return 実際に受信されたバイト長．エラー時は -1
 */
size_t uart2_read_nb ( char *buf, size_t bytes )
{
        size_t l;
        ESP_ERROR_CHECK ( uart_get_buffered_data_len ( UART2_UART_NUM, &l));
        if ( l > bytes ) {
                l = bytes;
        }

        return uart_read_bytes ( UART2_UART_NUM, buf, l, 1 );
}

/**
 * 文字列の送信
 * 
 * \0 終端された文字列を送信する
 * 
 * @param[in]   buf     送信する文字列 
 * @return 送信した文字数．エラー時は負数を返す
 */
int uart2_puts ( const char *buf )
{
        int len = strlen ( buf );
        return uart2_write ( buf, len );
}


/**
 * UART 出力の printf
 * 
 * 
 * @param[in] fmt       printf フォーマット文字列
 *
 * @return フォーマット後でバッファ制限が無ければ出力されたであろう文字数
 */
int uprintf ( const char *fmt, ... )
{
        char buf[128];
        int len;
        va_list ap;

        va_start ( ap, fmt );
        len = vsnprintf ( buf, sizeof(buf), fmt, ap );
        if ( len > sizeof(buf) ) {
                len = sizeof(buf);
        }
        uart2_write ( buf, len );
        va_end ( ap );
        return len;
}
#endif
