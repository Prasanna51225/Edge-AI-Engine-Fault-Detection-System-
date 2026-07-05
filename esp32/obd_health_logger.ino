#include <WiFi.h>
#include <HTTPClient.h>

// UART Pins
#define RXD2 16
#define TXD2 17
#define UART_BAUD 115200

// WiFi credentials
const char* ssid = "Your WIFI SSID";
const char* password = "Password";

// ThingSpeak settings
const char* thingSpeakApiKey = "YOUR_THINGSPEAK_WRITE_API_KEY";
const char* thingSpeakServer = "http://api.thingspeak.com/update";

// Variables for data processing
String receivedData = "";
String processedValue = "";   // numeric data for ThingSpeak
String fullMessage = "";      // "Fault: 0" full text
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 15000; // 15 sec (ThingSpeak limit)

// WiFi connection status
bool wifiConnected = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(UART_BAUD, SERIAL_8N1, RXD2, TXD2);

  // Connect to WiFi
  connectToWiFi();
  
  Serial.println("UART Receiver Started...");
}

void loop() {
  // Check WiFi connection status
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    Serial.println("WiFi disconnected. Attempting to reconnect...");
    connectToWiFi();
  } else if (!wifiConnected) {
    // Just connected to WiFi
    wifiConnected = true;
    Serial.print("Connected to WiFi! IP Address: ");
    Serial.println(WiFi.localIP());
  }

  // Read full line from UART
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');  // read until newline
    data.trim(); // Remove any extra whitespace or carriage return

    int idx = data.indexOf("Fault:");
    if (idx != -1) {
      fullMessage = data.substring(idx);  // "Fault: 0"
      
      // Extract only the number after "Fault:"
      processedValue = fullMessage.substring(7); // skips "Fault: "
      processedValue.trim();  // remove spaces/newlines

      // Print both full message and numeric value
      Serial.print("Full message received: ");
      Serial.println(fullMessage);
      Serial.print("Numeric value to send: ");
      Serial.println(processedValue);
    } 
  }

  // Send to ThingSpeak every 15 sec
  if (millis() - lastSendTime >= sendInterval) {
    if (wifiConnected) {
      sendToThingSpeak();
    } else {
      Serial.println("Cannot send to ThingSpeak - WiFi not connected");
    }
    lastSendTime = millis();
  }

  delay(10);
}

void connectToWiFi() {
  Serial.println();
  Serial.println("Connecting to WiFi...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  WiFi.disconnect(true);  // Disconnect from any previous connection
  delay(1000);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    wifiConnected = false;
    Serial.println("\nFailed to connect to WiFi. Please check credentials.");
    Serial.println("Will retry in 10 seconds...");
    delay(10000); // Wait 10 seconds before retrying
  }
}

void sendToThingSpeak() {
  if (processedValue.length() > 0) {
    HTTPClient http;
    
    // Send only numeric value
    String url = String(thingSpeakServer) + "?api_key=" + String(thingSpeakApiKey) + "&field1=" + processedValue;

    Serial.print("Sending to ThingSpeak: ");
    Serial.println(processedValue);  // print numeric value

    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.print("ThingSpeak response code: ");
      Serial.println(httpResponseCode);
      
      if (httpResponseCode == 200) {
        String response = http.getString();
        int entryId = response.toInt();
        Serial.print("Data stored at entry ID: ");
        Serial.println(entryId);
      }
    } else {
      Serial.print("Error sending to ThingSpeak. HTTP code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("No processed value available to send to ThingSpeak.");
  }
}
