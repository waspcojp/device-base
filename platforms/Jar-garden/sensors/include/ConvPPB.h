/**
 * @file ConvPPB.h
 * 
 * @brief センサ数値計算レイヤ
 */
#ifndef _CONVPPB_H_
#define _CONVPPB_H_

extern "C" {
#include <freertos/FreeRTOS.h>
#include <esp_err.h>
}

#include "Mat22.h"
#include "SensorInfo.h"
#include "LMP91K.h"
#include "Filter.h"
#include "Volt_Filter.h"


#define APP_SENSOR_CALIBRATE_LOG_SIGNATURE      0xdeadbeef;

class ConvPPB {
private:
        SensorInfo      *sensor_temp;
        static ConvPPB  *self;

public:
        /**
         * 校正ログ要素
         */
        typedef struct {
                float          temperature;    /**< 校正時の気温[℃] */
                float          humidity;       /**< 校正時湿度[％] */
                float          pressure;       /**< 校正時気圧[Pa] */
                float          o3_ppb;         /**< 校正ガス O3 濃度 [ppb] */
                float          o3_volt;        /**< 校正時 O3 センサ電圧 [V] */
                float          nox_ppb;        /**< 校正ガス NOx 濃度 [ppb] */
                float          nox_volt;       /**< 校正時 NOx センサ電圧 [V] */
                unsigned       signature;
        } calibrate_log_t;


        /**
         * センサ種別
         */
        typedef enum {
                SENSOR_O3,
                SENSOR_NOX,
        } gas_sensor_t;



        /**
         * センサごとの NVRAM 保存データ
         */
        typedef struct {
                char            name[16];               /**< センサ名 */
                gas_sensor_t    sensor;                 /**< センサ種別 */
                float           v_Zero;                /**< ゼロ点電圧 */
                float           nA_per_PPM_x100;        /**< 感度 */
        } calibrate_t;


        /**
         * センサごとの ppb 値計算パラメータ
         * 
         * こちらは定数．校正で変更はしない．
         */
        typedef struct {
                float  Zcf_low;
                float  Zcf_high;
                float  Scf_low;
                float  Scf_high;
                float  Rgain;
        } const_param_t;

        /**
         * ppb 値のペア
         */
        typedef struct {
                float o3;
                float nox;
        } ppb_pair_t;


        float ppb_single ( const float vout, const float T_float, 
                const calibrate_t *cal );
        esp_err_t calibrate_single ( calibrate_t *calib,
                const calibrate_log_t *callog_zero,
                const calibrate_log_t *callog_span );

        esp_err_t ppb_cross ( ppb_pair_t *ppb_c,
                const ppb_pair_t *ppb_s,
                const Mat22 *coeff );
        esp_err_t calibrate_cross ( Mat22 *coeff,
                const calibrate_t *cal_o3,
                const calibrate_t *cal_no2,
                const calibrate_log_t *cal_span1,
                const calibrate_log_t *cal_span2 );

        Volt_Filter     *vf_o3, *vf_nox;

        /** 校正データ．ゼロ，O3校正ガス，NOx校正ガス */
        calibrate_log_t  callog_zero, callog_span_o3, callog_span_nox;

        /** 電圧→濃度　変換パラメータ */
        calibrate_t      cal_o3, cal_nox;

        /** 交差感度補正行列*/
        Mat22            cal_cross;

        /** 計測値 */
        float   v_raw_o3, v_raw_nox;                    /**< A/D 出力電圧．処理前 */
        float   v_o3, v_nox, temp_c;                    /**< A/D 出力電圧．フィルタリング後 */
        float   v_slow_o3, v_slow_nox;                  /**< A/D 出力電圧．校正用狭帯域フィルタ */
        float   ppb_single_o3, ppb_single_nox;          /**< 単体校正値を用いた ppb 値 */
        float   ppb_cross_o3, ppb_cross_nox;            /**< 交差感度補正後の ppb 値 */
        float   ppb_cross_f_o3, ppb_cross_f_nox;        /**< 廃止予定: ppb_cross_o3, ppb_cross_nox と同じ */

        ConvPPB ( SensorInfo *temp, Volt_Filter *p_o3, Volt_Filter *p_nox );
        ~ConvPPB ( void );
        void measure_cal ( calibrate_log_t *p );

        esp_err_t measure ( void );

        /**
         * ConvPPB インスタンスへのポインタの取得
         * 
         * @return ConvPPB* ポインタ
         */
        ConvPPB *get_instance ( void ) { return self; }
};
#endif /* _CONVPPB_H_ */