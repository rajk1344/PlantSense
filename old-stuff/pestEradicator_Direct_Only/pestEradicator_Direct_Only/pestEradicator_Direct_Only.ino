//Temperature sensor libraries
#include "OneWire.h"
#include "DS18B20.h"
#include <DallasTemperature.h>

//Timer
//#include <SimpleTimer.h>
//SimpleTimer timer; //may not work

//Touch screen libraries + header files
#include <stdint.h>
#include <SPI.h>
#include <Wire.h>
uint8_t addr  = 0x38;
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"

//Pin for temperature sensors
#define ONE_WIRE_BUS_PIN 31//<---------------------------pin for sensor signal

//Pin for touch screen
#define RA8875_INT 4
#define RA8875_CS 10
#define RA8875_RESET 9
#define FT5206_WAKE 6
#define FT5206_INT   7
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t tx, ty;

//Touch screen functions + variables
enum {
  eNORMAL = 0,
  eTEST   = 0x04,
  eSYSTEM = 0x01
};

struct TouchLocation {
  uint16_t x;
  uint16_t y;
};

TouchLocation touchLocations[5];

uint8_t readFT5206TouchRegister( uint8_t reg );
uint8_t readFT5206TouchLocation( TouchLocation * pLoc, uint8_t num );
uint8_t readFT5206TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len );

uint8_t buf[30];

uint8_t readFT5206TouchRegister( uint8_t reg ) {
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  uint8_t retVal = Wire.endTransmission();

  uint8_t returned = Wire.requestFrom(addr, uint8_t(1) );    // request 6 uint8_ts from slave device #2

  if (Wire.available()) {
    retVal = Wire.read();
  }

  return retVal;
}

uint8_t readFT5206TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len ) {
  Wire.beginTransmission(addr);
  Wire.write( regAddr );  // register 0
  uint8_t retVal = Wire.endTransmission();

  uint8_t returned = Wire.requestFrom(addr, len);    // request 1 bytes from slave device #2

  uint8_t i;
  for (i = 0; (i < len) && Wire.available(); i++) {
    pBuf[i] = Wire.read();
  }

  return i;
}

uint8_t readFT5206TouchLocation( TouchLocation * pLoc, uint8_t num ) {
  uint8_t retVal = 0;
  uint8_t i;
  uint8_t k;

  do
  {
    if (!pLoc) break; // must have a buffer
    if (!num)  break; // must be able to take at least one

    uint8_t status = readFT5206TouchRegister(2);

    static uint8_t tbuf[40];

    if ((status & 0x0f) == 0) break; // no points detected

    uint8_t hitPoints = status & 0x0f;

    Serial.print("number of hit points = ");
    Serial.println( hitPoints );

    readFT5206TouchAddr( 0x03, tbuf, hitPoints*6);

    for (k=0,i = 0; (i < hitPoints*6)&&(k < num); k++, i += 6)
    {
      pLoc[k].x = (tbuf[i+0] & 0x0f) << 8 | tbuf[i+1];
      pLoc[k].y = (tbuf[i+2] & 0x0f) << 8 | tbuf[i+3];
    }

    retVal = k;

  } while (0);

  return retVal;
}

void writeFT5206TouchRegister( uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  Wire.write( val );  // value

  uint8_t retVal = Wire.endTransmission();
}

void readOriginValues(void)
{
  writeFT5206TouchRegister(0, eTEST);
  delay(1);
  //uint8_t originLength = readFT5206TouchAddr(0x08, buf, 8 );
  uint8_t originXH = readFT5206TouchRegister(0x08);
  uint8_t originXL = readFT5206TouchRegister(0x09);
  uint8_t originYH = readFT5206TouchRegister(0x0a);
  uint8_t originYL = readFT5206TouchRegister(0x0b);

  uint8_t widthXH  = readFT5206TouchRegister(0x0c);
  uint8_t widthXL  = readFT5206TouchRegister(0x0d);
  uint8_t widthYH  = readFT5206TouchRegister(0x0e);
  uint8_t widthYL  = readFT5206TouchRegister(0x0f);
}

//Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);

//Pass our oneWire reference to Dallas Temperature
DallasTemperature sensors(&oneWire);

