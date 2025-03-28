void Read_Sensors(){
  /////////// CẢM BIẾN NHIỆT ĐỘ /////////////
  if(sampleStoreTS<=avgCountTS){                               //TEMPERATURE SENSOR - Lite Averaging
    TS = TS + analogRead(TempSensor);
    sampleStoreTS++;   
  }
  else{
    TS = TS/sampleStoreTS;
    TSlog = log(ntcResistance*(4095.00/TS-1.00));
    temperature = (1.0/(1.009249522e-03+2.378405444e-04*TSlog+2.019202697e-07*TSlog*TSlog*TSlog))-273.15;
    sampleStoreTS = 0;
    TS = 0;
  }
  /////////// CẢM BIẾN ĐIỆN ÁP VÀ DÒNG ĐIỆN /////////////
  VSI = 0.0000;
  VSO = 0.0000;  
  CSI = 0.0000;      

  //CẢM BIẾN ĐIỆN ÁP 
  for(int i = 0; i<avgCountVS; i++){
    VSI = VSI + ads.computeVolts(ads.readADC_SingleEnded(2));
    VSO = VSO + ads.computeVolts(ads.readADC_SingleEnded(1));
  }
  voltageInput  = (VSI/avgCountVS)*inVoltageDivRatio; 
  voltageOutput = (VSO/avgCountVS)*outVoltageDivRatio; 
  if(voltageInput < 0){voltageInput  = 0.0000;}
  
  //CẢM BIẾN DÒNG 
  for(int i = 0; i<avgCountCS; i++){
    CSI = CSI + ads.computeVolts(ads.readADC_SingleEnded(3));
  }
  CSI_converted = (CSI/avgCountCS)*inAmpe;
   currentInput  = ((CSI_converted-currentMidPoint)*1.23)/currentSensV; 
  if(voltageOutput<=vInSystemMin){currentOutput = 0.0000;}
  else{currentOutput = (voltageInput*currentInput*0.90)/voltageOutput;} 

   ///////// TỰ ĐỘNG CÂN BẰNG DÒNG KHI NOLOAD ////////
  if(buckEnable==0){                
    currentMidPoint = ((CSI/avgCountCS)*inAmpe)-0.003;
  } 
  
  //CÔNG SUẤT NGÕ VÀO RA TÍNH TOÁN
  powerInput      = voltageInput*currentInput;
  powerOutput     = voltageInput*currentInput*efficiencyRate;
  outputDeviation = (voltageOutput/voltageBatteryMax)*90.000;

  //PHẦN TRĂM PIN
  batteryPercent  = ((voltageOutput-voltageBatteryMin)/(voltageBatteryMax-voltageBatteryMin))*101;
  batteryPercent  = constrain(batteryPercent,0,100);

  //TÍNH TOÁN CS TIÊU THỤ
  currentRoutineMillis = millis();
  if(currentRoutineMillis-prevRoutineMillis>=millisRoutineInterval){   
    prevRoutineMillis = currentRoutineMillis;                          
    Wh = Wh+(powerInput/(3600.000*(1000.000/millisRoutineInterval)));  
    kWh = Wh/1000.000;
    MWh = Wh/1000000.000;
  } 
}
