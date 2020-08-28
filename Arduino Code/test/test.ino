
/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS_1 2 
#define ONE_WIRE_BUS_2 3 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire_1(ONE_WIRE_BUS_1); 
OneWire oneWire_2(ONE_WIRE_BUS_2); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensor_1(&oneWire_1);
DallasTemperature sensor_2(&oneWire_2);

 
int analogPin = 0;     
int data = 0;           
char userInput;
float a ,b;

void setup(){

  Serial.begin(9600);                        //  setup serial
  sensor_1.begin(); 
  sensor_2.begin(); 

}

void loop(){
  sensor_1.requestTemperatures();
  sensor_2.requestTemperatures();

  Serial.print(sensor_1.getTempCByIndex(0));
  Serial.print(',');
  Serial.print(sensor_2.getTempCByIndex(0));
  Serial.print('\n');
} // Void Loop
