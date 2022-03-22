/**
 * @file ADS1220_Temp.h
 * @brief ADS1220 温度センサ
 */
#ifndef _ADS1220_TEMP_H_
#define _ADS1220_TEMP_H_

#include "ADS1220.h"
#include "SensorInfo.h"
#include "SenseBuffer.h"

class ADS1220_Temp : public SensorInfo {
protected:
        ADS1220 *dev;
public:
        ADS1220_Temp ( ADS1220 *ads );
        ~ADS1220_Temp();
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

#endif /* _ADS1220_TEMP_CPP_ */