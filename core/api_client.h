#ifndef	__INC_API_CLIENT_H__
#define	__INC_API_CLIENT_H__

#include	"esp_http_client.h"
#include	"types.h"

extern	void	initialize_api(void);
extern	Bool	api_device_new(esp_http_client_handle_t client, char *user, char *pass, char *name, char *description);
extern	Bool	api_session_new(esp_http_client_handle_t client, char *user, char *pass);
extern	Bool	api_post_data(esp_http_client_handle_t client, esp_http_client_method_t method, const char *path, uint8_t *data, size_t size);
extern	int		api_get_data(esp_http_client_handle_t client, const char *path, uint8_t *data, size_t size, int *status);
extern	Bool	api_exec_ota(void);
extern	void	api_relogin_device(esp_http_client_handle_t client);

#endif
