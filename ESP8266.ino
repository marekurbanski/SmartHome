/*
 * Biblioteka ESP8266
 * 
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
*


 *//*
 * Nagłówek HTMLL
 * używany do wyświatlania każdej strony html
 */
String HTML_header =  "<!DOCTYPE html><html lang='pl'>"
                      "<head>"
                      "<meta charset='utf-8' />"
                      "<meta name='viewport' content='width=device-width'>"
                      "<title>SmartHome</title>"
                      "</head>"
                      "<body>"
                      "<style>"
                      " body {"
                      "     font-family: arial;"
                      "     font-size:16px"
                      "     }"
                      " input.password, select, input.text {"
                      "     width:200px;"
                      "     padding:5px;"
                      "     }"
                      " input.submit {"
                      "     border:1px solid black;"
                      "     padding:10px; "
                      "     background-color: gray;"
                      "     color: white;"
                      "}"
                      " h1 {"
                      "     background-color: black;"
                      "     color: white;"
                      "     padding:30px;"
                      "}"
                      " input {"
                      "     border-radius: 5px;"
                      "     border:1px solid black;"    
                      "     font-size:16px;"                  
                      "}"
                      " table td {"
                      "     padding: 5px 18px 10px 0px;"
                      "}"       
                      "#t1 td {"
                      "  padding: 8px;"
                      "  font-size: 13px;"
                      "}"
                      "</style>"
                      "<h1>Witaj w SmartHome</h1>"
                      ""
                      "<br>";
                      
String responseHTML = "";
String responseHTML_SAVED = "<br>Urządzenie zrestartuje sie automatycznie za 10 sekund<br>"
                            "Jeśli nie uda się połączyc do sieci, ustawienia będą zrestartowane<br>";

                            
/*
 * ###############################################################################################################################
 * Softwarowy restart urządzenia
 * 
 * ###############################################################################################################################
 */
void(* resetFunc) (void) = 0; //declare reset function @ address 0





/*
 * ###############################################################################################################################
 * Zmiana statusu pina
 * 
 * ###############################################################################################################################
 */
String setPinStatus(int state, int LED_NO)
    {           
      if(state == -1)
        { 
        if(digitalRead(LED_NO) == HIGH)
            {
               // przekaznik
               digitalWrite(LED_NO, LOW); 
               // wbudowany LED
               digitalWrite(LED, LOW); //bylo high
               return "LOW";
            }
          else
            {
              // przekaznik
              digitalWrite(LED_NO, HIGH);
              // wbudowany LED
              digitalWrite(LED, HIGH); //bylo LOW
              return "HIGH";
            }
        }
     else
        {
          if(state == 1)
              {
                // przekaznik
                digitalWrite(LED_NO, HIGH);
                // wbudowany LED
                digitalWrite(LED, HIGH); // bylo LOW
                return "HIGH";
              }
          if(state == 0)
              {
                // przekaznik
                digitalWrite(LED_NO, LOW);
                // wbudowany LED
                digitalWrite(LED, LOW); //bylo HIGH
                return "LOW";
              }
        }
    }




/*
 * ###############################################################################################################################
 * funkcja zamieniająca adres IP na Strings
 * 
 * ###############################################################################################################################
 */
String ip2Str(IPAddress ip)
    {
    String s="";
    for (int i=0; i<4; i++) 
      {
      s += i  ? "." + String(ip[i]) : String(ip[i]);
      }
    return s;
    }


/*
 * ###############################################################################################################################
 * Metoda sygnalizująca diodą przed
 * zerowaniem urządzenia
 * 
 * ###############################################################################################################################
 */
void BlinkForReset()
    {
    setPinStatus(LOW, LED);      
    delay(250);
    for(int b=0; b<20; b++)
        {
        setPinStatus(HIGH, LED);      
        delay(100);    
        setPinStatus(LOW, LED);      
        delay(100);
        }
    }


/*
 * ###############################################################################################################################
 * Metoda sygnalizująca diodą przed
 * restartem urządzenia
 * 
 * ###############################################################################################################################
 */
void BlinkForReboot()
    {
    setPinStatus(LOW, LED);      
    delay(250);
    for(int b=0; b<10; b++)
        {
        setPinStatus(HIGH, LED);      
        delay(100);    
        setPinStatus(LOW, LED);      
        delay(100);
        }
    }


