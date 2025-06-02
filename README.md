# Hệ Thống Giám Sát & Điều Khiển Môi Trường Chuồng Trại IoT

**Tên dự án**: Farm Environment Monitor & Controller  
**Phần cứng chính**: Arduino UNO R3, NodeMCU ESP8266, DHT11, MQ-135, BH1750, Relay 2 kênh JQC-3F 5VDC  
**Phần mềm chính**: Arduino IDE, Node-RED, Mosquitto MQTT Broker

---

## Mô Tả Dự Án

Hệ thống này thu thập **nhiệt độ**, **độ ẩm**, **nồng độ khí gas** và **cường độ ánh sáng** trong chuồng trại, gửi dữ liệu qua MQTT đến Node-RED để hiển thị trên Dashboard và đưa ra **lệnh điều khiển quạt** và **phun sương**. Có hai chế độ:

- **Auto**: Node-RED tự động bật/tắt quạt và phun sương dựa trên ngưỡng đã cài sẵn.
- **Manual**: Người dùng trực tiếp bấm nút trên Dashboard để bật/tắt quạt hoặc kích phun sương.

---

## Cấu Trúc Repository

-/project-root

-│

-├── README.md

-├── Arduino_UNO

-│ └── uno_sensor.ino # Mã nguồn Arduino đọc DHT11, MQ-135, BH1750

-│

-├── ESP8266

-│ └── esp_main.ino # Mã nguồn ESP8266 (NodeMCU) kết nối Wi-Fi, MQTT, điều khiển relay

-│

-├── Node-RED

-└── flow.json # Flow xuất khẩu (export) cho Node-RED



---

## 1. Phần Cứng & Nối Dây

### 1.1 Linh Kiện Cần Chuẩn Bị

- **Arduino UNO R3** (1×)  
- **NodeMCU ESP8266** (1×)  
- **Cảm biến DHT11** (1×)  
- **Cảm biến MQ-135** (1×)  
- **Cảm biến BH1750** (1×)  
- **Module relay 2 kênh JQC-3F 5VDC** (1×)  
- **Quạt 5V mini** (1×)  
- **Module phun sương 5V** (1×)  
- **Adapter 5V ≥2A** (1×)  
- **Dây nối (jumper wires)**, **breadboard** hoặc **PCB thử nghiệm**

### 1.2 Sơ Đồ Nối Dây

#### Arduino UNO ⇄ Cảm biến

- DHT11:  
  - VCC → 5V (Arduino)  
  - GND → GND (Arduino)  
  - DATA → Digital 2 (D2)  

- MQ-135:  
  - VCC → 5V (Arduino)  
  - GND → GND (Arduino)  
  - A0 → Analog A0 (Arduino)  

- BH1750:  
  - VCC → 5V (Arduino)  
  - GND → GND (Arduino)  
  - SDA → A4 (Arduino)  
  - SCL → A5 (Arduino)  

#### Arduino UNO ⇄ ESP8266 (NodeMCU)

- Arduino TX (D1) → NodeMCU RX (GPIO3)  
- Arduino RX (D0) → NodeMCU TX (GPIO1)  
- GND (Arduino) ↔ GND (NodeMCU)

*(Nếu dùng SoftwareSerial trên Arduino, có thể chọn cặp pin khác; đảm bảo baud rate khớp 115200.)*

#### NodeMCU ⇄ Module Relay & Thiết bị

- NodeMCU 5V (VU) → VCC (relay module)  
- NodeMCU GND → GND (relay module)  
- NodeMCU D6 (GPIO12) → IN1 (relay kênh Fan)  
- NodeMCU D7 (GPIO13) → IN2 (relay kênh Mist)  

- Module relay COM1 → +5V quạt  
- Module relay NO1  → + cực quạt  
- Quạt – → GND chung  

- Module relay COM2 → +5V phun sương  
- Module relay NO2  → + cực phun sương  
- Phun sương – → GND chung  

---

## 2. Phần Mềm & Cấu Hình

### 2.1 Cài Đặt Arduino IDE

1. Tải và cài đặt [Arduino IDE](https://www.arduino.cc/en/software).  
2. Trong `File → Preferences`, thêm `http://arduino.esp8266.com/stable/package_esp8266com_index.json` vào mục **Additional Boards Manager URLs**.  
3. Mở **Boards Manager**, tìm “esp8266” và cài “esp8266 by ESP8266 Community”.  
4. Chọn **Board**: “NodeMCU 1.0 (ESP-12E Module)” khi cài code ESP. Chọn “Arduino/Genuino UNO” khi cài code UNO.

#### 2.1.1 Mã nguồn Arduino UNO: `uno_sensor.ino`
