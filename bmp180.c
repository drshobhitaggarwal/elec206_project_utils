#include "bmp180.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Private helper to read 16-bit registers
static esp_err_t read_16(i2c_master_dev_handle_t dev_handle, uint8_t reg, uint16_t *val) {
    uint8_t data[2];
    esp_err_t err = i2c_master_transmit_receive(dev_handle, &reg, 1, data, 2, -1);
    if (err == ESP_OK) {
        *val = (data[0] << 8) | data[1];
    }
    return err;
}

esp_err_t bmp180_init_calibration(i2c_master_dev_handle_t dev_handle, bmp180_calib_t *calib) {
    esp_err_t err = ESP_OK;
    err |= read_16(dev_handle, 0xAA, (uint16_t*)&calib->ac1);
    err |= read_16(dev_handle, 0xAC, (uint16_t*)&calib->ac2);
    err |= read_16(dev_handle, 0xAE, (uint16_t*)&calib->ac3);
    err |= read_16(dev_handle, 0xB0, &calib->ac4);
    err |= read_16(dev_handle, 0xB2, &calib->ac5);
    err |= read_16(dev_handle, 0xB4, &calib->ac6);
    err |= read_16(dev_handle, 0xB6, (uint16_t*)&calib->b1);
    err |= read_16(dev_handle, 0xB8, (uint16_t*)&calib->b2);
    err |= read_16(dev_handle, 0xBA, (uint16_t*)&calib->mb);
    err |= read_16(dev_handle, 0xBC, (uint16_t*)&calib->mc);
    err |= read_16(dev_handle, 0xBE, (uint16_t*)&calib->md);
    return err;
}

esp_err_t bmp180_read_data(i2c_master_dev_handle_t dev_handle, bmp180_calib_t *calib, float *temp, float *press) {
    // 1. Read Raw Temperature
    uint8_t cmd_temp[] = {0xF4, 0x2E};
    i2c_master_transmit(dev_handle, cmd_temp, 2, -1);
    vTaskDelay(pdMS_TO_TICKS(5));
    uint16_t ut; 
    read_16(dev_handle, 0xF6, &ut);

    long x1 = (ut - calib->ac6) * calib->ac5 / 32768;
    long x2 = calib->mc * 2048 / (x1 + calib->md);
    long b5 = x1 + x2;
    *temp = (float)((b5 + 8) / 16) / 10.0;

    // 2. Read Raw Pressure
    uint8_t cmd_press[] = {0xF4, 0x34};
    i2c_master_transmit(dev_handle, cmd_press, 2, -1);
    vTaskDelay(pdMS_TO_TICKS(5));
    
    uint8_t p_raw[3];
    uint8_t p_reg = 0xF6;
    i2c_master_transmit_receive(dev_handle, &p_reg, 1, p_raw, 3, -1);
    long up = ((p_raw[0] << 16) | (p_raw[1] << 8) | p_raw[2]) >> 8;

    // Pressure Calculation
    long b6 = b5 - 4000;
    x1 = (calib->b2 * (b6 * b6 / 4096)) / 2048;
    x2 = calib->ac2 * b6 / 2048;
    long x3 = x1 + x2;
    long b3 = (((long)calib->ac1 * 4 + x3) + 2) / 4;
    x1 = calib->ac3 * b6 / 8192;
    x2 = (calib->b1 * (b6 * b6 / 4096)) / 65536;
    x3 = ((x1 + x2) + 2) / 4;
    uint32_t b4 = calib->ac4 * (uint32_t)(x3 + 32768) / 32768;
    uint32_t b7 = ((uint32_t)up - b3) * 50000;
    long p = (b7 < 0x80000000) ? (b7 * 2) / b4 : (b7 / b4) * 2;
    x1 = (p / 256) * (p / 256);
    x1 = (x1 * 3038) / 65536;
    x2 = (-7357 * p) / 65536;
    *press = (float)(p + (x1 + x2 + 3791) / 16) / 100.0;

    return ESP_OK;
}