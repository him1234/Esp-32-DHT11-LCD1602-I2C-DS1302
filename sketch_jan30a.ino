#include <DS1302.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#define dht11pin 13
const char a[][45] = {"Happy Lunar New Year! ", "Wish You GoodLuck Today ", "*Happy New Year* ", "Happy Lunar New Year ", "Happy Spring Festival! ", "hope you find your pot of gold ", "May many fortunes find their way to you! ", "Have a thriving and happy new year ", "Wishing you many future successes. ", "Wish you Longevity and health! "};
LiquidCrystal_I2C lcd(0x27, 16, 2);  //i2c
DHT dht(13, DHT11); //dht11
WebServer server(80); //web
int i = 0;
int set = rand() % 2; //0-1随机数
int button = 0; //按钮初始
int sent = 0;
//char buf[20];
char date[11];//日期
char tim[10];//时间
char te[8];//TEMP
char hu[3];//HUM
String web;
namespace {

// Set the appropriate digital I/O pin connections. These are the pin
// assignments for the Arduino as well for as the DS1302 chip. See the DS1302
// datasheet:
//
//   http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
const int kCePin   = 18;  // Chip Enable
const int kIoPin   = 19;  // Input/Output
const int kSclkPin = 23;  // Serial Clock

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sun";
    case Time::kMonday: return "Mon";
    case Time::kTuesday: return "Tue";
    case Time::kWednesday: return "Wed";
    case Time::kThursday: return "Thu";
    case Time::kFriday: return "Fri";
    case Time::kSaturday: return "Sat";
  }
  return "(unknown day)";
}
void website() {
  String tes(te);
  String hus(hu);
  web = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <title>GUI-nephelo's toolchain</title>\n    <meta charset=\"utf-8\" />\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/mdui@1.0.2/dist/css/mdui.min.css\"/>\n\n    \n</head>\n<script src=\"https://cdn.jsdelivr.net/npm/mdui@1.0.2/dist/js/mdui.min.js\"></script>\n\n<body class=\"mdui-theme-primary-indigo mdui-theme-accent-pink mdui-theme-layout-auto  mdui-drawer-body-left mdui-drawer-body-left mdui-appbar-with-toolbar  mdui-theme-primary-amber\">\n    <header class=\"mdui-appbar mdui-appbar-fixed\">\n    \n        <div class=\"mdui-toolbar mdui-color-theme\">\n          <button  class=\"mdui-btn mdui-btn-icon mdui-ripple mdui-ripple-white\"  mdui-drawer=\"{target: '#main-drawer', swipe: true}\"><i  class=\"mdui-icon material-icons\">menu</i></button>\n          <a  class=\"mdui-typo-headline mdui-hidden-xs\">GUI-nephelo's toolchain</a>\n          <a href=\"index.html\" class=\"mdui-typo-title\" style=\"font-size: medium;\">工具链用户端入口</a>\n          <div class=\"mdui-toolbar-spacer\"></div>\n          <a onclick=\"beta()\" class=\"mdui-btn mdui-btn-icon\"><i class=\"mdui-icon material-icons\">search</i></a>\n          <a href=\"index.html\" class=\"mdui-btn mdui-btn-icon\"><i class=\"mdui-icon material-icons\">refresh</i></a>\n        </div>\n</header>\n\n      <div class=\"mdui-drawer  \" id=\"main-drawer\" >\n        <ul class=\"mdui-list\" mdui-collapse=\"{accordion: true}\" style=\"margin-bottom: 76px;\">\n            <li onclick=\"beta()\" class=\"mdui-list-item mdui-ripple\">\n              <i  class=\"mdui-list-item-avatar mdui-icon material-icons\">folder</i>\n              <div  class=\"mdui-list-item-content\">Photos</div>\n            </li>\n            <li onclick=\"beta()\" class=\"mdui-list-item mdui-ripple\">\n              <i class=\"mdui-list-item-avatar mdui-icon material-icons\" >folder</i>\n              <div class=\"mdui-list-item-content\">Recipes</div>\n            </li>\n            <li onclick=\"beta()\" class=\"mdui-list-item mdui-ripple\">\n              <i class=\"mdui-list-item-avatar mdui-icon material-icons\">assignment</i>\n              <div class=\"mdui-list-item-content\">Work</div>\n            </li>\n          </ul>\n          \n      </div>\n    <div align=\"center\" vertical-align=\"middle\" horizen-align=\"middle\" class=\"mdui-container \">\n            <h1></h1>\n            <br>\n            <h1></h1>\n            <h1></h1>\n            <h1></h1>\n            \n            <br>\n            <h1>TEMP:" + tes + "</h1>\n" + "<h1>HUM:" + hus + "</h1>\n            <br>\n            <br>\n            <br>\n            \n            <br>\n            <br>\n            <br>\n            <br>\n            <br>\n            <br>\n            <br>\n            <h1></h1>\n            <h1></h1>\n            <br>\n            \n            <br>\n            <h5>©2020-2021 版权所有</h5>\n            <h5>最后更新时间：2021/10/21 14：06</h5>\n            <br>\n            <br>\n            \n    </div>\n    <div class=\"sidemenu\" id=\"sidemenu\" hidden>\n        <div class=\"sidemenu-hero fade\">\n            \n            <span class=\"title\"></span>\n            <span class=\"text\"></span>\n        </div>\n        <ul class=\"menu\"> \n            <li ripple><a href=\"#\"><i class=\"icon-person-add\"></i>Current</a></li> \n        </ul>\n    </div>\n        <div class=\"debug\"></div>\n<canvas></canvas>\n\n\n<!-- Global site tag (gtag.js) - Google Analytics -->\n<script async src=\"https://www.googletagmanager.com/gtag/js?id=UA-180926318-1\"></script>\n\n<script>\n    function beta(){\n            mdui.alert(\"©2020-2021 版权所有 最后更新时间：2021/10/21 14：06 \")\n        }\n</script>\n\n\n    \n</body>\n</html>";
}
void indexHandler()
{
  server.send(200, "text/html", web);
}
void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.

  //  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d %s",
  //           t.yr, t.mon, t.date,
  //           t.hr, t.min, t.sec,
  //           day.c_str());
  snprintf(date, sizeof(date), "%04d-%02d-%02d", t.yr, t.mon, t.date);
  snprintf(tim, sizeof(tim), "%02d:%02d:%02d", t.hr, t.min, t.sec);


  // Print the formatted string to serial so we can see the time.
  //  Serial.println(buf);
  //lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(date);
  lcd.setCursor(2, 1);
  lcd.print(tim);
  lcd.print(" ");
  lcd.print(day.c_str());
}
}//namespace
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin("zhaolanbin", "MikeZhao888");
  WiFi.setAutoReconnect(true);
  //WiFi.setAutoConnect(true);
  pinMode(dht11pin, OUTPUT);
  lcd.init();
  lcd.backlight();
  dht.begin();
  pinMode(5, INPUT);
  wel();
  con();
  server.on("/", indexHandler);
  server.begin();
}
void loop() {
  website();
  server.handleClient();
  button = digitalRead(3);
  if (button == LOW) {
    lcd.clear();
    delay(300);
    sent ++;
    if (sent > 2) {
      sent = 0;
    }
  }
  if (sent == 0) {
    //lcd.clear();
    qwer();
  }
  if (sent == 1) {
    lcd.clear();
    show();
  }
  if (sent == 2) {
    printTime();
  }


}

