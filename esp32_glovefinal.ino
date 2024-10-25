#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define OLED width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// I2C address of the OLED display (most common is 0x3C)
#define OLED_I2C_ADDRESS 0x3C

// Create an instance of the SSD1306 display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_I2C_ADDRESS);

// WiFi credentials
const char* ssid = "_Viren_";       
const char* password = "Sanju@077";

// AWS IoT credentials
const char* aws_endpoint = "a1trnkrnx34ztb-ats.iot.ap-southeast-2.amazonaws.com";  // AWS IoT endpoint
const int aws_port = 8883;  // MQTT over TLS port

// AWS IoT root certificate (Amazon Root CA 1)
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n";

// AWS IoT device certificate
const char* device_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDWTCCAkGgAwIBAgIUPxaX03Q3UxrdQ0bxdBVr8PVrNsEwDQYJKoZIhvcNAQEL\n" \
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n" \
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTAxOTA5MTk1\n" \
"MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n" \
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOP/rP3r3Fu7lX6ySdvw\n" \
"jXf7Tge2It6boGFR4U/Q5N3SpOtRY+XkY6YKs0jG5Vbaw2coajYwm8/1dopIVU+f\n" \
"LqRIfk7DQge/rGzfIxfKbk7VTo6e31aeItMgM0WMdR9p+B1zxxRLmeT0To6pP5bz\n" \
"FeyYtQ6CCpjJnDMlPnk0WXNxzDO5h/HPzvu916cTo7DkaH3QlyoAyqZcB3DYXCJl\n" \
"bT+nku7l8z0UX5uj2FGOQoCG+IGti/4rQhAwZLOGmIWFCcmGdHZH3plYlMEzgKBc\n" \
"4YtdbdcS1lrvlWqgMY8uuBdQb5bwtqKiQevcn1kEerV2WHq1eLTrTvH91fLJdwYa\n" \
"G/MCAwEAAaNgMF4wHwYDVR0jBBgwFoAUdWVsomE/cuJiXZwD3/NstacyJO4wHQYD\n" \
"VR0OBBYEFAxcE44HinCyiHNOvoq36ybUmdl9MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n" \
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBMs0PZRaBTZbhGqh24USgrGuhM\n" \
"OC9o08V9OqcBQL6vyQcGIWVUNj2PgREEwnp1Iff/hum+Sj0mn/fE8blhqmNOR63C\n" \
"pMDCfErNRub2nvg+Ivj2SolfwLzSV8mTuUWeR34K17Iom92MGQf9VIPm9SeYtnPw\n" \
"MI8o5t0+0o6C7JpJ6Tp9CDkAvGhDFOqTimwUaoC1PdCbuV9dhgWgeymVylFxHJdN\n" \
"BEhRYQTMf9F3DVXOfvnp/AZ/UFV+EjrgnsmlQ+yG/nYxAzbMGuMjLZmefo/M/zPy\n" \
"NPg628oikCLdm8y0ekhnJmO0Omah98a7FicJBVxVglIUgjCg/IwUBPoQBzJC\n" \
"-----END CERTIFICATE-----\n";

// AWS IoT private key
const char* private_key = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpAIBAAKCAQEA4/+s/evcW7uVfrJJ2/CNd/tOB7Yi3pugYVHhT9Dk3dKk61Fj\n" \
"5eRjpgqzSMblVtrDZyhqNjCbz/V2ikhVT58upEh+TsNCB7+sbN8jF8puTtVOjp7f\n" \
"Vp4i0yAzRYx1H2n4HXPHFEuZ5PROjqk/lvMV7Ji1DoIKmMmcMyU+eTRZc3HMM7mH\n" \
"8c/O+73XpxOjsORofdCXKgDKplwHcNhcImVtP6eS7uXzPRRfm6PYUY5CgIb4ga2L\n" \
"/itCEDBks4aYhYUJyYZ0dkfemViUwTOAoFzhi11t1xLWWu+VaqAxjy64F1BvlvC2\n" \
"oqJB69yfWQR6tXZYerV4tOtO8f3V8sl3Bhob8wIDAQABAoIBAQC8aXb9x0GslNGv\n" \
"h4o9tQZTVltfSAZ8y0K36cA2Azx+iyo6RGI3gotOlZ0e0S6InfPIdb9/1J+ONRym\n" \
"navSRot2u2mVkQop+R12QtdvO5v4jZGDjoqlu7yvl/aGzNGlCh89x4WGogslBzc7\n" \
"8WVZSrydbnyA5rn3FPR915NO+hHJsXtW7hDZsCtBTvFoOIbEF6MFT0aqQF5Yhi9G\n" \
"H7DhFBbjnHR+yqRdCGu8bvFF9SScOMZdOWH6hvDRUcwRbpgEoJDDYKhLiZXHtFyK\n" \
"g4UwXKOvn2p3hRRxdDl/F+blqDfQecihSlk7oweAxlPUKkC6Ku5SXTLxPNVbT318\n" \
"Gmjet57RAoGBAPb5P2ADULgvZPvM6/qETCIq3On1OTqIQlu77FtfDNoQNhUKPOjH\n" \
"q0sGu9qFDMUPICnAsOU+gEbJIzZ6bqT2jLgD4ulrvUwvpLJjSc9Wt1mlKJ2mPx3I\n" \
"VQEToI96jTiJwK6IVasujyucHpNcM3KniImmISio8CJlLCMsDVCa5CGFAoGBAOxU\n" \
"5NpTjY7zBZE33IuqeHUUAcW433Wa10cKfkliOOoMZq+APvQ+YrIH7+VsMqINGXaR\n" \
"kVhF4SLZmgkyMGulA3fmSR5vTPlb4WmlmdP716x3IzqNHgwOjLmhAYO1Ie5PhwhQ\n" \
"m4sW6lX3CbjIO+cWQmX9qJDoS0X/w54rPTCrvYUXAoGBALcg3BhCfREFu5hmO7g3\n" \
"bwiKxeII/lVjgejpcHnUAgCTsrZpL/7fRylLpfGDZJOeAphTFBf3/z/9lsmngPF7\n" \
"C57K6u9v/x5F2ZaYvrsqLqwfJf23rudTi4xP1GdvVNXOdvwzhWp4DJsmmRllbZSf\n" \
"m0ON1tqCYgTvBgpSeNEC+RPNAoGAGfxDpZXmTL9/7zW7NEgsMoNepoTrXR0HarJG\n" \
"rH72TxXJ/KYXRffOFlE1GX7KGnNiR10+I/ybAp/2hehU1qEwPnC9vXDzf7+Qi3M/\n" \
"9MUOM9JttzRpB15aLtb7OaYi8vcfm9RxJdwBLC6bjQvHOmOCLYC0lipUjH/Q3IVY\n" \
"lw9rKUUCgYAZPkCMajwDsGKAXzW/iclkSdcBFXqQ3zvrK9DGqnlS1qf20h99Loqg\n" \
"8B1BMnvOkkUxMpWtUvd4ugtOqrxyOHNQNElqsmKsg4fjf33RGJr3sCJ+1WAIayd1\n" \
"K7wbddZttdb0VEnx/oGAqHA0ZbmOpWpE1ouzku+Rt/bHmnh1lLpaAQ==\n" \
"-----END RSA PRIVATE KEY-----\n";


