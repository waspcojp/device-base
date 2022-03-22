/**
 * @file UART2.h
 * @brief UART2 ドライバ
 */
#ifndef _UART2_H_
#define _UART2_H_
#include "hardware.h"
#include <stdarg.h>

void uart2_init ( void );
void uart2_fini ( void );
size_t uart2_write ( const char *buf, size_t bytes );
size_t uart2_read ( uint8_t *buf, size_t bytes );
size_t uart2_read_nb ( char *buf, size_t bytes );

int uart2_puts ( const char *buf );
int uprintf ( const char *fmt, ... );

#endif /* _UART2_H_ */