/*
 * ###############################################################################################################################
 * Resetowanie przełącznika do stanu początkowego
 * Kasowanie wszystkich danych z pamięci
 * 
 * ###############################################################################################################################
 */
void ResetToDefaults()
    {      
    saveToEeprom("NO", 1); // czy podlazony do sieci wifi
    saveToEeprom("SSID", 30); // ssid
    saveToEeprom("password", 200); // haslo do ssid
    saveToEeprom(" ", 150); // allowed IP      
    saveToEeprom(" ", 300); // resver url for request on action
    saveToEeprom("N", 450); // czy skonfigurowany
    }


/*
 * ###############################################################################################################################
 * Inicjalizacja - rozruch przełącznika
 * 
 * ###############################################################################################################################
 */
void InitializeESP(int type)
    {
    Serial.begin(115200);
    delay(10);    
    pinMode(PinLight, OUTPUT);
    pinMode(LED, OUTPUT);
    pinMode(PinSwitch, INPUT);
    
    SN = getSerial();
    Serial.println("SN = "+SN); 
    /*
     * sprawdzanie czy nie trzeba usunac defaultowego SSID i hasla
     * Jeśli pin PinSwitch = HIGH to resetujemy SSID i haslo
    */
               
    is_configured = readFromEeprom(1);
    if((is_configured == "YES") || (is_configured == "TEST"))
      {        
      ssid     = readFromEeprom(30);
      password = readFromEeprom(200);
      allowed_ip_request = readFromEeprom(150);
      allowed_ip_request.trim();
      server_address = readFromEeprom(300);
      DefaultON = readFromEeprom(450);
      server_address.trim();
      Serial.println("");  
      Serial.println("is_configured = "+String(is_configured));  
      Serial.println("Mam zapisane SSIS do WiFi "+String(ssid));       
      Serial.println("Mam zapisane Haslo do WiFi "+String(password));       
      }
    else
      {
      Serial.println("Nie mam zapisanych danych do WiFi");         
      }

   // czy wlaczamy na start lampke czy nie
   if( DefaultON == "Y")
        {
        setPinStatus(1, PinLight);
        }
   
    if (connectToWiFi() == true) 
        {
        Serial.println("OK :) Polaczylem sie do sieci "+String(ssid));       
        }
    else
        {
        Serial.println("Nie udalo sie polaczyc do sieci Wifi:"+String(ssid));  
        Serial.println("is_configured = "+String(is_configured));  
        is_configured = readFromEeprom(1);
        Serial.println("is_configured (jeszcze raz pobrane) = "+String(is_configured));  
        if(is_configured == "YES")
            {
            resetFunc();  
            }
        else
            {
            Serial.println("Nie polaczylem sie do "+String(ssid)+", ustawię serwer AP");  
    
            responseHTML = "To jest przewonik podłączania urządzenia do systemu SmartHome<br>"
                          "Wybierz jedną sieć do której chcesz podłączyć SmartHome i wpisz hasło dostępowe<br>"
                          "Po kliknięciu 'Zapisz dane', urządzenie zostanie zrestartowane i uruchomione z nowymi ustawieniami<br>"
                          "W przypadku gdy urządzenie nie pojawi się w centralce SmartHome, przejdzie automatycznie w tryb nadawania i znowu będzie dostępna sieć WiFi: 'SmartHome'<br>"
                          "<form action='http://"+IPString+"/' method='POST' name='aktualizuj'>"
                          "<table cellpadding=10 cellspacing=10 border=0>"
                          "<tr><td>Wybierz sieć:</td><td>"+scanNetworkSSIDs()+"</td></tr>"
                          "<tr><td>Podaj hasło:</td><td><input class='password' type='password' name='haslo'></td></tr>"
                          "<tr><td>Nazwę urządzenia:</td><td>"+nazwa_urzadzenia+"</td></tr>"
                          "<tr><td colspan='2'><input type='submit' class='submit' value='Zapisz dane i zrestartuj urządzenie'></td></tr>"
                          "</table>"
                          ""
                          ;
    
                          
            createHotSpot();
            }
        }           
    }


/*
 * ###############################################################################################################################
 * Sprawdzanie czy string jest znakiem
 * alfanumerycznym
 * 
 * ###############################################################################################################################
 */
