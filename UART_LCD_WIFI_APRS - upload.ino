/***************************************************
 DFRobot Gravity: HCHO Sensor
 <https://www.dfrobot.com/wiki/index.php/Gravity:_HCHO_Sensor_SKU:_SEN0231>
 
 ***************************************************
 This example reads the concentration of HCHO in air by UART mode.
 
 Created 2016-12-15
 By Jason <jason.ling@dfrobot.com@dfrobot.com>
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/
 
 /***********Notice and Trouble shooting***************
 1. This code is tested on Arduino Uno with Arduino IDE 1.0.5 r2.
 2. In order to protect the sensor, do not touch the white sensor film on the sensor module, 
 and high concentration of Hydrogen sulfide, hydrogen, methanol, ethanol, carbon monoxide should be avoided. 
 3. Please do not use the modules in systems which related to human being’s safety.
 ****************************************************/
#define BLINKER_WIFI
#include "DHTesp.h"
#include <Blinker.h>
#include <DFRobotHCHOSensor.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
// FOR APRS
#define CALLSIGN "BG5DDX-11"
#define PASSCODE "16569" // GENERATE IT ON apps.magicbug.co.uk/passcode/
#define LATITUDE "3044.04N" // LOOK !! -> 4 DIGITS POINT 2 DIGITS
#define LONGITUDE "12043.55E" // LOOK !! -> 5 DIGITS POINT 2 DIGITS
#define SERVER_NAME "china.aprs2.net"
#define PORT 14580
#define SensorSerialPin	12	//this pin read the uart signal from the HCHO sensor

SoftwareSerial sensorSerial(SensorSerialPin,SensorSerialPin);

DFRobotHCHOSensor hchoSensor(&sensorSerial);
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//D2-SDA D1-Scl
   DHTesp dht; 
#define DHTTYPE DHT11   // DHT 11
#define Aout A0 
int temp=0; //读取A0

float sinVal;            
int toneVal;



ESP8266WiFiMulti wifiMulti;
void sendAPRS(){
    WiFiClient client;
    int retr=20;
    while(!client.connect(SERVER_NAME, PORT) && retr>0){
      delay(50);
      --retr;
    }
    if(client.connected()){ // CONNECTED TO THE HOST
      Serial.println("APRS server connected !");
      /*********************************************** 
       *            APRS AUTHENTICATION              *
       ***********************************************/
      client.print("user ");
      client.print(CALLSIGN);
      client.print(" pass ");
      client.print(PASSCODE);
      client.print(" vers ArduinoWX 1.0 UDP 8888\n");
      /*********************************************** 
       *               SEND APRS DATA                *
       ***********************************************/
      Serial.println("Sending...");
      client.print(CALLSIGN);
      client.print(">APRS,TCPIP*:=");
      client.print(LATITUDE);
      client.print("/");
      client.print(LONGITUDE);
      client.print("温度");
      client.print((int)dht.getTemperature());
      client.print("相对湿度");
      client.print((int)dht.getHumidity());
      client.print("%/HCHO");
      client.print(hchoSensor.uartReadPPM());
            client.print("PPM");
      client.print("\r\n");
     /*********************************************** 
      *              CLOSE CONNECTION               *
      ***********************************************/
      Serial.println("Done !");
      client.stop();
      
    }else{ // CONNECTION FAILURE
      Serial.println("Can't connect to the server (APRS)");
      client.stop();
    }
}
/*****************************************************/



void setup()
{  
    sensorSerial.begin(9600);	//the baudrate of HCHO is 9600
    sensorSerial.listen();
    Serial.begin(9600);
  dht.setup(2, DHTesp::DHT11); // Connect DHT sensor to GPIO 
   // dht.begin();
    lcd.begin(16,2);                      // initialize the lcd 
  lcd.init();
    // Print a message to the LCD.
  lcd.backlight();
 pinMode(15, OUTPUT);
pinMode(Aout,INPUT);
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("D-Link_DIR-600M-Koalawang573", "12345wangkai");
  wifiMulti.addAP("902", "12345678");
  wifiMulti.addAP("lmn", "zll54836837");

  Serial.println("Connecting Wifi...");
  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void loop()
{
 if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    delay(1000);
  }
    if(hchoSensor.available()>0)	
    {
      Serial.print(hchoSensor.uartReadPPM());
      Serial.println("ppm");
     float HCHO = (hchoSensor.uartReadPPM());
       lcd.noBlink();
   lcd.setCursor(0,0);
    lcd.print("HCHO:");
//    lcd.print(HCHO);
 //   lcd.print("ppm");

 //    lcd.setCursor(5,1);
     lcd.print(HCHO*1.34);//  Unit conversion ：1ppm = 1.34 mg/m3  
     lcd.print("mg/m3");
  if(HCHO<0.06)
     {
      lcd.setCursor(4,1);
    lcd.print("SAFE");
    noTone(15);
     }
     else if(HCHO<0.2)
    { 
      lcd.setCursor(4,1);
      lcd.print("WARNING");
            for(int x=0; x<180; x++){
                        //将sin函数角度转化为弧度
                        sinVal = (sin(x*(3.1412/180)));
                       //用sin函数值产生声音的频率
                       toneVal = 2000+(int(sinVal*1000));
                      //给引脚8一个
                       tone(15, toneVal);
                       delay(2);
}
    }
    else if(HCHO<4)
    {
             lcd.setCursor(4,1);
      lcd.print("DANGER");
 
      blink_lcd();
            for(int x=0; x<180; x++){
                        //将sin函数角度转化为弧度
                        sinVal = (sin(x*(3.1412/180)));
                       //用sin函数值产生声音的频率
                       toneVal = 2000+(int(sinVal*1000));
                      //给引脚8一个
                       tone(15, toneVal);
                       delay(2);
    }
    }
    
      delay(5000);
    lcd.clear();
       float humidity = dht.getHumidity();
        float temperature = dht.getTemperature();
      if (isnan(humidity) || isnan(temperature)) {
        Serial.print("Failed to read from DHT sensor!");
        return;
    }
     delay(dht.getMinimumSamplingPeriod());
    lcd.setCursor(0,0);
    lcd.print("Humi:");
    lcd.print(humidity, 1);
    lcd.print("%");
    lcd.setCursor(0,1);
    lcd.print("Temp:");
    lcd.print(temperature, 1);
    lcd.print("C");
    delay(5000);
    lcd.clear();
     Serial.print(humidity);
      Serial.println(temperature);
    
    
 
//------------------------------------MQ0----//
 temp = analogRead(Aout); //读取A0的模拟数据  

 lcd.print(temp);
  if (temp>70)
    {

      lcd.setCursor(4,1);
      lcd.print("Fire WARNING");
            for(int x=0; x<180; x++){
                        //将sin函数角度转化为弧度
                        sinVal = (sin(x*(3.1412/180)));
                       //用sin函数值产生声音的频率
                       toneVal = 2000+(int(sinVal*1000));
                      //给引脚8一个
                       tone(15, toneVal);
                       delay(2);
    }
    }
   else{
    lcd.setCursor(4,1);
    lcd.print("Fire SAFE");
    noTone(15);
    }
  Serial.println(temp); //串口输出temp的数据  


    delay(1000);
    }
     sendAPRS();

}


void blink_lcd()
  {

  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  //add blink       //add blink     
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
   lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
     lcd.noDisplay();
  delay(100);
   // Turn on the display:
  lcd.display();
  delay(100);
  



  
    }
