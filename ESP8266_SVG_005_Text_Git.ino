#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "Wifi Router Name Here";
const char *password = "WiFi password >7 characters";

MDNSResponder mdns;
ESP8266WebServer server ( 80 );
     

void setup ( void ) {
  Serial.begin ( 115200 );
  WiFi.begin ( ssid, password );
  Serial.println ( "" );
  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );

  if ( mdns.begin ( "esp8266", WiFi.localIP() ) ) {    Serial.println ( "MDNS responder started" );  }
  server.on ( "/", handleRoot );  server.on ( "/test.svg", Polygon );  server.on ( "/inline", []() {server.send ( 200, "text/plain", "this works as well" );} );
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop ( void ) {
  mdns.update();
  server.handleClient();
}
void Polygon() {                   // This routine set up the Circle SVG string for parsing to w3.org
  String out = "";
  char temp[500];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"1000\" height=\"800\">\n";
  out += "<g >\n";               // Start our data string          
  // As we are pushing data into  into a temporary buffer ( temp ), be sure that you make it big enough. (else things will break)
  // text = the command for text
  // <text x=\"0\" y=\"100\"        these are our x,y positions for the text
  // font-size=\"80\"               Size of our text
  // fill=\"black\"                 Text Colour
  // >ESP8266 SVG Text<             Our text encapsulated within these  > <
  // <tspan  /tspan>                makes a container for multiple line of text
  // </text>                        Be sure to end the text with </text>  to close the text wrapper part
  sprintf(temp, "<text x=\"0\" y=\"100\" font-size=\"80\" fill=\"black\">ESP8266 SVG Text<tspan font-size=\"50\" fill=\"blue\" x=\"0\" y=\"200\">SVG Text size 50</tspan></text>\n"); out +=temp;
 //Serial.println(out);
  out += "</g>\n</svg>\n";                   // close the SVG wrapper
  server.send ( 200, "image/svg+xml", out);  // and send it to http://www.w3.org/2000/svg\
  
  delay(500); //If you have a Sizable graphic then allow time for the response from w3.org before polling the site again (else things will break)
 }

void handleRoot() {

  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  snprintf ( temp, 400,
"<html>\
  <head>\
    <meta http-equiv='refresh' content='1'/>\
    <title>ESP8266 SVG Text </title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>   ESP8266 SVG Text </h1>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",
    hr, min % 60, sec % 60
  );
  server.send ( 200, "text/html", temp );
  }
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}