boolean isAlphaNumeric(String hash)
    {
    int len = hash.length();
    for (int i=0; i<len; i++)
        {
        if (isAlphaNumeric(hash[i]))
            {
            Serial.println("Sprawdzam znak [poprawny]: "+hash[i]);  
            }
        else    
            {
            Serial.println("Sprawdzam znak [bledny]: "+hash[i]);    
            return false;
            }
        }
    return true;
    }



/*
 * ###############################################################################################################################
 * getSerial()
 * Generuje serial dla urządzenia
 * Zapisuje w pamięci i od tej pory używa już tylko tego jednego
 * 
 * ###############################################################################################################################
 */

String getSerial()
  {
    String serial;
    serial = readFromEeprom(100);
    serial.trim();
    Serial.println("Serial odczytany: "+serial);
       
    if((serial.length() != 7) || (isAlphaNumeric(serial) == false))
        {   
        ResetToDefaults();

        serial = "";         
        Serial.println("Generuje nowy serial");   
        char *letters = "ABCDEFG34345HJKLMNPR67STUWXY489";
        for(int i = 0; i<7; i++)
          {
           serial = serial + letters[random(0, 31)];
          }
        saveToEeprom(serial, 100);    
        }
    SN = serial;    
    HostName = "SmartHome-"+SN;
    DefaultSSID = "SmartHome ("+SN+")";
    nazwa_urzadzenia = nazwa_urzadzenia + "("+SN+")";
    
    return serial;
  }


/*
 * ###############################################################################################################################
 * changeCheckSwitchState()
 * Sprawdza zmianę statusu wciśniętego guzika - używane w Sonoffach i ESP8266
 * Jeśli jest zmiana stanu to zwraca true, jeśli jest powyżej 1000 to restartuje urządzenie
 * i ustawia defaultowe SSID i password
 * 
 * ###############################################################################################################################
 */
bool changeCheckSwitchState(int type, int count_to_reset)
    {
      bool reboot = false;
      int state = LOW;
      if(type == 1) // to jest myk na sonoff
          {
            state = LOW; 
            pinMode(PinSwitch, INPUT);
          }
      if(type == 2)
          {
            //pinMode(PinSwitch, OUTPUT);
            state = HIGH; 
          }
      
      
      if(digitalRead(PinSwitch) == state)
          {
          Serial.println("PRESSED = "+String(pressed_reset_count));  
          pressed_reset_count++;
          if(pressed_reset_count > count_to_reset/2)
              {
              BlinkForReboot();
              reboot = true;
              }
          if(pressed_reset_count > count_to_reset)
              {
              pressed_reset_count = 0;
              Serial.println("RESET TO DEFAULTS");  
                                
              ResetToDefaults();
              BlinkForReset();

              pinMode(PinSwitch, OUTPUT);    
              digitalWrite(PinSwitch, LOW);
              delay(20);
              resetFunc();
              }
          delay(14); 
          if(reboot)
              {
              resetFunc();  
              }
          }
        else
          {
              if(pressed_reset_count > 0)
                  {
                    pressed_reset_count = 0;
                    return true;
                  }
              pressed_reset_count = 0;      
          }
        return false;
    }



/*
 * ###############################################################################################################################
 * scanNetworkSSIDs
 * Skanuje okoliczne sieci, jak nie może się połączyć
 * 
 * ###############################################################################################################################
 */
String scanNetworkSSIDs()
  {
    String res = "<select name='ssid' id='ssid'>";
    
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++)
      {
      Serial.println(WiFi.SSID(i));
      res = res + "<option value='"+WiFi.SSID(i)+"'>"+WiFi.SSID(i)+" ("+WiFi.RSSI(i)+" dB) </option>";
      }

    res = res + "</select>";
    return res;
  }  




