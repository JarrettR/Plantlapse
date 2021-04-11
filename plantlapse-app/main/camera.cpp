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

esp_err_t config_camera(Settings *settings_in) {
    camera_config.jpeg_quality = settings_in->quality;

    sensor_t *s = esp_camera_sensor_get();
    // s->set_ae_level(s,settings_in->quality);
    // s->set_aec2(s,st.aec2);
    s->set_aec_value(s,settings_in->exposure);
    s->set_agc_gain(s,settings_in->gain);
    // s->set_awb_gain(s,st.awb_gain);
    s->set_bpc(s,settings_in->blackpixelcorrection);
    s->set_brightness(s,settings_in->brightness);
    // s->set_colorbar(s,st.colorbar);
    s->set_contrast(s,settings_in->contrast);
    // s->set_dcw(s,st.dcw);
    // s->set_denoise(s,st.denoise);
    s->set_exposure_ctrl(s,settings_in->autoexposure);
    // s->set_framesize(s,st.framesize);
    s->set_gain_ctrl(s,settings_in->autogain);    
    
    gainceiling_t ceil;
    switch (settings_in->gainceiling) {
        case 2:
        ceil = GAINCEILING_2X;
        break;
        case 4:
        ceil = GAINCEILING_4X;
        break;
        case 8:
        ceil = GAINCEILING_8X;
        break;
        case 16:
        ceil = GAINCEILING_16X;
        break;
        case 32:
        ceil = GAINCEILING_32X;
        break;
        case 64:
        ceil = GAINCEILING_64X;
        break;
        case 128:
        ceil = GAINCEILING_128X;
        break;
        default:
        return false;
    }      
    s->set_gainceiling(s,ceil);
    s->set_hmirror(s,settings_in->horizontalflip);
    s->set_lenc(s,settings_in->lenscorrection);
    s->set_quality(s,settings_in->quality);
    // s->set_raw_gma(s,st.raw_gma);
    s->set_saturation(s,settings_in->saturation);
    s->set_sharpness(s,settings_in->sharpness);
    // s->set_special_effect(s,st.special_effect);
    s->set_vflip(s,settings_in->verticalflip);
    // s->set_wb_mode(s,st.wb_mode);
    // s->set_whitebal(s,st.awb);
    s->set_wpc(s,settings_in->whitepixelcorrection);

    return ESP_OK;
}

esp_err_t camera_capture(char * filename){
    //acquire a frame
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Camera Capture Failed");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Filename %s", filename);
    FILE* f = fopen(filename, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return ESP_FAIL;
    }
    //replace this with your own function
    //process_image(fb->width, fb->height, fb->format, fb->buf, fb->len);
    fwrite(fb->buf,fb->len,1,f);
    ESP_LOGI(TAG, "Writing %d bytes", fb->len);
	fclose(f);

    //return the frame buffer back to the driver for reuse
    esp_camera_fb_return(fb);
    return ESP_OK;
}