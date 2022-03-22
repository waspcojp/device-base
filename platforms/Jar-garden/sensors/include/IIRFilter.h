/**
 * @file IIRFilter.h
 * @brief 1次 IIR フィルタ
 */
#ifndef _IIRFILTER_H_
#define _IIRFILTER_H_
#include "Filter.h"

extern "C" {
#include <stdbool.h>
}

class IIRFilter : public Filter {
protected:
        float   a;
        float   yo;
        bool    first_time;
public:
        IIRFilter ( float coeff ) 
        {
                a = coeff;
                yo = 0.0; 
                first_time = true;
        };
        ~IIRFilter ( void ) {};
        /**
         * フィルタリング
         * 
         * @param[in]   x       入力
         * @return              フィルタリング出力
         */
        float filter ( const float x ) override
        {
                if ( first_time ) {
                        first_time = false;
                        return yo = x;
                }
                return yo = a*x + ( 1.0 - a )*yo;
        };

};

#endif /* _IIRFILTER_H_ */