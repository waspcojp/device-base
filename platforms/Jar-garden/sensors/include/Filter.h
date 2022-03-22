/**
 * @file Filter.h
 * @brief フィルタ基底クラス
 * 
 */
#ifndef _FILTER_H_
#define _FILTER_H_

class Filter {
public:
        virtual float filter ( const float x ) { return x; };
};

#endif /* _FILTER_H_ */