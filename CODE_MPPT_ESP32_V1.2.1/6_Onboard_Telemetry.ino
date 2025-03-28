void Onboard_Telemetry(){    

  /////////////////////// USB SERIAL DATA TELEMETRY ////////////////////////   
  // 0 - Disable Serial
  // 1 - Display All
  // 2 - Display Essential Data
  // 3 - Display Numbers Only 

  currentSerialMillis = millis();
  if(currentSerialMillis-prevSerialMillis>=millisSerialInterval){   //Run routine every millisRoutineInterval (ms)
    prevSerialMillis = currentSerialMillis;                         //Store previous time
                                                          
      Serial.print(" ERR:");   Serial.print(ERR);
      Serial.print(" BNC:");   Serial.print(BNC);  
      Serial.print(" REC:");   Serial.print(REC);  
      Serial.print(" "); 
      Serial.print(" BYP:");   Serial.print(bypassEnable);
      Serial.print(" EN:");    Serial.print(buckEnable);
      Serial.print(" FAN:");   Serial.print(fanStatus);        
      Serial.print(" ");  
      Serial.print(" PI:");    Serial.print(powerInput,0); 
      Serial.print(" PWM:");   Serial.print(PWM); 
      Serial.print(" PPWM:");  Serial.print(PPWM); 
      Serial.print(" VI:");    Serial.print(voltageInput,1); 
      Serial.print(" VO:");    Serial.print(voltageOutput,1); 
      Serial.print(" CI:");    Serial.print(currentInput,2); 
      Serial.print(" CO:");    Serial.print(currentOutput,2); 
      Serial.print(" Wh:");    Serial.print(Wh,2); 
      Serial.print(" Temp:");  Serial.print(temperature,1);  
      Serial.print(" "); 
      Serial.print(" CSMPV:"); Serial.print(currentMidPoint,3);  
      Serial.print(" CSV:");   Serial.print(CSI_converted,3);   
      Serial.print(" VO%Dev:");Serial.print(outputDeviation,1);   
      Serial.print(" SOC:");   Serial.print(batteryPercent);Serial.print("%");    
      Serial.println("");    
    }  
  } 