/*
 * ###############################################################################################################################
 * getGetRequest()
 * Pobieranie zapytan www - dziala w ustawieniach jako klient juz podlączony do SmartHome
 * zwraca stringa z $_GET - dziala w LOOPie
 * 
 * ###############################################################################################################################
*/
String getGetRequest()
    {
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) 
        {
        return "";
        }
 
    // Wait until the client sends some data
    Serial.println("new client");
    int timewate = 0;
    while(!client.available())
        {
        delay(1);
        timewate = timewate +1;
        if(timewate>1800)
            {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return "";
            }
        }

    
    Serial.println("IP klienta: "+ip2Str(client.remoteIP()));
 
    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    String msg = "<span style='font-size:14px; font-family:arial'>";
    msg = msg + "<h3>Witaj w SmartHome</h3><br>";
    msg = msg + "<br>";
    msg = msg + "<table id='t1' border='1' cellspacing='0' cellpadding='0'>";
    msg = msg + "<tr><td colspan='2' style='background-color:black; color: white'>Dane urządzenia:</td></tr>";
    msg = msg + "<tr><td>Model: </td><td>"+nazwa_urzadzenia+"</td></tr>";
    msg = msg + "<tr><td>Wersja: </td><td>"+wersja_urzadzenia+"</td></tr>";
    msg = msg + "<tr><td>Nazwa: </td><td>"+HostName+"</td></tr>";
    msg = msg + "<tr><td>Serial: </td><td>"+SN+"</td></tr>";
    msg = msg + "<tr><td>Odbieraj z IP: </td><td>"+allowed_ip_request+"</td></tr>";
    msg = msg + "<tr><td>Adres serwera: </td><td>"+server_address+"</td></tr>";
    
    msg = msg + "<tr><td colspan='2' style='background-color:black; color: white'>Dostepne metody:</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=ON</td><td>Włączenie przelacznika</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=OFF</td><td>Wylaczenie przelacznika</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=SN</td><td>Wyświetla Numer Seryjny urządzenia SN, w tym przypadku: "+SN+"</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=STATE</td><td>Podaje aktualny status przelacznika</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=PARAMS</td><td>Podaje aktualne parametry przelacznika</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=RESTART</td><td>Restartuje urządzenie</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=SET_DEFAULT_ON</td><td>Ustawia urządzenie jako włączone po podłączeniu zasilania</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=SET_DEFAULT_OFF</td><td>Ustawia urządzenie jako wyłączone po podłączeniu zasilania</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=SRVER_ADDR:http://192.168.0.1/smart-home/</td><td>Ustawia adres serwera na który będzie wysyłana informacja o zmianie stanu [0/1] (w tym przypadku po wlaczeniu bedzie wyslany GET: http://192.168.0.1/smart-home/?act=stateChange&SN="+SN+"&state=1)</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=SETUP_IP:192.168.0.1</td><td>Ustawia odbieranie polecen tylko z tego adresu IP (w tym przypadku 192.168.0.1), puste bedzie odbierac z kazdego klienta</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=SET_SSID:nazwa_sieci_wifi</td><td>Ustawia nową sieć wifi do które będzie się łączyć (w tym przypadku 'nazwa_sieci_wifi') - nie restartuje urządzenia</td></tr>";
    msg = msg + "<tr><td>http://"+ip2Str(WiFi.localIP())+":8080/=SET_PASSWORD:123456</td><td>Ustawia nowe hasło do sieci wifi. (w tym przypadku '123456') - nie restartuje urządzenia</td></tr>";

    msg = msg + "<tr><td colspan='2' style='background-color:black; color: white'>Opisy LED:</td></tr>";    
    msg = msg + "<tr><td>Mruganie co 250ms</td><td>Próba podłączenia urządzenia do sieci WiFi</td></tr>";
    msg = msg + "<tr><td>3 mrugnięcia co 100ms</td><td>Połączenie poprawne do sieci WiFi</td></tr>";
    msg = msg + "<tr><td>10 mrugnięć co 100ms</td><td>Restart urządzenia</td></tr>";
    msg = msg + "<tr><td>20 mrugnięć co 100ms</td><td>Reset do ustawień fabrycznych</td></tr>";
    msg = msg + "<tr><td>Dioda włączona</td><td>Włączony obwód prądowy</td></tr>";
    msg = msg + "<tr><td>Dioda wyłączona</td><td>Wyłączony obwód prądowy</td></tr>";

    String msg2 = "<tr><td colspan='2' style='background-color:black; color: white'>Dodatkowe informacje:</td></tr>";    
    msg2 = msg2 + "<tr><td colspan='2'>";
    msg2 = msg2 + "Aby przywrócić urządzenie do stanu fabrycznego, takiego w którym będzie on HotSpot`em, ";
    msg2 = msg2 + "należy przytrzymać przycisk włączania przez "+info_czas_trzymania_do_restartu+" sekund. Po tym czasie dioda LED zamruga 20 razy co 100ms.";
    msg2 = msg2 + "Można także przywrócić do ustawień fabrycznych ";
    msg2 = msg2 + "Po restarcie urządzenie będzie HotSpotem do którego będzie można podłączyć się przez Wifi i skonfigurować od nowa.<br>";
    msg2 = msg2 + "<br>";
    msg2 = msg2 + "";
    msg2 = msg2 + "Więcej informacji jak podłączyć urządzenie do systemu SmartHome jest dostępnych na stronie https://github.com/marekurbanski<br>";
    msg2 = msg2 + "</td></tr>";
    msg2 = msg2 + "</table><br>";
    msg2 = msg2 + "<br>";
    msg2 = msg2 + "";
    msg2 = msg2 + "<br>";
    msg2 = msg2 + "</span>";

    allowed_ip_request.trim();
    if((allowed_ip_request == "") || (allowed_ip_request == ip2Str(client.remoteIP())))
        {
        // Match the request 
        if (request.indexOf("/=ON") != -1)  
            {
            setPinStatus(1, PinLight);              
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println("OK");

            String url = server_address+"?act=stateChange&SN="+SN+"&state=1";
            HTTPClient http;
            http.begin(url);
            int httpCode = http.GET();
            String payload = http.getString(); 
            Serial.println(httpCode);
            Serial.println(payload);
            http.end();

            return request;
            }
        else
        if (request.indexOf("/=OFF") != -1)  
            {
            setPinStatus(0, PinLight);  
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println("OK");

            String url = server_address+"?act=stateChange&SN="+SN+"&state=1";
            HTTPClient http;
            http.begin(url);
            int httpCode = http.GET();
            String payload = http.getString(); 
            Serial.println(httpCode);
            Serial.println(payload);
            http.end();
            return request;
            }
        else
        if (request.indexOf("/=SN") != -1)  
            {            
            setPinStatus(0, PinLight);  
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println(SN);
            return request;
            }
        else
        if (request.indexOf("/=SRVER_ADDR:") != -1)  
            {
            String url = String(request.substring(17));
            url.replace(" HTTP/1.1","");
            url.replace("HTTP/1.1","");
            url.trim();
            Serial.println("URL="+url);
            server_address = url;
            saveToEeprom(url, 300);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println("OK");
            
            return request;
            }
        else
        if (request.indexOf("/=SETUP_IP:") != -1)  
            {
            String ip = String(request.substring(15));
            ip.replace(" HTTP/1.1","");
            ip.replace("HTTP/1.1","");
            ip.trim();
            allowed_ip_request = ip;
            saveToEeprom(ip, 150);
            Serial.println("IP="+ip);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println("OK");
            
            return request;
            }
        else
        if (request.indexOf("/=PARAMS") != -1)  
            {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one

            ssid     = readFromEeprom(30);
            password = readFromEeprom(200);
            allowed_ip_request = readFromEeprom(150);
            allowed_ip_request.trim();
            server_address = readFromEeprom(300);
            server_address.trim();
            is_configured = readFromEeprom(1);
            DefaultON = readFromEeprom(450);
            SN = getSerial();
            
            client.println("PARAMS:<br>");
            client.println("SSID:"+ssid+"<br>");
            client.println("PASSWORD:'"+password.substring(0,2)+"'...<br>");
            client.println("Allowed IP request:'"+allowed_ip_request+"'<br>");
            client.println("Server address:'"+server_address+"'<br>");
            client.println("On at startup:"+DefaultON+"<br>");
            client.println("Serial:'"+SN+"'<br>");
            if(digitalRead(PinLight) == HIGH)
                client.println("State:'ON'<br>");
            else
                client.println("State:'OFF'<br>");
            client.println("Is configured:'"+is_configured+"'<br>");
            
            return request;
            }
        else
        if (request.indexOf("/=SET_SSID:") != -1)  
            {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println("OK");
            String new_ssid = String(request.substring(15));
            new_ssid.replace(" HTTP/1.1","");
            new_ssid.replace("HTTP/1.1","");
            new_ssid.trim();
            saveToEeprom(new_ssid, 30);
            delay(100);
            
            return request;
            }
        else
        if (request.indexOf("/=SET_PASSWORD:") != -1)  
            {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println("OK");
            String new_pass = String(request.substring(19));
            new_pass.replace(" HTTP/1.1","");
            new_pass.replace("HTTP/1.1","");
            new_pass.trim();
            saveToEeprom(new_pass, 200);  
            delay(100);
            
            return request;
            }
        else
        if (request.indexOf("/=RESTART") != -1)  
            {
            setPinStatus(0, PinLight);  
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println("OK");
            delay(2000);
            resetFunc();
            return request;
            }
        else
        if (request.indexOf("/=STATE") != -1)  
            {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            if(digitalRead(PinLight) == HIGH)
                client.println("ON");
            else
                client.println("OFF");
            return request;    
            }
        else
        if (request.indexOf("/=SET_DEFAULT_ON") != -1)  
            {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println("OK:ON");
            saveToEeprom("Y", 450);  
            return request;    
            }
        else
        if (request.indexOf("/=SET_DEFAULT_OFF") != -1)  
            {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println(""); //  do not forget this one
            client.println("OK:OFF");
            saveToEeprom("N", 450);  
            return request;    
            }
        else
            {
              String res = checkRequest(request);
              Serial.println("Zapytanie: "+request);      
              Serial.println("Wynik: "+res);      
          
              if(res != "")
                  {
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-Type: text/html");
                  client.println(""); //  do not forget this one
                  client.println(res);
                  return request;
                  }                
              else
                  {
                  // Return the response
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-Type: text/html");
                  client.println(""); //  do not forget this one
                  client.println(HTML_header);
                  client.println(msg+msg2);
                  client.println("");
                  Serial.println("Client disonnected");
                  Serial.println("");    
                  return request;
                  }
            }
        }
        else
        {
          // Return the response
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println(""); //  do not forget this one
          client.println(HTML_header);
          client.println("Błąd autoryzacji");
          client.println("");
          Serial.println("Client disonnected");
          Serial.println("");    
          return request;
          
        }
    }



