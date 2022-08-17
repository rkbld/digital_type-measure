#include <AS5600.h>
#include <SPI.h>
#include <Wire.h>
#include <GyverOLED.h>
#include <ezButton.h>

int set = 0;

byte BUTTON_PIN = 3;
String ONSTART = "Hello";
bool flag = false;
uint32_t btnTimer = 0;

static long distance;//!
static long start;
static long rev;

float area = 0.00;
float diameter = 0;
float angle = 0;
float hepotenuse = 0;
float cubic = 0;
float prev_len;
float sec_len;
float cm = 0;
int prev_rer;
float x;

ezButton button(BUTTON_PIN);

AS5600 encoder;
float DIAMETER = 10.0;
float coeficient = (DIAMETER * 3.141592) / 4096;
//float coeficient = (DIAMETER * 3.141592) / 1024;

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;
byte get_q(int mes) {
  byte quad;
    if (mes >= 0 && mes <= 1024)
      quad = 1;
    if (mes > 1024 && mes <= 2048)
      quad = 2;
    if (mes > 2048 && mes <= 3072)
      quad = 3;
    if (mes > 3072 && mes <= 4095)
      quad = 4;
//  if (mes >= 0 && mes <= 255)
//    quad = 1;
//  if (mes > 255 && mes <= 512)
//    quad = 2;
//  if (mes > 512 && mes <= 768)
//    quad = 3;
//  if (mes > 768 && mes <= 1023)
//    quad = 4;
  return quad;
}
void display(float cm, float diameter, float area, float angle, float hepotenuse, float cubic);
void reset();

float getDist() {
  if (button.isPressed()) {
    distance = 0;//?
//    start = analogRead(A0);
    start = encoder.getRawAngle();
    rev = 0;
  }
  bool f = true;
  int out;
  byte prev_quad;
  byte quad;
  byte start_quad = get_q(start);

  //  int start = encoder.getRawAngle();
  bool dw = false;
  while (f) {
    button.loop();
    //out = analogRead(A0);
    out = encoder.getRawAngle();
    quad = get_q(out);
    if (button.isPressed())
      f = false;
    if (prev_quad == 1 && quad == 4 && dw == true) {
      dw = false;
      if (rev > 0)
        rev--;
      else {
        rev = 0;
        distance = 0;
        start = out;
        
      }
    }
    else if(prev_quad == 1 && quad == 4 && dw == false && start_quad == 1 && rev == 0){
        rev = 0;
        distance = 0;
        start = out;
      }
    if (prev_quad == 2 && quad == 3)
      dw = false;

    if (prev_quad == 3 && quad == 2)
      dw = true;
    if (prev_quad == 4 && quad == 1 && dw == false) {
      rev++;
      dw = true;
    }

    prev_quad = quad;

    if (rev > 0) {
      distance = out + ((rev * 4096) - start);
//      distance = out + ((rev * 1024) - start);
      if (distance < 0) {
        distance = 0;
        start = out;
      }
    }
    else
      distance = out - start;
    if (distance < 0) {
      distance = 0;
      start = out;
    }
    display((distance * coeficient), diameter, area, angle, hepotenuse, cubic);

  }
  if (rev > 0) {
    distance = out + ((rev * 4096) - start);
    //distance = out + ((rev * 1024) - start);
    if (distance < 0) {
      distance = 0;
      start = out;
    }
  }
  else
    distance = out - start;
  if (distance < 0) {
    distance = 0;
    start = out;
  }
  return distance * coeficient;
  //0.00766 mm distance per step with radius 5 mm (dist = steps * (diameter * P)/4096)
}

void display(float cm, float diameter, float area, float angle, float hepotenuse, float cubic) {
  oled.clear();
  oled.setScale(1);
  oled.setCursor(0, 1);   // курсор в (пиксель X, строка Y)
  oled.print("Length: ");
  oled.setCursor(48, 1);
  oled.print(cm);
  oled.setCursor(111, 1);
  oled.print("mm");
  oled.setCursor(0, 2);   // курсор в (пиксель X, строка Y)
  oled.print("Diamet: ");
  oled.setCursor(48, 2);
  oled.print(diameter);
  oled.setCursor(111, 2);
  oled.print("mm");
  oled.setCursor(0, 3);   // курсор в (пиксель X, строка Y)
  oled.print("Area: ");
  oled.setCursor(48, 3);
  oled.print(area);
  oled.setCursor(111, 3);
  oled.print("mm2");
  oled.setCursor(0, 4);   // курсор в (пиксель X, строка Y)
  oled.print("Angle: ");
  oled.setCursor(48, 4);
  oled.print(angle);
  oled.setCursor(111, 4);
  oled.print("deg");
  oled.setCursor(0, 5);   // курсор в (пиксель X, строка Y)
  oled.print("Hepots: ");
  oled.setCursor(48, 5);
  oled.print(hepotenuse);
  oled.setCursor(111, 5);
  oled.print("mm");
  oled.setCursor(0, 6);   // курсор в (пиксель X, строка Y)
  oled.print("Volume: ");
  oled.setCursor(48, 6);
  oled.print(cubic);
  oled.setCursor(111, 6);
  oled.print("mm3");
  oled.update();
}

void reset() {
  cm = 0;
  area = 0;
  diameter = 0;
  angle = 0;
  hepotenuse = 0;
  cubic = 0;
  prev_len = 0;
  sec_len = 0;
  set = 0;
  distance = 0;//?
  //start = analogRead(A0);
  start = encoder.getRawAngle();
  rev = 0;
  delay(800);
}

void setup()
{
  oled.init();
  Wire.begin();
  prev_rer = encoder.getRawAngle();
  //prev_rer = analogRead(A0);
  oled.clear();
  start = analogRead(A0);
  display(cm, diameter, area, angle, hepotenuse, cubic);
}

void loop()
{
  bool moved;
  button.loop();
  //int rer = analogRead(A0);
  int rer = encoder.getRawAngle();
  if (rer - prev_rer >= 10 && set != 3)
    moved = true;
  else
    moved = false;
  if (button.isPressed() || moved && !flag) {
    flag = true;
    btnTimer = millis();
    switch (set)
    {
      case 0:
        cm = getDist();
        diameter = cm / 3.14;
        prev_len = cm;
        display(cm, diameter, area, angle, hepotenuse, cubic);
        set = 1;
        delay(100);
        break;

      case 1:
        cm = getDist();
        sec_len = cm;
        diameter = cm / 3.14;
        area = prev_len * cm;
        angle = atan(prev_len / cm) * 180 / 3.14;
        x = (prev_len * prev_len) + (cm * cm);
        hepotenuse = sqrt(x);
        display(cm, diameter, area, angle, hepotenuse, cubic);
        set = 2;
        delay(100);
        break;
      case 2:
        cm = getDist();
        area = sec_len * cm;
        angle = atan(sec_len / cm) * 180 / 3.14;
        cubic = (cm * prev_len * sec_len) / 10000;
        display(cm, diameter, area, angle, hepotenuse, cubic);
        set = 3;
        delay(100);
        break;
      case 3:
        reset();
        display(cm, diameter, area, angle, hepotenuse, cubic);
        break;

    }

  }
  prev_rer = rer;
  if (button.isReleased()) {
    flag = false;
    btnTimer = millis();
  }

}
