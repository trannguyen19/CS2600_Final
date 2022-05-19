#include <WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#define LED_BUILTIN  2

// WiFi
const char *ssid = "tiger"; // Enter your WiFi name
const char *password = "12345678";  // Enter WiFi password
const char *mqtt_username = "testing";
const char *mqtt_password = "12345678";
const int MES_SIZE = 1;
int mess_len;
char message[MES_SIZE];
//char *message;


// MQTT Broker
const char *mqtt_broker = "192.168.64.184";
const char *topic = "esp32/main";
const char *topic2 = "esp32/cmd";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
 // Set software serial baud to 115200;
 Serial.begin(115200);
 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public MQTT broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
 // publish and subscribe
 client.publish(topic, "Published message from ESP32");
 client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
 
 mess_len = length;
 

 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
    // char_buff[i] = (char) payload[i];
     message[i] = (char) payload[i];
 }
 Serial.println();
 Serial.println("-----------------------");
 //message = (char *) malloc(length + 1);
 //strcpy(message,char_buff);
}

void loop() {
 client.loop();
 Serial.println(message);
 if(strcmp(message,"1")== 0){
   digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
   client.publish(topic2,"LED: ON");
 }
 else if(strcmp(message,"2") == 0) {
   digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
   client.publish(topic2,"LED: OFF");
  }
 //free(message);
}
