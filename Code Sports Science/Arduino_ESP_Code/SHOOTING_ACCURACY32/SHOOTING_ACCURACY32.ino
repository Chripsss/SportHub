#include <WiFi.h>
#include <HTTPClient.h>

// Set internet connection
const char* ssid = "sportScience";
const char* password = "sportHub123";
const char* host = "192.168.0.200";

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

    int shoot = data.substring(0, data.indexOf(',')).toInt();
    data = data.substring(data.indexOf(',') + 1);

    int goal = data.substring(0, data.indexOf(',')).toInt();
    data = data.substring(data.indexOf(',') + 1);

    float accuracy = data.substring(0, data.indexOf(',')).toFloat();

    // Print the data from Arduino
    Serial.println(String(uid) + "," + String(shoot) + "," + String(goal) + "," + String(accuracy));

    // Connect to server
    WiFiClient client;
    if (!client.connect(host, 8080)) {
      Serial.println("Failed to connect to web server");
      return;
    }

    // Create an HTTPClient object
    String link = "http://192.168.0.200:8080/database/SHOOTING_ACCURACY32.php?a=" + String(uid) + "&b=" + String(shoot) + "&c=" + String(goal) + "&d=" + String(accuracy);
    HTTPClient http;
    http.begin(client, link);

    // Execute the link
    http.GET();
    http.end();
    delay(1000);
  }
}
