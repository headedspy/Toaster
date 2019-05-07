/*********
  ESP12-E MODULE
  MCU 1.0
  9600 baud
*********/

#include <ESP8266WiFi.h>

#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "net365.mobi";
const char* password = "28007442";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

uint8_t Pos1 = 1;
uint8_t Pos2 = 1;
uint8_t Pos0 = 0;

static const uint8_t UC1 = 5; //D1
static const uint8_t DC1 = 4; //D2
static const uint8_t UC2 = 14;//D5
static const uint8_t DC2 = 12;//D6

static const uint8_t ENABLE = 13;//D7
static const uint8_t PHASE = 15; //D8

void setup() {
  Serial.begin(9600);

  pinMode(UC1, OUTPUT);
  pinMode(DC1, OUTPUT);
  pinMode(UC2, OUTPUT);
  pinMode(DC2, OUTPUT);

  pinMode(ENABLE, OUTPUT);
  pinMode(PHASE, OUTPUT);

  digitalWrite(UC1, HIGH);
  digitalWrite(DC1, HIGH);
  digitalWrite(UC2, HIGH);
  digitalWrite(DC2, HIGH);
  
  digitalWrite(PHASE, LOW);
  digitalWrite(ENABLE, LOW);

  hold(false, true);
  hold(false, false);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if(header.indexOf("/uc1") >= 0){
              digitalWrite(UC1, LOW);
              delay(5);
              digitalWrite(UC1, HIGH);
            }else if(header.indexOf("/dc1") >= 0){
              digitalWrite(DC1, LOW);
              delay(5);
              digitalWrite(DC1, HIGH);
            }else if(header.indexOf("/uc2") >= 0){
              digitalWrite(UC2, LOW);
              delay(5);
              digitalWrite(UC2, HIGH);
            }else if(header.indexOf("/dc2") >= 0){
              digitalWrite(DC2, LOW);
              delay(5);
              digitalWrite(DC2, HIGH);
            }else if(header.indexOf("DOWN1") >= 0){
              hold(true, true);
              Pos1 = 1;
            }else if(header.indexOf("DOWN2") >= 0){
              hold(true, false);
              Pos2 = 1;
            }else if(header.indexOf("UP1") >= 0){
              hold(false, true);
              Pos1 = 64;
            }else if(header.indexOf("UP2") >= 0){
              hold(false, false);
              Pos2 = 64;
            }else if(header.indexOf("down1") >= 0){
              change(true, true);
              Pos1++;
            }else if(header.indexOf("down2") >= 0){
              change(true, false);
              Pos2++;
            }else if(header.indexOf("up1") >= 0){
              change(false, true);
              Pos1--;
            }else if(header.indexOf("up2") >= 0){
              change(false, false);
              Pos2--;
            }else if(header.indexOf("/goDown") >= 0){
              goDown();
            }else if(header.indexOf("/goUp") >= 0){
              goUp();
            }else if(header.indexOf("/StepZero") >= 0){
              Serial.print("ZERO");
              while(Pos0 != 0){
                goUp();
              }
            }else if(header.indexOf("/StepOne") >= 0){
              Serial.print("ONE");
              while(Pos0 != 1){
                if(Pos0 == 0){
                  goDown();
                }else{
                  goUp();
                }
              }
            }else if(header.indexOf("/StepTwo") >= 0){
              Serial.print("TWO");
              while(Pos0 != 2){
                if(Pos0 < 2){
                  goDown();
                }else{
                  goUp();
                }
              }
            }else if(header.indexOf("/StepThree") >= 0){
              Serial.print("THREE");
              while(Pos0 != 3){
                if(Pos0 < 3){
                  goDown();
                }else{
                  goUp();
                }
              }
            }else if(header.indexOf("/StepFour") >= 0){
              Serial.print("FOUR");
              while(Pos0 != 4){
                if(Pos0 < 4){
                  goDown();
                }else{
                  goUp();
                }
              }
            }else if(header.indexOf("/MotorF") >= 0){
              digitalWrite(PHASE, LOW);
              digitalWrite(ENABLE, HIGH);
              delay(500);
              digitalWrite(ENABLE, LOW);
            }

            Serial.print("[POS1]");
            Serial.print(Pos1);
            Serial.print("[POS2]");
            Serial.print(Pos2);
            Serial.print("[POS0]");
            Serial.print(Pos0);
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

            client.println("<script>window.onload = function(){var x = document.getElementsByClassName(\"button"+String(Pos0)+"\"); var i; for(i = 0; i < x.length; i++){x[i].classList.add(\"selected\");}}</script>");

            
            client.println("<style>");

            client.println("body{ background-color: #333}");
            client.println("ul{ padding: 0; list-style-type: none; }");
            client.println("li{ font-size: 25px; width: 8em; height: 2em; color: orange; border-left: 0.08em solid; position: relative; margin-top: 0.8em; cursor: pointer; margin-left: auto; margin-right: auto; }");
            client.println("li::before, li::after{ content: ''; position: absolute; width: inherit; border-left: inherit; z-index: -1; }");
            client.println("li::before{ height: 80%; top: 10%; left: calc(-0.15em - 0.08em * 2); filter: brightness(0.8); }");
            client.println("li::after{ height: 60%; top: 20%; left: calc(-0.15em * 2 - 0.08em * 3); filter: brightness(0.6); }");
            client.println("li span{ position: relative; height: 120%; top: -10%; box-sizing: border-box; border: 0.08em solid; background-color: #333; display: flex; align-items: center; justify-content: center; font-family: sans-serif; text-transform: capitalize; transform: translateX(calc(-0.15em * 3 - 0.08em * 2)); transition: 0.3s; }");
            client.println("li:hover span{ transform: translateX(0.15em); }");
            client.println("a{ color: #FFA500; text-decoration: none}");
            client.println(".red{color: #F26 !important}");
            client.println(".selected{color: #62F !important}");
            client.println("hr{height: 10px; border: 0; margin: 20px; box-shadow: 0 10px 10px -10px #ffa500 inset;}");
            client.println(".title{font-family: Helvetica Neue, Helvetica, Arial, sans-serif; font-size: 45pt; font-weight: 100; letter-spacing: 2px; text-align: center; color: #f35626; background-image: -webkit-linear-gradient(92deg, #f35626, #feab3a); -webkit-background-clip: text; -webkit-text-fill-color: transparent; -webkit-animation: hue 10s infinite linear; } @-webkit-keyframes hue { from { -webkit-filter: hue-rotate(0deg); } to { -webkit-filter: hue-rotate(-360deg); } }");
            
            client.println("</style>");
            client.println("<link rel=\"icon\" href=\"data:,\"></head>");
            
            client.println("<body>");

            client.println("<h1 class=\"title\">Toaster Web Server</h1>");
            client.println("");
            client.println("<ul>");
            client.println("<li class=\"red\"><span><a class=\"red\" href=\"/MotorF\">EJECT</a></span></li>");
            client.println("<hr>");
            client.println("<li class=\"button0\"><span><a class=\"button0\" href=\"/StepZero\">0</a></span></li>");
            client.println("<li class=\"button1\"><span><a class=\"button1\" href=\"/StepOne\">1</a></span></li>");
            client.println("<li class=\"button2\"><span><a class=\"button2\" href=\"/StepTwo\">2</a></span></li>");
            client.println("<li class=\"button3\"><span><a class=\"button3\" href=\"/StepThree\">3</a></span></li>");
            client.println("<li class=\"button4\"><span><a class=\"button4\" href=\"/StepFour\">4</a></span></li>");
            client.println("</ul>");
            
            client.println("</body></html>");
            
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
}

