#ifndef _REST_SERVER_H_
#define _REST_SERVER_H_

#include "esp_system.h"
#include "esp_log.h"
#include "include/index.hpp"
#include "include/settings.hpp"



esp_err_t web_init(const char *base_path, Settings *settings_in);
esp_err_t start_rest_server(const char *base_path);


#endif
