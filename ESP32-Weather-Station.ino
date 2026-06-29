#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 4     
#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE);

// ⚠️ CHANGE THESE TO YOUR HOME WI-FI CREDENTIALS
const char* ssid     = "<your wifi>";
const char* password = "<wifi password>";

// Set web server port number to 80 (standard HTTP port)
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Connect to Wi-Fi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("Wi-Fi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client Connected.");          
    String currentLine = "";                // make a String to hold incoming data from the client
    
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Read sensor values
            float h = dht.readHumidity();
            float t = dht.readTemperature();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // Simple CSS styling to make it look clean on dark mode
            client.println("<style>html { font-family: sans-serif; display: inline-block; margin: 0px auto; text-align: center; background-color: #121212; color: white;}");
            client.println(".box { border: 2px solid #2196F3; border-radius: 10px; padding: 20px; display: inline-block; margin: 20px; min-width: 200px; }");
            client.println("h1 {color: #2196F3;} .data { font-size: 2rem; font-weight: bold; }</style>");
            // Web Page Title
            client.println("<title>ESP32 Weather Station</title></head>");
            client.println("<body><h1>My Room's Climate</h1>");
            
            // Temperature Box
            client.println("<div class=\"box\"><h2>Temperature</h2><p class=\"data\">");
            client.println(t);
            client.println(" &deg;C</p></div>");
            
            // Humidity Box
            client.println("<div class=\"box\"><h2>Humidity</h2><p class=\"data\">");
            client.println(h);
            client.println(" %</p></div>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            break;
          } else {    // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
  }
}
