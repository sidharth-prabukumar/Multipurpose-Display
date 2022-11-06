Building the Project
***************
1. Install west

pip install west

2. Initialize

west init

3. Update west

west update

4. Install espressif specific dependencies

west blobs fetch hal_espressif

5. Build the project

west build -b esp32 sntp_time

6. Flash to the ESP32 hardware

west flash --esp-device COM4