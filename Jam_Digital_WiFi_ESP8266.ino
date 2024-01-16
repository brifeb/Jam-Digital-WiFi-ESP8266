#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <time.h>

int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;
const byte buffer_size = 45;
char time_value[buffer_size];

// LED Matrix Pin -> ESP8266 Pin
// Vcc            -> 3.3v
// Gnd            -> GND
// DIN            -> D7
// CS             -> D6/D4
// CLK            -> D5

int pinCS = D4;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

// WiFi login parameters - network name and password
const char *ssid = "WIFINAME";
const char *password = "WIFIPASS";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  configTime(0 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "WIB-7", 1);  // timezone WIB
  matrix.setIntensity(7);    // brightness (0 - 15)
  matrix.setRotation(0, 1);
  matrix.setRotation(1, 1);
  matrix.setRotation(2, 1);
  matrix.setRotation(3, 1);
}

void loop() {
  matrix.fillScreen(LOW);

  String time = get_time();
  time.trim();
  Serial.println(time);
  time.toCharArray(time_value, 12);
  Serial.println("HH:MM dd/mm");
  Serial.println(time_value);  
  // 08:31 01/07
  // 012345678910  // posisi buffer

  // Jam

  matrix.drawChar(2, 0, time_value[0], HIGH, LOW, 1);  // H
  matrix.drawChar(8, 0, time_value[1], HIGH, LOW, 1);  //  H

  matrix.drawChar(20, 0, time_value[3], HIGH, LOW, 1);  //    M
  matrix.drawChar(26, 0, time_value[4], HIGH, LOW, 1);  //     M

  // Detik

  for (int i = 0; i < 10; i++) {

    matrix.drawChar(14, 0, time_value[2], HIGH, LOW, 1);
    matrix.write();  // Send bitmap to display
    delay(500);

    matrix.drawChar(14, 0, time_value[5], HIGH, LOW, 1);
    matrix.write();  // Send bitmap to display
    delay(500);
  }

  // Tanggal

  matrix.drawChar(2, 0, time_value[6], HIGH, LOW, 1);
  matrix.drawChar(8, 0, time_value[7], HIGH, LOW, 1);
  matrix.drawChar(14, 0, time_value[8], HIGH, LOW, 1);
  matrix.drawChar(20, 0, time_value[9], HIGH, LOW, 1);
  matrix.drawChar(26, 0, time_value[10], HIGH, LOW, 1);
  matrix.write();  // Send bitmap to display
  delay(2000);
}

String get_time() {
  time_t now;
  time(&now);
  char time_output[buffer_size];

  // Lihat format waktu disini:
  // http://www.cplusplus.com/reference/ctime/strftime/

  // contoh format: ( Sun ,Jul 21 2019 )   ( AM  10:03:20 )
  // strftime(time_output, buffer_size, "( %a  %d-%m-%y ) ( %p %T )", localtime(&now));

  // format: 08:32 17/01
  strftime(time_output, buffer_size, "%R %d/%m", localtime(&now));

  return String(time_output);
}
