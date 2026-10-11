# Test fizycznego pada PS2 — 2026-10-10

Egzemplarz HW-PS2-2026-10-10-01, model SCPH niepodany przez użytkownika.
Windows, lokalny Chromium Codex, rzeczywisty WebHID i Win32 HID.
Urządzenie ma firmware bcdDevice0100: wcześniejsza baza, bez Output/Feature Lab2.
Nie wgrano nowego UF2. ID79 i pressure opisują protokół, nie autentyczność/etykietę.

## Spoczynek potwierdzony przez użytkownika

Po ruchach użytkownik potwierdził, że obie gałki są całkowicie puszczone.
W 10 sekund zebrano642 poprawne snapshoty Win32 HID GET_INPUT_REPORT.

|Oś|Mediana RAW|min|max|rozrzut|Różnica od128|
|---|---:|---:|---:|---:|---:|
|LX|143|143|143|0|+15|
|LY|123|123|123|0|−5|
|RX|125|125|125|0|−3|
|RY|111|111|111|0|−17|

Aplikacja pokazała te same wartości w lokalnym pomiarze mediany z1s.
Pomiar odniesienia nie przesuwa RAW ani nie wysyła kalibracji do pada.
Zerowy rozrzut dotyczy bajtów protokołu w tym oknie; nie dowodzi braku szumu ADC.

## Ruch i pressure

Osobny 90-sekundowy zapis zawiera5390 poprawnych snapshotów, także podczas ruchów
osi i naciskania przycisków. Jego pierwsza próbka nie jest potwierdzonym spoczynkiem.
Nie wolno wnioskować o autokalibracji z różnicy pierwszej i ostatniej próbki.
W nim RX/RY/LX osiągnęły0 i255; LY osiągnęła0 i206. Dłuższy widok aplikacji
pokazał0..255 dla wszystkich4 osi: dowód DOM i zrzut ekranu są zapisane osobno.

W zapisanym native JSON są wartości pośrednie w8 kanałach pressure poza D-padem.
Widok aplikacji, obejmujący wcześniejsze naciskanie D-pada, potwierdził wszystkie12:
Right50, Left11, Up12, Down27, Triangle36, Circle35, Cross69, Square33,
L1 6, R1 12, L2 46, R2 38 różnych wartości pośrednich1..254.
Dla każdej z12 pozycji widok odnotował również0 i255.
To dowód odczytu analogowego nacisku z tego egzemplarza, nie pełna charakterystyka
liniowości ani dowód oryginalności. Liczniki UI i snapshoty obejmują różne okna.

## RAW → USB

Snapshoty odczytywano kolejno RAW → gamepad USB → RAW. Porównywano tylko pary,
w których osie, przyciski i flags po obu stronach się zgadzały i ramka była ważna.
5374 porównania w oknie ruchów +642 w potwierdzonym spoczynku =6016 zgodności,
bez rozbieżności. Sprawdzono bajty LX/LY/RX/RY, hat i12 przycisków bazowego HID.
Metoda jest programowym otoczeniem odczytu, nie atomowym pomiarem magistrali;
krótkie przejścia pomiędzy snapshotami mogły nie zostać uchwycone.

Licznik błędów PSX pozostał0 w obu plikach. Aplikacja pokazywała około222–223
raportów/s. Pierwszy zapis DOM pokazuje64 luki sekwencji, końcowy widok88.
Przyczyna luk nie została ustalona; nie są one błędami licznika PSX.
GET_INPUT_REPORT snapshoty nie służą do liczenia strat strumienia USB.
Gamepad API w przeglądarce Codex nie wykrył gamepada mimo działającego vendor RAW;
nie zweryfikowano dwóch niezależnych testerów ani nowego deskryptora Lab2.

## Pliki dowodowe

- lab/qa/hardware-ps2-baseline.json:5390 snapshotów z90s, full RAW i payload USB.
- lab/qa/hardware-ps2-rest.json:642 snapshoty z10s po potwierdzeniu spoczynku.
- lab/qa/hardware-app-observation.txt oraz hardware-ps2-real.png: pierwszy widok RAW.
- lab/qa/hardware-final-observation.txt oraz hardware-result.png: końcowy widok RAW.
- lab/qa/hardware-pressure-observation.txt oraz hardware-pressure-real.png:12 kanałów.
- W otwartym widoku zakończono rejestrację70375 raportów. Kliknięto eksport JSON,
  lecz zapis tego eksportu na dysku przeglądarki Codex nie został potwierdzony.
  Nie jest on przedstawiany jako zachowany plik. Potwierdzone są dwa native JSON.

## Pozostałe próby

Do sprawdzenia terminala, STOP/recovery, RX komend i nowego mappingu trzeba wgrać
UF2 Lab2. Nie wysyłano40 ani nieznanych komend. Nie wykonano prób power-cycle,
startu z wychyloną gałką, kontroli przeciw sekwencji ani elektrycznego pomiaru ACK.
Obecny stabilny offset nie określa mechanizmu jego powstania.
