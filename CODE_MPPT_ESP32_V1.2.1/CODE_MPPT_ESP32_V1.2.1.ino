/*
  Youtube  : https://www.youtube.com/c/403ERRORVN
  Fanpage  : https://www.facebook.com/403ERRORVN
  Blog     : https://thichchetaotv.blogspot.com/

  Date     : 17/02/2022
  Update   : 06/11/2022 (sửa thứ tự khai báo của Blynk)

  Library:
  LCD I2C         : https://github.com/johnrickman/LiquidCrystal_I2C
  Adafruit_ADS1X15: https://github.com/adafruit/Adafruit_ADS1X15
*/

//========== SETTING WiFi ==========//
#define BLYNK_TEMPLATE_ID "TMPL7klfzU_ye"
#define BLYNK_TEMPLATE_NAME "ESP32 MPPT"
#define BLYNK_AUTH_TOKEN "ROGQNX6-tepI_5I0b5tzeVPNloeXkocv"

//====================== LIBRARY USED ============================//
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_ADS1X15.h>
#include <driver/ledc.h> // Include this header for LEDC functions

LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27, 16 columns, 2 rows
TaskHandle_t Core2;
Adafruit_ADS1015 ads;

//====================== WiFi Config ============================//
#include <WiFiManager.h>      // config WiFi
#define TRIGGER_PIN 0
bool wm_nonblocking = false; // change to true to use non blocking

WiFiManager wm; // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )

//====================== V1.2.1 ===========================//
//====================== I/O ============================//
#define backflow_MOSFET 32
#define buck_IN         15
#define buck_EN         2
#define LED             2
#define FAN             13
#define TempSensor      33
#define Load            12
   
#define buttonLeft      17  
#define buttonRight     16 
#define buttonBack      18 
#define buttonSelect    5      

#include <BlynkSimpleEsp32.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
BlynkTimer timer;
WidgetRTC rtc;

String
currentTime,      // Time
currentDate;      // Date

void clockDisplay() {
  currentTime = String(hour()) + ":" + minute() + ":" + second();
  currentDate = String(day()) + "/" + month() + "/" + year();
}

BLYNK_CONNECTED() {
  rtc.begin();
}

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "adminwifi";
char pass[] = "12345678";

//====================================== THÔNG SỐ HOẠT ĐỘNG ==========================================//
int
pwmResolution           = 11,          //  Độ phân giải bit PWM
pwmFrequency            = 39000,       //  Tần số chuyển mạch
temperatureFan          = 50,            
temperatureMax          = 70,            
errorTimeLimit          = 1000,        //  Thời gian gửi lại bộ đếm lỗi (milliseconds)
errorCountLimit         = 5,           //  Số lỗi tối đa
millisRoutineInterval   = 250,         //  Time Interval Refresh Rate For Routine Functions (ms)
millisSerialInterval    = 1,            //  Time Interval Refresh Rate For USB Serial Datafeed (ms)
millisLCDInterval       = 1000;        //  Time Interval Refresh Rate For LCD Display (ms)

float
voltageBatteryMax       = 27.0000,     //   (Output V)
voltageBatteryMin       = 10.0000,     //   (Output V)
currentCharging         = 20.0000;     //   (A - Output)

//================================== THÔNG SỐ HIỆU CHỈNH =======================================//
int
avgCountVS              = 4,          //  Số lần lấy mẫu đo điện áp
avgCountCS              = 4,          //  Số lần lấy mẫu đo dòng
avgCountTS              = 300;        //  Số lần lấy mẫu nhiệt độ
float
inVoltageDivRatio       = 20.6000,    //  Hệ số của cầu phân áp đầu vào (100+5.1)/5.1
outVoltageDivRatio      = 20.6000,    //  Hệ số của cầu phân áp đầu ra  (100+5.1)/5.1
inAmpe                  = 1.3300,     //  Hệ số cầu phân áp cảm biến dòng (3.3+10)/10

