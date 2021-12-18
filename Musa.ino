/*
 * Embedded System Applications
 * December 2021
 * 
 * MQTT Mosquitto Messenger App
 *  
 * Contributors:
 * Musa Mert Karameşe
 * Umut Saatçıoğlu
 * Yaşar Enes Aydın
 * 
 * 
 * ESP2: Musa
 * ESP1: Umut
 * ESP0: Enes
*/

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

#include <WiFi.h>
#include <PubSubClient.h>

int mqtt_port = 1883;

const char* device_name = "Musa";    //Device Name
const char* ssid = "SUPERONLINE_WiFi_3477";   //Wi-Fi Name
const char* password = "RS200.krms";    //Wi-Fi Password
const char* mqtt_broker = "broker.hivemq.com";    //MQTT Broker Adress

String subscribe_1 = "BEFESP32/out_02";    //Private messages from Enes
String subscribe_2 = "BEFESP32/out_12";    //Private messages from Umut
String subscribe_3 = "BEFESP32/out_0";     //Messages from Enes
String subscribe_4 = "BEFESP32/out_1";     //Messages from Umut

//To Enes...
String publish_1 = "BEFESP32/out_20";
String message_to_1 = "Enes:";

//To Umut...
String publish_2 = "BEFESP32/out_21";
String message_to_2 = "Umut:";

//To all...
String publish_3 = "BEFESP32/out_2";

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

WiFiClient esp32client;   //Wi-Fi class
PubSubClient client(esp32client);   //MQTT Class <----> Wi-Fi Class

void setup() {
  Serial.begin(115200);   //Transmission baud rate = 115200
   while(!Serial) {
    }

  delay(10);
  //Wi-Fi connection

  Serial.print("Connecting to ");
  WiFi.begin(ssid, password);   //Trying to connect...

  while (WiFi.status() != WL_CONNECTED) {   //Is it connected?
    delay(500);   //Wait for 0.5 second if not
    Serial.print(".");
  }

  Serial.println("");   //Connection success
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   //LocalIp for the ESP32

  client.setServer(mqtt_broker, mqtt_port);   //Broker IP and Port
  client.setCallback(callback);
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

//Message read and write

void callback(char* subject, byte* message, unsigned int length) 
{
  String messageTemp;

  for (int i = 0; i < length; i++) {

    messageTemp += (char)message[i];    //subscribe message
  }
  

  if (String(subject) == subscribe_1 ) {
    Serial.println("Enes whispers: " + messageTemp);
  }
  else if (String(subject) == subscribe_2 ) {
    Serial.println("Umut whispers: " + messageTemp);
  }
  else if (String(subject) == subscribe_3 ) {
    Serial.println("Enes : " + messageTemp);
  }
  else if (String(subject) == subscribe_4) {
    Serial.println("Umut : " + messageTemp);
  }
  
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

//Connect to MQTT

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(device_name)) {
      Serial.println("connected");

      //subscribed adresses
      client.subscribe(subscribe_1.c_str());
      client.subscribe(subscribe_2.c_str());
      client.subscribe(subscribe_3.c_str());
      client.subscribe(subscribe_4.c_str());

    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

String TempMessage;
bool Ready;

void loop() {

  if (!client.connected()) {    //Is it connected to MQTTT?
    reconnect();    //Connect to MQTT
  }
  client.loop();    //Check messages


  if (Serial.available() > 0) {   //Check serial monitor
    char state;
    state = Serial.read(); //Read serial monitor
    
    if (state != 10)    //If letter ASCII value =/= 10, add to message
      TempMessage += String(state);

    if (state == 10) {    //If letter ASCII value = 10, send message to MQTT and reset TempMessage
      Ready = true;
      state = 0;
    }
  }
  
  if (Ready) {
    Ready = false;
    SendMessage();
    TempMessage = "";
  }
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

//Messaging

void SendMessage() {
  String Mes = TempMessage;
  if (Mes != "") {
    if (Mes.startsWith(message_to_1)) {
      Mes = Mes.substring(message_to_1.length());   //first 2 letters deleted
      client.publish(publish_1.c_str(), Mes.c_str());

      Serial.println(message_to_1.substring(0, message_to_1.length() - 1) + "(you whisper):" + Mes);

    }
    else if (Mes.startsWith(message_to_2)) {
      Mes = Mes.substring(message_to_2.length());   //first 2 letters deleted
      client.publish(publish_2.c_str(), Mes.c_str());

      Serial.println(message_to_2.substring(0, message_to_2.length() - 1) + "(you whisper):" + Mes);

    }
    else {
      client.publish(publish_3.c_str(), Mes.c_str());
      Serial.println(String (device_name) + "(you):"+Mes);
    }
  }

}
