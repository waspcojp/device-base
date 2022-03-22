/**
 * @file LMP91K.h
 * 
 * @brief LMP91000 センサードライバ
 */
#ifndef _LMP91K_H_
#define _LMP91K_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <stdint.h>
#include "config.h"
#include "hardware.h"
#include "ADS1220.h"
#include "I2C.h"


/** TIA_GAIN 設定値 */
typedef enum {
        LMP91K_TIA_GAIN_EXT       = 0b000,
        LMP91K_TIA_GAIN_2R75K     = 0b001,
        LMP91K_TIA_GAIN_3R5K      = 0b010,
        LMP91K_TIA_GAIN_7K        = 0b011,
        LMP91K_TIA_GAIN_14K       = 0b100,
        LMP91K_TIA_GAIN_35K       = 0b101,
        LMP91K_TIA_GAIN_120K      = 0b110,
        LMP91K_TIA_GAIN_350K      = 0b111,
} lmp91k_tia_gain_t;

/** RLOAD 設定値 */
typedef enum {
        LMP91K_RLOAD_10           = 0b00,
        LMP91K_RLOAD_33           = 0b01,
        LMP91K_RLOAD_50           = 0b10,
        LMP91K_RLOAD_100          = 0b11,
} lmp91k_rload_t;

/** REF_SOURCE 設定値 */
typedef enum {
        LMP91K_REF_SOURCE_INTERNAL        = 0,
        LMP91K_REF_SOURCE_EXTERNAL        = 1,
} lmp91k_ref_source_t;

/** INT_Z 設定値 */
typedef enum {
        LMP91K_INT_Z_20P          = 0b00,
        LMP91K_INT_Z_50P          = 0x01,
        LMP91K_INT_Z_67P          = 0b10,
        LMP91K_INT_Z_BYPASS       = 0b11,
} lmp91k_int_z_t;

/** BIAS_SIGN 設定値 */
typedef enum {
        LMP91K_BIAS_SIGN_NEGATIVE         = 0,
        LMP91K_BIAS_SIGN_POSITIVE         = 1,
} lmp91k_bias_sign_t;

/** BIAS 設定値 */
typedef enum {
        LMP91K_BIAS_0P    = 0b0000,
        LMP91K_BIAS_1P    = 0b0001,
        LMP91K_BIAS_2P    = 0b0010,
        LMP91K_BIAS_4P    = 0b0011,
        LMP91K_BIAS_6P    = 0b0100,
        LMP91K_BIAS_8P    = 0b0101,
        LMP91K_BIAS_10P   = 0b0110,
        LMP91K_BIAS_12P   = 0b0111,
        LMP91K_BIAS_14P   = 0b1000,
        LMP91K_BIAS_16P   = 0b1001,
        LMP91K_BIAS_18P   = 0b1010,
        LMP91K_BIAS_20P   = 0b1011,
        LMP91K_BIAS_22P   = 0b1100,
        LMP91K_BIAS_24P   = 0b1101,
} lmp91k_bias_t;

/** FET_SHORT 設定値 */
typedef enum {
        LMP91K_FET_SHORT_DISABLE  = 0,
        LMP91K_FET_SHORT_ENABLE   = 1,
} lmp91k_fet_short_t;

/** OP_MODE 設定値 */
typedef enum {
        LMP91K_OP_MODE_SLEEP            = 0b000,
        LMP91K_OP_MODE_2_LEAD           = 0b001,
        LMP91K_OP_MODE_STANDBY          = 0b010,
        LMP91K_OP_MODE_3_LEAD           = 0b011,
        LMP91K_OP_MODE_TEMP_TIA_OFF     = 0b110,
        LMP91K_OP_MODE_TEMP_TIA_ON      = 0b111,

        LMP91K_OP_MODE_UNDEFINED        = 0b100,
} lmp91k_op_mode_t;

/** LMP91K レジスタ設定 */
typedef struct {
        char                      name[16];
        lmp91k_tia_gain_t         tia_gain;
        lmp91k_rload_t            rload;
        lmp91k_ref_source_t       ref_source;
        lmp91k_int_z_t            int_z;
        lmp91k_bias_sign_t        bias_sign;
        lmp91k_bias_t             bias;
        lmp91k_fet_short_t        fet_short;
        lmp91k_op_mode_t          op_mode;
} lmp91k_config_t;

/** O3 センサ用レジスタ設定 */
extern const lmp91k_config_t  lmp91k_config_o3;

/** NOx センサ用レジスタ設定 */
extern const lmp91k_config_t  lmp91k_config_nox;


/** LMP91K クラス */
class LMP91K {
        protected:
                I2C                     *i2c;
                gpio_num_t              gpio;
                ADS1220                 *adc;
                int                     adc_ch;
                lmp91k_config_t         config;

                lmp91k_op_mode_t        mode;
                int                     error;

                //SemaphoreHandle_t       sem;
                SemaphoreHandle_t       sem_lock;

                /** LMP91000 レジスタ一覧 */
                enum {
                        REG_STATUS      = 0x00,
                        REG_LOCK        = 0x01,
                        REG_TIACN       = 0x10,
                        REG_REFCN       = 0x11,
                        REG_MODECN      = 0x12,
                };

                esp_err_t       cs_assert ( void );
                void            cs_negate ( void );

                esp_err_t       devwrite ( int addr, void *buf );
                //esp_err_t       devread  ( int addr, void *buf );

                esp_err_t       set_config ( void );
                esp_err_t       set_mode ( lmp91k_op_mode_t op_mode );

                esp_err_t       lock ( void );
                void            unlock ( void );

        public:
                enum {
                        DEVADDR = 0x48,         /**< LMP91000 I2C バスアドレス */
                };
                LMP91K ( 
                        I2C                     *i2c_param, 
                        const gpio_num_t        gpio_param,
                        ADS1220                 *adc_param,
                        const int               adc_ch,
                        const lmp91k_config_t   *conf );
                ~LMP91K ( void );

                void sleep ( void ) { set_mode ( LMP91K_OP_MODE_SLEEP ); }
                void standby ( void ) { set_mode ( LMP91K_OP_MODE_STANDBY ); }

                esp_err_t       measure_temp ( float *t_param );

                esp_err_t       measure_sensor ( float *v_param );

                esp_err_t       devread  ( int addr, void *buf );
};

#endif /* _LMP91K_H_ */