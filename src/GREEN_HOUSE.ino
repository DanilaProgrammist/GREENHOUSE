//Блок подключения необходимых библиотек
#include "GyverButton.h"
#include <iarduino_RTC.h>
#include <GyverOLED.h>
#include "GyverTimer.h" 
iarduino_RTC time(RTC_DS1302,7,5,6);   

int light_max = 30;//начальный допустимый уровень света
int water_max = 30;//начальный допустимый уровень воды
unsigned long timer;
//блок для подлключения кнопок, можно поменять
#define BTNWATERPLUS 0
#define BTNWATERMINUS 1
#define BTNLIGHTPLUS 2
#define BTNLIGHTMINUS 3
#define BTNLED 4

#define PUTON_LEVEL 8 //управляющий на транзистор пин, запускающий датчик почвы
#define CHECK_FREQUENCY 900000 // частота проверки в миллисекундах(1000*60*60) 15 минут
#define LAMP 9//пин управления реле на лампу
#define PUMP 10//пин управления насосом
#define WATERING_TIME 3000 // время полива(1000*60*60) 3 секунды
#define START_TIME 8 //начальное время работы
#define END_TIME 20 //конечное время работы
bool LED_ON = true;

GButton butt1(BTNWATERPLUS);
GButton butt2(BTNWATERMINUS);
GButton butt3(BTNLIGHTPLUS);
GButton butt4(BTNLIGHTMINUS);
GButton butt5(BTNLED);

GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> dis;
GTimer myTimer(MS, 3000);
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  set_time();
  show_dis();
  pinMode(PUTON_LEVEL, OUTPUT);
  pinMode(A0,INPUT);
  pinMode(LAMP, OUTPUT);
  pinMode(PUMP, OUTPUT);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  control();
  butt5.tick();
  if (butt5.isClick()) dis.clear();
  String hour = time.gettime("H");
  int val = hour.toInt();
  if ((val >= START_TIME) && (val <=END_TIME)){
      get_testimony();
  }
}
void show_dis(){
  dis.init();
  dis.clear();
  dis.setScale(2);
  dis.home();
  dis.print("Свет ");
  dis.setScale(3);
  dis.print(light_max);
  dis.setScale(2);
  dis.setCursor(0, 4);
  dis.print("Вода ");
  dis.setScale(3);
  dis.print(water_max);
}
void control(){
  butt1.tick();
  butt2.tick();
  butt3.tick();
  butt4.tick();
  if (butt1.isClick()){
    light_max = light_max + 5;
    if (light_max >=100){
      light_max = 5;
    }
    if (light_max <=0){
      light_max = 100;
    }
    dis.clear();
    show_dis();
  }
  if (butt2.isClick()){
    light_max = light_max - 5;
    if (light_max >=100){
      light_max = 5;
    }
    if (light_max <=0){
      light_max = 100;
    }
    dis.clear();
    show_dis();
  }
  if (butt3.isClick()){
    water_max = water_max  + 5;
    if (water_max >=100){
      water_max = 5;
    }
    if (water_max <=0){
      water_max = 100;
    }
    dis.clear();
    show_dis();
  }
  if (butt4.isClick()){
    water_max = water_max - 5;
    if (water_max >=100){
      water_max = 5;
    }
    if (water_max <=0){
      water_max = 100;
    }
    dis.clear();
    show_dis();
  }
}
void set_time(){
  time.begin();
  time.settime(59,14,14,8,4,22,5);//здесь задаётся начальное время: секунды, минуты, часы, день, месяц, год, день недели
}
void get_testimony(){
  if (millis() - timer > CHECK_FREQUENCY ){
    digitalWrite(PUTON_LEVEL, 1);
    int water_val = map(analogRead(A0),0,1023,0,100);
    digitalWrite(PUTON_LEVEL, 0);
    int light_level = map(analogRead(A1),0,1023,0,100);
    check_water(water_val);
    check_light( light_level);
    Serial.print("Уровень воды: ");
    Serial.println(water_val);
    Serial.print("Уровень света: ");
    Serial.println(light_level);
    timer = millis();
  }
}
void check_water(int wat){
  if (wat < water_max){
    digitalWrite(PUMP, 1);
    delay(WATERING_TIME);
    digitalWrite(PUMP,0);
  }
}
void check_light(int light){
  if (light < light_max){
    digitalWrite(LAMP, 1);
  }
  else{
    digitalWrite(LAMP, 0);
  }
}
