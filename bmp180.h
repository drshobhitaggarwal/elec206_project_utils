#ifndef BMP180_H
#define BMP180_H

#include "esp_err.h"
#include "driver/i2c_master.h"

#define BMP180_ADDR 0x77

// Calibration coefficients structure
typedef struct {
    int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
    uint16_t ac4, ac5, ac6;
} bmp180_calib_t;

/**
 * @brief Read calibration data from the BMP180
 * @param dev_handle The I2C device handle
 * @param calib Pointer to the calibration structure to fill
 * @return ESP_OK on success
 */
esp_err_t bmp180_init_calibration(i2c_master_dev_handle_t dev_handle, bmp180_calib_t *calib);

/**
 * @brief Read temperature and pressure
 * @param dev_handle The I2C device handle
 * @param calib Pointer to populated calibration data
 * @param temp Pointer to store temperature (°C)
 * @param press Pointer to store pressure (hPa)
 * @return ESP_OK on success
 */
esp_err_t bmp180_read_data(i2c_master_dev_handle_t dev_handle, bmp180_calib_t *calib, float *temp, float *press);

#endif // BMP180_H