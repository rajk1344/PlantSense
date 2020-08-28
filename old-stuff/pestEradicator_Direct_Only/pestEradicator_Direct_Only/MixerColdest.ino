bool mixerColdest(){
        sensors.requestTemperatures();
       float temperature_Probe01 = sensors.getTempF(Probe01);
       //Serial.print("temperature_Probe01: ") ;
       //Serial.println(temperature_Probe01);
       mixedTemperature=temperature_Probe01;
       float temperature_Probe02 = sensors.getTempF(Probe02);
       //Serial.print("temperature_Probe02: ") ;
       //Serial.println(temperature_Probe02);
       hotTankTemperature=temperature_Probe02;
       float temperature_Probe03 = sensors.getTempF(Probe03);
       //Serial.print("temperature_Probe03: ") ;
       //Serial.println(temperature_Probe03);
       coldTankTemperature =temperature_Probe03;
       float answer = ( mixedTemperature < coldTankTemperature + motorToleranceDegreeDelta );
       return answer;
  
}

