/**
 * @file FIRFilter.h
 * @brief FIR フィルタ
 * 
 */
#ifndef _FIRFILTER_H_
#define _FIRFILTER_H_

#include "Filter.h"



extern const float *tap_coeff_example;
extern const int tap_coeff_example_length;

class FIRFilter : public Filter {
protected:
        int     ntap;
        int     offset;
        float   *coeff;
        float   *tap;
public:
        FIRFilter ( const float *coeff_p, int ntap_p );
        ~FIRFilter ( void );
        float filter ( const float x );
};

#endif