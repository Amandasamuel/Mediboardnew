#include <Ticker.h>
#include <PxMatrix.h> //https://github.com/2dom/PxMatrix
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h> // Include WiFiManager library

#define SECRET_CH_ID_WEATHER_STATION 12397              //MathWorks weather station
#define SECRET_CH_ID_COUNTER 2737001          //Test channel for counting
#define SECRET_READ_APIKEY_COUNTER "P1823CW9MUPZN81Z" //API Key for Test channel

Ticker display_ticker;

// API details
const char* host = "api.mediboards.io";
const String path = "/api/public/hospitals/bc1999eb-8d4a-4a03-b056-766f87f4a81c/latest-patient";
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
  display.display(40);
}

void CheckApi() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    HTTPClient https;

    // Skip certificate validation
    client.setInsecure();

    // Configure timeout
    https.setTimeout(1000);

    // Add required headers
    https.addHeader("Accept", "text/html,application/xhtml+xml,text/plain");
    https.addHeader("Accept-Encoding", "identity");
    https.addHeader("User-Agent", "ESP8266HTTPClient");
    https.addHeader("Origin", "https://api.mediboards.io");
    https.addHeader("Connection", "keep-alive");

    // Begin HTTPS connection
    if (https.begin(client, String("https://") + host + path)) {
      int httpCode = https.GET();

      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          count = https.getString();
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }

    delay(1500);
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

  // Start display ticker
  display_ticker.attach(0.004, display_updater);

  // Display initial message
  display.clearDisplay();
  display.setCursor(35, 1);
  display.setTextSize(2);
  display.setTextColor(myRED); // Use Red color
  display.print("MEDIBOARDS ");
  display.setCursor(20, 17);
  display.setTextSize(2);
  display.setTextColor(myRED); // Use Red color
  display.print("BY SONVISAGE ");
  delay(3000);

  // Check API
  CheckApi();
}

void scroll_text(uint8_t ypos, unsigned long scroll_delay, String text) {
  uint16_t text_length = text.length();
  display.setTextWrap(false);
  display.setTextSize(2); // Increase text size to 2 (double the default size)
  display.setRotation(0);
  display.setTextColor(myRED); // Use Red color
   display.clearDisplay();
  // Draw static text once (outside the loop)
  display.setCursor(15, 1);
  display.setTextSize(2); // Size for static text
  display.setTextColor(myRED); // Use Red color
  display.print("JUTH EMERGENCY WARD");

  for (int xpos = 170; xpos > -(32 + text_length * 10); xpos--) { // Adjusted for larger text size
    // Clear only the area where the scrolling text is drawn
    display.fillRect(xpos, ypos, text_length * 10, 16, 0); // Clear previous text with black color

    // Draw scrolling text
    display.setCursor(xpos, ypos);
    display.println(text);

    yield();
  }

  CheckApi();
}

void loop() {
  display.clearDisplay();
  display.setCursor(30, 1);
  display.setTextSize(2);
  display.setTextColor(myRED); // Use Red color
  display.print(" WELCOME TO JUTH ");
  display.setCursor(17, 17);
  display.print("  EMERGENCY WARD ");
  delay(15000);

  String txt = "  HEALTH AND SERVICE  ";
  scroll_text(17, 0, String(txt)); // Use Red color
}
