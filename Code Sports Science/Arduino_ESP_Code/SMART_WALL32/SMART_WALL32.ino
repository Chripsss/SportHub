
#include <WiFi.h>
#include <HTTPClient.h>

// Set internet connection
const char* ssid = "sportHub";
const char* password = "sportScience123";
const char* host = "192.168.0.199";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.hostname("ESP32");
  WiFi.begin(ssid, password);

  // Check the internet connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WIFI_CONNECTED");
}

void loop() {
  if (Serial.available()) {
    // Read data from Arduino
    String data = Serial.readStringUntil('\n');

    // Parse the received data into variables
    String uid = data.substring(0, data.indexOf(','));
    data = data.substring(data.indexOf(',') + 1);

    float right = data.substring(0, data.indexOf(',')).toFloat();
    data = data.substring(data.indexOf(',') + 1);

    float fastest = data.substring(0, data.indexOf(',')).toFloat();
    data = data.substring(data.indexOf(',') + 1);

    float slowest = data.substring(0, data.indexOf(',')).toFloat();
    data = data.substring(data.indexOf(',') + 1);

    float avg = data.substring(0, data.indexOf(',')).toFloat();
    data = data.substring(data.indexOf(',') + 1);

    float accuracy = data.toFloat();


    // Print the data from Arduino
    Serial.println(String(uid) + "," + String(right) + "," + String(fastest, 3) + "," + String(slowest, 3) + "," + String(avg, 3) + "," + String(accuracy, 2));

    // Connect to server
    WiFiClient client;
    if (!client.connect(host, 80)) {
      Serial.println("Failed to connect to web server");
      return;
    }

    // Create an HTTPClient object
    String link = "http://192.168.0.199:80/database/SMART_WALL32.php?a=" + String(uid) + "&b=" + String(right) + "&c=" + String(fastest, 3) + "&d=" + String(slowest,3) + "&e=" + String(avg, 3) + "&f=" + String(accuracy,2);
    HTTPClient http;
    http.begin(client, link);

    // Execute the link
    http.GET();
    http.end();
    delay(1000);
  }
}
