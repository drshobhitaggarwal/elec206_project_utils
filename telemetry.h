#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "esp_err.h"

// Configuration
#define SERVER_URL "http://10.0.0.31:5000/data"

// Data structure for the sensor record
typedef struct {
    float temperature;
    float pressure;
    char category[16]; 
} sensor_record_t;

/**
 * @brief Sends sensor data to the configured server via HTTP POST
 * * @param record Pointer to the sensor_record_t structure
 * @return esp_err_t ESP_OK on success
 */
esp_err_t http_post_record(sensor_record_t *record);

#endif // TELEMETRY_H
