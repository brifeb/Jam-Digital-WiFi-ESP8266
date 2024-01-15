#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <time.h>

int pinCS = D6; // sama aja bisa juga D4; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays   = 1;
const byte buffer_size = 45;
char time_value[buffer_size];

int pinBuzz =  D1;
bool belumbunyi = true;

unsigned long timerStart = 0;
bool timerIsOFF = true;


// LED Matrix Pin -> ESP8266 Pin
// Vcc            -> 3v  (3V on NodeMCU 3V3 on WEMOS)
// Gnd            -> Gnd (G on NodeMCU)
// DIN            -> D7  (Same Pin for WEMOS)
// CS             -> D6/D4  (Same Pin for WEMOS)
// CLK            -> D5  (Same Pin for WEMOS)

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

int wait = 70; // In milliseconds

int spacer = 1;
int width  = 5 + spacer; // The font width is 5 pixels
// WiFi login parameters - network name and password
const char* ssid = "AZZAM";
const char* password = "azzamc230";

void setup() {
  pinMode(pinBuzz, OUTPUT);
  digitalWrite(D1, HIGH);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  configTime(0 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
  //setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02", 1);
  setenv("TZ", "WIB-7", 1);
  matrix.setIntensity(7); // Use a value between 0 and 15 for brightness
  matrix.setRotation(0, 1);    // The first display is position upside down
  matrix.setRotation(1, 1);    // The first display is position upside down
  matrix.setRotation(2, 1);    // The first display is position upside down
  matrix.setRotation(3, 1);    // The first display is position upside down
}

void loop() {
  matrix.fillScreen(LOW);
  String time = get_time();
  time.trim();
  Serial.println(time);
  //  time.substring(30, 37).toCharArray(time_value, 10);
  time.toCharArray(time_value, 12);
  Serial.println("HH:MM");
  Serial.println(time_value);
  //( Sun  21-07-19 ) ( PM 12:52:12 )
  //08:31 01/07
  matrix.drawChar(2, 0, time_value[0], HIGH, LOW, 1); // H
  matrix.drawChar(8, 0, time_value[1], HIGH, LOW, 1); // HH
  //  matrix.drawChar(14, 0, time_value[2], HIGH, LOW, 1); // HH:
  matrix.drawChar(20, 0, time_value[3], HIGH, LOW, 1); // HH:M
  matrix.drawChar(26, 0, time_value[4], HIGH, LOW, 1); // HH:MM
  //  matrix.write(); // Send bitmap to display
  //  delay(500);



  // buzz
  if (time_value[4] == '9') {
    if (timerIsOFF) {
      timerStart = millis();
      timerIsOFF = false;
      Serial.println("start timer");
    }

    unsigned long delayKeBuzz = 46 * 1000;
    Serial.println(delayKeBuzz);
    Serial.println(millis() - timerStart);

    if ((millis() - timerStart) > delayKeBuzz) {
      Serial.println("delayKeBuzz");
      
      
      if (belumbunyi) {
        Serial.println(">>>>>>>>>>>>>>> bunyii");
        digitalWrite(pinBuzz, LOW);
        delay(10);
        digitalWrite(pinBuzz, HIGH);
        belumbunyi = false;
      } 
      
    }


  } else {
    belumbunyi = true;
    timerIsOFF = true;
  }


  //for (int i = 0; i < 10; i++) {

    matrix.drawChar(14, 0, time_value[2], HIGH, LOW, 1);
    matrix.write(); // Send bitmap to display
    delay(500);

    matrix.drawChar(14, 0, time_value[5], HIGH, LOW, 1);
    matrix.write(); // Send bitmap to display
    delay(500);

  //}

  // SHOW DATE
  /*
  matrix.drawChar(2, 0, time_value[6], HIGH, LOW, 1);
  matrix.drawChar(8, 0, time_value[7], HIGH, LOW, 1);
  matrix.drawChar(14, 0, time_value[8], HIGH, LOW, 1);
  matrix.drawChar(20, 0, time_value[9], HIGH, LOW, 1);
  matrix.drawChar(26, 0, time_value[10], HIGH, LOW, 1);
  matrix.write(); // Send bitmap to display
  delay(2000);
  */


  //  display_message(time); // Display time in format 'Wed, Mar 01 16:03:20 2017
}

void display_message(String message) {
  for ( int i = 0 ; i < width * message.length() + matrix.width() - spacer; i++ ) {
    //matrix.fillScreen(LOW);
    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically
    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < message.length() ) {
        matrix.drawChar(x, y, message[letter], HIGH, LOW, 1); // HIGH LOW means foreground ON, background off, reverse to invert the image
      }
      letter--;
      x -= width;
    }
    matrix.write(); // Send bitmap to display
    delay(wait / 2);
  }
}

String get_time() {
  time_t now;
  time(&now);
  char time_output[buffer_size];
  // See http://www.cplusplus.com/reference/ctime/strftime/ for strftime functions
  // Desired format: ( Sun ,Jul 21 2019 )   ( AM  10:03:20 )
  // strftime(time_output, buffer_size, "( %a  %d-%m-%y ) ( %p %T )", localtime(&now));

  // Sunday, 21 July 2021
  //  strftime(time_output, buffer_size, "( %A  %d %B %Y ) ( %R %Z )", localtime(&now));
  strftime(time_output, buffer_size, "%R %d/%m", localtime(&now));
  return String(time_output); //      ( THURSDAY  01 JULY 20201 ) ( 08:31 WIB )
  //      ( wednesday  30 JUNY 20201 ) ( 08:31 WIB )
  //      01........1.........2.........3.........4
}
