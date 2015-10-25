/*
 I Love Lamp!

 This sketch causes the Galileo to subscribe to an MQTT broker, 
 listen for messages, and upon receiving a short message, apply power to
 pin 8.  When a message of any other length is received, pin 8 is shut down.
 Thus I can power a lamp via a powerswitch tail using MQTT.

 To take advantage of the Linux aspects of the galileo, certain messages
 are written to syslog as the MQTT broker connection is made.  Within 
 the Galileo's linux config, these syslog messages are set to log to a remote
 server (a Splunk server).

 The Splunk server is also an MQTT subscriber, so it receives the lamp on/off
 messages and acks as well.
 
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <SD.h>

// Update these with values suitable for your network.
IPAddress server(10, 0, 0, 1);

//Callback function header
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

//Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  digitalWrite(8, int(length) == 1 ? HIGH : LOW);
  Serial.println();
  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  client.publish("Lamp1Ack", p, length);
  // Free the memory
  free(p);
}

//EthernetClient ethClient;
//PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    system("logger -p user.notice -t 'ilovelamp_script' 'Attempting MQTT connection...'");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      system("logger -p user.notice -t 'ilovelamp_script' 'connected'");
      // Once connected, publish an announcement...
      //client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("Lamp1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      system("logger -p user.notice -t 'ilovelamp_script' 'failed; try again in 5 seconds'");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(8, OUTPUT);

  client.setServer(server, 1883);
  client.setCallback(callback);

  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
