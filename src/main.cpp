#include <Arduino.h>
#include "ota.h"
#include "credentials.h"

//set led pin
const int ledPin = 2;
#define DELAY 50
#define CLICK_TIME 800

// void setup() {
//   Serial.begin(115200);
//   Serial.println("Booting");

//   setupOTA("ESP32-Test-OTA", ssid, pass);

//   // Your setup code

//   //set led pin as output
//   pinMode(ledPin, OUTPUT);

// }

// void loop() {
//   ArduinoOTA.handle(); // Handle OTA events, must be called often

//   //blink led
//   digitalWrite(ledPin, HIGH);
//   delay(DELAY);
//   digitalWrite(ledPin, LOW);
//   delay(DELAY);

// }

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";

// Assign output variables to GPIO pins
const int output26 = 2;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  setupOTA("ESP32-Test-OTA", ssid, pass);

  // allow for OTA if something goes wrong
  for (int i = 0; i < 10; i++) {
    ArduinoOTA.handle();
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(50);
    ArduinoOTA.handle();
    digitalWrite(ledPin, HIGH);
    delay(250);
    digitalWrite(ledPin, LOW);
    delay(50);
  }

  // Initialize the output variables as outputs
  pinMode(ledPin, OUTPUT);
  pinMode(output26, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
    Serial.print(".");
  }
  digitalWrite(ledPin, HIGH);

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  ArduinoOTA.handle(); // Handle OTA events, must be called often
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      ArduinoOTA.handle(); // Handle OTA events, must be called often
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            bool show_site = true;

            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) { // 26 ON 
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } 
            else if (header.indexOf("GET /26/off") >= 0) { // 26 OFF
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } 
            else if (header.indexOf("GET /26/click") >= 0) { // 26 CLICK
              Serial.println("GPIO 26 click");
              digitalWrite(output26, LOW);
              delay(10);
              digitalWrite(output26, HIGH);
              delay(CLICK_TIME);
              digitalWrite(output26, LOW);
            }
            else if (header.indexOf("GET /26/state") >= 0) { // 26 STATE
              Serial.println("GPIO 26 state");
              client.println(output26State);
              show_site = false;
            }


              
            if (show_site) {
              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              // CSS to style the on/off buttons 
              // Feel free to change the background-color and font-size attributes to fit your preferences
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
              client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
              client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
              client.println("</style></head>"); //client.println(".button2 {background-color: #555555;}</style></head>"); 
              
              // Web Page Heading
              client.println("<body><h1>ESP32 Web Server</h1>");
              
              // Display current state, and ONN/OFF buttons for GPIO 26  
              client.println("<p>GPIO 26 - State " + output26State + "</p>");
              // client.println("<p>" + header + "</p>");
              // If the output26State is off, it displays the ONN button       
              if (output26State=="off") {
                client.println("<p><a href=\"/26/on\"><button class=\"button\">ONN</button></a></p>");
              } else {
                client.println("<p><a href=\"/26/off\"><button class=\"button\">OFF</button></a></p>"); //client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
              } 
            }
            // The HTTP response ends with another blank line
            client.println();

            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  delay(50);
}