//Declare variables for each temperature sensor
//Add or delete these lines depending on how many temperature probes you have
//See the tutorial on how to obtain these addresses:
//http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html
DeviceAddress Probe01 = {0x28, 0xFF, 0x1A, 0x61, 0xB1, 0x17, 0x05, 0x3C};//<---- MIX
DeviceAddress Probe02 = {0x28, 0xFF, 0x7C, 0x3A, 0x60, 0x17, 0x03, 0x92};//<---- HOT TANK
DeviceAddress Probe03 = {0x28, 0xFF, 0x75, 0x1F, 0x60, 0x17, 0x05, 0x4D};//<---- COLD TANK
DeviceAddress Probe04 = {0x28, 0xFF, 0x84, 0x14, 0x60, 0x17, 0x03, 0x6F};//<---- RED
DeviceAddress Probe05 = {0x28, 0xFF, 0x95, 0x3C, 0x60, 0x17, 0x05, 0x6F};//<---- GREEN
//DeviceAddress Probe06 = {0x28, 0xFF, 0xC4, 0xBC, 0x60, 0x17, 0x05, 0xEE};//<----YELLOW

//Declare variables for motor pins (forward and reverse)
int forwardMotorPin = 23;//<------------------------------------------------------------------------------Pin Numbers
int reverseMotorPin = 25;

//Declare variable for relay pin
bool pump=0;
int relayPin = 27;

//Declare variable for submerged pressure pin
int pressurePin = A10;

//Declare variables for final temperatrue and current average temperature
int finalTemperature = 114;//<-----------------------must be greater then coldtemp
float averageTemperature;
float tolerance=1;//<----------------------------------------------------------------------------tolerance for temp differance
int coldtemp = 100;//<------------will be less then this
//Declare variables for "time to delay motor" and "time to step motor"
int delayTime = 2000;
int stepTime = 100;

float hotTankTemperature = 0.; // temperature of the water measured on the sensor on the pipe from HOT water tank
float coldTankTemperature = 0.; // temperature of the water measured on the sensor on the pipe from COLD water tank
float mixedTemperature = 0.; // temperature of the water measured on the sensor on the pipe just past the mixer (PROBE01)
int motorToleranceDegreeDelta = 3; // positive real number - temperature 2-5 degrees to use avoiding that the motor tries to move the mixer past the stop
bool isMixerHottest; // true if mixer reached hottest position
bool isMixerColdest; // true if mixer reached coldest position

//Declare variable for voltage of pressure pin
float voltage = 0.0;

float waterLevel = 0.0;//<--hot water tank
int opModeFlag = 0;//<--code state
//

void setup(){
  //Start serial port to show results
  Serial.begin(9600);
  Serial.println("RA8875 start");
  Wire.begin();

  readOriginValues();
  pinMode     (FT5206_WAKE, INPUT);
  digitalWrite(FT5206_WAKE, HIGH );
  writeFT5206TouchRegister(0, eNORMAL);

  while (!tft.begin(RA8875_800x480))
  {
    Serial.println("RA8875 Not Found!");
    delay(100);
  }

  Serial.println("Found RA8875");

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);


  pinMode     (FT5206_INT, INPUT);
  //digitalWrite(FT5206_INT, HIGH );

  tft.touchEnable(false);

  randomSeed(analogRead(0));

//  timer.setInterval(1000, getTime);

  //Initialize the temperature measurement library
  sensors.begin();

  //Set the resolution to 12 bit (Can be 9 to 12 bits... lower is faster)
  //Add or delete these lines depending on how many temperature probes you have
  sensors.setResolution(Probe01, 12);
  sensors.setResolution(Probe02, 12);
  sensors.setResolution(Probe03, 12);
  sensors.setResolution(Probe04, 12);
  sensors.setResolution(Probe05, 12);
  //sensors.setResolution(Probe06, 12);

  //Set all the motor control pins to outputs
  pinMode(relayPin, OUTPUT);
  pinMode(forwardMotorPin, OUTPUT);
  pinMode(reverseMotorPin, OUTPUT);

//  digitalWrite(relayPin, HIGH);
}

