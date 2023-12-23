#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <IOXhop_FirebaseESP32.h>
#include "string.h"
#include "time.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
int flag = 0;
int EAT=0;
//--------------------------------내부사용변수------------------------------
int M_alarmHour = 0; // 알람 시간 (24시간 형식) 아침
int M_alarmMinute = 0; // 알람 분 아침
int late_M_alarmHour = 0;
int late_M_alarmMinute = 0;
int L_alarmHour = 0;
int L_alarmMinute = 0;
int late_L_alarmHour = 0;
int late_L_alarmMinute = 0;
int D_alarmHour = 0;
int D_alarmMinute = 0;
int late_D_alarmHour = 0;
int late_D_alarmMinute = 0;
const char* ntpServer = "pool.ntp.org";
const int daylightOffset_sec = 0;
const long gmtOffset_sec = 3600*9;
WiFiUDP ntpUDP;
int saveM = 0, saveL = 0, saveN = 0;
//--------------------------------알람, 시계 ----------------------------

SoftwareSerial mp3Serial(16, 17); //Esp32의 22,23핀을 mp3용 시리얼 통신으로 지정
DFRobotDFPlayerMini MP3Player;

//--------------------------------스피커---------------------------------

#define ir_detect 39

//--------------------------------ir센서-----------------------------

#define PIN_IN1  25 //원통모터
#define PIN_IN2  33 
#define PIN_ENA1  32 
#define PIN_IN3  27 //슬라이드 모터
#define PIN_IN4  26 
#define PIN_ENA2  14
int motorflag = 3;
int cyM=0;
int count=0;
//--------------------------------모터----------------------------------

#define FIREBASE_HOST "remedical-617a9-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "juPzocWzj96RjpuLu4625vXJG6GtPMxZMtZzSGIe"
int date = 0;

