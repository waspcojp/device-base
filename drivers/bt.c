#define	TRACE

#include 	"config.h"

#include 	<stdio.h>
#include	<sys/time.h>
#include	<string.h>

#include 	"freertos/FreeRTOS.h"
#include 	"freertos/task.h"
#include    "freertos/semphr.h"

#include    "esp_bt.h"
#include    "esp_bt_main.h"
#include    "esp_gap_bt_api.h"
#include    "esp_bt_device.h"
#include    "esp_spp_api.h"

#include	"bt.h"
#include	"types.h"
#include	"globals.h"
#include	"misc.h"
#include 	"debug.h"

#define SPP_SERVER_NAME         "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME     "ESP_SPP_ACCEPTOR"

static  const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static  const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static  const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;
static  SemaphoreHandle_t   mutex;
#define SIZE_BT_BUFFER          1024
static  char    input_buffer[SIZE_BT_BUFFER]
        ,       *read_pointer
        ,       *input_pointer;
static  size_t  buffer_use;

static  int
DeChar(void)
{
    int c;
    if  ( buffer_use > 0 )  {
        if (xSemaphoreTake(mutex, 1000 / portTICK_PERIOD_MS ) == pdTRUE) {
            c = *read_pointer;
            read_pointer ++;
            if  ( ( read_pointer - input_buffer ) == SIZE_BT_BUFFER )   {
                read_pointer = input_buffer;
            }
            buffer_use --;
            xSemaphoreGive(mutex);
        }
    } else {
        c = -1;
    }
    return  (c);
}
static  void
EnChar(
    int c)
{
    if  ( buffer_use < SIZE_BT_BUFFER)  {
        *input_pointer = c;
        input_pointer ++;
        if  ( ( input_pointer - input_buffer ) == SIZE_BT_BUFFER)  {
            input_pointer = input_buffer;
        }
        buffer_use ++;
    } else {
        dbgmsg("buffer overflow!!");
    }
}
static  char    *
bda2str(
    uint8_t *bda,
    char *str,
    size_t size)
{
    if (bda == NULL || str == NULL || size < 18) {
        return NULL;
    }

    uint8_t *p = bda;
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
                p[0], p[1], p[2], p[3], p[4], p[5]);
    return str;
}

static  void
esp_spp_cb(
    esp_spp_cb_event_t  event,
    esp_spp_cb_param_t  *param)
{
    char bda_str[18] = {0};

    switch (event) {
      case ESP_SPP_INIT_EVT:
        if (param->init.status == ESP_SPP_SUCCESS) {
            dbgmsg("ESP_SPP_INIT_EVT");
            esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
        } else {
            dbgprintf("ESP_SPP_INIT_EVT status:%d", param->init.status);
        }
        break;
      case  ESP_SPP_UNINIT_EVT:
        dbgmsg("ESP_SPP_UNINIT_EVT");
        break;
      case ESP_SPP_DISCOVERY_COMP_EVT:
        dbgmsg("ESP_SPP_DISCOVERY_COMP_EVT");
        break;
      case ESP_SPP_OPEN_EVT:
        dbgmsg("ESP_SPP_OPEN_EVT");
        break;
      case ESP_SPP_CLOSE_EVT:
        dbgprintf("ESP_SPP_CLOSE_EVT status:%d handle:%d close_by_remote:%d",
                    param->close.status,
                    param->close.handle,
                    param->close.async);
        break;
      case ESP_SPP_START_EVT:
        if  ( param->start.status == ESP_SPP_SUCCESS )  {
            dbgprintf("ESP_SPP_START_EVT handle:%d sec_id:%d scn:%d",
                    param->start.handle,
                    param->start.sec_id,
                    param->start.scn);
            esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        } else {
            dbgprintf("ESP_SPP_START_EVT status:%d", param->start.status);
        }
        break;
      case ESP_SPP_CL_INIT_EVT:
        dbgmsg("ESP_SPP_CL_INIT_EVT");
        break;
      case ESP_SPP_DATA_IND_EVT:
        dbgprintf("ESP_SPP_DATA_IND_EVT len:%d handle:%d",
                    param->data_ind.len,
                    param->data_ind.handle);
        if (xSemaphoreTake(mutex, 1000 / portTICK_PERIOD_MS ) == pdTRUE) {
            for ( int i = 0; i < param->data_ind.len ; i ++ )   {
                EnChar(param->data_ind.data[i]);
            }
            xSemaphoreGive(mutex);
        }
        break;
      case ESP_SPP_CONG_EVT:
        dbgmsg("ESP_SPP_CONG_EVT");
        break;
      case ESP_SPP_WRITE_EVT:
        dbgmsg("ESP_SPP_WRITE_EVT");
        break;
      case ESP_SPP_SRV_OPEN_EVT:
        dbgprintf("ESP_SPP_SRV_OPEN_EVT status:%d handle:%d, rem_bda:[%s]",
                    param->srv_open.status,
                    param->srv_open.handle,
                    bda2str(param->srv_open.rem_bda, bda_str, sizeof(bda_str)));
        break;
      case ESP_SPP_SRV_STOP_EVT:
        dbgmsg("ESP_SPP_SRV_STOP_EVT");
        break;
      default:
        break;
    }
}

