/* HTTP Restful API Server

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "cJSON.h"

#include "include/rest_server.hpp"

static const char *REST_TAG = "esp-rest";

Settings *websettings;

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".jpg")) {
        type = "image/jpg";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

/* Send HTTP response with the contents of the requested file */
static esp_err_t rest_common_get_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];

    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, "/index.html", sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(REST_TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = rest_context->scratch;
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(REST_TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            ESP_LOGI(REST_TAG, "Chunk size: 0x%02x", read_bytes);
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE(REST_TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(REST_TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* Handler for getting web index */
esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    ESP_LOGI(REST_TAG, "Index");
    httpd_resp_send(req, index_html_gz, sizeof_index_html_gz);
    return ESP_OK;
}

/* Simple handler for getting system handler */
static esp_err_t system_info_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    cJSON_AddStringToObject(root, "idfversion", IDF_VER);
    //cJSON_AddStringToObject(root, "version", esp_ota_get_app_description());
    cJSON_AddNumberToObject(root, "cores", chip_info.cores);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}

/* Handler for returning all settings */
static esp_err_t settings_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "timelapse_enabled", websettings->timelapse_enabled);
    //cJSON_AddBoolToObject(root, "ota_start", websettings->ota_start);
    cJSON_AddNumberToObject(root, "next_time", websettings->next_time);
    cJSON_AddNumberToObject(root, "interval", websettings->interval);
    cJSON_AddNumberToObject(root, "current_set", websettings->current_set);
    cJSON_AddNumberToObject(root, "current_photo", websettings->current_photo);
    cJSON_AddNumberToObject(root, "resolution", websettings->resolution);
    cJSON_AddNumberToObject(root, "format", websettings->format);
    cJSON_AddBoolToObject(root, "autogain", websettings->autogain);
    cJSON_AddBoolToObject(root, "autoexposure", websettings->autoexposure);
    cJSON_AddNumberToObject(root, "gain", websettings->gain);
    cJSON_AddNumberToObject(root, "exposure", websettings->exposure);
    cJSON_AddBoolToObject(root, "verticalflip", websettings->verticalflip);
    cJSON_AddNumberToObject(root, "quality", websettings->quality);
    cJSON_AddNumberToObject(root, "gainceiling", websettings->gainceiling);
    cJSON_AddNumberToObject(root, "brightness", websettings->brightness);
    cJSON_AddBoolToObject(root, "lenscorrection", websettings->lenscorrection);
    cJSON_AddNumberToObject(root, "saturation", websettings->saturation);
    cJSON_AddNumberToObject(root, "contrast", websettings->contrast);
    cJSON_AddNumberToObject(root, "sharpness", websettings->sharpness);
    cJSON_AddBoolToObject(root, "horizontalflip", websettings->horizontalflip);
    cJSON_AddBoolToObject(root, "blackpixelcorrection", websettings->blackpixelcorrection);
    cJSON_AddBoolToObject(root, "whitepixelcorrection", websettings->whitepixelcorrection);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}

/* Handler for returning last few filenames */
static esp_err_t filelist_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();

    cJSON *name = NULL;
    name = cJSON_CreateString("000001.jpg");
    cJSON_AddItemToObject(root, "filename", name);

    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}

