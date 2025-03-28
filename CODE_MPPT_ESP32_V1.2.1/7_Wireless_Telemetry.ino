void Wireless_Telemetry() {
  if(wm_nonblocking) wm.process();
  checkButton();
  int8_t quality = getWifiQuality();
  Blynk.run();
  Blynk.virtualWrite(20, powerInput);
  Blynk.virtualWrite(2, batteryPercent);
  Blynk.virtualWrite(3, voltageInput);
  Blynk.virtualWrite(4, currentInput);
  Blynk.virtualWrite(5, voltageOutput);
  Blynk.virtualWrite(6, currentOutput);
  Blynk.virtualWrite(7, temperature);
  Blynk.virtualWrite(8, Wh / 1000);
  if (dem == 0) {
    Blynk.virtualWrite(9, "OFF ");
  }
  if (dem == 1) {
    Blynk.virtualWrite(9, "ON  ");
  }
  if (dem == 2) {
    Blynk.virtualWrite(9, "AUTO");
  }
  //Read V10 -> Automation
  Blynk.virtualWrite(14, voltageBatteryMin); //Minimum Battery Voltage (Read & Write)
  Blynk.virtualWrite(15, voltageBatteryMax); //Maximum Battery Voltage (Read & Write)
  Blynk.virtualWrite(16, currentCharging);   //Charging Current  (Read & Write)
  //Read V17 -> Load
  Blynk.virtualWrite(18, quality);   //Charging Current  (Read & Write)

}

void checkButton() {
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    // poor mans debounce/press-hold, code not ideal for production
    delay(50);
    if ( digitalRead(TRIGGER_PIN) == LOW ) {
      Serial.println("Button Pressed");
      // still holding button for 3000 ms, reset settings, code not ideaa for production
      delay(3000); // reset delay hold
      if ( digitalRead(TRIGGER_PIN) == LOW ) {
        Serial.println("Button Held");
        Serial.println("Erasing Config, restarting");
        wm.resetSettings();
        ESP.restart();
      }
    }
  }
}


String getParam(String name) {
  //read parameter from server, for customhmtl input
  String value;
  if (wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void saveParamCallback() {
  Serial.println("[CALLBACK] saveParamCallback fired");
  Serial.println("PARAM customfieldid = " + getParam("customfieldid"));
}
