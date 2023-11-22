

#define PIN_IN1  25 //원통모터
#define PIN_IN2  33 
#define PIN_ENA1  32 
#define PIN_IN3  26 //슬라이드 모터
#define PIN_IN4  27 
#define PIN_ENA2  14 
void setup() 
{
  Serial.begin(115200);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENA1, OUTPUT);  
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_ENA2, OUTPUT);  
}

void loop() 
{/*
  digitalWrite(PIN_IN1, LOW); 
  digitalWrite(PIN_IN2, HIGH);  
  analogWrite(PIN_ENA1, 180);
  delay(300);
  Serial.println("stop");
  digitalWrite(PIN_IN2, LOW);
  analogWrite(PIN_ENA1, 0);
  delay(3000);*/
  digitalWrite(PIN_IN4, HIGH);
  analogWrite(PIN_ENA2, 255);
  delay(100);
  digitalWrite(PIN_IN4, LOW);
  analogWrite(PIN_ENA2, 0);
  delay(100);
  digitalWrite(PIN_IN3, HIGH);
  analogWrite(PIN_ENA2, 255);
  delay(100);
  digitalWrite(PIN_IN3, LOW);
  analogWrite(PIN_ENA2, 0);
  delay(1000);
}/*
//-------------------------------------------------------슬라이드센서
#define PIN_IN1  25 // ESP32 pin GPIO27 connected to the IN1 pin L298N
#define PIN_IN2  26 // ESP32 pin GPIO26 connected to the IN2 pin L298N
#define PIN_ENA  14 // ESP32 pin GPIO14 connected to the EN1 pin L298N
float get_position(int analog_pin)
{
  return analogRead(A0)/1023.0;
} 

void setup() 
{
  Serial.begin(115200);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(ENABLE_A, OUTPUT);
}
void loop() 
{
  digitalWrite(motor2, HIGH);  //right
  analogWrite(ENABLE_A, 255);
  delay(50);
  digitalWrite(motor2, LOW);
  analogWrite(ENABLE_A, 0);
  delay(3000);

  digitalWrite(motor1, HIGH);  //left
  analogWrite(ENABLE_A, 255);
  delay(50);
  digitalWrite(motor1, LOW);
  analogWrite(ENABLE_A, 0);
  delay(3000);
  //50에 255가 적당 
}
*/
