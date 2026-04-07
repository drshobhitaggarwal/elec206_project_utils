#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

static const char *TAG = "BMP180_BASIC";

// --- CONFIGURATION ---
#define BMP180_ADDR     0x77
#define SCL_PIN         GPIO_NUM_4 
#define SDA_PIN         GPIO_NUM_5 

// --- CALIBRATION STRUCTURE ---
typedef struct {
    int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
    uint16_t ac4, ac5, ac6;
} bmp180_calib_t;

bmp180_calib_t calib;
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;

// --- HELPERS ---
esp_err_t read_16(uint8_t reg, uint16_t *val) {
    uint8_t data[2];
    esp_err_t err = i2c_master_transmit_receive(dev_handle, &reg, 1, data, 2, -1);
    *val = (data[0] << 8) | data[1];
    return err;
}

void bmp180_init_calibration() {
    read_16(0xAA, (uint16_t*)&calib.ac1); read_16(0xAC, (uint16_t*)&calib.ac2);
    read_16(0xAE, (uint16_t*)&calib.ac3); read_16(0xB0, &calib.ac4);
    read_16(0xB2, &calib.ac5); read_16(0xB4, &calib.ac6);
    read_16(0xB6, (uint16_t*)&calib.b1); read_16(0xB8, (uint16_t*)&calib.b2);
    read_16(0xBA, (uint16_t*)&calib.mb); read_16(0xBC, (uint16_t*)&calib.mc);
    read_16(0xBE, (uint16_t*)&calib.md);
}

void bmp180_read_data(float *temp, float *press) {
    // Temperature
    i2c_master_transmit(dev_handle, (uint8_t[]){0xF4, 0x2E}, 2, -1);
    vTaskDelay(pdMS_TO_TICKS(5));
    uint16_t ut; read_16(0xF6, &ut);

    long x1 = (ut - calib.ac6) * calib.ac5 / 32768;
    long x2 = calib.mc * 2048 / (x1 + calib.md);
    long b5 = x1 + x2;
    *temp = (float)((b5 + 8) / 16) / 10.0;

    // Pressure
    i2c_master_transmit(dev_handle, (uint8_t[]){0xF4, 0x34}, 2, -1);
    vTaskDelay(pdMS_TO_TICKS(5));
    uint8_t p_raw[3];
    uint8_t p_reg = 0xF6;
    i2c_master_transmit_receive(dev_handle, &p_reg, 1, p_raw, 3, -1);
    long up = ((p_raw[0] << 16) | (p_raw[1] << 8) | p_raw[2]) >> 8;

    long b6 = b5 - 4000;
    x1 = (calib.b2 * (b6 * b6 / 4096)) / 2048;
    x2 = calib.ac2 * b6 / 2048;
    long x3 = x1 + x2;
    long b3 = (((long)calib.ac1 * 4 + x3) + 2) / 4;
    x1 = calib.ac3 * b6 / 8192;
    x2 = (calib.b1 * (b6 * b6 / 4096)) / 65536;
    x3 = ((x1 + x2) + 2) / 4;
    uint32_t b4 = calib.ac4 * (uint32_t)(x3 + 32768) / 32768;
    uint32_t b7 = ((uint32_t)up - b3) * 50000;
    long p = (b7 < 0x80000000) ? (b7 * 2) / b4 : (b7 / b4) * 2;
    x1 = (p / 256) * (p / 256);
    x1 = (x1 * 3038) / 65536;
    x2 = (-7357 * p) / 65536;
    *press = (float)(p + (x1 + x2 + 3791) / 16) / 100.0; // Result in hPa
}

void app_main(void) {
    // 1. I2C Bus Init
    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .glitch_ignore_cnt = 7,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BMP180_ADDR,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    // 2. Sensor Setup
    bmp180_init_calibration();
    ESP_LOGI(TAG, "BMP180 Initialized.");

    while (1) {
        float temperature, pressure;
        bmp180_read_data(&temperature, &pressure);

        printf("Temperature: %.1f °C | Pressure: %.2f hPa\n", temperature, pressure);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
