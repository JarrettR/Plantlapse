#include "include/camera.hpp"

static const char *TAG = "camera";


static camera_config_t camera_config = {
    .pin_pwdn  = PIN_CAM_PWDN,
    .pin_reset = PIN_CAM_RESET,
    .pin_xclk = PIN_CAM_XCLK,
    .pin_sscb_sda = PIN_CAM_SDA,
    .pin_sscb_scl = PIN_CAM_SCL,

    .pin_d7 = PIN_CAM_D7,
    .pin_d6 = PIN_CAM_D6,
    .pin_d5 = PIN_CAM_D5,
    .pin_d4 = PIN_CAM_D4,
    .pin_d3 = PIN_CAM_D3,
    .pin_d2 = PIN_CAM_D2,
    .pin_d1 = PIN_CAM_D1,
    .pin_d0 = PIN_CAM_D0,
    .pin_vsync = PIN_CAM_VSYNC,
    .pin_href = PIN_CAM_HREF,
    .pin_pclk = PIN_CAM_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,//YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_UXGA,//QQVGA-QXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 12, //0-63 lower number means higher quality
    .fb_count = 1 //if more than one, i2s runs in continuous mode. Use only with JPEG
};

esp_err_t camera_init(){
    //power up the camera if PWDN pin is defined
    if(PIN_CAM_PWDN != -1){
        //pinMode(PIN_CAM_PWDN, OUTPUT);
        //digitalWrite(PIN_CAM_PWDN, LOW);
    }

    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}

esp_err_t camera_capture(){
    //acquire a frame
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Camera Capture Failed");
        return ESP_FAIL;
    }
    //replace this with your own function
    //process_image(fb->width, fb->height, fb->format, fb->buf, fb->len);
  
    //return the frame buffer back to the driver for reuse
    esp_camera_fb_return(fb);
    return ESP_OK;
}