/* Simple handler for setting configuration variables */
static esp_err_t settings_post_handler(httpd_req_t *req)
{
    int total_len = req->content_len;
    ESP_LOGI(REST_TAG, "Request length: 0x%x", total_len);
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';
    ESP_LOGI(REST_TAG, "Request content: %s", buf);

    cJSON *root = cJSON_Parse(buf);
    if(root == NULL) {
        ESP_LOGW(REST_TAG, "Request invalid");
        return ESP_FAIL;
    }
    websettings->timelapse_enabled = cJSON_GetObjectItem(root, "timelapse_enabled")->valueint;
    // websettings->next_time = cJSON_GetObjectItem(root, "next_time")->valueint;
    websettings->interval = cJSON_GetObjectItem(root, "interval")->valueint;
    websettings->current_set = cJSON_GetObjectItem(root, "current_set")->valueint;
    websettings->current_photo = cJSON_GetObjectItem(root, "current_photo")->valueint;
    // websettings->resolution = cJSON_GetObjectItem(root, "resolution")->valueint;
    // websettings->format = cJSON_GetObjectItem(root, "format")->valueint;
    websettings->autogain = cJSON_GetObjectItem(root, "autogain")->valueint;
    websettings->autoexposure = cJSON_GetObjectItem(root, "autoexposure")->valueint;
    websettings->gain = cJSON_GetObjectItem(root, "gain")->valueint;
    websettings->exposure = cJSON_GetObjectItem(root, "exposure")->valueint;
    websettings->verticalflip = cJSON_GetObjectItem(root, "verticalflip")->valueint;
    websettings->quality = cJSON_GetObjectItem(root, "quality")->valueint;
    websettings->gainceiling = cJSON_GetObjectItem(root, "gainceiling")->valueint;
    websettings->brightness = cJSON_GetObjectItem(root, "brightness")->valueint;
    websettings->lenscorrection = cJSON_GetObjectItem(root, "lenscorrection")->valueint;
    websettings->saturation = cJSON_GetObjectItem(root, "saturation")->valueint;
    websettings->contrast = cJSON_GetObjectItem(root, "contrast")->valueint;
    websettings->sharpness = cJSON_GetObjectItem(root, "sharpness")->valueint;
    websettings->horizontalflip = cJSON_GetObjectItem(root, "horizontalflip")->valueint;
    websettings->blackpixelcorrection = cJSON_GetObjectItem(root, "blackpixelcorrection")->valueint;
    websettings->whitepixelcorrection = cJSON_GetObjectItem(root, "whitepixelcorrection")->valueint;
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

/* Handler for initiating OTA firmware updates */
static esp_err_t ota_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "raw", esp_random() % 20);
    cJSON_AddNumberToObject(root, "ota_start", 1);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    websettings->ota_start = true;
    return ESP_OK;
}

/* Handler for initiating restart */
static esp_err_t reset_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "raw", esp_random() % 20);
    cJSON_AddNumberToObject(root, "reset_start", 1);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    esp_restart();
    return ESP_OK;
}

/* Handler for initiating single picture */
static esp_err_t snap_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "raw", esp_random() % 20);
    cJSON_AddNumberToObject(root, "snap", 1);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    websettings->snap = true;
    return ESP_OK;
}


esp_err_t web_init(const char *base_path, Settings *settings_in)
{
    websettings = settings_in;
    return start_rest_server(base_path);
}

esp_err_t start_rest_server(const char *base_path)
{
    //REST_CHECK(base_path, "wrong base path", err);
    rest_server_context_t *rest_context = (rest_server_context_t*)calloc(1, sizeof(rest_server_context_t));
    //REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 10;
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(REST_TAG, "Starting HTTP Server");
    //REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);
    httpd_start(&server, &config);

    /* URI handler for fetching system info */
    httpd_uri_t system_info_get_uri = {
        .uri = "/api/get/system",
        .method = HTTP_GET,
        .handler = system_info_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &system_info_get_uri);

    /* URI handler for displaying settings */
    httpd_uri_t settings_get_uri = {
        .uri = "/api/get/settings",
        .method = HTTP_GET,
        .handler = settings_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &settings_get_uri);

    /* URI handler for displaying filenames */
    httpd_uri_t filelist_get_uri = {
        .uri = "/api/get/filelist",
        .method = HTTP_GET,
        .handler = filelist_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &filelist_get_uri);

    /* URI handler for setting configuration variables */
    httpd_uri_t settings_post_uri = {
        .uri = "/api/set/settings",
        .method = HTTP_POST,
        .handler = settings_post_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &settings_post_uri);

    /* URI handler for initiating OTA firmware updates */
    httpd_uri_t ota_set_uri = {
        .uri = "/api/set/ota/start",
        .method = HTTP_GET,
        .handler = ota_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &ota_set_uri);

    /* URI handler for starting picture */
    httpd_uri_t snap_set_uri = {
        .uri = "/api/set/snap/start",
        .method = HTTP_GET,
        .handler = snap_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &snap_set_uri);

    /* URI handler for initiating restart */
    httpd_uri_t reset_set_uri = {
        .uri = "/api/set/reset/start",
        .method = HTTP_GET,
        .handler = reset_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &reset_set_uri);
    
    httpd_uri_t index_get_uri = {
        .uri = "/index.html",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &index_get_uri);

    /* URI handler for getting web server files */
    httpd_uri_t common_get_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &common_get_uri);


    return ESP_OK;

}
