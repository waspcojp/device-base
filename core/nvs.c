#define TRACE
#define	DEBUG

#include	<esp_system.h>
#include	<nvs.h>
#include	<nvs_flash.h>
	
#include 	"config.h"
#include	"types.h"
#include	"misc.h"
#include	"nvs.h"
#include 	"debug.h"

static	nvs_handle_t	reset_handle;
static	nvs_handle_t	device_info_handle;
static	nvs_handle_t	sensor_info_handle;

extern	esp_err_t
initialize_nvs(void)
{
	esp_err_t err;

ENTER_FUNC;
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		dbgmsg("nvs_flash_init() error");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
LEAVE_FUNC;
    return	(err);
}
extern	void
open_device_info(void)
{
	esp_err_t err;

	err = nvs_open("deviceinfo", NVS_READWRITE, &device_info_handle);
	ESP_ERROR_CHECK(err);
}

static	Bool
get_str(
	nvs_handle_t	handle,
	char			*key,
	char			*buff,
	size_t			length
)
{
	esp_err_t err;
	size_t	size;

	err = nvs_get_str(handle, key, NULL, &size);
	dbgprintf("get_str size = %d", (int)size);
	if		(	( err == ESP_OK )
			&&	( size <= length ) ) {
		nvs_get_str(handle, key, buff, &length);
		dbgmsg("TRUE");
		return	(TRUE);
	} else {
		dbgmsg("FALSE");
		return	(FALSE);
	}
}

extern	void
open_sensor_info(void)
{
	esp_err_t err;

	//err = nvs_open("sensorinfo", NVS_READWRITE, &sensor_info_handle);
	err = nvs_open("deviceinfo", NVS_READWRITE, &sensor_info_handle);
	ESP_ERROR_CHECK(err);
}

extern	void
close_sensor_info(void)
{
	esp_err_t err;

	err = nvs_commit(sensor_info_handle);
    ESP_ERROR_CHECK(err);
	nvs_close(sensor_info_handle);
}
extern	esp_err_t
put_sensor_blob(
	const	char	*key,
	void	*value,
	size_t	size)
{
	esp_err_t err;

	err = nvs_set_blob(sensor_info_handle, key, value, size);
	return	(err);
}

extern	size_t
get_sensor_blob(
	const	char	*key,
	void	*value,
	size_t	size)
{
	esp_err_t	err;
	size_t	p_size;
	err = nvs_get_str(sensor_info_handle, key, NULL, &p_size);
#if	0
	if ( err != ESP_OK ) {
		return 0;
	}
#endif
	err = nvs_get_blob(sensor_info_handle, key, value, &size);
	return	(size);
}

extern	esp_err_t
put_sensor_int(
	const	char	*key,
	int32_t	value)
{
	esp_err_t	err;

	err = nvs_set_i32(sensor_info_handle, key, value);
	return	(err);
}

extern	esp_err_t
get_sensor_int(
	const	char	*key,
	int32_t	*value)
{
	esp_err_t	err;

	err = nvs_get_i32(sensor_info_handle, key, value);
	return	(err);
}

extern	void
set_device_id(
	char	*uuid)
{
	esp_err_t err;
	err = nvs_set_str(device_info_handle, "device_id", uuid);
	ESP_ERROR_CHECK(err);
}

extern	void
get_device_id(
	char	*uuid)
{
	memclear(uuid, SIZE_UUID + 1);
	get_str(device_info_handle, "device_id", uuid, SIZE_UUID + 1);
}

extern	void
set_session_key(
	char	*uuid)
{
	esp_err_t err;
	err = nvs_set_str(device_info_handle, "session_key", uuid);
	ESP_ERROR_CHECK(err);
}

extern	void
get_session_key(
	char	*uuid)
{
	memclear(uuid, SIZE_UUID + 1);
	get_str(device_info_handle, "session_key", uuid, SIZE_UUID + 1);
}

extern	void
set_my_ssid(
	char	*ssid)
{
	esp_err_t err;
	err = nvs_set_str(device_info_handle, "my_ssid", ssid);
	ESP_ERROR_CHECK(err);
}

extern	void
get_my_ssid(
	char	*ssid)
{
	memclear(ssid, SIZE_SSID + 1);
	if 		( !get_str(device_info_handle, "my_ssid", ssid, SIZE_SSID + 1) ) {
		strcpy(ssid, DEFAULT_SSID);
	}
}

extern	void
set_my_pass(
	char	*pass)
{
	esp_err_t err;
	err = nvs_set_str(device_info_handle, "my_pass", pass);
	ESP_ERROR_CHECK(err);
}

extern	void
get_my_pass(
	char	*pass)
{
	memclear(pass, SIZE_PASS + 1);
	if		( !get_str(device_info_handle, "my_pass", pass, SIZE_PASS + 1) ) {
		strcpy(pass, DEFAULT_PASS);
	}
}

extern	void
set_ap_ssid(
	char	*ssid)
{
	esp_err_t err;
	err = nvs_set_str(device_info_handle, "ap_ssid", ssid);
	ESP_ERROR_CHECK(err);
}

extern	void
get_ap_ssid(
	char	*ssid)
{
	memclear(ssid, SIZE_SSID + 1);
	if		(	( !get_str(device_info_handle, "ap_ssid", ssid, SIZE_SSID + 1) )
			||	( *ssid == 0 ) ) {
#ifdef	DEBUG
		strcpy(ssid, DEFAULT_AP_SSID);
#else
		*ssid = 0;
#endif
	}
}

extern	void
set_ap_pass(
	char	*pass)
{
	esp_err_t err;
	err = nvs_set_str(device_info_handle, "ap_pass", pass);
	ESP_ERROR_CHECK(err);
}

extern	void
get_ap_pass(
	char	*pass)
{
	memclear(pass, SIZE_PASS + 1);
	if		(	( !get_str(device_info_handle, "ap_pass", pass, SIZE_PASS + 1) )
			||	( *pass == 0 ) ) {
#ifdef	DEBUG
		strcpy(pass, DEFAULT_AP_PASS);
#else
		*pass = 0;
#endif
	}
}

extern	void
commit_device_info(void)
{
	esp_err_t err;

	err = nvs_commit(device_info_handle);
    ESP_ERROR_CHECK(err);
}

extern	void
close_device_info(void)
{
	nvs_close(device_info_handle);
}

extern	void
open_reset(void)
{
	esp_err_t err;

	err = nvs_open("reset", NVS_READWRITE, &reset_handle);
	ESP_ERROR_CHECK(err);
}

extern	int
get_reset(void)
{
	esp_err_t err;
	uint8_t		ret;

	ret = 0;
	err = nvs_get_u8(reset_handle, "count", &ret);
	if	( err == ESP_ERR_NVS_NOT_FOUND) {
		ret = 0;
	}
	return	(ret);
}

extern	void
set_reset(
	int		arg)
{
	esp_err_t err;
	uint8_t		ret = (uint8_t)arg;

	err = nvs_set_u8(reset_handle, "count", ret);
    ESP_ERROR_CHECK(err);
}
	
extern	void
commit_reset(void)
{
	esp_err_t err;

	err = nvs_commit(reset_handle);
    ESP_ERROR_CHECK(err);
}

extern	void
close_reset(void)
{
	nvs_close(reset_handle);
}
