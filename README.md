ELEC 206: Project UtilitiesThis repository provides standardized modules for ESP32-based IoT development, focusing on modularity and "Edge to Cloud" data flow for the ELEC 206 course projects.

Repository Structure
The project is organized into modular components to ensure separation of concerns between hardware, networking, and cloud communication
wifi_manager.c / .h: Encapsulates the ESP-IDF WiFi stack, event handling, and automatic reconnection logic
telemetry.c / .h: Handles the JSON serialization and HTTP POST communication to the central laboratory server
main.c: The primary conductor that initializes hardware and manages the main execution loop.

Setup & Configuration

1. CMake IntegrationTo ensure the ESP-IDF build system recognizes the new modules, your main/CMakeLists.txt must be updated to include all source files:

idf_component_register(SRCS "main.c" 
                            "wifi_manager.c" 
                            "telemetry.c"
                            "bmp180.c"
                    INCLUDE_DIRS ".")

2. Networking Credentials
  Modify the following definitions in wifi_manager.h to connect to the lab network:
  SSID: 
  Password:

4. Server Endpoint
The telemetry module is pre-configured to send data to the central monitoring station:
URL: http://<ip_address of your laptop>:5000/data

Student Quick Start Guide
Initialize NVS: Always call nvs_flash_init() at the start of app_main. WiFi will fail without this.
Start WiFi: Call wifi_init_sta().
Monitor Status: Use wifi_is_connected() to gate your HTTP requests.
Data Transmission: Pass your sensor_record_t struct to http_post_record() to sync data to the cloud.

Standard JSON Payload
The server expects data in this format:

{
  "temp": 28.50,
  "press": 1013.25,
  "cat": "Normal"
}
