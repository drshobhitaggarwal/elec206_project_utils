#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include "stdbool.h"

// Configuration constants
#define WIFI_SSID       "WIFI-NAME"    // modify according to your wifi
#define WIFI_PASS       "PASSWORD"     // modify according to your wifi
#define MAXIMUM_RETRY   5

/**
 * @brief Initialize WiFi in Station mode and connect to the AP
 */
void wifi_init_sta(void);

/**
 * @brief Check if WiFi is currently connected with an IP address
 * @return true if connected
 */
bool wifi_is_connected(void);

#endif // WIFI_MANAGER_H