// Create secure WiFi and MQTT clients
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// Constants for threshold values
const int threshold = 760 ;
unsigned int f, g, h; // Variables for sensor readings
String lastMessage = ""; 

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
}

void connectToAWS() {
  // Set the AWS IoT root certificate, device certificate, and private key
  wifiClient.setCACert(root_ca);
  wifiClient.setCertificate(device_cert);
  wifiClient.setPrivateKey(private_key);

  mqttClient.setServer(aws_endpoint, aws_port);

  // Connect to AWS IoT Core
  while (!mqttClient.connected()) {
    Serial.print("Connecting to AWS IoT...");
    if (mqttClient.connect("ESP32Client")) {  // Use your own unique client ID
      Serial.println("Connected to AWS IoT!");
      mqttClient.subscribe("esp32"); // Subscribe to a topic if necessary
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Trying again in 5 seconds...");
      delay(5000);
    }
  }
}

void publishToMQTT(const String &message) {
  if (mqttClient.connected()) {
    mqttClient.publish("esp32", message.c_str());
  } else {
    Serial.println("MQTT client not connected");
  }
}

void setup() {
  // Initialize Serial communication
  Serial.begin(115200); 

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Display a welcome message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Welcome To Sign"));
  display.println(F("Language Project"));
  display.display();

  Serial.println("Welcome to Sign Language Project");
  delay(3000);
  display.clearDisplay();

  // Connect to WiFi and AWS
  connectToWiFi();
  connectToAWS();
}

void loop() {
  // Read sensor values
  f = analogRead(34);  // Sensor 1 on GPIO 34
  g = analogRead(35);  // Sensor 2 on GPIO 35
  h = analogRead(32);  // Sensor 3 on GPIO 32

  // Debugging output to Serial Monitor
  Serial.print("f reading: ");
  Serial.println(f);
  Serial.print("g reading: ");
  Serial.println(g);
  Serial.print("h reading: ");
  Serial.println(h);

  display.clearDisplay();  // Clear previous content
  String message = "";      // String to hold the message to be published

  // Display messages based on sensor readings
  if ((f <= 4000) && (g > threshold) && (h > threshold)) {
    message = "I'm Thirsty, Need Water";
    display.setCursor(0, 0);
    display.println(F("I'm Thirsty"));
    display.println(F("Need Water"));
    display.display();
    Serial.println(message);
  } 
  else if ((f > threshold) && (g <= 4000) && (h > threshold)) {
    message = "I'm Hungry, Give Me Food";
    display.setCursor(0, 0);
    display.println(F("I'm Hungry"));
    display.println(F("Give Me Food"));
    display.display();
    Serial.println(message);
  } 
  else if ((f > threshold) && (g > threshold) && (h <= 4000)); {
    message = "I'm Lost, Please Guide Me";
    display.setCursor(0, 0);
    display.println(F("I'm Lost"));
    display.println(F("Plz Guide Me"));
    display.display();
    Serial.println(message);
  } 
  

  if (message != lastMessage) {
    // Display the message on OLED
    display.setCursor(0, 0);
    display.println(message);
    display.display();

    // Print message to Serial Monitor
    Serial.println(message);

    // Publish the message to AWS IoT via MQTT
    publishToMQTT(message);

    // Update the lastMessage to the current one
    lastMessage = message;
  }

  delay(200);  // Shorter delay for smoother updates

  // Maintain the MQTT connection
  mqttClient.loop();
}
