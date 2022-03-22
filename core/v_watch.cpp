/**
 * @file v_watch.cpp
 * @brief ArduinoIDE シリアルプロッタ用 ADC 電圧出力
 * 
 */
#include        "config.h"
#ifdef  HAVE_LMP91K
#include        "v_watch.h"
#include        "UART2.h"
#include        "Sensors.h"

extern "C" {
#include        <freertos/FreeRTOS.h>
#include        <freertos/task.h>
#include        <esp_log.h>
}

#define INTERVAL_MS     1000

static void v_watch_thread ( void *param )
{
        uart2_init();

        SensorInfo *o3_raw   = Sensors::find("LMP91000_Volt_O3");
        SensorInfo *nox_raw  = Sensors::find("LMP91000_Volt_NOx");
        SensorInfo *o3_cal   = Sensors::find("LMP91000_Volt_Filtered_Slow_O3");
        SensorInfo *nox_cal  = Sensors::find("LMP91000_Volt_Filtered_Slow_NOx");
        if ( o3_raw == NULL ) {
                uprintf ( "o3_raw == NULL\r\n" );
        }
        if ( o3_cal == NULL ) {
                uprintf ( "o3_cal == NULL\r\n" );
        }
        uprintf ( "O3_raw\tO3_cal.\tNOx_raw\tNOx_cal.\r\n");
        TickType_t last_wake = xTaskGetTickCount();
        for (;;) {
                uprintf ( "%f\t%f\t%f\t%f\r\n",
                        1000*o3_raw->value(),
                        1000*o3_cal->value(),
                        1000*nox_raw->value(),
                        1000*nox_cal->value()
                        );
                vTaskDelayUntil ( &last_wake, INTERVAL_MS / portTICK_PERIOD_MS );
        }
}

/**
 * 電圧出力スレッドの起動
 * 
 */
void start_v_watch ( void )
{
        //ESP_LOGI ( "", "%s", __func__);
        xTaskCreate(v_watch_thread, "v_watch_thread", 2048, NULL, 5, NULL);
}
#endif
