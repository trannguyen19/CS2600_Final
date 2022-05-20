#include <WiFi.h>
#include <stdlib.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
const uint16_t recvPin = 0; // Infrared receiving pin
IRrecv irrecv(recvPin);     // Create a class object used to receive class
decode_results results;     // Create a decoding results class object

// remote

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

#define SDA 13 // Define SDA pins
#define SCL 14 // Define SCL pins

// WiFi
const char *ssid = "tiger";        // Enter your WiFi name
const char *password = "12345678"; // Enter WiFi password
const char *mqtt_username = "testing";
const char *mqtt_password = "12345678";

// MQTT Broker
const char *mqtt_broker = "192.168.64.184";
const char *topic = "esp32/main";
const char *topic2 = "esp32/temp";
const char *topic3 = "esp32/irOutput";
const int mqtt_port = 1883;
char respond[100]; // respond and buffer to handle payload
char buf[50];

WiFiClient espClient;
PubSubClient client(espClient);

// LCD

DHTesp dht;                         // create dht object
LiquidCrystal_I2C lcd(0x27, 16, 2); // initialize the LCD
int dhtPin = 18;                    // the number of the DHT11 sensor pin

//-RBG------------------

const byte ledPins[] = {15, 2, 4}; // define led pins
const byte chns[] = {0, 1, 2};     // define the pwm channels
//-RBG------------------

//Variables
int rgb_cond = 0;
int lcd_cond = 0;
int mode_cond = 0;
int menu_cond = 0;
int add_minus_cond = 0;
int mode_1 = 1;
int mode_2 = 2;
const int speed_length = 15;
char arr_speed[speed_length] = "*******       "; // intitial for mode 1
int current_speed = 6;
int led_speed = 40;
const int rgb_length = 18;
int current_rgb = 0;
int ir_value;
int arr_values[9];
int remain_rgb = 0;
int traverse_rgb = 0;

// defint hex for IR remote
const unsigned long button_play = 0xFFA857;
const unsigned long button_menu = 0xFFE21D;
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

int red = 0, green = 0, blue = 0;
double temp = 0;
double preTemp = 0;
double preHumid = 0;
double humid = 0;




void setup()
{
  // Broker
  //  Set software serial baud to 115200;
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected())
  {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public MQTT broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // publish and subscribe
  client.publish(topic, "Published message from ESP32");
  client.subscribe(topic);

  // LCD
  Wire.begin(SDA, SCL);             // attach the IIC pin
  lcd.init();                       // LCD driver initialization
  lcd.backlight();                  // Open the backlight
  dht.setup(dhtPin, DHTesp::DHT11); // attach the dht pin and initialize it

  // RBG
  for (int i = 0; i < 3; i++)
  { // setup the pwm channels
    ledcSetup(chns[i], 1000, 8);
    ledcAttachPin(ledPins[i], chns[i]);
  }
  // RBG
  Serial.begin(115200); // Initialize the serial port and set the baud rate to 115200
  irrecv.enableIRIn();  // Start the receiver
  Serial.print("IR Remote is now running and waiting for IR message: ");
  Serial.println(recvPin); // print the infrared receiving pin
}


