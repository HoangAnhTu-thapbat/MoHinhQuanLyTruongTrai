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

---

## 1. Giới thiệu

Dự án “Farm Environment Monitor & Controller” nhằm mục đích:

- **Giám sát liên tục**: Nhiệt độ, độ ẩm, chất lượng không khí (gas), cường độ ánh sáng trong chuồng trại.  
- **Điều khiển**: Quạt thông gió và hệ thống phun sương để duy trì môi trường lý tưởng.  
- **Giao diện trực quan**: Dashboard trên Node-RED cho phép người dùng xem biểu đồ, giá trị thời gian thực, và chuyển sang chế độ Auto/Manual.  
- **Hai chế độ vận hành**:  
  - **Manual**: Người dùng tự click nút trên Dashboard để bật/tắt.  
  - **Auto**: Node-RED tự động gửi lệnh theo ngưỡng cài sẵn (ví dụ: temp > 30°C → bật quạt).
## - Chỉ có hai file code nạp vào UNO và ESP còn lại flow của node red rất phức tạp mình đã xuất ra file flow.json bạn chỉ cần dùng thôi!
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
     - Nhiệt độ =< 30°C → publish `farm/fan = "0"` (tắt quạt).  
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

### Hình thực tế
- arduino UNO và esp là hai phần riêng biệt không liên kết
![image](https://github.com/user-attachments/assets/f876e1e3-8215-40af-afe8-6d0de025042a)



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
- Chọn board Arduino UNO ( hoặc board của bạn ) và chọn đúng com tương ứng
- Dán code và upload

### 5.2 Code cho ESP8266 (MQTT Subscriber & Relay Control)

Kiểm tra file: `esp_main.ino`

- Mở Arduino IDE
- Chọn board ( của tôi là esp8266 với nodeMCU nên tôi chọn NodeMCU 1.0 (ESP-12E Module), tùy loại và mạch kèm theo là gì quyết định board mà bạn chọn) và chọn đúng com tương ứng
- Dán code và upload
- CHÚ Ý!!! Thay dòng này: 'const char mqttServer[] = "172.16.18.239";' bằng ip của bạn, mở terminal gõ 'ipconfig' và tìm dòng 'IPv4 Address' copy địa chỉ đó và thay thế

## 6. Cài đặt Mosquitto & Node-RED

### 6.1 Mosquitto MQTT Broker

**Trên Windows**
  - Tải và cài đặt Mosquitto từ: https://mosquitto.org/download

  - Đảm bảo file mosquitto.conf (mặc định) nằm trong C:\Program Files\mosquitto\.

  - Mở Command Prompt ở chế độ Administrator, chạy:


```bash
cd "C:\Program Files\mosquitto"
mosquitto -c mosquitto.conf -v
```
  - Kiểm tra broker đã chạy thành công (có log “mosquitto version … running”).
  - PHẢI CHẮC CHẮN LÀ CHẾ ĐỘ ADMINSTRATOR VÀ TÌM ĐÚNG FILE "mosquitto.conf" ĐỂ CHẠY!!

## 6.2 Node-RED & Dashboard

### Cài Node-RED

- **Trên Windows**: tải installer từ [https://nodered.org](https://nodered.org) và làm theo hướng dẫn.

### Khởi động Node-RED

```bash
node-red
```

> Dashboard sẽ chạy tại: [http://127.0.0.1:1880](http://127.0.0.1:1880)

---

### Cài `node-red-dashboard`

Trong thư mục người dùng Node-RED:

```bash
cd ~/.node-red
npm install node-red-dashboard
```

> Khởi động lại Node-RED nếu đang chạy.

---

## 6.3 Import Flow Node-RED

1. Mở giao diện Node-RED: [http://127.0.0.1:1880](http://127.0.0.1:1880)
2. Nhấn ☰ (menu) → **Import** → **Clipboard**
3. Dán nội dung file `flow.json` trong dự án
4. Nhấn **Import** trên workspace, sau đó **Deploy**

---

### Các node chính

- `mqtt in` (`farm/env`): nhận JSON sensor  
- `function` (parse, auto logic): tách giá trị, tính toán so với ngưỡng  
- `mqtt out` (`farm/fan`): gửi `"1"`/`"0"` để bật/tắt quạt  
- `mqtt out` (`farm/mist`): gửi `"1"` để phun sương  
- `ui_gauge`, `ui_chart`: hiển thị giá trị sensor  
- `ui_button`, `ui_switch`: điều khiển Manual/Auto, quạt ON/OFF, mist ON

### Giao diện flow node red

![image](https://github.com/user-attachments/assets/d88b93c5-d1a4-41c2-8986-00fa66b71b9f)

![image](https://github.com/user-attachments/assets/d29bca85-8fa4-4180-8c61-d9c524b95cd6)


### Giao diện dashboard

![image](https://github.com/user-attachments/assets/a1dcbfa4-a44d-44f0-8252-8032e040c1a0)

## 7. Cách sử dụng

### 7.1 Khởi động hệ thống

1. Upload code **UNO** → Upload code **ESP**
2. Khởi động **Mosquitto** (broker lắng nghe cổng `1883`)
3. Khởi động **Node-RED**, **import flow** và **deploy**
4. Mở Dashboard: truy cập `http://<IP máy chạy Node-RED>:1880/ui`

---

#### Tab “Môi trường”
- Các `Gauge` & `Chart` thể hiện:
  - `temp` (nhiệt độ)
  - `hum` (độ ẩm)
  - `gas` (chất lượng không khí)
  - `light` (ánh sáng)

#### Tab “Điều khiển”
- **Nút Manual/Auto**
- **Khi Manual**: 2 switch/quạt và nút phun sương **khả dụng**
- **Khi Auto**: 2 switch/quạt và nút phun sương **bị vô hiệu (disabled)**

---

### 7.2 Gửi lệnh thủ công (Manual)

1. Chọn **Manual** trên Dashboard → 2 switch/quạt và nút phun sương **sáng lên**
2. Gạt switch **Fan (ON)**  
   → Node-RED gửi `farm/fan = "1"`  
   → ESP nhận → relay đóng → **quạt chạy**
3. Gạt switch **Fan (OFF)**  
   → Node-RED gửi `farm/fan = "0"`  
   → ESP nhận → relay mở → **quạt dừng**
4. Nhấn nút **Mist**  
   → Node-RED gửi `farm/mist = "1"`  
   → ESP nhận → relay Mist đóng **200ms**  
   → **phun sương xong**

---

### 7.3 Chế độ tự động (Auto)

1. Chọn **Auto** trên Dashboard  
   → 2 switch/quạt và nút phun sương **chuyển sang “không bấm được”**

2. Node-RED đọc dữ liệu `farm/env` liên tục:

```logic
- Nếu (temp > 30°C) → gửi farm/fan = "1"
- Nếu (temp < 28°C) → gửi farm/fan = "0"

- Nếu (hum < 40%) hoặc (gas > 600)
  → gửi farm/mist = "1" → tạo xung phun 200ms
```
### 7.4 Chạy thực tế

1. Bật quạt

![image](https://github.com/user-attachments/assets/1a94b4d7-29d0-4626-a1ab-053d30d69685)

- Gửi thông tin đến esp

![image](https://github.com/user-attachments/assets/c408a6fa-6afb-44f7-a702-23141c11ad2f)

- Esp đã nhận

![image](https://github.com/user-attachments/assets/56e6fd59-5a23-4022-ae55-ffb306f1114c)

- Realy 1 lên đèn và quạt bật

![image](https://github.com/user-attachments/assets/3ce9499d-210e-49ad-9e1a-5b113e15a6ea)

2. Máy phun sương

- Tương tự nhưng không phải switch mà là button ( tùy vì module phun sương tôi dùng là loại nguồn ngoài với dây khởi động cần xung nhịp ) gọi đến relay đóng và mở mạch trong thời gian ngắn kích hoạt module chạy.

![image](https://github.com/user-attachments/assets/c848fa41-d1e1-4ea4-bb6c-bc5173df57dc)

![image](https://github.com/user-attachments/assets/e7cce90f-8e3a-41b9-91b7-d1167893bbd8)

3. Chế độ auto

- Dựa vào logic để chạy

![image](https://github.com/user-attachments/assets/20f0d0fd-beda-40f1-bc75-c3ae60267001)

- Khi ấn auto thì thông báo chế độ hiện tại là auto

![image](https://github.com/user-attachments/assets/0012bfab-3d55-4b37-9c17-da02508ed3e9)

- Sau đó hệ thống phát hiện nhiệt độ trên 30 đô ( cụ thể là 31 như hình trên ) tự động gọi payload của quạt với tham số Bật

![image](https://github.com/user-attachments/assets/dc36ce82-5375-46fc-b46d-09c7f4d336d1)

- Quạt bật và tự tắt khi nhiệt độ xuống dưới 30 độ

![image](https://github.com/user-attachments/assets/0b5f2d7b-f9f0-4c9a-831a-80aa25cd3b05)

4. Cảnh báo

- Dựa vào thông tin để đưa ra cảnh báo

![image](https://github.com/user-attachments/assets/a9f73d7a-46e3-40e0-9499-cb251c47b8e5)

![image](https://github.com/user-attachments/assets/58281b12-3b3a-47e4-a3a5-89658bc60951)


✅ **Kết quả**: Khi môi trường vượt ngưỡng, hệ thống tự **bật quạt/phun sương**; khi hạ ngưỡng, tự **tắt thiết bị**

## 8. Phần mở rộng & Tùy chỉnh

### Thay đổi ngưỡng Auto Logic
- Mở node `function "autoLogic"` trong **Node-RED**, chỉnh giá trị `threshold` cho `temp/hum/gas`.
- Có thể thêm node `ui_numeric` để Dashboard cho phép **nhập giá trị ngưỡng real-time**.

---

### Thêm sensor khác
- Ví dụ cảm biến:
  - **DS18B20** (nhiệt độ chính xác)
  - **BMP280** (áp suất)
- Kết nối với Arduino, sửa `uno_sensor.ino` để đọc & đóng gói JSON thêm trường mới.
- Sửa **ESP code** để xử lý trường mới (không cần đổi logic relay).
- Thêm `gauge/chart` trong Node-RED.

---

### Bảo mật MQTT

**Trong `mosquitto.conf`:**
```conf
allow_anonymous false
password_file C:/mosquitto/passwd
```

**Tạo file password:**
```bash
mosquitto_passwd -c passwd_username passwd_password
```

**ESP code thêm credentials trước khi kết nối:**
```cpp
client.connect("ESPClient", "username", "password");
```

**Node-RED**: cấu hình `mqtt-broker` node với **username/password tương ứng**

---

### Lưu trữ dữ liệu lâu dài
- Thêm **node InfluxDB** trong Node-RED để ghi `farm/env` vào cơ sở **InfluxDB**
- Dùng **Grafana** để visualize lịch sử dài hạn

---

### Cảnh báo (Alert)
- Dùng node **email** hoặc **telegram** trong Node-RED để gửi thông báo khi vượt ngưỡng
- Ví dụ:
  - `temp > 35°C` → gửi email: **"Cảnh báo: Nhiệt độ quá cao"**

---

## 9. Khắc phục sự cố (Troubleshooting)

### ESP không kết nối Wi-Fi
- Kiểm tra **SSID/Password** trong code
- Quan sát **Serial Monitor** xem có in `WiFi connected` không
- Đảm bảo NodeMCU nằm trong vùng phủ sóng

---

### ESP không kết nối MQTT Broker
- Đảm bảo **broker đang chạy** (`mosquitto -v`)
- Kiểm tra IP broker & port (nếu máy khác → cần port forwarding)
- Serial Monitor:
  - `rc=4`: Network unreachable
  - `rc=5`: Auth error

---

### Relay không tách/khép
- Kiểm tra **VCC = 5V** cho module relay
- Kiểm tra **GND chung** giữa NodeMCU & relay
- Gửi `farm/fan = "1"` hoặc `"0"` → có nghe tiếng **“tạch”**
- Nếu không, đo chân **D6**: có thay đổi giữa **3.3V** (HIGH) và **0V** (LOW) không

---

### LED relay luôn sáng (không tắt)
- LED chỉ báo **“có điện”**, không đại diện trạng thái cuộn coil
- Để xác nhận relay mở/đóng: nghe tiếng **“tạch”** hoặc đo **COM–NO** bằng đồng hồ vạn năng

---

### Quạt giật ngược khi OFF
- Nguyên nhân:
  - Cắm quạt trực tiếp vào GPIO (**nguy hiểm**) → phải qua relay
  - Thiếu **diode flyback**
- Giải pháp:
  - Thêm **diode 1N4007** ngược chiều:
    - Cathode → +5V
    - Anode → GND/cuộn dây

---

### Node-RED không nhận message
- Kiểm tra node **mqtt in**:
  - Broker config đúng chưa? (`localhost:1883`)
  - Topic đúng chưa? (`farm/env`)
- Kiểm tra bằng dòng lệnh:
```bash
mosquitto_sub -h localhost -t farm/env -v
```

---

## 10. Liên hệ & Hỗ trợ

- **Tác giả**: Hoàng Anh Tú 
- **Email**: hoangtu.dainam@gmail.com 
- **GitHub**: [https://github.com/HoangAnhTu-thapbat/MoHinhQuanLyTruongTrai](https://github.com/HoangAnhTu-thapbat/MoHinhQuanLyTruongTrai)

💬 Mọi góp ý và issue vui lòng gửi trên GitHub hoặc qua email.
