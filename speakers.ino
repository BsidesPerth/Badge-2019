// Show the speakers and sessions

//URL of test file http://m.uploadedit.com/bbtc/1566736690361.txt
const char* host = "m.uploadedit.com"; // webserver
const char* url = "/bbtc/1566736690361.txt";

// Examples: ESP32 / Wifi / WifiClient
void sendGET() //client function to send/receive GET request data.
{
  Serial.println("sendGet");

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }


  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  bool fileStarted = false;
  String file;
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);

    if (fileStarted) {
      file += line;
    }

    // Assume that when we see "Content-Length" that the data is to follow
    if (line.indexOf("Content-Length") != -1) {
      fileStarted = true;
      Serial.println("\n=== Start File ===");
    }

  }

  Serial.println();
  Serial.println("closing connection");

  Serial.println("file:");
  Serial.println(file);
}
