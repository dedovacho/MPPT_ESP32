void backflowControl(){                                                                                                                            
    if(voltageInput>voltageOutput+voltageDropout){bypassEnable=1;}     //Cho phép dòng điện đi qua
    else                                         {bypassEnable=0;}     //Ngăn dòng chảy ngược
    digitalWrite(backflow_MOSFET,bypassEnable);                         
}

void Device_Protection(){
  //ERROR COUNTER RESET
  currentRoutineMillis = millis();
  if(currentErrorMillis-prevErrorMillis>=errorTimeLimit){                                           //Run routine every millisErrorInterval (ms)
    prevErrorMillis = currentErrorMillis;                                                           //Store previous time
    if(errorCount<errorCountLimit){errorCount=0;}                                                   //Reset error count if it is below the limit before x milliseconds  
    else{}                                                                                          // TO ADD: sleep and charging pause if too many errors persists   
  } 
  //FAULT DETECTION     
  ERR = 0;                                                                                          //Reset local error counter
  backflowControl();                                                                                //Run backflow current protection protocol  
  if(temperature>temperatureMax)                           {ERR++;errorCount++;}                    //OVERTEMPERATURE
  if(currentInput>currentInAbsolute)                       {ERR++;errorCount++;}                    //INPUT  OVERCURRENT
  if(currentOutput>currentOutAbsolute)                     {ERR++;errorCount++;}                    //OUTPUT OVERCURRENT 
  if(voltageOutput>voltageBatteryMax+voltageBatteryThresh) {ERR++;errorCount++;}                    //OUTPUT OVERVOLTAGE                   
  if(voltageInput<vInSystemMin&&voltageOutput<vInSystemMin){ERR++;errorCount++;}                    //Điện áp đầu vào/ra quá thấp
  
    backflowControl();                                                                              //Enable backflow current detection & control                           
    if(voltageOutput<vInSystemMin)                   {BNC=1;ERR++;}      else{BNC=0;}               //BNC - BATTERY NOT CONNECTED (for charger mode only, does not treat BNC as error when not under MPPT mode)
    if(voltageInput<voltageBatteryMax+voltageDropout){ERR++;REC=1;}                                 //IUV - INPUT UNDERVOLTAGE: Input voltage is below max battery charging voltage (for charger mode only)     

}
