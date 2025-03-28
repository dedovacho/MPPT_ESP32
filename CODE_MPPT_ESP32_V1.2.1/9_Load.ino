void load(){
  //hientai = pinValue;
  if(pinValue1 == HIGH){
    delay(30);
    if(pinValue1 == HIGH){
       dem++ ;
       if(dem > 2) dem = 0;
       saveSettings();
    }
    }
  else{}
  
  if(dem ==0) digitalWrite(Load, LOW);
  if(dem ==1) digitalWrite(Load, HIGH);
  if(dem ==2){
   if( pinValue2 == 1) digitalWrite(Load, HIGH);
   if( pinValue2 == 0) digitalWrite(Load, LOW);
  }
}
