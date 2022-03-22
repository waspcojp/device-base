/**
 * @file PPB_Filtered_O3.h
 * @brief O3 センサ
 */
#ifndef _PPB_FILTERED_O3_H_
#define _PPB_FILTERED_O3_H_

#include "LMP91K.h"
#include "ConvPPB.h"
#include "SensorInfo.h"
#include "SenseBuffer.h"

class PPB_Filtered_O3 : public SensorInfo {
protected:
        ConvPPB         *convppb;
public:
        PPB_Filtered_O3 ( ConvPPB *conv );
        ~PPB_Filtered_O3();
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

#endif /* _PPB_FILTERED_O3_H_ */