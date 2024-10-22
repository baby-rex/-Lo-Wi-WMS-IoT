#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

// Initialize SoftwareSerial on pins 2 (RX) and 3 (TX)
SoftwareSerial gsm(2, 3);

void setup() {
  // Start communication with the GSM module
  gsm.begin(9600);
  
  // Start the primary serial communication for debugging
  Serial.begin(115200);

  // Wait for the GSM module to initialize
  delay(1000); // Increase initialization delay
  Serial.println("Initializing...");

  // Check GSM module communication
  if (!checkGSMModule()) {
    Serial.println("GSM module not responding.");
    while (1) {
      // Prevent the watchdog timer reset loop by adding a delay
      delay(1000);
    }
  }

  // Send an SMS
  sendSMS("+917715958053", "Hello from ESP8266");
}

void loop() {
  // Check for messages received by the GSM module
  if (gsm.available()) {
    String message = gsm.readString();
    Serial.print("Received message: ");
    Serial.println(message);
  }

  // Delay to prevent watchdog timer reset
  delay(1000);
}

bool checkGSMModule() {
  gsm.println("AT"); // Check if the module is ready
  delay(1000);

  if (gsm.available()) {
    String response = gsm.readString();
    Serial.print("Response: ");
    Serial.println(response);
    return response.indexOf("OK") != -1;
  }

  return false;
}

void sendSMS(const char* number, const char* message) {
  gsm.println("AT+CMGF=1"); // Set the module to SMS mode
  delay(1000);
  readResponse();

  gsm.print("AT+CMGS=\"");
  gsm.print(number);
  gsm.println("\"");
  delay(1000);
  readResponse();

  gsm.print(message);
  delay(100);
  gsm.print((char)26); // Send Ctrl+Z to indicate the end of the message
  delay(5000); // Wait for the module to process the message
  readResponse();
}

void readResponse() {
  long timeout = millis() + 5000; // 5 seconds timeout
  while (millis() < timeout) {
    while (gsm.available()) {
      String response = gsm.readString();
      Serial.print("Response: ");
      Serial.println(response);
      if (response.indexOf("OK") != -1 || response.indexOf("+CMGS") != -1) {
        Serial.println("Message sent successfully.");
        return;
      }
      if (response.indexOf("ERROR") != -1) {
        Serial.println("Error sending message.");
        return;
      }
    }
  }
  Serial.println("No response from GSM module.");
}