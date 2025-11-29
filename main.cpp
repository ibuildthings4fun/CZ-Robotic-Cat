#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// ---------- WiFi / MQTT 配置 ----------
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* MQTT_BROKER   = "192.168.1.100";
const int   MQTT_PORT     = 1883;
const char* MQTT_TOPIC    = "cat/state";

// ---------- 硬件引脚 ----------
#define NEOPIXEL_PIN   13
#define NEOPIXEL_COUNT 2

#define TAIL_SERVO_PIN 25
#define HEAD_SERVO_PIN 26

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

Servo tailServo;
Servo headServo;

// ---------- 当前状态 ----------
String currentState = "CALM";

// 设置眼睛颜色
void setEyesColor(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

// 根据状态控制灯光和舵机
void applyState(const String& state) {
  currentState = state;

  if (state == "CALM") {
    setEyesColor(0, 50, 0);   // 柔和绿色
    tailServo.write(90);
    headServo.write(90);
  } else if (state == "ALERT") {
    setEyesColor(255, 165, 0); // 橙色
    tailServo.write(120);
    headServo.write(80);
  } else if (state == "CAUTIOUS") {
    setEyesColor(255, 255, 0); // 黄色
    tailServo.write(45);
    headServo.write(100);
  } else if (state == "PANIC") {
    setEyesColor(255, 0, 0);   // 红色
    tailServo.write(0);
    headServo.write(60);
  } else {
    // 未知状态时的兜底
    setEyesColor(0, 0, 50);    // 蓝色
    tailServo.write(90);
    headServo.write(90);
  }
}

// 处理 MQTT 消息回调
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  // 期望收到的 JSON 形如：{"state":"ALERT"}
  int idx = msg.indexOf("\"state\"");
  if (idx != -1) {
    int colon = msg.indexOf(':', idx);
    int quote1 = msg.indexOf('"', colon + 1);
    int quote2 = msg.indexOf('"', quote1 + 1);
    if (quote1 != -1 && quote2 != -1) {
      String state = msg.substring(quote1 + 1, quote2);
      applyState(state);
    }
  }
}

// 连接 WiFi
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// 连接 MQTT
void connectMQTT() {
  while (!client.connected()) {
    String clientId = "CZCat-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(MQTT_TOPIC);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pixels.begin();
  pixels.clear();
  pixels.show();

  tailServo.attach(TAIL_SERVO_PIN);
  headServo.attach(HEAD_SERVO_PIN);

  connectWiFi();

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(mqttCallback);

  connectMQTT();

  applyState("CALM");
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  // 这里可以按需要加入轻微的待机动作
  delay(10);
}