/*
 * ###############################################################################################################################
 * createAP()
 * Funkcja tworzy standAlone AP - serwer.
 * Do niego mozna sie podlaczyc z ustawieniami ponizej
 * 
 * ###############################################################################################################################
*/
bool createHotSpot()
    {
    Serial.print("Tworze HotSpot ");

    WiFi.hostname(HostName);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    boolean result = WiFi.softAP(string2char(DefaultSSID), "");
    dnsServer.start(DNS_PORT, "*", apIP);
    if(result == true)
        {
        Serial.println("Ready");
        Serial.println("IP = "+ String(WiFi.softAPIP()));
        }
    else
        {
        Serial.println("Failed!");
        }
  
    // replay to all requests with same HTML
    webServer.onNotFound([]() 
      {
      webServer.send(200, "text/html", HTML_header+responseHTML);
      });
    webServer.on("/", HTTP_POST, on_hotspot_post);
      
    webServer.begin();
    }



/*
 * ###############################################################################################################################
 * on_hotspot_post()
 * Zapisuje dane wyslane POSTem
 * 
 * ###############################################################################################################################
 */
void on_hotspot_post()
  {
  String l_haslo = webServer.arg("haslo");
  String l_ssid = webServer.arg("ssid");
  Serial.println("");
  Serial.println("Haslo POST =  "+l_haslo);
  Serial.println("SSID  POST =  "+l_ssid);
  Serial.println("");
  ssid = l_ssid;
  saveToEeprom("TEST", 1);
  saveToEeprom(l_ssid, 30);
  saveToEeprom(l_haslo, 200);
    
  webServer.send(200, "text/html", HTML_header+responseHTML_SAVED);
  delay(10000);
  resetFunc();
  }



