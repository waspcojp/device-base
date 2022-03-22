/**
 * @file LMP91K_Volt.h
 * @brief LMP91000 センサ出力電圧
 */
#ifndef _LMP91K_VOLT_H_
#define _LMP91K_VOLT_H_

#include "Volt_Filter.h"
#include "SensorInfo.h"
#include "SenseBuffer.h"

class LMP91K_Volt : public SensorInfo {
protected:
        Volt_Filter     *vf;
        char            ins_name[64];
public:
        LMP91K_Volt ( Volt_Filter *p_vf, const char *p_name );
        ~LMP91K_Volt();
        void    initialize ( void ) override {};
        void    get ( SenseBuffer *buff ) override;
        size_t  build ( char *p, SenseBuffer *buff ) override;
        void    stop ( int sec ) override {};
        void    start ( void ) override {};
        const char *name ( void ) override;
        const char *unit ( void ) override;
        const char *data_class_name ( void ) override;
        float   value ( void ) override;

        static const uint8_t    precision = 4;
        esp_err_t               err_code;
};

#endif /* _LMP91K_VOLT_H_ */