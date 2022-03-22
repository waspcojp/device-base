/**
 * @file SPI.cpp
 * @brief SPI バスドライバ
 */
#include "hardware.h"

#ifdef  USE_SPI
#include "SPI.h"
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <esp_log.h>
#include <esp_intr_alloc.h>

/**
 * コンストラクタ
 */
SPI::SPI ( void )
{
        spi_bus_config_t buscfg;
        buscfg.mosi_io_num = SPI_MOSI_IO_NUM;
        buscfg.miso_io_num = SPI_MISO_IO_NUM;
        buscfg.sclk_io_num = SPI_SCLK_IO_NUM;
        buscfg.quadhd_io_num = -1;
        buscfg.quadwp_io_num = -1;
        buscfg.max_transfer_sz = 32;
        buscfg.flags = SPICOMMON_BUSFLAG_NATIVE_PINS;
        buscfg.intr_flags = 0;//ESP_INTR_FLAG_INTRDISABLED;

        ESP_ERROR_CHECK ( spi_bus_initialize ( SPI_HOST_DEVICE, &buscfg, SPI_DMA_CH_AUTO ));
}

/**
 * デストラクタ
 */
SPI::~SPI ( void )
{
        ESP_ERROR_CHECK ( spi_bus_free ( SPI_HOST_DEVICE ) );
}

/**
 * SPI バス上にデバイスを追加する
 * 
 * @param[in]   conf    バスデバイス設定パラメータ
 * @return SPI デバイスハンドル
 */
spi_device_handle_t SPI::device_add ( spi_device_interface_config_t *conf )
{
        spi_device_handle_t dev = NULL;
        ESP_ERROR_CHECK ( spi_bus_add_device ( SPI_HOST_DEVICE, conf, &dev ) );
        return dev;
}


/**
 * SPI バスからデバイスを削除する
 * 
 * @param[in]   dev     SPI デバイスハンドル
 * @return      正常終了時は 0，異常時は非 0
 */
esp_err_t SPI::device_remove ( spi_device_handle_t dev )
{
        return spi_bus_remove_device ( dev );
}


/**
 * トランザクション（コマンド送信〜応答受信）の実行 
 * 
 * SPI デバイスと論理的な半二重通信を行う．
 * コマンド送信時に MISO から送られてきたデータは破棄されて rxbuff に
 * 格納される．
 * 
 * @param[in]           dev     通信先 SPI デバイス
 * @param[in]           txbuff  送信バッファ
 * @param[in]           txlen   送信バイト数
 * @param[out]          rxbuff  受信バッファ
 * @param[in]           rxlen   受信バイト数
 * 
 * @return 正常終了時は ESP_OK, 異常時はエラーコード
 */
esp_err_t SPI::transaction ( spi_device_handle_t dev,
                                uint8_t *txbuff, size_t txlen,
                                uint8_t *rxbuff, size_t rxlen )
{
        uint8_t txb[txlen + rxlen], rxb[txlen + rxlen];

        spi_transaction_t trans;
        trans.flags             = 0;
        trans.cmd               = 0;
        trans.addr              = 0,
        trans.length            = 8 * ( txlen + rxlen ); /* in bits */
        trans.tx_buffer         = txb;
        trans.rx_buffer         = rxb;
        trans.rxlength          = 8 * ( txlen + rxlen );
        trans.user              = 0;

        memset ( txb, 0, sizeof(txb) );
        memcpy ( txb, txbuff, txlen );

        spi_transaction_t *result = NULL;

        esp_err_t s;

        /* バスのロック */
        for ( int i = 10; i > 0; --i ) {
                s = spi_device_acquire_bus( dev, portMAX_DELAY );
                if ( s == ESP_OK ) {
                        goto ok;
                }
        }
        return s;
ok:
        //ESP_LOGI ("", "%s: bus lock OK.", __func__);
        s = spi_device_queue_trans ( dev, &trans, 1000 / portTICK_PERIOD_MS );
        if ( s != ESP_OK ) {
                goto ng;
        }

        //ESP_LOGI ("", "%s: queue ok.", __func__ );
        for ( int i = 10; i > 0; --i ) {
                s = spi_device_get_trans_result ( dev, &result, 1000 / portTICK_PERIOD_MS );
                if ( s == ESP_OK ) {
                        goto ok2;
                }
        }
ok2:
        //ESP_LOGI ("", "%s: get result ok.", __func__ );
        memcpy ( rxbuff, rxb + txlen, rxlen );

ng:
        /* バスのアンロック */
        spi_device_release_bus ( dev );
        return s;
}
#endif
