# Hệ Thống Giám Sát & Điều Khiển Môi Trường Chuồng Trại IoT

> Tổng hợp hướng dẫn cấu hình, cài đặt và vận hành dự án “Farm Environment Monitor & Controller” sử dụng Arduino UNO, ESP8266, Module Relay, và Node-RED.

---

## Mục lục

- [1. Giới thiệu](#1-giới-thiệu)  
- [2. Tính năng chính](#2-tính-năng-chính)  
- [3. Yêu cầu và thành phần](#3-yêu-cầu-và-thành-phần)  
  - [3.1 Phần cứng](#31-phần-cứng)  
  - [3.2 Phần mềm](#32-phần-mềm)  
- [4. Sơ đồ đấu nối (Wiring Diagram)](#4-sơ-đồ-đấu-nối-wiring-diagram)  
- [5. Cài đặt và Upload Code](#5-cài-đặt-và-upload-code)  
  - [5.1 Code cho Arduino UNO (Sensor Reader)](#51-code-cho-arduino-uno-sensor-reader)  
  - [5.2 Code cho ESP8266 (MQTT Subscriber & Relay Control)](#52-code-cho-esp8266-mqtt-subscriber--relay-control)  
- [6. Cài đặt Mosquitto & Node-RED](#6-cài-đặt-mosquitto--node-red)  
  - [6.1 Mosquitto MQTT Broker](#61-mosquitto-mqtt-broker)  
  - [6.2 Node-RED & Dashboard](#62-node-red--dashboard)  
  - [6.3 Import Flow Node-RED](#63-import-flow-node-red)  
- [7. Cách sử dụng](#7-cách-sử-dụng)  
  - [7.1 Khởi động hệ thống](#71-khởi-động-hệ-thống)  
  - [7.2 Gửi lệnh thủ công (Manual)](#72-gửi-lệnh-thủ-công-manual)  
  - [7.3 Chế độ tự động (Auto)](#73-chế-độ-tự-động-auto)  
- [8. Phần mở rộng & Tùy chỉnh](#8-phần-mở-rộng--tùy-chỉnh)  
- [9. Khắc phục sự cố (Troubleshooting)](#9-khắc-phục-sự-cố-troubleshooting)  
- [10. Liên hệ & Hỗ trợ](#10-liên-hệ--hỗ-trợ)  
- [11. License](#11-license)  

---

## 1. Giới thiệu

Dự án “Farm Environment Monitor & Controller” nhằm mục đích:

- **Giám sát liên tục**: Nhiệt độ, độ ẩm, chất lượng không khí (gas), cường độ ánh sáng trong chuồng trại.  
- **Điều khiển**: Quạt thông gió và hệ thống phun sương để duy trì môi trường lý tưởng.  
- **Giao diện trực quan**: Dashboard trên Node-RED cho phép người dùng xem biểu đồ, giá trị thời gian thực, và chuyển sang chế độ Auto/Manual.  
- **Hai chế độ vận hành**:  
  - **Manual**: Người dùng tự click nút trên Dashboard để bật/tắt.  
  - **Auto**: Node-RED tự động gửi lệnh theo ngưỡng cài sẵn (ví dụ: temp > 30°C → bật quạt).

---

## 2. Tính năng chính

1. **Đọc sensor:**  
   - DHT11: nhiệt độ (°C), độ ẩm (%)  
   - MQ-135: nồng độ khí gas (giá trị analog)  
   - BH1750: cường độ ánh sáng (lux)  

2. **Gửi dữ liệu lên MQTT Broker**: Arduino UNO thu thập, đóng gói JSON, truyền qua Serial đến ESP, ESP publish lên topic `farm/env`.  

3. **Hiển thị Dashboard (Node-RED):**  
   - Gauge và chart cho từng chỉ số (temp, hum, gas, light).  
   - Nút toggle Auto/Manual.  
   - Switch hoặc button để bật/tắt quạt và nút phun sương.  
   - Hiển thị trạng thái relay bằng đèn nền hoặc text indicator.  

4. **Điều khiển Relay:**  
   - Module Relay 2 kênh JQC-3F 5VDC (active-low).  
   - Kênh 1 (IN1/D6) điều khiển quạt 5 V.  
   - Kênh 2 (IN2/D7) điều khiển phun sương 5 V.  

5. **Chế độ tự động (Auto Logic):**  
     - Nhiệt độ > 30°C → publish `farm/fan = "1"` (bật quạt).  
     - Nhiệt độ < 28°C → publish `farm/fan = "0"` (tắt quạt).  
     - Tương tự với độ ẩm/gas để phun sương.  

## 3. Yêu cầu và thành phần

### 3.1 Phần cứng

- **Arduino UNO R3**  
  - Kết nối DHT11, MQ-135, BH1750.  
- **NodeMCU ESP8266 (ESP-12F)**  
  - Kết nối Wi-Fi, subscribe/publish MQTT, điều khiển relay.  
- **Cảm biến**  
  - **DHT11**: đo nhiệt độ + độ ẩm.  
  - **MQ-135**: đo nồng độ khí gas (CO₂, NH₃, etc.).  
  - **BH1750**: đo cường độ ánh sáng (lux).  
- **Module Relay 2 kênh JQC-3F 5VDC** (active-low)  
  - IN1 → D6 (quạt), IN2 → D7 (phun sương).  
  - VCC = 5 V (VU), GND chung.  
  - LED báo (luôn sáng khi VCC = 5 V); chỉ nghe tiếng “tạch”/“tách” để xác nhận trạng thái relay.  
- **Quạt DC 5 V**  
  - Nối COM1 → +5 V, NO1 → (+) quạt, (−) quạt → GND chung.  
- **Module phun sương 5 V**  
  - Nối COM2 → +5 V, NO2 → (+) phun sương, (−) phun sương → GND chung.  
- **Adapter 5 V ≥ 2 A** (hoặc dùng VU của NodeMCU nếu đủ dòng)  
- **Dây nối (jumper wires)**, **breadboard** (nếu prototyping) hoặc **PCB** (nếu hoàn thiện).

### 3.2 Phần mềm

- **Arduino IDE** (phiên bản ≥1.8.x)  
- **Board Support for ESP8266** (qua Boards Manager)  
- **Thư viện Arduino**:  
  - `DHT.h` (cho DHT11)  
  - `Wire.h` + `BH1750.h`  
  - `PubSubClient.h` (MQTT)  
  - `ESP8266WiFi.h`  
  - `SoftwareSerial.h` (cho giao tiếp UNO ↔ ESP)  
- **Mosquitto MQTT Broker** (cài đặt local hoặc remote)  
- **Node-RED** (≥ v1.0) + `node-red-dashboard`  
- **Mosquitto clients** (mosquitto_pub, mosquitto_sub) để test thủ công.

---

## 4. Sơ đồ đấu nối (Wiring Diagram)

> Xem hình `wiring_diagram.png` trong thư mục `docs/` để có sơ đồ trực quan. Dưới đây mô tả chi tiết:


### Chú ý:

- **UNO ↔ ESP SoftwareSerial**:  
  - Arduino UNO (TX → ESP D5), UNO (RX ← ESP D6). Ấn định baud 115200 để cả hai đọc/gửi JSON.  
- **Module Relay (active-low)**:  
  - **VCC** được cấp 5 V từ VU của NodeMCU.  
  - **GND** chung với NodeMCU.  
  - **IN1 (relay kênh Fan)** ← D6 (GPIO12) ESP.  
  - **IN2 (relay kênh Mist)** ← D7 (GPIO13) ESP.  
  - **COM1** → +5 V nguồn  
  - **NO1** → + của quạt; − của quạt → GND chung.  
  - **COM2** → +5 V nguồn  
  - **NO2** → + của phun sương; − của phun sương → GND chung.  
- **LED relay**: thường luôn sáng khi VCC = 5 V (chỉ báo module có điện). Relay đóng/mở nghe tiếng “tạch”/“tách”.  

---

## 5. Cài đặt và Upload Code

### 5.1 Code cho Arduino UNO (Sensor Reader)

Kiểm tra file: `uno_sensor.ino`

- Mở Arduino IDE
- Chọn board Arduino UNO và chọn đúng com tương ứng
- Dán code và upload

### 5.2 Code cho ESP8266 (MQTT Subscriber & Relay Control)

Kiểm tra file: `esp_main.ino`


