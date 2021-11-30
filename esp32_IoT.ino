#include <Ambient.h>
#include <WiFi.h>
#include <Wire.h>
//#include <FreeRTOS.h>
//#include <esp32-hal-log.h>

#define DELAYTIME 30000 //ms

// LED点灯用設定
#define LED_PIN 25
bool LED_FLG = true;

//Ambient設定
unsigned int channelId = 44361;
const char* writeKey = "80be9e962dc558c0";

// WiFi設定
const char* ssid = "IODATA-ae53fc-2G";
const char* passwd = "9008815764874";
WiFiClient client;
Ambient ambient;

// ADT7140(温度センサ)設定
#define ADT7410_ADDR 0x48

// デュアルコアテスト

//void task0 (void* arg)
//{
//  while(1) {
//    log_i("This is Core 0");
//    delay(1000);
//  }
//}
//
//void task1(void* arg)
//{
//  while(1)
//  {
//    log_i("This is Core 1");
//    delay(1500);
//  }
//}

void setup() {
//  log_i("Hello, this is ESP32");
//  xTaskCreatePinnedToCore(task0, "Task0", 4096, NULL, 1, NULL, 0);
//  xTaskCreatePinnedToCore(task1, "Task1", 4096, NULL, 1, NULL, 1);

  Serial.begin(115200);
  Wire.begin();
  WiFi.begin(ssid, passwd);
  waitWiFiConnect();
  ambient.begin(channelId, writeKey, &client);
}

void loop()
{
  float temp = getCurrentTemp();
  serialPrintCurrentData(temp);
  sendAmbient(temp);
  delay(DELAYTIME);
}

void waitWiFiConnect()
{
  pinMode(LED_PIN, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) // WiFi接続出来るまでループ
  {
    LED_FLG = !LED_FLG; // ループする度にbit反転
    digitalWrite(LED_PIN, LED_FLG); // LED着火/消火
    Serial.print("."); // TODO: ターミナル出力、最後は消す
    delay(500);
  }
  digitalWrite(LED_PIN, HIGH);
  Serial.print("WiFi connected\r\nIP address: ");
  Serial.println(WiFi.localIP());
}

// コンソールにデータ出力
void serialPrintCurrentData(float t)
{
  Serial.print(t, 1);
  Serial.println("℃");
}

// Ambientにデータ送信
void sendAmbient(float t)
{
  ambient.set(1, t);
  ambient.send();
}

// ADT7410より温度取得
float getCurrentTemp()
{
  uint16_t uintVal;
  float t;
  int iVal;

  Wire.requestFrom(ADT7410_ADDR, 2);
  uintVal = (uint16_t)Wire.read() << 8;
  uintVal |= Wire.read();
  uintVal >>= 3;

  if (uintVal & 0x1000)
  {
    iVal = uintVal - 0x2000;
  }
  else
  {
    iVal = uintVal;
  }

  t = (float)iVal / 16.0;

  return t;

}