void loop()
{
  client.loop();
  if (irrecv.decode(&results))
  {                                        // Waiting for decoding
    serialPrintUint64(results.value, HEX); // Print out the decoded results
    Serial.println("");
    strcpy(respond, "");
    snprintf(buf, 50, "%c", convertChar(results.value));
    strcat(respond, buf);
    client.publish(topic3, respond);
    if (results.value == button_play) // Go back to the main screen when play is pressed
    {
      lcd.clear();
      lcd_cond = 0;
      menu_cond = 1;
      mode_cond = 1;
      mode_1 = 1;
      mode_2 = 1;
      rgb_cond = 0;
    }
    else if (results.value == button_menu) // Go to menu screen
    {
      lcd.clear();
      lcd_cond = 1;
      menu_cond = 0;
      mode_cond = 0;
      mode_1 = 1;
      mode_2 = 1;
    }

    // render mode screen if needed first
    if (mode_cond == 0)
    {

      if ((results.value == button_1 || mode_1 == 0) && mode_2 == 1) // mode 1: RGB speed
      {
        rgb_cond = 1;
        menu_cond = 1;
        mode_2 = 1;
        mode_1 = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("1: RGB Speed");
        lcd.setCursor(0, 1);
        
        for (int i = 0; i < 16; i++)
        {
          if (i ==0){
            lcd.print("-");
          }
          else if (i == 15){
            lcd.print("+");
          }
          else{
            lcd.print(arr_speed[i-1]);
          }
          
        }
        
        if (results.value == button_add)
        {
          if (current_speed < 13)
          {
            ++current_speed;
            arr_speed[current_speed] = '*';
            led_speed -= 5;
          }
        }
        else if (results.value == button_minus)
        {
          if (current_speed >= 0)
          {
            arr_speed[current_speed] = ' ';
            current_speed--;
            led_speed += 5;
          }
        }
      }
      
      if ((results.value == button_2 || mode_2 == 0) && mode_1 == 1) // Mode 2: Set RGB
      {

        mode_2 = 0;
        mode_1 = 1;

        // logic for rbg input and allow ir input to drill down through the logics

        // Set input for rgb
        if ((results.value == button_0 || results.value == button_1 || results.value == button_2 ||
             results.value == button_3 || results.value == button_4 || results.value == button_5 ||
             results.value == button_6 || results.value == button_7 || results.value == button_8 ||
             results.value == button_9) &&
            menu_cond == 1)
        {
          ir_value = convertHEX(results.value);
          if (current_rgb == 0 || current_rgb == 3 || current_rgb == 6)
          {
            if (ir_value >= 0 && ir_value <= 2)
            {
              arr_values[current_rgb] = ir_value;
              current_rgb++;
            }
          }

          else
          {
            arr_values[current_rgb] = ir_value;
            current_rgb++;
          }

          for (int j = 0; j < 9; j++)               //Print the current array of number in serial monitor
          {
            Serial.print(arr_values[j]);
            Serial.print(",");
          }
          Serial.println("");
        }
        menu_cond = 1;

        remain_rgb = current_rgb;
        traverse_rgb = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("2: Set RGB Values");
        lcd.setCursor(0, 1);

        if (current_rgb == 9)
        {
          red = 0;
          blue = 0;
          green = 0;
          for (int i = 0; i < 3; i++)
          {
            if (i == 0)
            {
              red += arr_values[i] * 100;
              blue += arr_values[i + 3] * 100;
              green += arr_values[i + 6] * 100;
            }
            if (i == 1)
            {
              red += arr_values[i] * 10;
              blue += arr_values[i + 3] * 10;
              green += arr_values[i + 6] * 10;
            }
            else
            {
              red += arr_values[i];
              blue += arr_values[i + 3];
              green += arr_values[i + 6];
            }
          }
          current_rgb = 0;
          setColorMode2(red, blue, green); // Calculated rgb value and send it to the LED
        }

        for (int i = 0; i < rgb_length - 1; i++)
        {
          if (i == 0)
          {
            lcd.print("(");
          }
          else if (i == 12)
          {
            lcd.print(")");
          }
          else if (i % 4 == 0)
          {
            lcd.print(",");
          }
          else
          {
            if (remain_rgb > 0)
            {
              lcd.print(arr_values[traverse_rgb]);
              remain_rgb--;
              traverse_rgb++;
            }
            else
            {
              lcd.print(" ");
            }
          }
        }
      }
    }
    irrecv.resume();
    // Release the IRremote. Receive the next value
  }

  // IF the default  screen mode is chosen, this will be rendered
  if (lcd_cond == 0)
  {
  flag:
    TempAndHumidity DHT = dht.getTempAndHumidity();
    if (dht.getStatus() != 0)
    { // Determine if the read is successful, and if it fails, go back to flag and re-read the data
      goto flag;
    }

    temp = (DHT.temperature * 1.8) + 32;
    humid = DHT.humidity;

    lcd.setCursor(0, 0);       // set the cursor to column 0, line 1
    lcd.print("Temperature:"); // display the Humidity on the LCD1602
    lcd.print(temp);
    lcd.setCursor(0, 1);       // set the cursor to column 0, line 0
    lcd.print("Humidity   :"); // display the Humidity on the LCD1602
    lcd.print(humid);

    if (rgb_cond == 0)
    {
      if (temp != preTemp || preHumid != humid)
      {
        strcpy(respond, "Temperature: ");
        snprintf(buf, 50, "%.2f", temp);
        strcat(respond, buf);
        strcat(respond, "  Humidity: ");
        snprintf(buf, 50, "%.2f", humid);
        strcat(respond, buf);
        client.publish(topic2, respond); // Publish the temp and humid
        setColorMode0(0, 0, 0);
        delay(100);
      }
      if (temp >= 80 && temp <= 100.0) // RBG LED will change color accordingly
      {
        setColorMode0((int)DHT.temperature + 155, 0, 0);
      }
      else if (temp >= 50 && temp <= 100.0)
      {
        setColorMode0(0, (int)temp + 155, 0);
      }
      else
      {
        setColorMode0(0, 0, (int)temp + 155);
      }
      preTemp = temp;
      preHumid = humid;
    }
  }
  else if (menu_cond == 0)
  {

    lcd.setCursor(0, 0);
    lcd.print("    LED MODE");
    lcd.setCursor(0, 1);
    lcd.print("     1    2"); // 1 is set specific color, 2 is set the speed
  }

  // RGB
  if (rgb_cond == 1) // Mode 1: RGB will loop through the color wheel
  {
    for (int i = 0; i < 256; i++)
    {
      setColor(wheel(i));
      delay(led_speed);
    }
  }
  else if (rgb_cond == 2)
  {
  }

  // remote
}

