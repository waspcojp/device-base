/**
 * @file testcode.cpp
 * @brief テスト用コード
 */
#include "config.h"
#include "debug.h"
#ifdef TESTCODE

#ifdef TESTCODE_CONVPPB
#include        "Sensors.h"
#include        "ConvPPB.h"
#include        "PPB_O3.h"
#endif


/**
 * 誤差を考慮した浮動小数点数どうしの比較
 * 
 * 浮動小数点どうしを比較し，誤差範囲内なら true を返す
 * 
 * @param[in] val 値１
 * @param[in] ref 値２
 *
 * @return 誤差範囲内なら true，範囲外なら false
 */
static bool testcode_float_cmp ( float val, float ref )
{
        const float testcode_float_cmp_err = 1e-5;      /* 許容相対誤差 */
        float e;
        if ( ( -1e-10 < ref ) && ( ref < 1e-10 ) ) {
                /* 0 除算を避ける */
                e = val - ref;
        } else {
                e = ( val - ref ) / ref;
        }

        return ( -testcode_float_cmp_err < e ) && ( e < testcode_float_cmp_err );
}

#ifdef TESTCODE_CONVPPB
/**
 * ConvPPB::calibrate_single のテスト
 * 
 * 適当な２点データを与えて単体校正を行い，その校正値を元に ppb 値を計算し，
 * 元の「適当な測定値」と一致するか確かめる
 * 
 */
static bool testcode_ConvPPB_calibrate_single ( void )
{
ENTER_FUNC;
        const char *testfunc = "ConvPPB::calibrate_single";

        PPB_O3 *o3 = (PPB_O3 *)Sensors::find ((char *)"LMP91000_O3");
        ConvPPB *convppb = o3->convppb;
        ConvPPB::calibrate_t    cal_o3;
        esp_err_t err;

        /*
         * 校正値を求める
         */

        /* 「適当な値」 */
        const ConvPPB::calibrate_log_t callog_zero = {
                .temperature    = 20.0,
                .humidity       = 40.0,
                .pressure       = 1024.0,
                .o3_ppb         = 0.0,
                .o3_volt        = 1.2,
        };
        const ConvPPB::calibrate_log_t callog_span= {
                .temperature    = 20.0,
                .humidity       = 30.0,
                .pressure       = 1000.0,
                .o3_ppb         = 1000.0,
                .o3_volt        = 1.0,
        };
        cal_o3.sensor = ConvPPB::SENSOR_O3;
        err = convppb->calibrate_single ( &cal_o3, &callog_zero, &callog_span );
        ESP_LOGI ( "", "%s returns %d", testfunc, err );
        if ( err != ESP_OK ) {
                ESP_LOGI ( "", "%s fail", testfunc );
                LEAVE_FUNC;
                return false;
        }

        /*
         * 校正値の検算
         */
        float ppb;
        bool cmp = true;
        const char *testfunc2 = "ConvPPB::ppb_single";

        ppb = convppb->ppb_single ( callog_zero.o3_volt, callog_zero.temperature, &cal_o3 );
        ESP_LOGI ( "", "%s: returns = %f, expected = %f", testfunc2, ppb, callog_zero.o3_ppb );
        cmp = testcode_float_cmp ( ppb, callog_zero.o3_ppb );

        ppb = convppb->ppb_single ( callog_span.o3_volt, callog_span.temperature, &cal_o3 );
        ESP_LOGI ( "", "%s: returns = %f, expected = %f", testfunc2, ppb, callog_span.o3_ppb );
        cmp = cmp && testcode_float_cmp ( ppb, callog_span.o3_ppb );

        if ( cmp == true ) {
                ESP_LOGI ( "", "%s ok", testfunc );
        } else {
                ESP_LOGI ( "", "%s fail", testfunc );
        }
LEAVE_FUNC;
        return true;
}

#endif /* TESTCODE_CONVPPB */

void testcode ( void )
{
ENTER_FUNC;
#ifdef TESTCODE_CONVPPB
        testcode_ConvPPB_calibrate_single();
#endif
LEAVE_FUNC;
}

#endif /* TESTCODE */