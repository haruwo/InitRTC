#include <M5StickCPlus.h>
#include <WiFi.h>
#include <Unit_RTC.h>

#include "local_ssid_define.h"

Unit_RTC rtc;

void setup() {
  M5.begin(
      true, // LCD
      true, // Power
      false // Serial
  );

  // Lcd
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setRotation(3);
  M5.Lcd.printf("Initializing ...\n");

  // Power
  M5.Axp.begin();

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");

  // RTC
  rtc.begin();
}

void loop() {
  M5.update();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);

  auto now = time(NULL);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  // print now
  char buf[64];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
  M5.Lcd.printf(" %s\n", buf);

  if (now < 1000000000) {
    M5.Lcd.printf("Waiting for time sync ...\n");
    M5.Lcd.printf("WIFI: %s\n", WiFi.isConnected() ? "Connected" : "Disconnected");
    delay(1000);
    return;
  }

  // update RTC
  rtc_date_type rtc_date;
  rtc_time_type rtc_time;

  rtc_date.Date = timeinfo.tm_mday;
  rtc_date.Month = timeinfo.tm_mon + 1;
  rtc_date.Year = timeinfo.tm_year + 1900;
  rtc_date.WeekDay = timeinfo.tm_wday;

  rtc_time.Hours = timeinfo.tm_hour;
  rtc_time.Minutes = timeinfo.tm_min;
  rtc_time.Seconds = timeinfo.tm_sec;

  auto rc1 = rtc.setDate(&rtc_date);
  auto rc2 = rtc.setTime(&rtc_time);
  M5.Lcd.printf("setDate: %d\n", rc1);
  M5.Lcd.printf("setTime: %d\n", rc2); 
  delay(5000);
  M5.Axp.PowerOff();
}