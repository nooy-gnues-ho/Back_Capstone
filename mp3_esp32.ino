#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
SoftwareSerial mp3Serial(16, 17); //Esp32의 22,23핀을 mp3용 시리얼 통신으로 지정
DFRobotDFPlayerMini MP3Player;
void setup () {

  mp3Serial.begin (9600); //mp3용 시리얼 포트 시작
  MP3Player.begin(mp3Serial);
}

void loop () {

  MP3Player.volume(30);
  MP3Player.play(1); 
  delay(5000); 

}