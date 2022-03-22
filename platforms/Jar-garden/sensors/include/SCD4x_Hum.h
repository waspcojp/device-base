/**
 * @file SCD4x_Hum.h
 * @brief SCD4x 湿度センサ
 */
#ifndef _SCD4x_HUM_H_
#define _SCD4x_HUM_H_

#include "SCD4x.h"
#include "SensorInfo.h"
#include "SenseBuffer.h"

class SCD4x_Hum : public SensorInfo {
protected:
        SCD4x  *dev;
public:
        SCD4x_Hum ( SCD4x *dev_param );
        ~SCD4x_Hum();
        void    initialize ( void ) override {};
        void    get ( SenseBuffer *buff ) override;
        size_t  build ( char *p, SenseBuffer *buff ) override;
        void    stop ( int sec ) override {};
        void    start ( void ) override {};
        const char *name ( void ) override;
        const char *unit ( void ) override;
        const char *data_class_name ( void ) override;
	virtual	float	value(void);

        static const uint8_t    precision = 1;
        esp_err_t               err_code;
};

#endif /* _SCD4x_HUM_CPP_ */