void show() {
  int sum = rand() % 10;//随机数
  // put your main code here, to run repeatedly:
  Serial.print(a[sum]);
  Serial.print("\n");

  for (i = 0; i < (strlen(a[sum]) - 3) * 2; i++) {
    button = digitalRead(5);
    if (button == LOW) {
      delay(500);
      sent++;
      break;
    }
    Serial.print(button);
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    lcd.setCursor(3, 1 - set);
    if (set == 1) {
      lcd.print("TEMP:");
      lcd.print(temp);
    } else {
      lcd.print("HUM:");
      lcd.print(hum);
      lcd.print("%");
    }
    if (i >= 16) {
      lcd.setCursor(0, set);
    } else {
      //lcd.clear();
      lcd.setCursor(16 - i, set);
    }
    if (i <= 16) {
      for (int h = 0; h <= i; h++) {
        lcd.print(a[sum][h]);
        //delay(100);
      }
    }
    if (i > 16 && strlen(a[sum]) > 16) {
      lcd.print(a[sum] + i - 16);
      //delay(600);
    }
    if (i > 16 && strlen(a[sum]) <= 16) {
      lcd.print(a[sum]);
    }
    delay(600);
    //lcd.clear();
  }
  lcd.clear();
  if (set == 0) {
    set = 1;
  } else {
    set = 0;
  }
}

void qwer() {

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int tm = temp * 10;
  if (tm < 0) {
    tm = -tm;
  }
  snprintf(te, sizeof(te), "%2d.%1dC", (int)temp, tm % 10);
  snprintf(hu, sizeof(hu), "%02d", (int)hum);
  lcd.setCursor(3, 0);
  lcd.print("TEMP:");
  lcd.print(te);
  button = digitalRead(5);
  if (button == LOW) {
    delay(300);
    sent++;
  }
  lcd.setCursor(5, 1);
  lcd.print("HUM:");
  lcd.print(hu);
  lcd.print("%");
  delay(500);
  lcd.setCursor(7, 0);
  lcd.print(" ");
  lcd.setCursor(8, 1);
  lcd.print(" ");
  delay(500);

}

void wel() {
  lcd.setCursor(2, 0);
  lcd.print("Welcome-0213");
  lcd.setCursor(4, 1);
  lcd.print("Ver 3.1.0");
  delay(3000);
  lcd.clear();
}
void con() {
  Serial.print("是否连接:");
  Serial.println(WiFi.isConnected());
  Serial.print("本地IP:");
  Serial.println(WiFi.localIP());
  Serial.print("本地IPv6:");
  Serial.println(WiFi.localIPv6());
  Serial.print("mac地址:");
  Serial.println(WiFi.macAddress());
  Serial.print("网络ID:");
  Serial.println(WiFi.networkID());
  Serial.print("休息:");
  Serial.println(WiFi.getSleep());
  Serial.print("获取状态吗:");
  Serial.println(WiFi.getStatusBits());
  Serial.print("getTxPower:");
  Serial.println(WiFi.getTxPower());
  Serial.print("是否自动连接:");
  Serial.println(WiFi.getAutoConnect());
  Serial.print("是否自动重连:");
  Serial.println(WiFi.getAutoReconnect());
  Serial.print("获取模式:");
  Serial.println(WiFi.getMode());
  Serial.print("获取主机名:");
  Serial.println(WiFi.getHostname());
  Serial.print("获取网关IP:");
  Serial.println(WiFi.gatewayIP());
  Serial.print("dnsIP:");
  Serial.println(WiFi.dnsIP());
  Serial.print("状态:");
  Serial.println(WiFi.status());
  delay(1000);
}