vOutSystemMax           = 30.0000,    //  Điện áp tối đa mà đầu ra cho phép
cOutSystemMax           = 30.0000,    //  Dòng điện tối đa mà đầu ra cho phép
ntcResistance           = 10000.00,   //  NTC 10K
voltageDropout          = 1.0000,     //  Ngưỡng quá điện áp cộng thêm để mở/đóng cổng chặn dòng chảy ngược(V)
voltageBatteryThresh    = 1.5000,     //  Ngưỡng quá điện áp đầu ra cộng thêm (Output V)
currentInAbsolute       = 21.0000,    //  Giới hạn dòng quá tải đầu vào(A)
currentOutAbsolute      = 30.0000,    //  Giới hạn dòng quá tải đầu ra(A)
PPWM_margin             = 99.5000,    //  Chu kỳ hoạt động tối thiểu(%)
PWM_MaxDC               = 97.0000,    //  Chu kỳ hoạt động tối đa(%)
efficiencyRate          = 0.9500,     //  Hiệu suất dự đoán(%)
currentMidPoint         = 2.5000,     //  Current Sensor Midpoint (V)
currentSensV            = 0.0660,     //  Độ nhạy cảm biến dòng (30A-66mV/A; 20A-100mV/A; 5A-185mV/A)
vInSystemMin            = 10.000;     //  Giới hạn điện áp hđ thấp nhất(V)

//===================================== THAM SỐ HỆ THỐNG =========================================//
bool
buckEnable            = 0,           // Buck Enable Status
fanStatus             = 0,           // Fan status (1 = On, 0 = Off)
bypassEnable          = 0,             
chargingPause         = 0,              
buttonRightStatus     = 0,              
buttonLeftStatus      = 0,              
buttonBackStatus      = 0,              
buttonSelectStatus    = 0,               
buttonRightCommand    = 0,              
buttonLeftCommand     = 0,              
buttonBackCommand     = 0,      
boolTemp              = 0,           // SYSTEM PARAMETER -        
buttonSelectCommand   = 0,              
settingMode           = 0,              
setMenuPage           = 0,               
confirmationMenu      = 0,              
BNC                   = 0,              
REC                   = 0;                 
 
int
temperature           = 0,            
sampleStoreTS         = 0,              
pwmMax                = 0,           
pwmMaxLimited         = 0,            
PWM                   = 0,              
PPWM                  = 0,              
pwmChannel            = 0,              
batteryPercent        = 0,              
errorCount            = 0,              
menuPage              = 0,              
subMenuPage           = 0,              
ERR                   = 0,              
conv1                 = 0,              
conv2                 = 0,              
intTemp               = 0;              
float
VSI                   = 0.0000,      //   Raw input voltage sensor ADC voltage
VSO                   = 0.0000,      //   Raw output voltage sensor ADC voltage
CSI                   = 0.0000,      //   Raw current sensor ADC voltage
CSI_converted         = 0.0000,      //   Actual current sensor ADC voltage
TS                    = 0.0000,      //   Raw temperature sensor ADC value
powerInput            = 0.0000,      //   Input power (solar power) in Watts
powerInputPrev        = 0.0000,      //   Previously stored input power variable for MPPT algorithm (Watts)
powerOutput           = 0.0000,      //   Output power (battery or charing power in Watts)
voltageInput          = 0.0000,      //   Input voltage (solar voltage)
voltageInputPrev      = 0.0000,      //   Previously stored input voltage variable for MPPT algorithm
voltageOutput         = 0.0000,      //   Input voltage (battery voltage)
currentInput          = 0.0000,      //   Output power (battery or charing voltage)
currentOutput         = 0.0000,      //   Output current (battery or charing current in Amperes)
TSlog                 = 0.0000,      //   Part of NTC thermistor thermal sensing code
ADC_BitReso           = 0.0000,      //   System detects the approriate bit resolution factor for ADS1015/ADS1115 ADC
Wh                    = 0.0000,      //   Stores the accumulated energy harvested (Watt-Hours)
kWh                   = 0.0000,      //   Stores the accumulated energy harvested (Kiliowatt-Hours)
MWh                   = 0.0000,      //   Stores the accumulated energy harvested (Megawatt-Hours)
outputDeviation       = 0.0000,      //   Output Voltage Deviation (%)
buckEfficiency        = 0.0000,      //   Measure buck converter power conversion efficiency (only applicable to my dual current sensor version)
floatTemp             = 0.0000,
vOutSystemMin         = 0.0000;     //  CALIB PARAMETER -
unsigned long
currentErrorMillis    = 0,           // 
currentButtonMillis   = 0,           // 
currentSerialMillis   = 0,           // 
currentRoutineMillis  = 0,           // 
currentLCDMillis      = 0,           //  
currentWiFiMillis     = 0,           // 
currentMenuSetMillis  = 0,           // 
prevButtonMillis      = 0,           // 
prevSerialMillis      = 0,           // 
prevRoutineMillis     = 0,           // 
prevErrorMillis       = 0,           // 
prevWiFiMillis        = 0,           // 
prevLCDBackLMillis    = 0,           //  SYSTEM PARAMETER -
currentLCDBackLMillis = 0,           //SYSTEM PARAMETER -
prevLCDMillis         = 0,           // 
backLightInterval     = 300000;       // Thời gian tắt đèn nền (ms)

