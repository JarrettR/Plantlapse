#ifndef _REST_SERVER_H_
#define _REST_SERVER_H_

#include "esp_system.h"
#include "esp_log.h"


#ifdef __cplusplus
extern "C" {
#endif


esp_err_t start_rest_server(const char *base_path);


#ifdef __cplusplus
}
#endif


#endif
