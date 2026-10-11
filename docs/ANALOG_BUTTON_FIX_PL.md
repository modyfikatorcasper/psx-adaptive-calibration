# ANALOG — poprawka Modi PSX Lab / Adapter2.1

W firmware2.0 pad nadal odpowiadał `FF 73 00 FF FF 8B 6C 8F 79`, ale parser i status trace wymagały nagłówka5A. Odrzucenie00 usuwało VALID, a po30 ms status połączenia i wszystkie wejścia gamepada. USB nadal wysyłało około222 raportów/s. Relacja użytkownika: problem także w zewnętrznym testerze. Pad PS1 według użytkownika; SCPH nieustalone.

Źródło pierwotne: [PSX-SPX, Controllers Configuration Commands, caution3](https://psx-spx.consoledev.net/ps1/sio/controllersandmemorycards/controllers-configuration-commands/). Po konfiguracji i przycisku ANALOG normalna odpowiedź może mieć00 zamiast5A. Ta sama dokumentacja opisuje watchdog po przerwie komunikacji około1 s.

Parser i transport2.1 przyjmują00 wyłącznie dla41/53/73/79; F3 oraz nieznane ID z00 pozostają odrzucone. Długość i rozpoznane formaty nadal są sprawdzane. Oryginalny00 pozostaje w natywnej ramce RAW. Inicjalizacja rozpoznaje model45, pomija pressure dla DS1 i kończy po świeżym poprawnym poll; późniejszy ANALOG nie uruchamia ponownego wymuszania analogu. DS2 nadal może przejść do pressure79. Nie zmieniono osi ani kalibracji.

Testy C obejmują przechwyconą ramkę,00 dla wszystkich256 ID, krótkie/błędne odpowiedzi, digital D-pad i przyciski oraz20 przełączeń73/41 bez ponownej inicjalizacji; DS2 retry50, digital retry3, F3, wrap i reset. Testy RAW/MDV2 C→JS oraz walidacja rzeczywistego UF2 przeszły.

Tymczasowe odzyskanie2.0: STOP,43 enter,43 exit,polling. Przerwa przekroczyła1 s, więc watchdog jest czynnikiem zakłócającym — brak dowodu, że same43 przywróciły działanie. Po odzyskaniu było41/5A,222 raporty/s, licznik wcześniejszych błędów67649. Logi w lab/qa/analog-button-*.json.

UF2:52224 bajty,102 bloki, SHA256 `d0a419b2ad917f17dd1e3e9c806cd8015a59d85b67c401259e50d841186f3884`. Po ręcznym BOOT użytkownika skopiowano na RPI-RP2/G:, a Windows potwierdził oba HID z bcdDevice0201. Aplikacja2.1 ponownie połączyła się i odczytała79/pressure. Szczegółowy dalszy test sprzętowy: lab/qa/hardware-analog-fix-2_1.json; wynik podsumowany w VALIDATION_LAB.md.

Pomiar sprzętowy2.1:1742/1742 snapshoty podłączone i VALID, licznik błędów0→0. Zarejestrowano79/5A →41/00 →73/00 →41/00 →73/00. Digital:56 snapshotów, w tym 6 z wciśniętymi przyciskami. Użytkownik potwierdził: ANALOG wyłącza tylko gałki, ponowne naciśnięcie je włącza. Początkowe79 oznacza zgodność protokołu DS2; fizyczny model/SCPH niepotwierdzony. Przykład stanu UI:FF7300,222 raporty/s,0 błędów;112 luk w dostarczeniu RAW oraz4035 utraconych wpisów trace to osobne liczniki.

Porównanie Win32 RAW→gamepad→RAW jest nieatomowe:16 z1606 porównań ze zgodnymi wartościami końcowymi różniło się (15 o1 RX,1 Start). Między tymi snapshotami upłynęło5..107 poll; nie jest to dowód identyczności tej samej próbki ani ustalonego błędu mapowania. Nie liczymy tego jako bezbłędnej walidacji RAW/USB. Zakres całej próby zawiera cyfrowe zera zastępcze, nie zmierzone minima gałek. Szczegóły w analog-fix-2_1-analysis.json.
