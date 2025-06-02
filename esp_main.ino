#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Thông tin Wi-Fi
const char ssid[]     = "Van thu";
const char password[] = "0367497856";

// IP LAN máy chạy Mosquitto
const char mqttServer[] = "192.168.1.18";
const uint16_t mqttPort = 1883;

// Định nghĩa chân GPIO nối IN của module relay (active-low)
#define FAN_PIN   D6  // GPIO12 → IN1 (relay Fan: LOW → bật, HIGH → tắt)
#define MIST_PIN  D7  // GPIO13 → IN2 (relay Mist: LOW → bật, HIGH → tắt)

// Các topic MQTT
const char topicFan[]  = "farm/fan";
const char topicMist[] = "farm/mist";

WiFiClient    net;
PubSubClient  client(net);

// Flag để tạo xung phun sương
volatile bool mistFlag = false;

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  char msgBuf[10];
  if (length >= sizeof(msgBuf)) length = sizeof(msgBuf) - 1;
  memcpy(msgBuf, payload, length);
  msgBuf[length] = '\0';

  // Điều khiển quạt
  if (strcmp(topic, topicFan) == 0) {
    if (msgBuf[0] == '1') {
      // LOW → đóng relay → quạt chạy
      digitalWrite(FAN_PIN, LOW);
      Serial.println(">> Relay Fan: ON (LOW)");
    } else {
      // HIGH → mở relay → quạt tắt
      digitalWrite(FAN_PIN, HIGH);
      Serial.println(">> Relay Fan: OFF (HIGH)");
    }
  }
  // Điều khiển phun sương (tạo 1 xung)
  else if (strcmp(topic, topicMist) == 0) {
    if (msgBuf[0] == '1') {
      mistFlag = true;
      Serial.println(">> Mist flag set");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // 1. Khởi tạo GPIO ngay lập tức và kéo HIGH để relay OFF
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, HIGH);   // IN1 = HIGH → relay Fan tắt

  pinMode(MIST_PIN, OUTPUT);
  digitalWrite(MIST_PIN, HIGH);  // IN2 = HIGH → relay Mist tắt

  // 2. Kết nối Wi-Fi
  Serial.print("WiFi connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected, IP = ");
  Serial.println(WiFi.localIP());

  // 3. Cấu hình MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(onMqttMessage);

  Serial.print("MQTT connecting to ");
  Serial.print(mqttServer);
  Serial.print(":");
  Serial.print(mqttPort);
  if (client.connect("ESPTestClient")) {
    Serial.println(" ✓ Connected");
    client.subscribe(topicFan);
    client.subscribe(topicMist);
  } else {
    Serial.println();
    Serial.print("MQTT: Connect failed, state = ");
    Serial.println(client.state());
  }
}

void loop() {
  // Reconnect MQTT nếu mất kết nối
  if (!client.connected()) {
    Serial.print("MQTT disconnected, state = ");
    Serial.println(client.state());
    delay(3000);
    if (client.connect("ESPTestClient")) {
      Serial.println("Reconnected ✓");
      client.subscribe(topicFan);
      client.subscribe(topicMist);
    }
  }
  client.loop();

  // Xử lý phun sương nếu flag = true
  if (mistFlag) {
    mistFlag = false;
    digitalWrite(MIST_PIN, LOW);   // IN2 = LOW → relay Mist đóng
    delay(200);
    digitalWrite(MIST_PIN, HIGH);  // IN2 = HIGH → relay Mist mở
    Serial.println(">> Mist pulse sent");
  }
}