/*
 * ###############################################################################################################################
 * Funkcja zamienia String na Char
 * 
 * ###############################################################################################################################
 */
char* string2char(String command)
  {
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
  }



/*
 * ###############################################################################################################################
 * connectToWiFi()
 * Funkcja laczy sie do dzialajacego juz SmartHoma,
 * pobiera dane do polaczenia ze zmiennych zapisanych juz wczesniej w pamieci flash
 * 
 * ###############################################################################################################################
*/
bool connectToWiFi()
    {
    int i = 0;
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.mode(WIFI_STA);
    //WiFi.mode(WIFI_AP_STA);
    //WiFi.setPhyMode(WIFI_PHY_MODE_11G);
    //WiFi.setPhyMode(WIFI_PHY_MODE_11N);
    WiFi.begin(string2char(ssid), string2char(password));
    WiFi.hostname(HostName);
    while ((WiFi.status() != WL_CONNECTED) && (i<80))
        {           
        i++;

        setPinStatus(HIGH, LED);      
        delay(250);    
        setPinStatus(LOW, LED);      
        delay(250);
        
        if(changeCheckSwitchState(1, 11))
            {
            setPinStatus(-1, PinLight);
            }

        
        Serial.print("Proba "+String(i)+" z 80: ");
        if (WiFi.status() == WL_IDLE_STATUS) 
            { 
            Serial.println("WL_IDLE_STATUS");
            }
        else if (WiFi.status() == WL_CONNECTED) 
            {
            clientApConnected = true;
            Serial.println("WL_CONNECTED ");
            Serial.println(String(WiFi.localIP()));

            setPinStatus(LOW, LED);      
            delay(250);
            for(int b=0; b<3; b++)
                {
                //IPAddress ip = WiFi.localIP();
                //ip[3] = 255;  
                //ping(ip, 4);
  
                setPinStatus(HIGH, LED);      
                delay(100);    
                setPinStatus(LOW, LED);      
                delay(100);
                }
            is_configured = readFromEeprom(1);
            if(is_configured == "TEST")
                  {
                  saveToEeprom("YES", 1);  
                  }
            }
        else if (WiFi.status() == WL_NO_SSID_AVAIL) 
            {
            Serial.println("WL_NO_SSID_AVAIL");
            }
        else if (WiFi.status() == WL_CONNECT_FAILED) 
            {
            Serial.println("WL_CONNECT_FAILED");
            }
        else if (WiFi.status() == WL_CONNECTION_LOST) 
            {
            Serial.println("WL_CONNECTION_LOST");
            }
        else if (WiFi.status() == WL_DISCONNECTED) 
            {
            Serial.println("WL_DISCONNECTED");
            }
        else 
            {
            //Serial.println("unknown status: ");
            //Serial.println(String(WiFi.status()));
            }
        }
     
    Serial.println("");
    Serial.println("WiFi connected");
     
    // Start the server
    server.begin();
    Serial.println("Server started");
     
    // Print the IP address
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/"); 
    WiFi.printDiag(Serial);
    Serial.print(".");
    Serial.print(WiFi.localIP());
    Serial.setDebugOutput(true);

    if (WiFi.status() == WL_CONNECTED) 
        return true;
    else
        return false;

    }



