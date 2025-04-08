#include <Ticker.h>
#include <PxMatrix.h> //https://github.com/2dom/PxMatrix
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <WiFiManager.h> // Include WiFiManager library

#define SECRET_CH_ID_WEATHER_STATION 12397              //MathWorks weather station
#define SECRET_CH_ID_COUNTER 2737001          //Test channel for counting
#define SECRET_READ_APIKEY_COUNTER "P1823CW9MUPZN81Z" //API Key for Test channel

Ticker display_ticker;

// API details
const char* host = "api.mediboards.io";
const String path = "/api/public/hospitals/687f47fe-2429-4465-8b59-18432a3195fe/latest-patient";
String count;

WiFiClient client;

// Pin Definition for Nodemcu to HUB75 LED MODULE
#define P_LAT 16 //nodemcu pin D0
#define P_A 5    //nodemcu pin D1
#define P_B 4    //nodemcu pin D2
#define P_C 15   //nodemcu pin D8
#define P_OE 2   //nodemcu pin D4
#define P_D 12   //nodemcu pin D6
#define P_E 0    //nodemcu pin GND // no connection

PxMATRIX display(256, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);

// Single color (Red)
uint16_t myRED = display.color565(255, 0, 0);

// ISR for display refresh
void display_updater() {
  display.display(100);
}

void fetchAndDisplayData() {
  WiFiClientSecure client;
  HTTPClient https;

  // Skip certificate validation (use with caution)
  client.setInsecure();

  // Configure timeout
  https.setTimeout(10000);

  // Begin HTTPS connection
  if (https.begin(client, String("https://") + host + path)) {
    Serial.println("Fetching data from API...");

    // Send GET request
    int httpCode = https.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        // Read the response
        count = https.getString();
        Serial.println("API Response:");
        Serial.println(count);

        // Parse JSON response
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, count);

        if (error) {
          Serial.print("JSON parsing failed: ");
          Serial.println(error.c_str());
          return;
        }

        // Extract data from JSON
        const char* patientName = doc["patientName"];
        const char* bedNumber = doc["bedNumber"];
        const char* status = doc["status"];

        // Display data on P4 LED matrix
    
        Serial.println("Data displayed on LED matrix");
      } else {
        Serial.printf("HTTP GET failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
    } else {
      Serial.printf("HTTP GET failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.println("Unable to connect to API");
  }
}

void setup() {
  // Initialize display
  display.begin(16);
  display.clearDisplay();
  display.setTextColor(myRED); // Set default text color to Red
  Serial.begin(115200);

  // Initialize WiFiManager
  WiFiManager wifiManager;

  // Uncomment the following line to reset saved settings (for testing)
  // wifiManager.resetSettings();

  // Set a timeout for the configuration portal
  wifiManager.setTimeout(180); // 3 minutes

  // Attempt to connect to Wi-Fi or launch the configuration portal
  if (!wifiManager.autoConnect("MediboardsAP")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.reset(); // Reset and try again
    delay(5000);
  }

  Serial.println("Connected to WiFi");
 fetchAndDisplayData();
  // Start display ticker
  display_ticker.attach(0.0099, display_updater);

  // Display initial message
  display.clearDisplay();
  display.setCursor(55, 1);
  display.setTextSize(2);
  display.setTextColor(myRED); // Use Red color
  display.print(" MEDIBOARDS ");
  display.setCursor(45, 17);
  display.setTextSize(2);
  display.setTextColor(myRED); // Use Red color
  display.print(" BY SONVISAGE ");
  delay(6000);

  // Check API
   
}

void scroll_text(uint8_t ypos, unsigned long scroll_delay, String text) {
  uint16_t text_length = text.length();
  display.setTextWrap(false);
  //display.setTextSize(2); // Increase text size to 2 (double the default size)
  //display.setRotation(0);
  //display.setTextColor(myRED); // Use Red color
  display.clearDisplay();
  // Draw static text once (outside the loop)
  display.setCursor(15, 1);
  
  display.setTextColor(myRED); // Use Red color
  display.print(" JUTH EMERGENCY WARD");
   display.setTextSize(2); // Size for static text
  for (int xpos = 192; xpos > -(32 + text_length * 10); xpos--) { // Adjusted for larger text size
    // Clear only the area where the scrolling text is drawn
    display.fillRect(xpos, ypos, (text_length+10) * 10, 16, 0); // Clear previous text with black color

    // Draw scrolling text
    display.setCursor(xpos, ypos);
    display.println(text);

    yield();

  }

 
  
}

void loop() { fetchAndDisplayData();
Serial.println("count");
 Serial.println(count);
 //count="B12";
  if(count !=""){
    display.clearDisplay();
    display.setCursor(50, 1);
    display.setTextSize(2);
    display.setTextColor(myRED);
    display.print(" NEW PATIENT ");
    display.setCursor(50, 17);
    display.print(" COUCH " + (count));
    
     delay(15000);
  //display.setCursor(1, 1);
     String txt = ("NEW PATIENT AT COUCH "+ (count));
  //This will display the scrolling text.
     scroll_text(17, 0, String(txt));
     scroll_text(17, 0, String(txt));
     scroll_text(17, 0, String(txt));
    //scroll_text(y-pos, delay, "TEXT", R, G, B); 
    display.clearDisplay();
    }
    else {
      display.clearDisplay();
    display.setCursor(40, 1);
    display.setTextSize(2);
    display.setTextColor(myRED);
    display.print("WELCOME TO JUTH");
    display.setCursor(50, 17);
    display.print("EMERGENCY WARD ");
    //CheckApi();
     delay(15000);
  //display.setCursor(1, 1);
     String txt = ("  HEALTH AND SERVICE  ");
  //This will display the scrolling text.
     scroll_text(17, 0, String(txt));
     scroll_text(17, 0, String(txt));
     scroll_text(17, 0, String(txt));
    //scroll_text(y-pos, delay, "TEXT", R, G, B); 
    display.clearDisplay();
      }}
