/**
 * @file LMP91K_Temp.h
 * @brief LMP91000 温度センサ
 */
#ifndef _LMP91K_TEMP_H_
#define _LMP91K_TEMP_H_

#include "LMP91K.h"
#include "SensorInfo.h"
#include "SenseBuffer.h"

class LMP91K_Temp : public SensorInfo {
protected:
        LMP91K  *dev;
public:
        LMP91K_Temp ( LMP91K *dev_param );
        ~LMP91K_Temp();
        void    initialize ( void ) override {};
        void    get ( SenseBuffer *buff ) override;
        size_t  build ( char *p, SenseBuffer *buff ) override;
        void    stop ( int sec ) override {};
        void    start ( void ) override {};
        const char *name ( void ) override;
        const char *unit ( void ) override;
        const char *data_class_name ( void ) override;

        static const uint8_t    precision = 1;
        esp_err_t               err_code;
};

#endif /* _LMP91K_TEMP_CPP_ */