/*
 * ###############################################################################################################################
 * saveToEeprom()
 * Funkcje zapisuje dane do pamieci EEPROM
 * ustawiany wartosc, od ktorego bajtu zaczynamy i na ktorym konczymy
 * !!!! Trzeba pamietac zeby string nie byl mniejszy niz ilosc bajtow w pamieci - jest obsluga bledow, ale zawsze
 * 
 * ###############################################################################################################################
*/
void saveToEeprom(const String &strToWrite, int addrOffset)
  {
    byte len = strToWrite.length();
    EEPROM.begin(1024);
    delay(10);
    EEPROM.write(addrOffset, len);
    for (int i = 0; i < len; i++)
        {
          EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
        }

     bool success = EEPROM.commit();
     EEPROM.end();
     Serial.println("");
     if(success)
          Serial.println("Zapis '"+strToWrite+"' do EEPROM poprawny");
         else
          Serial.println("Zapis '"+strToWrite+"' do EEPROM bledny !!!");
    
  }


/*
 * ###############################################################################################################################
 * readFromEeprom()
 * Funkcje czyta dane z pamięci EEPROM
 * 
 * ###############################################################################################################################
*/
String readFromEeprom(int addrOffset)
    {
    EEPROM.begin(1024);
    delay(10);
    int newStrLen = EEPROM.read(addrOffset);
    char data[newStrLen + 1];
    for (int i = 0; i < newStrLen; i++)
        {
        data[i] = EEPROM.read(addrOffset + 1 + i);
        }
    data[newStrLen] = '\0';
    return String(data);
    }

  