int dem                 ;
int pinValue1, pinValue2;


//====================================== CHƯƠNG TRÌNH CHÍNH =============================================//
BLYNK_WRITE(V17)
{
  pinValue1 = param.asInt(); // assigning incoming value from pin V1 to a variable
}
BLYNK_WRITE(V10) {
  pinValue2 = param.asInt(); // assigning incoming value from pin V1 to a variable
}
//================= CORE0: SETUP (DUAL CORE MODE) =====================//
void coreTwo(void * pvParameters) {
  Blynk.begin(auth, ssid, pass);                                        
  //================= CORE0: LOOP (DUAL CORE MODE) ======================//
  while (1) {
    Wireless_Telemetry();                              
  }
}

//================== CORE1: SETUP (DUAL CORE MODE) ====================//
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.setDebugOutput(true);  
  Serial.println("\n Starting");
  Serial.println("> Serial Initialized");

  //GPIO PIN INITIALIZATION
  pinMode(backflow_MOSFET, OUTPUT);
  pinMode(buck_EN, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(Load, OUTPUT);
  pinMode(buttonLeft, INPUT);
  pinMode(buttonRight, INPUT);
  pinMode(buttonBack, INPUT);
  pinMode(buttonSelect, INPUT);
  pinMode(TRIGGER_PIN, INPUT);

  //PWM INITIALIZATION
  ledcSetup(pwmChannel, pwmFrequency, pwmResolution);        //Set PWM parameters
  ledcAttachPin(buck_IN, pwmChannel);                        //Set pin as PWM
  ledcWrite(pwmChannel, PWM);                                //Write PWM value at startup (duty = 0)
  pwmMax = pow(2, pwmResolution) - 1;                        //Get PWM Max Bit Ceiling
  pwmMaxLimited = (PWM_MaxDC * pwmMax) / 100.000;            //Tối đa pwm

  //ADC INITIALIZATION
  ads.setGain(GAIN_ONE); ADC_BitReso = 1.0000; // Gain: 2x    Range: +/- 2.048V
  ads.begin();                                               //Initialize ADC

  //GPIO INITIALIZATION
  buck_Disable();

  //ENABLE DUAL CORE MULTITASKING
  xTaskCreatePinnedToCore(coreTwo, "coreTwo", 10000, NULL, 0, &Core2, 0);

  //INITIALIZE AND LIOAD FLASH MEMORY DATA
  EEPROM.begin(512);
  Serial.println("> FLASH MEMORY: STORAGE INITIALIZED");  //Startup message
  loadSettings();
  Serial.println("> FLASH MEMORY: SAVED DATA LOADED");    //Startup message

  lcd.begin();
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("  SAC MPPT 20A  ");
  lcd.setCursor(0, 1);
  lcd.print("          V1.2.1");
  delay(1000);
  lcd.clear();
}
//================== CORE1: LOOP (DUAL CORE MODE) ======================//
void loop() {
  Read_Sensors();         //TAB#2 - Đo lường tính toán
  Device_Protection();    //TAB#3 - Phát hiện lỗi
  System_Processes();     //TAB#4 - Quy trình hệ thống
  Charging_Algorithm();   //TAB#5 - Thuật toán sạc pin
  Onboard_Telemetry();    //TAB#6 - Theo dõi qua Serial
  LCD_Menu();             //TAB#8 - LCD
  load();                 //TAB#9 - Load
}