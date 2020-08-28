float avgInternalTempF(){
     sensors.requestTemperatures();
       float temperature_Probe04 = sensors.getTempF(Probe04);
       //Serial.print("temperature_Probe04: ") ;
       //Serial.println(temperature_Probe04);
       float temperature_Probe05 = sensors.getTempF(Probe05);
     //  Serial.print("temperature_Probe05: ") ;
      // Serial.println(temperature_Probe05);
      //  float temperature_Probe06 = sensors.getTempF(Probe06);
       float mean=(temperature_Probe04 + temperature_Probe05 )/2;
       return mean;
}