static  void
esp_bt_gap_cb(
    esp_bt_gap_cb_event_t   event,
    esp_bt_gap_cb_param_t   *param)
{
    char    bda_str[18] = {0};

    switch (event) {
      case  ESP_BT_GAP_DISC_RES_EVT:
        dbgmsg("ESP_BT_GAP_DISC_RES_EVT");
        break;
/*      case  SP_BT_GAP_DISC_STATE_CHANGED_EVT:
        dbgmsg("SP_BT_GAP_DISC_STATE_CHANGED_EVT");
        break;
*/
      case  ESP_BT_GAP_RMT_SRVCS_EVT:
        dbgmsg("ESP_BT_GAP_RMT_SRVCS_EVT");
        break;
      case  ESP_BT_GAP_RMT_SRVC_REC_EVT:
        dbgmsg("ESP_BT_GAP_RMT_SRVC_REC_EVT");
        break;
      case ESP_BT_GAP_AUTH_CMPL_EVT:
        if  ( param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS )  {
            dbgprintf("authentication success: %s bda:[%s]", param->auth_cmpl.device_name,
                bda2str(param->auth_cmpl.bda, bda_str, sizeof(bda_str)));
        } else {
            dbgprintf("authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
      case ESP_BT_GAP_PIN_REQ_EVT:
        dbgprintf("ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if  ( param->pin_req.min_16_digit ) {
            dbgmsg("Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            dbgmsg("Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
#if (CONFIG_BT_SSP_ENABLED == true)
      case ESP_BT_GAP_CFM_REQ_EVT:
        dbgprintf("ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
      case ESP_BT_GAP_KEY_NOTIF_EVT:
        dbgprintf("ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
      case ESP_BT_GAP_KEY_REQ_EVT:
        dbgmsg("ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif
      case  ESP_BT_GAP_READ_RSSI_DELTA_EVT:
        dbgmsg("ESP_BT_GAP_READ_RSSI_DELTA_EVT");
        break;
      case  ESP_BT_GAP_CONFIG_EIR_DATA_EVT:
        dbgmsg("ESP_BT_GAP_CONFIG_EIR_DATA_EVT");
        break;
      case  ESP_BT_GAP_SET_AFH_CHANNELS_EVT:
        dbgmsg("ESP_BT_GAP_SET_AFH_CHANNELS_EVT");
        break;
      case  ESP_BT_GAP_READ_REMOTE_NAME_EVT:
        dbgmsg("ESP_BT_GAP_READ_REMOTE_NAME_EVT");
        break;
      case ESP_BT_GAP_MODE_CHG_EVT:
        dbgprintf("ESP_BT_GAP_MODE_CHG_EVT mode:%d bda:[%s]", param->mode_chg.mode,
                 bda2str(param->mode_chg.bda, bda_str, sizeof(bda_str)));
        break;
      case  ESP_BT_GAP_REMOVE_BOND_DEV_COMPLETE_EVT:
        dbgmsg("ESP_BT_GAP_REMOVE_BOND_DEV_COMPLETE_EVT");
        break;
      case  ESP_BT_GAP_QOS_CMPL_EVT:
        dbgmsg("ESP_BT_GAP_QOS_CMPL_EVT");
        break;
      default:
        dbgprintf("event: %d", event);
        break;
    }
    return;
}

extern  void
initialize_bt(void)
{
    esp_err_t   ret;

    input_pointer = read_pointer = input_buffer;
    buffer_use = 0;

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if      ( ( ret = esp_bt_controller_init(&bt_cfg) ) != ESP_OK )   {
        dbgprintf("initialize controller failed: %s", esp_err_to_name(ret));
        return;
    }

    if      ( ( ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT )) != ESP_OK )  {
        dbgprintf("enable controller failed: %s", esp_err_to_name(ret));
        return;
    }

    if      ( ( ret = esp_bluedroid_init() ) != ESP_OK ) {
        dbgprintf("initialize bluedroid failed: %s", esp_err_to_name(ret));
        return;
    }

    if      ( ( ret = esp_bluedroid_enable()) != ESP_OK )   {
        dbgprintf("enable bluedroid failed: %s", esp_err_to_name(ret));
        return;
    }

    if      ( ( ret = esp_bt_gap_register_callback(esp_bt_gap_cb) ) != ESP_OK ) {
        dbgprintf("gap register failed: %s", esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        dbgprintf("spp register failed: %s", esp_err_to_name(ret));
        return;
    }

    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK) {
        dbgprintf("spp init failed: %s", esp_err_to_name(ret));
        return;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);

    mutex = xSemaphoreCreateMutex();
}