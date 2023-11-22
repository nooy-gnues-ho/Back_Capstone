#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "time.h"
#define WIFI_SSID "U+NetE351"
#define WIFI_PASSWORD "1C2C022808"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "kr.pool.ntp.org"); // NTP 서버 설정

const int alarmHour = 01; // 알람 시간 (24시간 형식)
const int alarmMinute = 01; // 알람 분
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600*9;
const int daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Wi-Fi 연결
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // NTP 시간 가져오기
  timeClient.begin();
  timeClient.setTimeOffset(9 * 3600);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void loop() {
  timeClient.update(); // NTP 시간 업데이트
  printLocalTime();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentDay = timeClient.getDay();
  

  // 알람 시간이 되면 알람 실행
  if (currentHour == alarmHour && currentMinute == alarmMinute) {
    Serial.println("Alarm time reached!");
    // 여기에서 알람 동작을 수행하세요 (예: 부저 울리기)
  }
  delay(1000); // 1초마다 시간 확인
}
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  int date = (timeinfo.tm_year + 1900) * 10000 + (timeinfo.tm_mon + 1) * 100 + timeinfo.tm_mday;
  Serial.println(date);
}






