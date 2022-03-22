/**
 * @file Volt_Filter.h
 * @brief ADC測定電圧のフィルタリング
 */
#ifndef _VOLT_FILTER_H_
#define _VOLT_FILTER_H_

#include "config.h"
#include "LMP91K.h"
#include "Filter.h"
extern "C" {
#include        <freertos/task.h>
#include        <freertos/semphr.h>
#include        <esp_err.h>
}

class Volt_Filter {
private:
        TaskHandle_t            thread;
        SemaphoreHandle_t       sem;
        float                   volt;   /**< 生の電圧値 */
        float                   volt_f; /**< フィルタリングされた電圧値 */
        float                   volt_f_slow; /**< 校正用フィルタリング値 */

public:
        Volt_Filter ( LMP91K *p_lmp91k, Filter *p_filter, Filter *p_filter_slow, int p_int_ms = 1000 );
        ~Volt_Filter ( void );
        float   get_raw ( void );
        float   get_filtered ( void );
        float   get_filtered_slow ( void );

        /* 以下，スレッド用．外部からはアクセスしないこと */
        LMP91K                  *lmp91k;
        Filter                  *filter, *filter_slow;
        int                     interval_ms;

        void            lock ( void );
        void            unlock ( void );
        void            set_raw ( float f );
        void            set_filtered ( float f );
        void            set_filtered_slow ( float f );
};

#endif /* _VOLT_FILTER_H_ */