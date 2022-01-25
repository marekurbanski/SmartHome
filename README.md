# SmartHome
New firmware for Sonoff switches


How it works (after loading this firmware to the switch;):
- when you turn it on for the first time, the switch will create a HotSpot named: SmartHome-XYZ (the first time it will set its unique Serial XYZ which will not change anymore)
- connect to the HotSpot SmartHome-XYZ with a mobile phone
- a website will appear with a list of available WiFi networks
- choose the network in which our socket will work, enter the password and it's ready :)

We can configure the socket ourselves for each SmartHome opensource.
You can use it directly through your browser or program it in any programming language.

The available methods can be obtained by opening in the browser: http://192.168.0.24:8080/ where the address 192.168.0.24 is the socket address.
Sample page will be displayed:



<!DOCTYPE html><html lang='pl'><head><meta charset='utf-8' /><meta name='viewport' content='width=device-width'><h1>Witaj w SmartHome</h1><br>
<span style='font-size:14px; font-family:arial'><h3>Witaj w Sky-Home.eu</h3><br><br><table id='t1' border='1' cellspacing='0' cellpadding='0'><tr><td colspan='2' style='background-color:black; color: white'>Dane urządzenia:</td></tr><tr><td>Model: </td><td>Włącznik światła Sky-Home.eu(NR4HSFT)</td></tr><tr><td>Wersja: </td><td>v1.6</td></tr><tr><td>Nazwa: </td><td>Sky-Home-NR4HSFT</td></tr><tr><td>Serial: </td><td>NR4HSFT</td></tr><tr><td>Odbieraj z IP: </td><td></td></tr><tr><td>Adres serwera: </td><td>http://server.local/smart-home/</td></tr><tr><td colspan='2' style='background-color:black; color: white'>Dostepne metody:</td></tr><tr><td>http://192.168.0.24:8080/=ON</td><td>Włączenie przelacznika</td></tr><tr><td>http://192.168.0.24:8080/=OFF</td><td>Wylaczenie przelacznika</td></tr><tr><td>http://192.168.0.24:8080/=SN</td><td>Wyświetla Numer Seryjny urządzenia SN, w tym przypadku: NR4HSFT</td></tr><tr><td>http://192.168.0.24:8080/=STATE</td><td>Podaje aktualny status przelacznika</td></tr><tr><td>http://192.168.0.24:8080/=PARAMS</td><td>Podaje aktualne parametry przelacznika</td></tr><tr><td>http://192.168.0.24:8080/=RESTART</td><td>Restartuje urządzenie</td></tr><tr><td>http://192.168.0.24:8080/=SET_DEFAULT_ON</td><td>Ustawia urządzenie jako włączone po podłączeniu zasilania</td></tr><tr><td>http://192.168.0.24:8080/=SET_DEFAULT_OFF</td><td>Ustawia urządzenie jako wyłączone po podłączeniu zasilania</td></tr><tr><td>http://192.168.0.24:8080/=SRVER_ADDR:http://192.168.0.1/smart-home/</td><td>Ustawia adres serwera na który będzie wysyłana informacja o zmianie stanu [0/1] (w tym przypadku po wlaczeniu bedzie wyslany GET: http://192.168.0.1/smart-home/?act=stateChange&SN=NR4HSFT&state=1)</td></tr><tr><td>http://192.168.0.24:8080/=SETUP_IP:192.168.0.1</td><td>Ustawia odbieranie polecen tylko z tego adresu IP (w tym przypadku 192.168.0.1), puste bedzie odbierac z kazdego klienta</td></tr><tr><td>http://192.168.0.24:8080/=SET_SSID:nazwa_sieci_wifi</td><td>Ustawia nową sieć wifi do które będzie się łączyć (w tym przypadku 'nazwa_sieci_wifi') - nie restartuje urządzenia</td></tr><tr><td>http://192.168.0.24:8080/=SET_PASSWORD:123456</td><td>Ustawia nowe hasło do sieci wifi. (w tym przypadku '123456') - nie restartuje urządzenia</td></tr><tr><td colspan='2' style='background-color:black; color: white'>Opisy LED:</td></tr><tr><td>Mruganie co 250ms</td><td>Próba podłączenia urządzenia do sieci WiFi</td></tr><tr><td>3 mrugnięcia co 100ms</td><td>Połączenie poprawne do sieci WiFi</td></tr><tr><td>10 mrugnięć co 500ms</td><td>Restart urządzenia</td></tr><tr><td>20 mrugnięć co 100ms</td><td>Reset do ustawień fabrycznych</td></tr><tr><td>Dioda włączona</td><td>Włączony obwód prądowy</td></tr><tr><td>Dioda wyłączona</td><td>Wyłączony obwód prądowy</td></tr><tr><td colspan='2' style='background-color:black; color: white'>Dodatkowe informacje:</td></tr><tr><td colspan='2'>Aby przywrócić urządzenie do stanu fabrycznego, takiego w którym będzie on HotSpot`em, należy przytrzymać przycisk włączania przez 8 sekund. Po tym czasie dioda LED zamruga 20 razy co 100ms.Można także przywrócić do ustawień fabrycznych Po restarcie urządzenie będzie HotSpotem do którego będzie można podłączyć się przez Wifi i skonfigurować od nowa.<br><br>Więcej informacji jak podłączyć urządzenie do systemu Sky-Home.eu jest dostępnych na stronie https://Sky-Home.eu<br></td></tr></table><br><br><br></span>

  
  Have fun :)
  