void setColorMode2(int &red, int &green, int &blue)
{
  ledcWrite(chns[2], 255 - red);
  ledcWrite(chns[1], 255 - green);
  ledcWrite(chns[0], 255 - blue);
}
void setColorMode0(int red, int green, int blue)
{
  ledcWrite(chns[2], 255 - red);
  ledcWrite(chns[1], 255 - green);
  ledcWrite(chns[0], 255 - blue);
}

void setColor(long rgb)
{
  rgb_cond = 2;
  ledcWrite(chns[0], 255 - (rgb >> 16) & 0xFF);
  ledcWrite(chns[1], 255 - (rgb >> 8) & 0xFF);
  ledcWrite(chns[2], 255 - (rgb >> 0) & 0xFF);
}

long wheel(int pos)
{
  long WheelPos = pos % 0xff;
  if (WheelPos < 85)
  {
    return ((255 - WheelPos * 3) << 16) | ((WheelPos * 3) << 8);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return (((255 - WheelPos * 3) << 8) | (WheelPos * 3));
  }
  else
  {
    WheelPos -= 170;
    return ((WheelPos * 3) << 16 | (255 - WheelPos * 3));
  }
}

int convertHEX(long unsigned value) // convert ir input to int
{
  switch (value)
  {
  case button_0:
    ir_value = 0;
    break;
  case button_1:
    ir_value = 1;
    break;
  case button_2:
    ir_value = 2;
    break;
  case button_3:
    ir_value = 3;
    break;
  case button_4:
    ir_value = 4;
    break;
  case button_5:
    ir_value = 5;
    break;
  case button_6:
    ir_value = 6;
    break;
  case button_7:
    ir_value = 7;
    break;
  case button_8:
    ir_value = 8;
    break;
  case button_9:
    ir_value = 9;
    break;
  }
  return ir_value;
}
char convertChar(long unsigned values) // convert ir input to char
{
  char value;
  switch (values)
  {
  case button_0:
    value = '0';
    break;
  case button_1:
    value = '1';
    break;
  case button_2:
    value = '2';
    break;
  case button_3:
    value = '3';
    break;
  case button_4:
    value = '4';
    break;
  case button_5:
    value = '5';
    break;
  case button_6:
    value = '6';
    break;
  case button_7:
    value = '7';
    break;
  case button_8:
    value = '8';
    break;
  case button_9:
    value = '9';
    break;
  case button_add:
    value = '+';
    break;
  case button_minus:
    value = '-';
    break;
  case button_play:
    value = 'P';
    break;
  case button_menu:
    value = 'M';
    break;
  defaut:
    value = '!';
  }
  return value;
}

// Handle call back and print it to the LCD
void callback(char *topic, byte *payload, unsigned int length)
{
  if (lcd_cond == 0)
  {
    lcd.clear();
    strcpy(respond, "   It is a ");
    lcd.setCursor(0, 0);
    lcd.print(respond);
    snprintf(buf, length + 1, "%s", payload);
    strcpy(respond, "   ");
    strcat(respond, buf);
    strcat(respond, " day!");
    lcd.setCursor(0, 1);
    lcd.print(respond);
    delay(2200);
  }
}
