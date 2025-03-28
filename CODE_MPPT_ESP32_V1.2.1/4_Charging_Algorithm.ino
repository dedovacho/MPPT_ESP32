void buck_Enable(){                                                                  //Enable MPPT Buck Converter
  buckEnable = 1;
  digitalWrite(buck_EN,HIGH);
  digitalWrite(LED,HIGH);
}
void buck_Disable(){                                                                 //Disable MPPT Buck Converter
  buckEnable = 0; 
  digitalWrite(buck_EN,LOW);
  digitalWrite(LED,LOW);
  PWM = 0;
}   
void predictivePWM(){                                                                //PREDICTIVE PWM ALGORITHM 
  if(voltageInput<=0){PPWM=0;}                                                       //Prevents Indefinite Answer when voltageInput is zero
  else{PPWM =(PPWM_margin*pwmMax*voltageOutput)/(100.00*voltageInput);}              //Compute for predictive PWM Floor and store in variable
  PPWM = constrain(PPWM,0,pwmMaxLimited);
}   

void PWM_Modulation(){
    predictivePWM();                                                                 //Runs and computes for predictive pwm floor
    PWM = constrain(PWM,PPWM,pwmMaxLimited);                                         //CHARGER MODE PWM - limit floor to PPWM and ceiling to maximim allowable duty cycle)                                        
    ledcWrite(pwmChannel,PWM);                                                       //Set PWM duty cycle and write to GPIO when buck is enabled
    buck_Enable();                                                                   //Turn on MPPT buck (IR2104)
}
     
void Charging_Algorithm(){
  if(ERR>0||chargingPause==1){buck_Disable();}                                       //Nếu có lỗi
  else{
    if(REC==1){                                                                      //IUV RECOVERY - (Only active for charging mode)
      REC=0;                                                                         //Reset IUV recovery boolean identifier 
      buck_Disable();                                                                //Disable buck before PPWM initialization
      lcd.setCursor(0,0);lcd.print("POWER SOURCE    ");                              //Display LCD message
      lcd.setCursor(0,1);lcd.print("DETECTED        ");                              //Display LCD message 
      for(int i = 0; i<40; i++){Serial.print(".");delay(30);}                        //For loop "loading... effect                                                      
      Read_Sensors();
      predictivePWM();
      PWM = PPWM; 
      lcd.clear();
    }  
    else{                                                                                     
        /////////////////////// THUẬT TOÁN SẠC ///////////////////////                                                                                                                                                          
        if(currentOutput>currentCharging){PWM--;}                                      //Current Is Above → Decrease Duty Cycle
        else if(voltageOutput>voltageBatteryMax){PWM--;}                               //Voltage Is Above → Decrease Duty Cycle   
        else{                                                                          //MPPT ALGORITHM
          if(powerInput>powerInputPrev && voltageInput>voltageInputPrev)     {PWM--;}  //  ↑P ↑V ; →MPP  //D--
          else if(powerInput>powerInputPrev && voltageInput<voltageInputPrev){PWM++;}  //  ↑P ↓V ; MPP←  //D++
          else if(powerInput<powerInputPrev && voltageInput>voltageInputPrev){PWM++;}  //  ↓P ↑V ; MPP→  //D++
          else if(powerInput<powerInputPrev && voltageInput<voltageInputPrev){PWM--;}  //  ↓P ↓V ; ←MPP  //D--
          else if(voltageOutput<voltageBatteryMax)                           {PWM++;}  //  MP MV ; MPP Reached - 
          powerInputPrev   = powerInput;                                               //Store Previous Recorded Power
          voltageInputPrev = voltageInput;                                             //Store Previous Recorded Voltage  
          PWM_Modulation();                                                            //Set PWM signal to Buck PWM GPIO           
        }                                                                     
        
    }
  }
}

  int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if (dbm <= -100) {
    return 0;
  } else if (dbm >= -50) {
    return 100;
  } else {
    return 2 * (dbm + 100);
  }
}
  
