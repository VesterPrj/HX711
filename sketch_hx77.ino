#include <Wire.h>
#include "HX711.h"

#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // ширина дисплея в пикселях 
#define SCREEN_HEIGHT 64 // высота дисплея в пикселях

// Создаем объект дисплея 
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



int Button_1 = 22;  //Кнопка увеличения коэффициента калибровки
int Button_2 = 23;  //Кнопка уменьшения коэффициента калибровки
int Button_3 = 24;  //Кнопка применения коэффициента калибровки
int Button_4 = 25;  //Кнопка назначения нулевого веса

// Подключение модуля ацп
const int LOADCELL_DOUT_PIN = A0;
const int LOADCELL_SCK_PIN = A1;

// Создаем Объект модуля АЦП
HX711 scale;

float calibration_factor =111.9  ; // коэффициент калибровки
float units;  //Код АЦП
float ounces;  // Вес в унциях
float gramm;  // Вес в граммах
float gramm_max=0;  // Вес в граммах
float gramm_min=1000;  // Вес в граммах


void setup() {
  Serial.begin(9600);  //   запускаем последновательный порт на скорости 9600


  
  // Запускаем дисплей
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // по адрессу 0x3С 
    Serial.println(F("SSD1306 allocation failed")); //выводим сообщение если не получилось
    for(;;);
  }
  // Очищаем дисплей
  display.clearDisplay();   
  display.setTextColor(WHITE); 
  display.setTextSize(1); 

  // Выводим информационый экран
  display.setTextSize(2); 
  display.setTextColor(WHITE); 
  display.setCursor(0,32); 
  display.println("TMM-002");
  display.setTextSize(1); 
  display.setCursor(0,48); 
  display.println("WWW.INTEREL.RU");
  display.display();

  //Запускаем АЦП
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  delay(5000);
  scale.tare();                              //Сбрасываем на 0
   scale.set_scale(calibration_factor);       //Применяем калибровку

   pinMode(Button_1, INPUT);
   pinMode(Button_2, INPUT);
   pinMode(Button_3, INPUT);
   pinMode(Button_4, INPUT);


}

void loop() {


  
  Serial.print("Reading: ");
  
  //for(int i = 0;i < 10; i ++) units =+ scale.get_units(), 10;   // усредняем показания считав 10 раз 
  //units / 10;                                                   // делим на 10

units = scale.get_units();

   ounces=units;
  gramm = ounces * 0.035274;                                    // переводим унции в граммы              
  Serial.print(gramm);                                          // отправляем в монитор порта
  Serial.print(" grams");  
  Serial.println(); 
  display.clearDisplay();

  display.setTextSize(1); 
  display.setCursor(0,0);

if (gramm_min>gramm)gramm_min=gramm;
if (gramm_max<gramm)gramm_max=gramm;

 //Увеличиваем коэффициент калибровки
if(digitalRead(Button_1) == LOW) {
 display.print("1: On"); calibration_factor+=0.1;
 } else {
 display.print("1: Off");
 }

  display.setCursor(0,8);

 //Уменьшаем коэффициент калибровки
if(digitalRead(Button_2) == LOW) {
 display.print("2: On"); calibration_factor-=0.1;
 
 } else {
 display.print("2: Off");
 }

  display.setCursor(64,0);

 //Применяем калибровочный коэффициент
if(digitalRead(Button_3) == LOW) {
 display.print("3: On");    scale.set_scale(calibration_factor);       //Применяем калибровку
 gramm_max=0;
 gramm_min=1000;
 } else {
 display.print("3: Off");
 }
  display.setCursor(64,8);

 //Принимаем текущий вес за 0
if(digitalRead(Button_4) == LOW) {
 display.print("4: On"); 
   scale.tare();                              //Сбрасываем на 0
   scale.set_scale(calibration_factor);       //Применяем калибровку
 gramm_max=0;
 gramm_min=1000;
 } else {
 display.print("4: Off");
 }


  // Выводим на экран информацию о текущем весе
  display.setTextSize(2); 
  display.setCursor(0,16);
  display.print("BEC:");
  display.println(gramm);

  display.setTextSize(2); 
  display.setCursor(0,32);
  display.print("DIF:");
  display.println(gramm_max-gramm_min);

    display.setTextSize(2); 
  display.setCursor(0,48);
    display.print("K:");
  display.println(calibration_factor);


  display.display();


}
