
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
const uint16_t recvPin = 0; // Infrared receiving pin
IRrecv irrecv(recvPin);      // Create a class object used to receive class
decode_results results;       // Create a decoding results class object

//remote

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

#define SDA 13                    //Define SDA pins
#define SCL 14                    //Define SCL pins

DHTesp dht;                       // create dht object
LiquidCrystal_I2C lcd(0x27,16,2); //initialize the LCD
int dhtPin = 18;                  // the number of the DHT11 sensor pin

//-RBG------------------

const byte ledPins[] = {15, 2, 4};    //define led pins
const byte chns[] = {0, 1, 2};        //define the pwm channels
//-RBG------------------

void setup() {
  Wire.begin(SDA, SCL);           // attach the IIC pin
  lcd.init();                     // LCD driver initialization
  lcd.backlight();                // Open the backlight
  dht.setup(dhtPin, DHTesp::DHT11); //attach the dht pin and initialize it


  //RBG
  for (int i = 0; i < 3; i++) {   //setup the pwm channels
    ledcSetup(chns[i], 1000, 8);
    ledcAttachPin(ledPins[i], chns[i]);
  }
  //RBG
  Serial.begin(115200);       // Initialize the serial port and set the baud rate to 115200
  irrecv.enableIRIn();        // Start the receiver
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(recvPin);   //print the infrared receiving pin
}

int lcd_cond = 0;
int mode_cond = 0;
int menu_cond = 0;
const int led_speed = 20;
unsigned long button_play = 0xFFA857;
unsigned long button_menu = 0xFFE21D;
unsigned long button_add = 0xFF02FD;
unsigned long button_minus = 0xFF9867;
unsigned long button_0 = 0xFF6897;
unsigned long button_1 = 0xFF30CF;
unsigned long button_2 = 0xFF18E7;
unsigned long button_3 = 0xFF7A85;
unsigned long button_4 = 0xFF10EF;
unsigned long button_5 = 0xFF38C7;
unsigned long button_6 = 0xFF5AA5;
unsigned long button_7 = 0xFF42BD;
unsigned long button_8 = 0xFF4AB5;
unsigned long button_9 = 0xFF52AD;

int red, green, blue;



void loop() {
  if (irrecv.decode(&results)) {          // Waiting for decoding
    serialPrintUint64(results.value, HEX);// Print out the decoded results
    Serial.println("");
    if(results.value == button_play){
      lcd.clear();
      lcd_cond = 0;
      menu_cond = 1;
      mode_cond = 1;
    
    }
    else if (results.value == button_menu){
      lcd.clear();
      lcd_cond = 1;
      menu_cond = 0;
      mode_cond = 0;
    }

    irrecv.resume();
    delay(10);            // Release the IRremote. Receive the next value
  }

  //render mode screen if needed first
  if (mode_cond == 0){
    
    if (results.value == button_1){
        menu_cond = 1;
        lcd.clear(); 
        lcd.setCursor(0, 0);
        lcd.print("1: RGB Speed");
        lcd.setCursor(0, 1); 
        lcd.print("-            +");
        
              
      
    }
    else if (results.value == button_2){
        menu_cond = 1;
        lcd.clear(); 
        lcd.setCursor(0, 0);
        lcd.print("2: Set RGB Values");
        lcd.setCursor(0, 1); 
        lcd.print("-            +"); 
    }
    
  }
  
  // read DHT11 data and save it 
  if (lcd_cond ==0){
    flag:TempAndHumidity DHT = dht.getTempAndHumidity();
    if (dht.getStatus() != 0) {       //Determine if the read is successful, and if it fails, go back to flag and re-read the data
      goto flag;
    }
    lcd.setCursor(0, 0);              //set the cursor to column 0, line 1
    lcd.print("Temperature:");        //display the Humidity on the LCD1602
    lcd.print(DHT.temperature);   
    lcd.setCursor(0, 1);              //set the cursor to column 0, line 0 
    lcd.print("Humidity   :");        //display the Humidity on the LCD1602
    lcd.print(DHT.humidity);  
  }
  else if (menu_cond == 0){
    
    lcd.setCursor(0, 0);
    lcd.print("    LED MODE");
    lcd.setCursor(0, 1); 
    lcd.print("     1    2");        //1 is set specific color, 2 is set the speed
    //lcd.print(results.value);
     
  }


  //RGB
  for (int i = 0; i < 256; i++) {
    setColor(wheel(i));
    delay(led_speed);
  }
  //remote 
}



void setColor(long rgb) {
  ledcWrite(chns[0], 255 - (rgb >> 16) & 0xFF);
  ledcWrite(chns[1], 255 - (rgb >> 8) & 0xFF);
  ledcWrite(chns[2], 255 - (rgb >> 0) & 0xFF);
}

long wheel(int pos) {
  long WheelPos = pos % 0xff;
  if (WheelPos < 85) {
    return ((255 - WheelPos * 3) << 16) | ((WheelPos * 3) << 8);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return (((255 - WheelPos * 3) << 8) | (WheelPos * 3));
  } else {
    WheelPos -= 170;
    return ((WheelPos * 3) << 16 | (255 - WheelPos * 3));
  }
}






