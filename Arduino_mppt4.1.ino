
#include <EEPROM.h>

#include "LiquidCrystal_I2C.h" 
LiquidCrystal_I2C lcd(0x27, 16, 2);
int dut,pwm,v,j,tmp;

bool st,m;

float vo,vi,current,current1,cutoff,cv,p,p1,ef;

void setup() {

    lcd.begin();
  lcd.backlight();
   
  

   TCCR1B = TCCR1B & B11111000 | B00000001; // Set PWM frequency for D9 & D10:

   Serial.begin(115200);
   
   pinMode(9,OUTPUT);
   
   pinMode(10,INPUT_PULLUP);
   
   pinMode(11,INPUT_PULLUP);
   
   pinMode(12,INPUT_PULLUP);
   
   v=EEPROM.read(0); 
   if(v>2)v=0;
   switch(v){
    case 0:
   cutoff=15.4; // flooded
   break;
    case 1:
   cutoff=14.8; //agm
   break;
    case 2:
   cutoff=14.2; //AUTO
   break;
   }
      
}

void loop(){ 
  
  j++;
  
  
      
   if(m==0){
    
      current=0;

      

      for(int i = 0; i < 100; i++) {
      
          current = current + (.049 * analogRead(A2) -25);// for 20A mode
          current1 = current1 + (.049 * analogRead(A3) -25);// for 20A mode
          vo+= analogRead(A0);
  vi+=analogRead(A1);
        delayMicroseconds(100);
      
      } 
      
 current = current/60;
  current1 = current1/70;
      if(current<0.1)current=0;
      if(current1<0.1)current1=current=0;
      
      vo=((vo*0.06)/100);
       vi=(((vi)*0.6)/80);
       //if(v)
     vi=(vo*10)-vi;

     
      p=current*vo;
      p1=current1*vi;
      ef=(p/p1)*100;
      if(vo>cutoff)pwm--;
      
      if(pwm<0)pwm=0;
      
      if(vo<cutoff+0.5)pwm++;
      
      if(pwm>240)pwm=240;
      
      
      analogWrite(9,pwm);

    
   if(j>30){   
    j=0;
   lcd.setCursor(0,0); 
      
    
   lcd.print("P ");
   tmp=vi;
   if(tmp<10)lcd.print(" ");
   lcd.print(tmp);
   lcd.print("V ");
   if(current1<10){tmp=current1*10;
    lcd.print(tmp/10);
     lcd.print(".");
    lcd.print(tmp%10);}
    else{ tmp=current1;
    lcd.print(" ");
    lcd.print(tmp);
    }
    lcd.print("A ");
   tmp= p1;
     lcd.print(tmp);
   lcd.print("W    ");
   lcd.setCursor(0,1); 
  lcd.print("B ");
   tmp=vo;
   if(tmp<10)lcd.print(" ");
   lcd.print(tmp);
   lcd.print("V ");
   if(current<10){tmp=current*10;
    lcd.print(tmp/10);
     lcd.print(".");
    lcd.print(tmp%10);}
    else{ tmp=current;
    lcd.print(" ");
    lcd.print(tmp);
    }
    lcd.print("A ");
   tmp= p;
     lcd.print(tmp);
   lcd.print("W    ");
   //-----------------------------
      
     

   tmp=vi;
   if(tmp<10)Serial.print(0);
   Serial.print(tmp);
   
    tmp=vo;
   if(tmp<10)Serial.print(0);
   Serial.print(tmp);

  tmp=current1*10;
   if(current1<100)Serial.print(0);
    if(current1<10)Serial.print(0);
   
    Serial.print(tmp);
    
   
  
 
   tmp=current*10;
   if(current<100)Serial.print(0);
    if(current<10)Serial.print(0);
   
    Serial.println(tmp);
    
  
 


      
      
     
      
      
     

      //-------------------------
      } 
      
      
      }
if(digitalRead(10)&digitalRead(11)&digitalRead(12))st=0;
  
  if(!digitalRead(11)&st==0){
    
    st=1;
  
    lcd.clear();
  
    if(m==1){
      
     EEPROM.update(0, v);
   switch(v){
    case 0:
   cutoff=15.4; // flooded
   break;
    case 1:
   cutoff=14.8; //agm
   break;
    case 2:
   cutoff=14.2; //AUTO
   break;
   }
      
      }
      
    m=!m;
    
    }
    
  if(m==1){
    
    if(!digitalRead(10)&st==0){
      
      st=1;
      
      v=v+1;
      if(v>2)v=0;
      }
    
    
    if(!digitalRead(12)&st==0){
      
      st=1;
      
      v=v-1;
     if(v<0)v=2; 
      }
      
     
     
 
     
     lcd.setCursor(0,0);
     
     lcd.print("  Battery Type  ");
     
    lcd.setCursor(0,1);
     
     
       switch(v){
    case 0:
  lcd.print("    Flooded 15.4");
   break;
    case 1:
   lcd.print("      AGM 14.8  ");
   break;
    case 2:
   lcd.print("      AUTO 14.2V       ");
   break;
   }
     
      }
      
      
      
  delay(10);
    
    }
