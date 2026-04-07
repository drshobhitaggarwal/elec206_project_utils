#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "bmp180.h"

static const char *TAG = "APP_MAIN";

#define SCL_PIN GPIO_NUM_4 
#define SDA_PIN GPIO_NUM_5 

void app_main(void) {
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    bmp180_calib_t calib;

    // 1. Setup I2C Bus
    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .glitch_ignore_cnt = 7,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    // 2. Add BMP180 to Bus
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BMP180_ADDR,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    // 3. Initialize Sensor
    if (bmp180_init_calibration(dev_handle, &calib) == ESP_OK) {
        ESP_LOGI(TAG, "BMP180 Initialized.");
    }

    while (1) {
        float temperature, pressure;
        if (bmp180_read_data(dev_handle, &calib, &temperature, &pressure) == ESP_OK) {
            printf("Temperature: %.1f °C | Pressure: %.2f hPa\n", temperature, pressure);
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}