#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <DHT.h>
#include <ESP8266WiFi.h>                                                // esp8266 library
#include <FirebaseArduino.h>                                             // firebase library


#define DHTPIN D8
#define DHTTYPE DHT22

#define FIREBASE_HOST "bwell-e70a9.firebaseio.com"                         // the project name address from firebase id
#define FIREBASE_AUTH "OGpGRNSrmcqTbdEbOLql7wTKBNx9BeUUXhOv17OA"                    // the secret key generated from firebase
#define WIFI_SSID "iSpace Event"                                          // input your home or public wifi name 
#define WIFI_PASSWORD "ipsmart2019"                                    //password of wifi ssid


#define green D7
#define yellow D10

#define red D5


String readings;
String fireStatus = "";
char msg[50];

#define buzzer D9

DHT dht(DHTPIN, DHTTYPE);

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

int chk;
float hum;
float temp;

int i;

void setup(void) 
{
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);      
  //pinMode(led, OUTPUT);                 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                      //try to connect with wifi
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                                      //print local IP address
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);                                       // connect to firebase
  //Firebase.setString("AZWADO", "OFF");     
  
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  
  ads.begin();
  dht.begin();
  pinMode(buzzer, OUTPUT);

  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
}

void buzzing()
{
  for(i=0; i<=3; i++)
  {
    Firebase.setString("AZWADO", readings); 
    tone(buzzer, 500); // Send 1KHz sound signal...
    delay(2000);        // ...for 1 sec
    noTone(buzzer);     // Stop sound...
    delay(1000);        // ...for 1sec
    
  }

  
   
  
}

void warning(int16_t mon, int16_t meth, int16_t butan)
{

  if(mon> 25)
  {
    digitalWrite(red,HIGH);
    digitalWrite(green,LOW);
    digitalWrite(yellow,LOW);

    buzzing();

  }
  
  else if(meth> 8 || mon > 20 || butan> 4)
  {
    digitalWrite(red,LOW);
    digitalWrite(green,LOW);
    digitalWrite(yellow,HIGH);
  }


  else
  {
    digitalWrite(red,LOW);
    digitalWrite(green,HIGH);
    digitalWrite(yellow,LOW);
  }
  

  //Serial.println(mon);


   
}



void loop(void) 
{
  int16_t adc0, adc1, adc2;

  
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  
  Serial.print("AIN0: "); Serial.println(adc0);
  Serial.print("AIN1: "); Serial.println(adc1);
  Serial.print("AIN2: "); Serial.println(adc2);
  Serial.println(" ");

  //Read data and store it to variables hum and temp
   hum = dht.readHumidity();
   temp= dht.readTemperature();
   //Print temp and humidity values to serial monitor
   Serial.print("Humidity: ");
   Serial.print(hum);
   readings.concat(hum);
   readings.concat(",");
   Serial.print(" %, Temp: ");
   Serial.print(temp);
   readings.concat(temp);
   Serial.println(" Celsius");

   adc1 = map(adc1, 715, 22875, 4, 16.4);
   adc0 = map(adc0, 1980, 30245, 12, 75);
   adc2 = map(adc2,1380, 21814,1.80, 8.44); 

   //warning(adc0, adc1, adc2);


   readings = adc1;
   readings.concat(",");
   readings.concat(adc0);
   readings.concat(",");
   readings.concat(adc2);
   readings.concat(",");
   readings.concat(hum);
   readings.concat(",");
   readings.concat(temp);

   Firebase.setString("AZWADO", readings); 

   warning(adc0, adc1, adc2);

   delay(900);

   
   

   

   //buzzing();
}
