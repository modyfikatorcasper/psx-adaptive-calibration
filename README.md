# Adaptacyjna kalibracja PS1 i PS2 — badania RAW

Zmierzyliśmy, jak historia wychyleń i wymiana analogów wpływają na wartości raportowanego środka w badanych kontrolerach PS1 i PS2.

**[Otwórz prosty tester RAW](https://modyfikatorcasper.github.io/psx-adaptive-calibration/tester/)** · **[Pełny raport](docs/ADAPTIVE_CALIBRATION_PL.md)** · **[Zbuduj adapter](docs/DIY_PL.md)**

## Najważniejszy wynik

W PS2 po wymianie analogów pierwsze cztery kierunki dały powroty prawej gałki 107/134, 105/148, 105/121 i 132/121. Następna seria: 133/122, 133/122, 131/122 i 133/122. Rozrzut powrotów spadł z **27/27 do 2/0 RAW**. Lewa po wcześniejszym obrocie wracała po kierunkach do **126–127/125**.

To mocny dowód zachowania zgodnego z adaptacją zależną od poznanego zakresu i historii ruchu. Nie jest to odczyt kodu firmware Sony ani dowód identycznego mechanizmu we wszystkich rewizjach. RAW oznacza bajty wysyłane przez kontroler, nie surowy ADC. Samo uruchomienie nie gwarantowało 128/128; nie twierdzimy też, że start nie uczestniczy w kalibracji.

W naszych próbach zmodyfikowany pad PS1 dawał mniej powtarzalne środki niż PS2 po wymianie analogów. Przykładowo LY w wybranych powrotach PS1 przyjmowała 130–137, podczas gdy końcowa seria kierunkowa lewej gałki PS2 po obrocie utrzymywała LY 125 i LX 126–127. To porównanie tych egzemplarzy i prób, nie wszystkich padów PS1 i PS2; PS1 był niezłożony, a warunki oraz historie ruchów różniły się.

## Materiały

- [Raport: przebieg, wyniki, interpretacja i ograniczenia](docs/ADAPTIVE_CALIBRATION_PL.md).
- [Krótki tekst do publikacji](docs/ADAPTIVE_CALIBRATION_POST_PL.md).
- [Pakiet dowodowy i skróty SHA-256](docs/research/center-2026-10-11/evidence.json).
- [Pełne zachowane dane badawcze](data/README.md), również przerwane próby oznaczone w analizach.
- [Schemat i budowa z RP2040-Zero oraz gniazdem odzyskanym z PS2](docs/DIY_PL.md).
- [Firmware 2.1: UF2, źródła, zależności i licencje](firmware/README.md).
- [Potwierdzenie odczytu pressure](docs/HARDWARE_BASELINE_PL.md), [poprawka ANALOG](docs/ANALOG_BUTTON_FIX_PL.md).
- [Przegląd dokumentacji i wcześniejsze hipotezy](docs/RESEARCH_PL.md).

Publiczny tester pokazuje gałki, przyciski, pressure, ramkę RAW i statystyki oraz pozwala pobrać własny zapis. Nie zawiera demo, zakładki developerskiej, terminala ani automatów badawczych. Nie wysyła komend do adaptera. Narzędzia developerskie pozostają lokalne.

**Wibracje nie zostały sprawdzone; schemat nie zasila silników.** Odczyt pressure nie wymaga wibracji.

Badanie zamknięto 11 października 2026 r. Pomiary wykonano na konkretnych egzemplarzach, a oznaczenia modułów i kąt 60° pochodziły od prowadzącego; nie zmierzono niezależnie ich charakterystyki elektrycznej.

---

Badania i pomiary: **Kacper Lewandowski, znany też jako modyfikator Modi / modyfikator 89**. Adapter korzysta z kodu Joypad OS, Pico SDK i TinyUSB; pełne informacje o pochodzeniu i licencjach zawiera [NOTICE](NOTICE). Nazwy Sony i PlayStation identyfikują badane urządzenia; projekt jest niezależny.
