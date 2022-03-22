/**
 * @file PPB_O3.h
 * @brief O3 センサ
 */
#ifndef _PPB_O3_H_
#define _PPB_O3_H_

#include "BME280_Temp.h"
#include "LMP91K.h"
#include "ConvPPB.h"
#include "SensorInfo.h"
#include "SenseBuffer.h"

class PPB_O3 : public SensorInfo {
public:
        PPB_O3 ( ConvPPB *conv );
        ~PPB_O3();
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
        ConvPPB         *convppb;
        esp_err_t               err_code;
};

#endif /* _PPB_O3_H_ */