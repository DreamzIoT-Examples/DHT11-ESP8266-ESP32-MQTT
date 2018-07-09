/* Here ESP32 will keep 2 roles: 
1/ read data from DHT11/DHT22 sensor
2/ control led on-off
So it willpublish temperature topic and scribe topic bulb on/off
*/

#include <WiFi.h> // For ESP32
//#include <ESP8266WiFi.h> // If you are compiling this code for ESP8266, Uncomment this line and comment above line  For ESP8266
#include <PubSubClient.h>
#include <DHT.h>


//-------------------------------------------------
// Enter All Configuration Details Here below 

#define MQTT_BROKER     "io.dreamziot.com"
#define MQTT_USERNAME   "user_fd831657"
#define MQTT_PASSWORD   "password_61d60876"
#define MQTT_TOPIC      "mqtt_6065207b_weather-station"
#define MQTT_CLIENT_ID  "mqtt_6065207b"   //Enter the deviceID within double quote. 

#define WIFI_SSID       "DreamzIOT"
#define WIFI_PASSWORD   "admin@123"

//--------------------------------------------------


/* define DHT pins */
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float temperature = 0;
float humidity = 0;
/* create an instance of PubSubClient client */
WiFiClient espClient;
PubSubClient client(espClient);

void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    /* client ID */
 //   String clientId = "mqtt_0f807f64";
    /* connect now */
    if (client.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}



void setup_wifi() {
  delay(10);
  //We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void setup() {
 
  Serial.begin(115200);     //Initialize serial
  setup_wifi();
  /* configure the MQTT server with IPaddress and port */
  client.setServer(MQTT_BROKER, 1883);
  dht.begin();
}


//Read DHT sensor's output
char DHT_processing(void)
{
  char Status = 0;
  /* read DHT11/DHT22 sensor and convert to string */
    humidity = dht.readHumidity();     //Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    temperature = dht.readTemperature();
    if (!(isnan(temperature) && isnan(humidity))) {
      Status = 1;
    }
    return Status;
}

long lastMsg = 0;
//Publish temprature and humidity on mqtt broker
void mqttPublish(void)
{
  
  char msg[75]; 
  char dhtStatus = 0;
  static int count = 0;
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    dhtStatus = DHT_processing();
    if(dhtStatus)
    {
    sprintf(msg, "{\n\"temp\":%d"",\n\"humi\":%d"",\n\"deviceID\":\"%s\"\n}", (int)temperature, (int)humidity, MQTT_CLIENT_ID);
    Serial.println(msg);
    client.publish(MQTT_TOPIC, msg);
    }
  }
}


void loop() {
  /* if client was disconnected then try to reconnect again */
  if (!client.connected()) {
    mqttconnect();
  }
  /* this function will listen for incomming 
  subscribed topic-process-invoke receivedCallback */
  client.loop();
  mqttPublish();
  //client.stop();
  //Serial.println("Client disconnected.");
}
