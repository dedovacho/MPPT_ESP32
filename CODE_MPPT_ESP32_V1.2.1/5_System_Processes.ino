void System_Processes(){
  ///////////////// ĐIỀU KHIỂN QUẠT TẢN /////////////////                                                                      
  if(temperature>=temperatureFan){fanStatus=1;}               //Turn on fan when set fan temp reached
  else if(temperature<temperatureFan){fanStatus=0;}                //Turn off fan when set fan temp reached
  digitalWrite(FAN,fanStatus);                                     //Send a digital signal to the fan MOSFET    
}

void factoryReset(){
  EEPROM.write(1,14); //Max Battery: Phần nguyên
  EEPROM.write(2,0);  //Max Battery: Phần thập phân
  EEPROM.write(3,10); //Min Battery: Phần nguyên
  EEPROM.write(4,0);  //Min Battery: Phần thập phân
  EEPROM.write(5,20); //Dòng sạc: Phần nguyên
  EEPROM.write(6,0);  //Dòng sạc: Phần thập phân
  EEPROM.write(7,0);  //Tải đầu ra
  EEPROM.write(8,60); //STORE: Fan Temp (Integer)
  EEPROM.write(9,90); //STORE: Shutdown Temp (Integer)
  EEPROM.commit();
  loadSettings();
}

void loadSettings(){                                     
  voltageBatteryMax  = EEPROM.read(1)+(EEPROM.read(2)*.01);  // Load saved maximum battery voltage setting
  voltageBatteryMin  = EEPROM.read(3)+(EEPROM.read(4)*.01);  // Load saved minimum battery voltage setting
  currentCharging    = EEPROM.read(5)+(EEPROM.read(6)*.01);  // Load saved charging current setting
  dem                = EEPROM.read(7);                       // 
  temperatureFan     = EEPROM.read(8);                       // Load saved fan temperature settings
  temperatureMax     = EEPROM.read(9);                       // Load saved shutdown temperature settings  
      
}

void saveSettings(){
  conv1 = voltageBatteryMax*100;       //STORE: Maximum Battery Voltage (gets whole number)
  conv2 = conv1%100;                   //STORE: Maximum Battery Voltage (gets decimal number and converts to a whole number)
  EEPROM.write(1,voltageBatteryMax);
  EEPROM.write(2,conv2);
  conv1 = voltageBatteryMin*100;       //STORE: Minimum Battery Voltage (gets whole number)
  conv2 = conv1%100;                   //STORE: Minimum Battery Voltage (gets decimal number and converts to a whole number)
  EEPROM.write(3,voltageBatteryMin);
  EEPROM.write(4,conv2);
  conv1 = currentCharging*100;         //STORE: Charging Current
  conv2 = conv1%100;
  EEPROM.write(5,currentCharging);
  EEPROM.write(6,conv2);
  EEPROM.write(7,dem);
  EEPROM.write(8,temperatureFan);      //STORE: Fan Temp
  EEPROM.write(9,temperatureMax);      //STORE: Shutdown Temp
  EEPROM.commit();                     //Saves setting changes to flash memory
}
