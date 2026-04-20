#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"
#include "telemetry.h"

static const char *TAG = "TELEMETRY";

esp_err_t http_post_record(sensor_record_t *record) {
    char post_data[128];
    
    // Format the JSON payload
    snprintf(post_data, sizeof(post_data), 
             "{\"temp\": %.2f, \"press\": %.2f, \"cat\": \"%s\"}", 
             record->temperature, record->pressure, record->category);

    esp_http_client_config_t config = {
        .url = SERVER_URL,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
        // Disable buffer usage if not needed to save RAM
        .buffer_size = 1024,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP POST Status = %d", status_code);
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}