void loop(){
   Serial.print("from the top");
  tft.fillScreen(RA8875_WHITE);//clear screen
while ((opModeFlag == 0)==true) {
  //tft.fillScreen(RA8875_WHITE);
  directGui();//draw buttons and start water check
  pump_button_off();//draw pump button
  digitalWrite(relayPin, LOW);// pump off
  while(true){
    //display various information on screen and serial monitor
        sensors.requestTemperatures();
       float temperature_Probe01 = sensors.getTempF(Probe01);
       Serial.print("temperature_Probe01: ") ;
       Serial.println(temperature_Probe01);
       mixedTemperature=temperature_Probe01;
       float temperature_Probe02 = sensors.getTempF(Probe02);
       Serial.print("temperature_Probe02: ") ;
       Serial.println(temperature_Probe02);
       hotTankTemperature=temperature_Probe02;
       float temperature_Probe03 = sensors.getTempF(Probe03);
       Serial.print("temperature_Probe03: ") ;
       Serial.println(temperature_Probe03);
       coldTankTemperature =temperature_Probe03;
       float temperature_Probe04 = sensors.getTempF(Probe04);
       Serial.print("temperature_Probe04: ") ;
       Serial.println(temperature_Probe04);
       float temperature_Probe05 = sensors.getTempF(Probe05);
       Serial.print("temperature_Probe05: ") ;
       Serial.println(temperature_Probe05);
    //   float temperature_Probe06 = sensors.getTempF(Probe06);
     //  Serial.print("temperature_Probe06: ") ;
      // Serial.println(temperature_Probe06);
      
       float mean=avgInternalTempF();     
         
       isMixerHottest == ( mixedTemperature > hotTankTemperature - motorToleranceDegreeDelta );
       isMixerColdest == ( mixedTemperature < coldTankTemperature + motorToleranceDegreeDelta );
       ProbeTemp_screen( "MX",temperature_Probe01, 450, 60);       
       ProbeTemp_screen( "HT",temperature_Probe02, 450, 100);
       ProbeTemp_screen( "CD",temperature_Probe03, 450, 140);
       ProbeTemp_screen( "IN",mean, 450, 180);
       water_level_display();

    //touch screen initialize
    static uint16_t w = tft.width();
    static uint16_t h = tft.height();
    float xScale = 1024.0F/w;
    float yScale = 1024.0F/h;

    uint8_t attention = digitalRead(FT5206_INT);
    static uint8_t oldAttention = 1;
    uint8_t i=0;
      /* Wait around for touch events */
    if (!attention && oldAttention ){
      uint8_t count = readFT5206TouchLocation( touchLocations, 5 );

      //static uint8_t lastCount = count;

     if (count)
      {
        static TouchLocation lastTouch = touchLocations[0];

        lastTouch = touchLocations[0];// only care about the initial touch

        if ((contains(lastTouch, 310, 55, 80, 80)) && (isMixerHottest == false)) {
         
         Serial.println("up");
         //digitalWrite(forwardMotorPin, HIGH);
         stepHot(500); // turn motor valve hot
        }
        else if ((contains(lastTouch, 310, 155, 80, 80)) &&(isMixerColdest == false)){
         Serial.println("down");
         stepCold(500); //turn motor valve cold
        }
        else if (contains(lastTouch,100, 255, 130, 80)&& (pump == 0) ) {
          pump_button_on();
          Serial.println("on");
          digitalWrite(relayPin, HIGH); //turn pump on
          pump=1;
          delay(500);
        }
      else if (contains(lastTouch,100, 255, 130, 80)&& (pump == 1) ) {
          pump_button_off();
          pump=0;
          digitalWrite(relayPin, LOW);//turn pump off
          delay(500);
        }

      }
  }
  else{
   stepOff(); // stop motor valvue rotation 
   Serial.println("off");     
      }
  } 
}

}

//Increase water temperature
void stepHot(int stepTime) {
  digitalWrite(forwardMotorPin, HIGH);
  delay(stepTime);
//digitalWrite(forwardMotorPin, LOW);
}

//Decrease water temperature
void stepCold(int stepTime) {
  digitalWrite(reverseMotorPin, HIGH);
  delay(stepTime);
  //digitalWrite(reverseMotorPin, LOW);
}

//Turn off motor changing temperature
void stepOff() {
  digitalWrite(forwardMotorPin, LOW);
  digitalWrite(reverseMotorPin, LOW);
}

float checkWaterLevel() {
  voltage = (analogRead(pressurePin) * (5.0 / 1023.0));

  waterLevel = ((voltage - 1.2) / 2.21) * 100;

  Serial.print("Water level is: ");
  Serial.print(waterLevel);
  Serial.print("%");
  Serial.println();
}


void printTemperature(DeviceAddress deviceAddress) {

  float tempC = sensors.getTempC(deviceAddress);

   if (tempC == -127.00) {
   Serial.print("Error getting temperature  ");
   }

   else {
   Serial.print("C: ");
   Serial.print(tempC);
   Serial.print(" F: ");
   Serial.print(DallasTemperature::toFahrenheit(tempC));
   }

}

bool contains (TouchLocation lastTouch, int x, int y, int width, int height) {
  if (lastTouch.x > x && lastTouch.x < x + width && lastTouch.y > y && lastTouch.y < y + height) {
    return true;
  }
  else {
    return false;
  }
}
