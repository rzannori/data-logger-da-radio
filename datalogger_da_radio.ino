/*PIN out
 * GPS : RX=D6, TX = D7 

 * SD : MISO = D9,MOSI=D10,SCK=D8, CS= D3

 */

#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include<SD.h>
#include <ServoInput.h>


ServoInputPin<2> ch;
const float threshold = 0.7; // 70%
const int cs_sd=3; // pin di CS della SD
const int TState = 200;
static const int RXPin = 6, TXPin = 7; //comunicazione GPS
static const uint32_t GPSBaud = 9600;
int ledPin; //11 rosso , 12 blu,13 verde
int DLState;

int DatiVoloArray[] ={0,0,0}; // array dove registro i valori massimi di: 1 vmax, 2 hmax, 3 satelliti
int maxspeed, maxalt, maxsatellite;
int SatH, SpeedH, AltH;

TinyGPSPlus gps;// oggetto TinyGPSPlus 
SoftwareSerial ss(RXPin, TXPin);// connessione seriale al GPS

//------------------------------------------------------------------------------
void setup() {
 
 Serial.begin(115200);
 ss.begin(GPSBaud);
 while ( !SD.begin(cs_sd)){
       }
  File data = SD.open("DatiVolo.txt",FILE_WRITE);              // crea cartella dati volo nella sd
  data.println("");
  data.println("Dati Registrati");    
  data.close(); 
 pinMode(ledPin, OUTPUT);
 ch.attach();
  }
//------------------------------------------------------------------------------
void loop() {
  Serial.begin(9600); 

  //costruisco e aggiorno array con valori massimi
 int speed1 = (gps.speed.kmph());
  if ( speed1 > maxspeed) {
    maxspeed = speed1;
    DatiVoloArray[1] = maxspeed;
    }
    SpeedH = DatiVoloArray[1];
      
 int alt1 = (gps.altitude.meters());
  if ( alt1 > maxalt) {
    maxalt=alt1;
    DatiVoloArray[2]= maxalt;
  }
  AltH=DatiVoloArray[2];

 int satellite = (gps.satellites.value());
 if ( satellite > maxsatellite) {
     maxsatellite = satellite;
     DatiVoloArray[3] = maxsatellite;
    } 
    SatH=DatiVoloArray[3];
    
    
if (satellite > 0){ 
   
    String Time=String(gps.time.hour()+1)+(":")+(gps.time.minute())+(":")+(gps.time.second());
    String Date=String(gps.date.day())+("/")+(gps.date.month())+("/")+(gps.date.year());
   
    boolean state = ch.getBoolean();
    
    if (state == true) {
          
          DLState=13;//scrivo
          }
           else  {
          DLState=12;// pronto
          }
         
          ledPin=DLState;  
    switch (DLState) { 
        case 13:  { 
              Serial.println ("writing");
              File data=SD.open("DatiVolo",FILE_WRITE);
              data.println(Date + " , " + Time + " , " + String(gps.location.lat(), 6)+ " , " + String(gps.location.lng(), 6)+ " , " + String(gps.altitude.meters(),0)+ " , " + String(gps.speed.kmph(),0) + " , " + String(gps.satellites.value())); 
              data.close();
              digitalWrite(ledPin, LOW);   // turn the LED on (HIGH is the voltage level)
              delay(TState);                       // wait for a second
              digitalWrite(ledPin, HIGH);    // turn the LED off by making the voltage LOW
              delay(TState); 
          }
         break;
        case 12:  { 
               Serial.println ("waiting");
               digitalWrite(ledPin,LOW );   // turn the LED on (HIGH is the voltage level)
               delay(TState);                       // wait for a second
               digitalWrite(ledPin, HIGH);    // turn the LED off by making the voltage LOW
               delay(TState);
           }
         break;
      /*  default:{
              Serial.println ("looking");
              DLState=11;
              digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
              delay(TState);                       // wait for a second
              digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
              delay(TState);
             }
          break;*/
          }
      }
      else {
        ledPin=11;
        Serial.println ("looking");
        digitalWrite(ledPin, LOW);   // turn the LED on (HIGH is the voltage level)
        delay(TState); 
        digitalWrite(ledPin,HIGH );    // turn the LED off by making the voltage LOW
        delay(TState);
      }
 
  DelayGPS(500);
   
  }
 
 //==========================================sub routine=====================================
  static void DelayGPS(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
