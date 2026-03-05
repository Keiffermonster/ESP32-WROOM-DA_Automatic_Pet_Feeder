// include libraries requried for esp to interact with hardware
#include <Adafruit_I2CDevice.h>
#include <RTClib.h>
#include <ESP32Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Wire.h>

// create 4 rows and 4 colulms 
const byte ROWS = 4;
const byte COLS = 4;

// define the keymap of the 4x4 matrix
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// connect keypad ROW0, ROW1, ROW2 and ROW3 to pin 2, 3, 4, 5.
byte rowPins[ROWS] = { 2, 3, 4, 5 };
// connect keypad COL0, COL1 and COL2 to pins 34, 35, 36, 39.
byte colPins[COLS] = { 34, 35, 36, 39 };
// create the Keypad using the keypad library
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// initialize the rtc object
RTC_DS3231 rtc;

// initialize the servo object
Servo myServo;  

// makes the lcd object and assigns the pins to (rs, enable, d4, d5, d6, d7) VSS = GROUND VDD = 5v K = GROUND A = 5v
LiquidCrystal lcd(17, 18, 19, 25, 26, 23);

// char's to break full time into single numbers later ------ is this needed? 
char t1, t2, t3, t4, t5, t6;
// condition for interupt alarm
boolean feed = true;
char key;
//  char's for the time that will be set by the user
char r[6];


void setup() 
{ 
  // attach the signal pin of servo to pin4 of esp32
  myServo.attach(4); 
  //Wire.begin(25, 26);  //21 and 22 are default SDA = 21 SCL 22
  rtc.begin();
  //set rtc time to computer time
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  delay(1000);
  lcd.begin(16,2);
  myServo.write(55); 
  Serial.begin(115200);

  //lcd pins
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);

  //make button take input
  pinMode(27, INPUT);
  
} 

void loop() {

  lcd.setCursor(0, 0);
  int buttonPress;
  buttonPress = digitalRead(27);
  
  if (buttonPress == 1) {
    setFeedingTime();
  }
  
  lcd.print("Time:  ");
  
  // Get current time from DS3231
  DateTime now = rtc.now();
  
  // Format the time as HH:MM:SS
  String timeStr = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  lcd.print(timeStr);
  
  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  
  // Format date as DD/MM/YYYY or your preferred format
  String dateStr = (String(now.day()) +  "/" + String(now.month()) + "/" + String(now.year()));
  lcd.print(dateStr);
  
  
  // Check feeding time
  if (t1 == r[0] && t2 == r[1] && t3 == r[2] && t4 == r[3] && t5 < 1 && t6 < 3 && feed == true) {
    myServo.write(100);
    delay(400);   
    myServo.write(55); 
    feed = false;
  }

}       
void setFeedingTime()
{
  feed = true;
   int i=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set feeding Time");
  lcd.clear();
  lcd.print("HH:MM");
  lcd.setCursor(0,1);
  while(1){
    key = kpd.getKey();
    char j;
  if(key!=NO_KEY){
    lcd.setCursor(j,1);
    lcd.print(key);
    r[i] = key-48;
    i++;
    j++;
    if (j==2)
    {
      lcd.print(":"); j++;
    }
    delay(500);
  }
  if (key == 'D')
  {key=0; break; }
  }
}