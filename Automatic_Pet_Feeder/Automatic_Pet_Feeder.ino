// include libraries requried for esp to interact with hardware

#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <ESP32Servo.h>
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
byte rowPins[ROWS] = { 32, 33, 34, 35 };
// connect keypad COL0, COL1 and COL2 to pins 34, 35, 36, 39.
byte colPins[COLS] = { 15, 2, 4, 5 };
// create the Keypad using the keypad library
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// initialize the rtc object
RTC_DS3231 rtc;

// initialize the servo object
Servo myServo;  

// makes the lcd object and assigns the pins to (rs, enable, d4, d5, d6, d7) VSS = GROUND VDD = 5v K = GROUND A = 5v
LiquidCrystal_I2C lcd(0x27, 16, 2);

// char's to break full time into single numbers later ------ is this needed? 
char t1, t2, t3, t4, t5, t6;
// condition for interupt alarm
boolean feed = true;
char key;
//  char's for the time that will be set by the user
char r[6];


void setup() 
{ 
  Serial.begin(74880);
  Wire.begin(21, 22); // 21 and 22 are default SDA = 21 SCL 22
  Wire.setClock(100000); // Standard mode, more stable than default fast mode
  
  // attach the signal pin of servo to pin4 of esp32
  myServo.attach(12);

  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1);
  }

  if (rtc.lostPower()) {
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.init();       // replaces lcd.begin(16,2)
  lcd.backlight();  // turn on backlight
  lcd.clear();

  myServo.write(55);


  //keypad pins
  //coumns are outputs 
  //rows are inputs
  pinMode(15, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT);  
  pinMode(34, INPUT);
  pinMode(35, INPUT); 

  //make button take input
  pinMode(27, INPUT_PULLDOWN);
  
} 

void recoverI2C() {
  // Manually pulse SCL 9 times to release stuck slave
  pinMode(21, OUTPUT); // SDA
  pinMode(22, OUTPUT); // SCL

  digitalWrite(21, HIGH);
  for (int i = 0; i < 9; i++) {
    digitalWrite(22, HIGH);
    delayMicroseconds(5);
    digitalWrite(22, LOW);
    delayMicroseconds(5);
  }
  // Send stop condition
  digitalWrite(21, LOW);
  delayMicroseconds(5);
  digitalWrite(22, HIGH);
  delayMicroseconds(5);
  digitalWrite(21, HIGH);

  // Reinitialise properly
  Wire.begin(21, 22);
  Wire.setClock(100000);
  delay(100);
  lcd.init();
  lcd.backlight();
}

void loop() {

  DateTime now;
  Wire.beginTransmission(0x68); // RTC address
  if (Wire.endTransmission() == 0) {
    now = rtc.now(); // only read if RTC is responding
  } else {
    Serial.println("RTC not responding");
    return; // skip this loop iteration
  }

  static unsigned long lastCheck = 0;

  if (millis() - lastCheck > 5000) {
    lastCheck = millis();
    Wire.beginTransmission(0x27);
    byte error = Wire.endTransmission();
    Serial.printf("I2C health check: %d\n", error); // 0 = good
    if (error != 0) {
      Serial.println("Recovering...");
      recoverI2C();
    }
  }

  static int lastSecond = -1;
  if (now.second() != lastSecond) {
    lastSecond = now.second();

    char timeStr[9];
    sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
    char dateStr[11];
    sprintf(dateStr, "%02d/%02d/%04d", now.day(), now.month(), now.year());

    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    lcd.print(timeStr);
    lcd.setCursor(0, 1);
    lcd.print("Date: ");
    lcd.print(dateStr);
  }

  int buttonPress;
  buttonPress = digitalRead(27);
  
  if (digitalRead(27) == 1) {
    Serial.println("Button triggered!"); // is this firing randomly?
    setFeedingTime();
  }
  
  
  // Check feeding time
  if (t1 == r[0] && t2 == r[1] && t3 == r[2] && t4 == r[3] && t5 < 1 && t6 < 3 && feed == true) {
    myServo.write(100);
    delay(400);   
    myServo.write(55); 
    feed = false;
  }

  delay(100);
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
    int j = 0;
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