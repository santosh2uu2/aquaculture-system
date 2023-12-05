#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <esp32DHT.h>


const char *ssid = "Santosh";
const char *password = "santosh2";

WebServer server(80);
DHT dht(26, DHT11);
const int motorPin1 = 2;
const int motorPin2 = 3;
const int motorPin3 = 4;
const int motorPin4 = 5;
const int rainSensorPin = 25;
const int ldrSensorPin = 34;

void handleRoot() {
  char msg[2000];
  if (server.hasArg("motor")) {
    String motorAction = server.arg("motor");
    if (motorAction == "on") {
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, HIGH);
      digitalWrite(motorPin4, LOW);
    } else if (motorAction == "off") {
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
    }
  }

  int ldrValue = readLDRSensor(); // Read the LDR sensor value
  String ldrStatus = (ldrValue > 500) ? "Light" : "Dark"; // Adjust the threshold as needed

  snprintf(msg, 2000,
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 Sensors and Motor</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 2.0rem; }\
    p { font-size: 2.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 Sensors and Motor</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperature</span>\
        <span>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span>\
        <span>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </p>\
      <p>\
        <i class='fas fa-cloud-showers-heavy' style='color:#1f618d;'></i>\
        <span class='dht-labels'>Rain Sensor</span>\
        <span>%s</span>\
      </p>\
      <p>\
        <i class='fas fa-lightbulb' style='color:#f1c40f;'></i>\
        <span class='dht-labels'>Light Level</span>\
        <span>%s</span>\
      </p>\
      <p>Public IP: %s</p>\
  </body>\
</html>",
           readDHTTemperature(), readDHTHumidity(), readRainSensor(), ldrStatus.c_str(), WiFi.localIP().toString().c_str()
          );
  server.send(200, "text/html", msg);
}

void setup(void) {
  Serial.begin(115200);
  delay(100);
  
  pinMode(26, INPUT);
  pinMode(rainSensorPin, INPUT);
  pinMode(ldrSensorPin, INPUT);
  dht.begin();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2); // Allow the CPU to switch to other tasks
}

float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    return t;
  }
}

float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    return h;
  }
}

String readRainSensor() {
  int rainValue = digitalRead(rainSensorPin);
  return (rainValue == HIGH) ? "Wet" : "Dry";
}

int readLDRSensor() {
  int ldrValue = analogRead(ldrSensorPin); // Read the LDR sensor value
  return ldrValue;
}