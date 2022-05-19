
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

int rgb_cond = 0;
int lcd_cond = 0;
int mode_cond = 0;
int menu_cond = 0;
int add_minus_cond = 0;
int mode_1 = 1;
int mode_2 = 2;
const int speed_length = 15;
char arr_speed[speed_length]= "*******       ";
int current_speed = 6;
int led_speed = 40;
const int rgb_length = 18;
char arr_rgb[rgb_length] = " (   ,    ,   )  ";
int current_rgb = 0;
int ir_value;
int arr_values[9];
int input_state = 1;

const unsigned long button_play = 0xFFA857;
const  long button_menu = 0xFFE21D;
const unsigned long button_add = 0xFF02FD;
const unsigned long button_minus = 0xFF9867;
const unsigned long button_0 = 0xFF6897;
const unsigned long button_1 = 0xFF30CF;
const unsigned long button_2 = 0xFF18E7;
const unsigned long button_3 = 0xFF7A85;
const unsigned long button_4 = 0xFF10EF;
const unsigned long button_5 = 0xFF38C7;
const unsigned long button_6 = 0xFF5AA5;
const unsigned long button_7 = 0xFF42BD;
const unsigned long button_8 = 0xFF4AB5;
const unsigned long button_9 = 0xFF52AD;

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
      mode_1 = 1;
      mode_2 = 1;
    
    }
    else if (results.value == button_menu){
      lcd.clear();
      lcd_cond = 1;
      menu_cond = 0;
      mode_cond = 0;
      mode_1 = 1;
      mode_2 = 1;
    }

  

  //render mode screen if needed first
  if (mode_cond == 0){
    
    if ((results.value == button_1 || mode_1 == 0) && mode_2 == 1 ){
        menu_cond = 1;
        mode_2 = 1;
        mode_1 = 0;
        lcd.clear(); 
        lcd.setCursor(0, 0);
        lcd.print("1: RGB Speed");
        lcd.setCursor(0, 1); 
        lcd.print("-");
        for (int i = 0 ; i < speed_length - 1;i++){
          lcd.print(arr_speed[i]);
        }
        lcd.print("+");
        if (results.value == button_add){
            if (current_speed <  13){
                current_speed++;
                arr_speed[current_speed] = '*';
                led_speed -= 5;
            }
            
        }
        else if (results.value == button_minus){
             if (current_speed >=0){
                 arr_speed[current_speed] = ' ';
                 current_speed--;
                 led_speed += 5;
              }
          
        }
   
    }
    else if ((results.value == button_2 || mode_2 == 0) && mode_1 == 1){
        
        mode_2 = 0;
        mode_1 = 1;
        lcd.clear(); 
        lcd.setCursor(0, 0);
        lcd.print("2: Set RGB Values");
        lcd.setCursor(0, 1); 
        for (int i = 0 ; i < rgb_length - 1;i++){
          lcd.print(arr_rgb[i]);
        }

        //logic for rbg input
        
        //Set input for rgb
        if ((results.value == button_0 || results.value == button_1 || results.value == button_2 || 
            results.value == button_3 || results.value == button_4 || results.value == button_5 ||
            results.value == button_6 || results.value == button_7 || results.value == button_8 ||
            results.value == button_9) && menu_cond == 1){
                if ((current_rgb + 2) == 2 || (current_rgb + 2) == 5 || (current_rgb+2) == 9){
                    if ( convertHEX(results.value) >= 0 && convertHEX(results.value) <= 2){
                        ir_value = convertHEX(results.value);
                        arr_values[current_rgb] = ir_value;
                        arr_rgb[current_rgb+2] = (char)ir_value;
                        current_rgb++;
                    }
                  
                }
                else{
                    arr_values[current_rgb] = convertHEX(results.value);
                    arr_rgb[current_rgb+2] = (char)ir_value;
                    current_rgb++;
                }

                for(int j = 0; j< 9;j++){
                    Serial.print(arr_values[j]);
                    Serial.print("," );
                }
                Serial.println("");
              
            }
            menu_cond = 1;

            
    }
    
  }
    irrecv.resume();
    //delay(100);            // Release the IRremote. Receive the next value
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

int convertHEX (long unsigned value){
  switch (value){
      case button_0:
        ir_value = 0; break;
      case button_1:
        ir_value = 1; break;
      case button_2:
        ir_value = 2; break;
      case button_3:
        ir_value = 3; break;
      case button_4:
        ir_value = 4; break;
      case button_5:
        ir_value = 5; break;
      case button_6:
        ir_value = 6; break;
      case button_7:
        ir_value = 7; break;
      case button_8:
        ir_value = 8; break;
      case button_9:
        ir_value = 9; break;
  }
  return ir_value;
}






