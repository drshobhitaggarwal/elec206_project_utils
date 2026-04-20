idf_component_register(SRCS "main.c" "bmp180.c" "wifi_manager.c" "telemetry.c"
                       PRIV_REQUIRES spi_flash
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_wifi esp_http_client nvs_flash esp_event esp_netif)
