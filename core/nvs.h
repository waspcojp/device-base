#ifndef	__INC_NVS_H__
#define	__INC_NVS_H__

#include 	"esp_system.h"

extern	esp_err_t	initialize_nvs(void);
extern	void		open_reset(void);
extern	int			get_reset(void);
extern	void		set_reset(int arg);
extern	void		commit_reset(void);
extern	void		close_reset(void);

extern	void		open_device_info(void);
extern	void		set_device_id(char *uuid);
extern	void		get_device_id(char *uuid);
extern	void		set_session_key(char *uuid);
extern	void		get_session_key(char *uuid);
extern	void		set_my_ssid(char *ssid);
extern	void		get_my_ssid(char *ssid);
extern	void		set_my_pass(char *pass);
extern	void		get_my_pass(char *pass);
extern	void		set_ap_ssid(char *ssid);
extern	void		get_ap_ssid(char *ssid);
extern	void		set_ap_pass(char *pass);
extern	void		get_ap_pass(char *pass);

extern	void		commit_device_info(void);
extern	void		close_device_info(void);

extern	void        open_sensor_info(void);
extern	esp_err_t   put_sensor_blob(const char *key, void *value, size_t size);
extern	size_t      get_sensor_blob(const char *key, void *value, size_t size);
extern	esp_err_t   put_sensor_int(const char *key, int32_t value);
extern	esp_err_t   get_sensor_int(const char *key, int32_t *value);

extern	size_t      get_sensorng(char *key, char *buff);
extern	void        close_sensor_info(void);

#endif