void hold(bool isDown, bool isFirst){
  uint8_t pin = 0;
  if(isFirst){
    if(isDown){
      pin = DC1;
    }else{
      pin = UC1;
    }
  }else{
    if(isDown){
      pin = DC2;
    }else{
      pin = UC2;
    }
  }

  digitalWrite(pin, LOW);
  delay(6700);
  digitalWrite(pin, HIGH);
}

void change(bool isDown, bool isFirst){
uint8_t pin = 0;
  if(isFirst){
    if(isDown){
      pin = DC1;
    }else{
      pin = UC1;
    }
  }else{
    if(isDown){
      pin = DC2;
    }else{
      pin = UC2;
    }
  }

  digitalWrite(pin, LOW);
  delay(2600);
  digitalWrite(pin, HIGH);
  delay(15);
}

void goUp(){
  if(Pos1 == 4){
    if(Pos2 == 1){
      change(false, true);
      Pos1--;
    }else{
      change(false, false);
      Pos2--;
    }
  }else{
    change(false, true);
    Pos1--;
  }
  updatePositions();
}

void goDown(){
  if(Pos1 < 4){
    change(true, true);
    Pos1++;
  }else if(Pos2 < 4){
    change(true, false);
    Pos2++;
  }
  updatePositions();
}

void updatePositions(){
  if(Pos1 > 4)Pos1 = 4;
  if(Pos1 < 1)Pos1 = 1;
  if(Pos2 > 4)Pos2 = 1;
  if(Pos2 < 1)Pos2 = 1;

  Pos0 = Pos1 + Pos2 - 2;
}
