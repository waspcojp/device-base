/**
 * @file Mat22.h
 * 
 * @brief 2x2 行列クラスと  ベクトルクラス
 */
#ifndef _APP_MAT22_H_
#define _APP_MAT22_H_

/**
 * 2x2 行列クラス
 * 
 * a b
 * c d
 */
class Mat22 {
public:
        float a, b, c, d;
        
        Mat22 ( const float ap = 0, const float bp = 0, 
                const float cp = 0, const float dp = 0 ) {
                a = ap;
                b = bp;
                c = cp;
                d = dp;
        }
        Mat22 operator + ( const Mat22 &x ) {
                return Mat22 ( 
                        a + x.a,        b + x.b,
                        c + x.c,        d + x.d
                );
        }
        Mat22 operator - ( const Mat22 &x ) {
                return Mat22 (
                        a - x.a,        b - x.b,
                        c - x.c,        d - x.d
                );
        }
        Mat22 operator * ( const Mat22 &x ) {
                return Mat22 (
                        a * x.a + b * x.c,      a * x.b + b * x.d,
                        c * x.a + d * x.c,      c * x.b + d * x.d
                );
        }
        /**
         * 行列式
         */
        double det ( void ) {
                return a*d - b*c;
        }
        /**
         * 逆行列
         */
        Mat22 inv ( void ) {
                float dd = det();
                if ( dd == 0.0 ) {
                        return Mat22(0,0,0,0);     /**< @todo エラー処理 */
                }
                return Mat22 (
                        d/dd,           -b/dd,
                        -c/dd,          a/dd
                );
        }

};

/**
 * 行列のスカラ倍
 */
inline Mat22 operator * ( float x, Mat22 y )
{
        return Mat22 (
                x*y.a,          x*y.b,
                x*y.c,          x*y.d
        );
}


/**
 * 1 x 2 ベクトルクラス
 * 
 * x
 * y
 */
class Vect2 {
public:
        float  x, y;
        /**
         * コンストラクタ
         * 
         * @param[in]   xp      ベクトルの第一要素．省略時は 0 が入る
         * @param[in]   yp      ベクトルの第二要素．省略時は 0 が入る
         */
        Vect2 ( const float xp = 0, const float yp = 0 )
        {
                x = xp;
                y = yp;
        }
        Vect2 operator + ( const Vect2 &vp )
        {
                return Vect2 (
                        x + vp.x,
                        y + vp.y
                );
        }
        Vect2 operator - ( const Vect2 &vp )
        {
                return Vect2 (
                        x - vp.x,
                        y - vp.y
                );
        }
};



/**
 * ベクトルのスカラ倍
 */
static inline Vect2 operator * ( const float a, const Vect2 &v )
{
        return Vect2 (
                a*v.x,
                a*v.y
        );
}



/**
 * 行列・ベクトル → ベクトル　演算
 */
static inline Vect2 operator * ( const Mat22 &m, const Vect2 &v )
{
        return Vect2 (
                m.a * v.x + m.b * v.y,
                m.c * v.x + m.d * v.y
        );
}

#endif /* _APP_MAT22_H_ */