//--------------------------------DB----------------------------------
#define WIFI_SSID "오승윤의 iPhone"
#define WIFI_PASSWORD "fbiorange123"
// #define WIFI_SSID "U+NetE351"
// #define WIFI_PASSWORD "1C2C022808"
NTPClient timeClient(ntpUDP, "kr.pool.ntp.org"); 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void cylinderMotor(int f)
{
  if(f==0)
  {
    digitalWrite(PIN_IN1, HIGH);  
    analogWrite(PIN_ENA1, 100);
    delay(450);
    digitalWrite(PIN_IN1, LOW);
    analogWrite(PIN_ENA1, 0);
    delay(3000);
  }
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void slideMotor()
{
  digitalWrite(PIN_IN3, HIGH);  //right
  analogWrite(PIN_ENA2, 255);
  delay(50);
  digitalWrite(PIN_IN3, LOW);
  analogWrite(PIN_ENA2, 0);
  delay(300);
  digitalWrite(PIN_IN4, HIGH);  //left
  analogWrite(PIN_ENA2, 255);
  delay(50);
  digitalWrite(PIN_IN4, LOW);
  analogWrite(PIN_ENA2, 0);
  delay(300);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Connect_WiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GET_NTP_TIME()
{
  timeClient.begin();
  timeClient.setTimeOffset(9 * 3600);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void firebase_err_handler()
{
  if (Firebase.failed()) 
  {
    Serial.print("값 failed:");
    Serial.println(Firebase.error());  
    return;
  }
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void getfire_base()
{
  String time_1 =  Firebase.getString("/time/illtime1"); 
  String part1_1 = time_1.substring(0, 2);
  String part1_2 = time_1.substring(2, 4);
  M_alarmHour = part1_1.toInt();
  M_alarmMinute = part1_2.toInt();
  Serial.print("M : ");
  Serial.print(M_alarmHour);
  Serial.println(M_alarmMinute);
  //---------------아침 시간 받아와 저장
  
  String time_2 = Firebase.getString("/time/illtime2"); 
  String part2_1 = time_2.substring(0, 2);
  String part2_2 = time_2.substring(2, 4);
  L_alarmHour = part2_1.toInt();
  L_alarmMinute = part2_2.toInt();
  Serial.print("L : ");
  Serial.print(L_alarmHour);
  Serial.println(L_alarmMinute);
  //---------------점심 시간 받아와 저장
  
  String time_3 = Firebase.getString("/time/illtime3"); 
  String part3_1 = time_3.substring(0, 2);
  String part3_2 = time_3.substring(2, 4);
  D_alarmHour = part3_1.toInt();
  D_alarmMinute = part3_2.toInt();
  Serial.print("D : ");
  Serial.print(D_alarmHour);
  Serial.println(D_alarmMinute);
  //---------------저녁 시간 받아와 저장
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int Alarm_to_fire()
{
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  if (currentHour == M_alarmHour && currentMinute == M_alarmMinute) 
  {
    Serial.println("아침약 먹을 시간");
    if(cyM == 0)
    {
      cylinderMotor(cyM);
      while(1)
      {
        if(EAT==1)
          break;
        slideMotor();
        count++;
        if(count==100)
        {
          Serial.println("약 없음");
          Firebase.setString("/sensor/warning_message", "there is no medicine");
          firebase_err_handler();
          EAT=1;
        }
        if(digitalRead(ir_detect)==0)
          break;
      }
      cyM=1;
      count=0;
    }
    return 1;
  }
  else if (currentHour == L_alarmHour && currentMinute == L_alarmMinute) 
  {
    Serial.println("점심약 먹을 시간");
    if(cyM == 0)
    {
      cylinderMotor(cyM);
      while(1)
      {
        if(EAT==1)
          break;
        slideMotor();
        count++;
        if(count==100)
        {
          Serial.println("약 없음");
          Firebase.setString("/sensor/warning_message", "there is no medicine");
          firebase_err_handler();
          EAT=1;
        }
        if(digitalRead(ir_detect)==0)
          break;
      }
      cyM=1;
      count=0;
    }
    return 2;
  }
  else if (currentHour == D_alarmHour && currentMinute == D_alarmMinute) 
  {
    Serial.println("저녁약 먹을 시간");
    if(cyM == 0)
    {
      cylinderMotor(cyM);
      while(1)
      {
        if(EAT==1)
          break;
        slideMotor();
        count++;
        if(count==100)
        {
          Serial.println("약 없음");
          Firebase.setString("/sensor/warning_message", "there is no medicine");
          firebase_err_handler();
          EAT=1;
        }
        if(digitalRead(ir_detect)==0)
          break;
      }
      cyM=1;
      count=0;
    }
    return 3;
  }
  else
    return 0;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void decom_to_medicine(int period)
{
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int M_alarmMinuteF = 0;
  int L_alarmMinuteF = 0;
  int D_alarmMinuteF = 0;
  if(currentHour == M_alarmHour && currentMinute == M_alarmMinute+1 || currentHour == L_alarmHour && currentMinute == L_alarmMinute+1 ||currentHour == D_alarmHour && currentMinute == D_alarmMinute+1){
      EAT=0;
      cyM=0;
    }

  if(period == 1) {
    while(1){
      if(EAT==1)
        break;
      MP3Player.play(1); 
      delay(5000);
      if(digitalRead(ir_detect) == 0 && M_alarmMinuteF == 900) {
        Serial.println("환자가 아침약을 안먹음");
        Firebase.setString("/sensor/warning_message", "Patient does not take morning medication");
        firebase_err_handler();
        cyM=0;
        EAT=1;
        break;
      }
      else if(digitalRead(ir_detect) == 1){
        Serial.println("환자가 아침약을 복용하였습니다.");
        Firebase.setString("/sensor/message", "take morning medicine");
        String q = String(date);
        Firebase.setString("/time_to_app/morning/"+q,"0");
        firebase_err_handler();
        EAT=1;
        break;
      }
      M_alarmMinuteF++;
    }
  }

  else if(period == 2)
  {
    while(1)
    {
      if(EAT==1)
        break;
      MP3Player.play(2); 
      delay(5000);
      if(digitalRead(ir_detect) == 0 && L_alarmMinuteF == 900) // 30분이내 안먹음
      {
        Serial.println("환자가 점심약을 안먹음");
        Firebase.setString("/sensor/warning_message", "Patient does not take lunch medication");
        firebase_err_handler();
        cyM=0;
        EAT=1;
        break;
      }
      else if(digitalRead(ir_detect) == 1)
      {
        Serial.println("환자가 점심약을 복용하였습니다.");
        Firebase.setString("/sensor/message", "take lunch medicine");
        String q = String(date);
        Firebase.setString("/time_to_app/lunch/"+q,"0");
        firebase_err_handler();
        EAT=1;
        break;
      }
      L_alarmMinuteF++;
    }
    
  }

  else if(period == 3)
  {
    while(1)
    {
      if(EAT==1)
        break;
      MP3Player.play(1); 
      delay(5000);
      if(digitalRead(ir_detect) == 0 && D_alarmMinuteF == 900) // 30분이내 안먹음
      {
        Serial.println("환자가 저녁약을 안먹음");
        Firebase.setString("/sensor/warning_message", "Patient does not take evening medication");
        firebase_err_handler();
        cyM=0;
        EAT=1;
        break;
      }
      else if(digitalRead(ir_detect) == 1)
      {
        Serial.println("환자가 저녁약을 복용하였습니다.");
        Firebase.setString("/sensor/message", "take evening medicine");
        String q = String(date);
        Firebase.setString("/time_to_app/evening/"+q,"0");
        firebase_err_handler();
        EAT=1;
        break;
      }
      D_alarmMinuteF++;
    }
  }
}

void printLocalTime() 
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  date = (timeinfo.tm_year + 1900) * 10000 + (timeinfo.tm_mon + 1) * 100 + timeinfo.tm_mday;
}

void setup() 
{
  Serial.begin(115200);
  mp3Serial.begin (9600); //mp3용 시리얼 포트 시작
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENA1, OUTPUT);  
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_ENA2, OUTPUT);  
  MP3Player.begin(mp3Serial);
  Connect_WiFi(); // wifi 연결
  GET_NTP_TIME(); // NTP 시간 받아오기 kr 기준 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  pinMode(ir_detect, INPUT);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  MP3Player.volume(30);
}

void loop()
{
  timeClient.update();
  printLocalTime();
  getfire_base();
  flag = Alarm_to_fire();
  decom_to_medicine(flag);
}

