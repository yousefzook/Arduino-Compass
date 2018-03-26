#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include <Wire.h>
#include <LSM303.h>

/* definitions */
#define QIBLAH 135.377639     /*qiblah direction for alex*/
#define FLICKER_INTERVAL 100  /*flicker interval when compass direction is near qiblah*/
#define CHAR_H 8              /*lcd character height (for size 1)*/
#define CHAR_W 5              /*lcd character width (for size 1)*/
#define LED_PIN 13            /*lcd flash pin*/


/*pin connections*/
/*
rst     6
ce      7
dc      5
din     4
clk     8
vcc     3.3
light   11
gnd     gnd
*/

/*initiations*/
LSM303 compass; //lsm303 sensor 8, 4, 7, 5, 6)
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 4, 5, 7, 6); //nokia display

int r = 24;  // radius of compass rose
int x0= 58;  // x-origin
int y0 = 24; // y-origin
float angleActual;  //actual angle measured by the sensor
int last_on; //for led flickering



/*function definitions*/
void drawCompass(); // drawing on lcd
void calculateCompass(); //calculate angle
void drawDirections();  //drawing n,w,e,s,q on screen
void drawInit();    //draw splash intro

/*code*/
void setup() {
  /*setup serial communication*/
  Serial.begin(9600);

  /*initiallize nokia lcd library*/
  display.begin();
  display.setContrast(80);
  display.clearDisplay();
  display.display();

  /*initiallize compass library*/
  Wire.begin();
  compass.init();
  compass.enableDefault();

  pinMode(LED_PIN,OUTPUT);
  analogWrite(LED_PIN,190);//light on

  last_on = millis();
  angleActual = 0; // initially put angle= 0
  drawInit();
}

void loop() {
  delay(1000);  //delay for flickering
  drawCompass(); //draw this reading
  calculateCompass();  //calculate compass heading reading
}

void calculateCompass(){
  compass.read(); //read compass values
  angleActual = compass.heading();  //calculate heading value (with -y axis in our case)
  if(last_on - millis() > FLICKER_INTERVAL && fabs(QIBLAH-angleActual) <= 10){
    /* if we are close to qiblah make some flashes*/
    last_on = millis();
    digitalWrite(LED_PIN,!digitalRead(LED_PIN));
  }else if (fabs(QIBLAH-angleActual) > 10){
    analogWrite(LED_PIN,190);
  }
  Serial.print("compass heading: ");
  Serial.println(compass.heading());
}

void drawCompass(){

  display.clearDisplay();
  /*draw the angle*/
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println((int)angleActual);
  Serial.println((int)angleActual);
  drawDirections();
  
  /*print angle indicator inside of compass circle*/
  display.drawLine(x0,y0,x0+r*sin(radians(270-angleActual)),y0-r*cos(radians(270-angleActual)),BLACK);
  display.display();
}

void drawDirections(){
  /*draw the compass circle*/
  display.drawCircle(x0, y0, r, BLACK);

  /*prepare settings for drawing*/
  display.setTextSize(1);
  display.setTextColor(BLACK);

  /*draw directions*/
  display.setCursor(x0-CHAR_W/2,0);
  display.println("N");

  display.setCursor((x0+r)-CHAR_W,y0-CHAR_H/2);
  display.println("E");

  display.setCursor(x0-CHAR_W/2,y0*2-CHAR_H);
  display.println("S");

  display.setCursor((x0-r),y0-CHAR_H/2);
  display.println("W");

  /*draw qiblah indicator*/
  display.setCursor(x0+r*sin(QIBLAH/180.0*PI)-CHAR_W/2,y0-r*cos(QIBLAH/180.0*PI)-CHAR_H/2);
  display.println("Q");
}

void drawInit(){
  /*splash screen drawer*/
    for(int i = 0 ; i < 1 ;i++){
      display.setTextSize(2);
      display.setCursor(0,0);
      display.println("Compass");
      display.display();
      delay(2000);
      display.clearDisplay();
      display.display();
      delay(500);
    }  
}

