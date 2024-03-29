
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_wifi.h"
#include "protocol_examples_common.h"
#include "string.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "esp_spiffs.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "mdns.h"
#include "lwip/apps/netbiosns.h"

#include "nvs.h"
#include "nvs_flash.h"


#include "include/pins.h"
#include "include/settings.hpp"
#include "include/rest_server.hpp"
#include "include/camera.hpp"

#define MOUNT_POINT "/sdcard"

static const char *TAG = "main";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

Settings plSettings;

volatile time_t epochDiff = 0;

#define OTA_URL_SIZE 256 


// DMA channel to be used by the SPI peripheral
#ifndef SPI_DMA_CHAN
#define SPI_DMA_CHAN    1
#endif //SPI_DMA_CHAN

#define MDNS_INSTANCE "plantlapse"

extern "C" {
	void app_main(void);
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    }
    return ESP_OK;
}

void ota_task(void *pvParameter)
{
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if(plSettings.ota_start == true) {
        
            ESP_LOGI(TAG, "Starting OTA");

            esp_http_client_config_t config = {
                .url = CONFIG_FIRMWARE_UPGRADE_URL,
                .cert_pem = (char *)server_cert_pem_start,
                .event_handler = _http_event_handler,
            };

        #ifdef CONFIG_FIRMWARE_UPGRADE_URL_FROM_STDIN
            char url_buf[OTA_URL_SIZE];
            if (strcmp(config.url, "FROM_STDIN") == 0) {
                example_configure_stdin_stdout();
                fgets(url_buf, OTA_URL_SIZE, stdin);
                int len = strlen(url_buf);
                url_buf[len - 1] = '\0';
                config.url = url_buf;
            } else {
                ESP_LOGE(TAG, "Configuration mismatch: wrong firmware upgrade image url");
                abort();
            }
        #endif

        #ifdef CONFIG_SKIP_COMMON_NAME_CHECK
            config.skip_cert_common_name_check = true;
        #endif

            esp_err_t ret = esp_https_ota(&config);
            if (ret == ESP_OK) {
                esp_restart();
            } else {
                ESP_LOGE(TAG, "Firmware upgrade failed");
            }
        }
    }
}

void sd_init()
{
    esp_err_t err;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_FORMAT_IF_MOUNT_FAILED
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif // FORMAT_IF_MOUNT_FAILED
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t* card;
    const char mount_point[] = MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");
    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // To use 1-line SD mode, uncomment the following line:
    // slot_config.width = 1;

    // GPIOs 15, 2, 4, 12, 13 should have external 10k pull-ups.
    // Internal pull-ups are not sufficient. However, enabling internal pull-ups
    // does make a difference some boards, so we do that here.
    gpio_set_pull_mode(PIN_SD_CMD, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode(PIN_SD_D0, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
    gpio_set_pull_mode(PIN_SD_D1, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
    gpio_set_pull_mode(PIN_SD_D2, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
    gpio_set_pull_mode(PIN_SD_D3, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

    err = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (err != ESP_OK) {
        if (err == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set the FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(err));
        }
        return;
    }
    
    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "Opening file");
    FILE* f = fopen(MOUNT_POINT"/hello.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello %s!\n", card->cid.name);
    fclose(f);
    ESP_LOGI(TAG, "File written");
}

static void initialise_mdns(void)
{
    mdns_init();
    mdns_hostname_set(CONFIG_MDNS_HOST_NAME);
    mdns_instance_name_set(MDNS_INSTANCE);

    mdns_txt_item_t serviceTxtData[] = {
        {"board", "esp32"},
        {"path", "/"}
    };

    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
                                     sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));
}
void snap_task(void *pvParameter)
{
    while(1) {
        if(plSettings.snap == true) {
            ESP_LOGI(TAG, "Click!");
            char filename[] = MOUNT_POINT"/now.jpg";
            config_camera(&plSettings);
            camera_capture(filename);
            plSettings.snap = false;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void lapse_task(void *pvParameter)
{
  while(1) {
    while(plSettings.ota_start == true) {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    if(plSettings.timelapse_enabled == true) {
      time_t current_time = (esp_timer_get_time() / 1000000) + epochDiff;
      char filename[60];
      sprintf(filename, MOUNT_POINT"/%03d", plSettings.current_set);
      mkdir(filename, 755);
      sprintf(filename, MOUNT_POINT"/%03d/%06d.jpg", plSettings.current_set, plSettings.current_photo);
      if (current_time >= plSettings.next_time) {
        ESP_LOGI(TAG, "Click!");
        ESP_LOGI(TAG, "T: %i, %s", (int)current_time, filename);
        config_camera(&plSettings);
        camera_capture(filename);
        plSettings.current_photo++;
        plSettings.next_time = current_time + (plSettings.interval / 1000);
        ESP_LOGI(TAG, "%i", (int)plSettings.next_time);
      }
      time_t elapsed = ((esp_timer_get_time() / 1000000) + epochDiff) - current_time;
      if (elapsed > plSettings.interval) {
        elapsed = plSettings.interval;
      }

      vTaskDelay((plSettings.interval - elapsed) / portTICK_PERIOD_MS);
    } else {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Version: %s", esp_ota_get_app_description()->version);

    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Init network
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initialise_mdns();
    netbiosns_init();
    netbiosns_set_name(CONFIG_MDNS_HOST_NAME);
    ESP_ERROR_CHECK(example_connect());
    esp_wifi_set_ps(WIFI_PS_NONE);

    ESP_ERROR_CHECK(web_init(CONFIG_WEB_MOUNT_POINT, &plSettings));
    
    sd_init();
    camera_init();

    xTaskCreate(&ota_task, "ota_task", 8192, NULL, 5, NULL);
    xTaskCreate(&snap_task, "snap_task", 8192, NULL, 5, NULL);
    xTaskCreate(&lapse_task, "lapse_task", 8192, NULL, 5, NULL);
}
