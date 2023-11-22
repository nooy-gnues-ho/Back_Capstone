#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <IOXhop_FirebaseESP32.h>
#include "string.h"
#include "time.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
int flag = 0;
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
int eatM = 0, eatL = 0, eatD = 0;
int saveM = 0, saveL = 0, saveN = 0;
//--------------------------------알람, 시계 ----------------------------

SoftwareSerial mp3Serial(16, 17); //Esp32의 22,23핀을 mp3용 시리얼 통신으로 지정
DFRobotDFPlayerMini MP3Player;

//--------------------------------스피커---------------------------------

#define ir_detect 15

//--------------------------------ir센서-----------------------------

#define PIN_IN1  32 //원통모터
#define PIN_IN2  33 
#define PIN_ENA1  25 
#define PIN_IN3  26 //슬라이드 모터
#define PIN_IN4  27 
#define PIN_ENA2  14 
int motorflag = 3;
int cyM=0;

//--------------------------------모터----------------------------------

#define FIREBASE_HOST "remedical-617a9-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "juPzocWzj96RjpuLu4625vXJG6GtPMxZMtZzSGIe"
int date = 0;

//--------------------------------DB----------------------------------
#define WIFI_SSID "MERCUSYS_9C0C"
#define WIFI_PASSWORD "87739398"
// #define WIFI_SSID "U+NetE351"
// #define WIFI_PASSWORD "1C2C022808"
NTPClient timeClient(ntpUDP, "kr.pool.ntp.org"); 
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void cylinderMotor()
{
  digitalWrite(PIN_IN1, HIGH);  
  analogWrite(PIN_ENA1, 180);
  delay(300);

  digitalWrite(PIN_IN1, LOW);
  analogWrite(PIN_ENA1, 0);
  delay(3000);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void slideMotor()
{
    digitalWrite(PIN_IN3, HIGH);  //right
    analogWrite(PIN_ENA2, 255);
    delay(100);
    digitalWrite(PIN_IN3, LOW);
    analogWrite(PIN_ENA2, 0);
    delay(100);
    digitalWrite(PIN_IN4, HIGH);  //left
    analogWrite(PIN_ENA2, 255);
    delay(100);
    digitalWrite(PIN_IN4, LOW);
    analogWrite(PIN_ENA2, 0);
    delay(100);
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
  String time_1 =  Firebase.getString("/time/illtime"); 
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
    firebase_err_handler();
    if(eatM==0)
    {
      MP3Player.play(1); 
      delay(5000);
    }
    if(cyM == 0)
    {
      cylinderMotor();
      for(int z=0; z<25; z++)
        slideMotor();
      cyM=1;
    }
    

    return 1;
  }
  else if (currentHour == L_alarmHour && currentMinute == L_alarmMinute) 
  {
    Serial.println("점심약 먹을 시간");
    firebase_err_handler();
    if(eatL==0)
    {
      MP3Player.play(2); 
      delay(5000);
    }
    if(cyM == 0)
    {
      cylinderMotor();
      for(int z=0; z<25; z++)
        slideMotor();
      cyM=1;
    }
    return 2;
  }
  else if (currentHour == D_alarmHour && currentMinute == D_alarmMinute) 
  {
    Serial.println("저녁약 먹을 시간");
    firebase_err_handler();
    if(eatD==0)
    {
      MP3Player.play(2); 
      delay(5000);
    }
    if(cyM == 0)
    {
      cylinderMotor();
      for(int z=0; z<25; z++)
        slideMotor();
      cyM=1;
    }
    return 3;
  }
  else
    return 4;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void decom_to_medicine(int period)
{
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  if(period == 1 && digitalRead(ir_detect) == 1) //약이 ir센서의 감지됨
  {
    saveM = digitalRead(ir_detect);
    if(currentHour == M_alarmHour && currentMinute == M_alarmMinute+30) // 30분이내 안먹음
    {
      Serial.println("환자가 아침약을 안먹음");

      Firebase.setString("/sensor/warning_message", "Patient does not take morning medication");
      firebase_err_handler();
    }
    else if(digitalRead(ir_detect) == 0)
    {
      Serial.println("환자가 아침약을 복용하였습니다.");
      Firebase.setString("/sensor/message", "take morning medicine");
      String q = String(date);
      Firebase.setString("/time_to_app/morning/"+q,"0");
      firebase_err_handler();
      eatM=1;eatL=0;eatD=0;
    }
  }
  else if(saveM != digitalRead(ir_detect))
  {
    Serial.println("환자가 아침약을 복용하였습니다.");
    Firebase.setString("/sensor/message", "take morning medicine");
    String q = String(date);
    Firebase.setString("/time_to_app/morning/"+q,"0");
    firebase_err_handler();
    eatM=1;eatL=0;eatD=0;
  }//아침약 

  else if(period == 2 && digitalRead(ir_detect) == 1)
  {
    saveL = digitalRead(ir_detect);
    if(currentHour == L_alarmHour && currentMinute == L_alarmMinute+30)
    {
      Serial.println("환자가 점심약을 안먹음");
      Firebase.setString("/sensor/warning_message", "Patient does not take lunch medication");
      firebase_err_handler();
    }
    else if(digitalRead(ir_detect) == 0)
    {
      Serial.println("환자가 점심약을 복용하였습니다.");
      Firebase.setString("/sensor/message", "take lunch medicine");
      String q = String(date);
      Firebase.setString("/time_to_app/lunch/"+q,"0");
      firebase_err_handler();
      eatM=0;eatL=1;eatD=0;
    }
  }
  else if(saveL != digitalRead(ir_detect))
  {
    Serial.println("환자가 점심약을 복용하였습니다.");
    Firebase.setString("/sensor/message", "take lunch medicine");
    String q = String(date);
    Firebase.setString("/time_to_app/lunch/"+q,"0");
    firebase_err_handler();
    eatM=0;eatL=1;eatD=0;
  }//점심약 

  else if(period == 3 && digitalRead(ir_detect) == 1)
  {
    saveN = digitalRead(ir_detect);
    if(currentHour == D_alarmHour && currentMinute == D_alarmMinute+30)
    {
      Serial.println("환자가 저녁약을 안먹음");
      Firebase.setString("/sensor/warning_message", "Patient does not take evening medication");
      firebase_err_handler();
    }
    else if(digitalRead(ir_detect) == 0)
    {
      Serial.println("환자가 저녁약을 복용하였습니다.");
      Firebase.setString("/sensor/message", "take evening medicine");
      String q = String(date);
      Firebase.setString("/time_to_app/evening/"+q,"0");
      firebase_err_handler();
      eatM=0;eatM=0;eatD=1;
    }
  }
  else if(saveN != digitalRead(ir_detect))
  {
    Serial.println("환자가 저녁약을 복용하였습니다.");
    Firebase.setString("/sensor/message", "take evening medicine");
    String q = String(date);
    Firebase.setString("/time_to_app/evening/"+q,"0");
    firebase_err_handler();
    eatM=0;eatM=0;eatD=1;
  }//저녁약 

  else if(period==1 || period==2 || period==3 && digitalRead(ir_detect)==0) // 감지가 안됨 == 약이 떨어진 상태 
  {
    Serial.println("약 없음");
    Firebase.setString("/sensor/warning_message", "there is no medicine");
    firebase_err_handler();
  }
  else
    Serial.println("시스템 이상 점검바람");
   
}

void printLocalTime() {
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

