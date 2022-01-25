/*
 * ================ Simple Wifi Switch Simple, taki wpinany w siec 2xIN, 2xOUT ====================
 * 
 * Poprawnie dzialajacy
 * - ustawiony tak żeby się sam wyłączał po wznowieniu zasilania
 * - guzik na przełączniku służy teraz do włącz/wyłącz
 *
 * Generic ESP8285 Module
 * CPU 80MHz
 * 1M 512k SPIFFS
 * Upload 115200kbs
 *
 * Programator AVRISP mkII
 * 
 * Kwadratowy pin +3,3V
 * Reszta pinów kolejno jak w PL2302
 * Przed podłączeniem USB PL 2302 do komputera należy ustawić 0 (zero) na pinie 0 GPIO0
 * GPIO0 - standardowo jest to drugie wyjście od dołu z prawej strony z układu PSF-A 
 * Po podłączeniu do komputera po sekundzie wystarczy odłączyć masę (zero) od GPIO0
 * i można ładować program
 * 
 * 
 * Autor:
 * Marek Urbański
 * marek@www2.pl
 * 
 *
*/


#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ICMPPing.h>



String nazwa_urzadzenia = "Włącznik światła SmartHome";
String wersja_urzadzenia = "v1.5";
String ssid     = "SSID"; // SSID do sieci WiFi, pozniej bedzie pobrane z pamieci
String password = "password"; // Haslo do sieci WiFi, pozniej bedzie pobrane z pamieci
String HostName; // ustawiane w getSerial();
String DefaultSSID; // ustawiane w getSerial();
String DefaultON = "N"; //czy po wlaczeniu zasilania ma sie lampka włączac czy nie
#define PinSwitch 0 // pin ktory klikamy
#define PinLight 12 // Pon do ktorego podlaczona jest lampka/przekaznik
#define LED 13 // wbudowany LED
String info_czas_trzymania_do_restartu = "8";

SOCKET pingSocket = 0;
char buffer [256];
ICMPPing ping(pingSocket, (uint16_t)random(0, 255));




const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
String IPString = "172.217.28.1";
DNSServer dnsServer;
ESP8266WebServer webServer(80);
bool clientApConnected = false;
String SN;
String is_configured = "NO"; //to zmienna ktora pokazuje czy byl juz skonfigurowany czy ustawiamy HotSpot
int pressed_reset_count = 0;
String server_address = ""; // adres serwera na ktory bedzie wysylana zmiana stanu
String allowed_ip_request = ""; // tylko ze tego adresu ip beda odbierane polaczenia


WiFiServer server(8080);


/*
 * ###############################################################################################################################
 * Inicjalizacja
 * 
 * ###############################################################################################################################
*/
void setup() {

  InitializeESP(1);
  
}


/*
 * ###############################################################################################################################
 * Standardowa pętla
 * 
 * ###############################################################################################################################
 */
void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  getGetRequest();
  // true bo to sonoff
  if(changeCheckSwitchState(1, 600)) 
      {
        String url;        
        
        if(setPinStatus(-1, PinLight) == "HIGH")
          {
          Serial.println("CLICK = ON");             
          url = server_address+"?act=stateChange&SN="+SN+"&state=1";
          }
        else
          {
          url = server_address+"?act=stateChange&SN="+SN+"&state=0";
          Serial.println("CLICK = OFF");    
          }
          
        
        HTTPClient http;
        http.begin(url);
        int httpCode = http.GET();
        String payload = http.getString(); 
        Serial.println("url="+url);
        Serial.println("httpCode="+httpCode);
        Serial.println("payload="+payload);
        http.end();
      }
}


/*
 * ###############################################################################################################################
 * Sprawdzanie zapytania
 * @TODO
 * 
 * ###############################################################################################################################
 */
String checkRequest(String request)
    {

          
    }
