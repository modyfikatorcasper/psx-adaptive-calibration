# Walidacja / validation — 2026-10-06

**Wynik:** zbudowano rzeczywisty prototyp UF2 dla Waveshare RP2040-Zero.
Nie oznacza to spełnienia całej sprzętowej Definition of Done. Fizycznej płytki
i kontrolerów nie wgrano ani nie testowano w tej sesji.

## Wykonane i zaliczone

- Czysta kompilacja ARM: `build.py`, GCC 15.2.1, CMake 3.31.6, Ninja 1.12.1,
  Pico SDK 2.2.0, pioasm 2.2.0. Target `waveshare_rp2040_zero`, 2 MB flash.
- UF2: 89 bloków / 45 568 bajtów. Walidacja wszystkich nagłówków, flag, numerów,
  adresów flash, family ID RP2040 `E48BFF56`, końców bloków i zgodności z BIN.
- Zweryfikowano sumę CRC boot2, stos SRAM oraz wektor startowy Thumb w flash.
- Odczytano deskryptory z faktycznie połączonego ELF: 2 HID, standard gamepad
  7 bajtów, vendor FF00:0001 64 bajty, endpoints 81/82 IN, interval 1 ms.
  USB product/manufacturer w źródle: Modi PSX Adapter / Modyfikator.
- Uruchomiono rzeczywisty parser C na Windows (host GCC z w64devkit 2.10.0),
  z `-Wall -Wextra -Werror`: 41, 53, 73, 79, F3, center 128 i nierówny center,
  wszystkie 256 wartości każdej osi, pressure, surowe bity przycisków, diagonalny
  D-pad, uszkodzone/niedokończone ramki, odłączenie i ponowne podłączenie,
  zwolnienie HID po błędnym odczycie i zawinięcie zegara uint32.
- Serializer C → 64-bajtowy fixture → rzeczywisty dekoder z HTML w Node:
  dokładnie RX/RY/LX/LY 124/130/121/133, 12 pressure w oryginalnej kolejności,
  przyciski, timestamp i sequence. Fixture zawiera oryginalną ramkę.
- Testy statystyk HTML: centrum spoczynkowe, p-p, min/max, range, 36 sektorów,
  odrzucanie nieaktualnych próbek i RESET bez modyfikacji wartości RAW.
- Tester otwarto w przeglądarce Codex na 127.0.0.1:8000. Wizualnie sprawdzono
  tryb DEMO, wykresy/trail i tabele; DEMO był wyraźnie oznaczony jako syntetyczny.
  Zapisano `tester-demo.png`. To nie jest próba połączenia z fizycznym WebHID.
- Wygenerowano i obejrzano SVG/PNG mapy lutowania. Sześć sieci, pull-up 4,7 kΩ,
  komponentowa strona konkretnej płytki i front/tył gniazda żeńskiego.

Szczegółowy hash UF2 i wersje kompilatora: `release/build-manifest.json`.
Pomiary rozmiaru ELF: text 26 748 B, data 0 B, BSS 4 108 B (pierwsza kompilacja;
rebuild zawiera ten sam kod). Wszystkie licencje/źródła zależności zachowane.

## Wymagające fizycznej weryfikacji — NIE WYKONANO

- [ ] Wgrać UF2 przez BOOT; sprawdzić powrót z bootloadera po zimnym starcie.
- [ ] Windows enumeruje Modi PSX Adapter jako oba interfejsy HID, bez błędów USB.
- [ ] `joy.cpl`: PS1 Digital Pad — komplet przycisków i D-pad.
- [ ] Oryginalny DualShock — wszystkie 4 osie, krańce i stabilny odczyt.
- [ ] Dual Analog — tryb 53 i osie (mogą wymagać fizycznego przycisku ANALOG).
- [ ] Oryginalny DualShock 2 — uzyskanie 79 i 12 pressure values.
- [ ] Oryginalne przewody / przedłużacze: DAT rise time, pull-up, 3V3 i brak
      przekroczenia wydajności zasilania, ewentualnie analizator logiczny.
- [ ] WebHID Chrome/Edge na Windows — wybór vendor collection, właściwe MPSX,
      zgodność z odczytem na przewodzie, rate i brak nieuzasadnionych błędów.
- [ ] Chrome Gamepad API i Steam — widoczność, przyciski i osie; mapowanie
      standardowe w przeglądarce nie jest gwarantowane dla własnego HID.
- [ ] Odłączyć pad z wciśniętym przyciskiem: HID zwalnia przycisk; po ≥30 ms
      RAW oznacza odłączenie. Podłączyć ten sam i inny pad bez resetu MCU.
- [ ] Użyć DS1 bez pressure i cyfrowego pada odrzucającego config: brak zawieszenia.
- [ ] Eksperyment cold/warm i gałka wychylona przy starcie: dane obserwowane
      bez wymuszania center/range przez adapter.

Po tych próbach dopiero można stwierdzić, że sprzętowe kryteria zostały spełnione.
Nie przypisano trwałego unikalnego VID/PID i nie zintegrowano profilu z nieudostępnionym
kodem aplikacji DriftGuard. Profil JSON jest przykładem adaptera/integracji.
