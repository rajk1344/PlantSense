
/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS_1 2 
#define ONE_WIRE_BUS_2 3 
#define ONE_WIRE_BUS_3 4 
#define ONE_WIRE_BUS_4 5
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire_1(ONE_WIRE_BUS_1); 
OneWire oneWire_2(ONE_WIRE_BUS_2); 
OneWire oneWire_3(ONE_WIRE_BUS_3); 
OneWire oneWire_4(ONE_WIRE_BUS_4); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensor_1(&oneWire_1);
DallasTemperature sensor_2(&oneWire_2);
DallasTemperature sensor_3(&oneWire_3);
DallasTemperature sensor_4(&oneWire_4);

 
int analogPin = 0;     
int data = 0;           
char userInput;
float a ,b;

void setup(){

  Serial.begin(9600);                        //  setup serial
  sensor_1.begin(); 
  sensor_2.begin();
  sensor_3.begin(); 
  sensor_4.begin();  

}

void loop(){

if(Serial.available()> 0){ 
    
    userInput = Serial.read();               // read user input
      
      if(userInput == 't'){
            Serial.print('t');
      }
      
      if(userInput == 'g'){                  // if we get expected value 
            sensor_1.requestTemperatures();
            sensor_2.requestTemperatures();
            sensor_3.requestTemperatures();
            sensor_4.requestTemperatures();

            Serial.print(sensor_1.getTempFByIndex(0));
            Serial.print(',');
            Serial.print(sensor_2.getTempFByIndex(0));
            Serial.print(',');            
            Serial.print(sensor_3.getTempFByIndex(0));
            Serial.print(',');
            Serial.print(sensor_4.getTempFByIndex(0));
            
      } // if user input 'g' 
} // Serial.available
} // Void Loop
