# Firmware 2.1 — Modi PSX Adapter

[UF2 dla RP2040-Zero](modi_psx_adapter_rp2040zero.uf2) · [Komplet źródeł i zależności](source.zip) · [Schemat](../docs/DIY_PL.md)

UF2 jest dokładnie obrazem używanym w badaniach. SHA-256:
`d0a419b2ad917f17dd1e3e9c806cd8015a59d85b67c401259e50d841186f3884`

Rozpakuj source.zip i uruchom `python build.py` (Python 3.10+, Windows x64: pobranie przypiętych oficjalnych narzędzi z kontrolą SHA-256). Inne systemy wymagają ARM GCC, CMake, Ninja i pioasm na PATH. Archiwum zawiera Pico SDK 2.2.0 / TinyUSB i wymagane licencje. Zawiera tylko firmware — bez aplikacji developerskiej, automatów i nagrań. Kopie własnych plików C są dostępne także w src/ dla wygodnego przeglądania.

Zmieniono jedynie etap pakowania build.py: nie kopiuje lokalnych testerów HTML do release. Kod firmware jest niezmieniony. Domyślny VID/PID 1209:0001 to współdzielony identyfikator prywatnych prób. Dla dystrybucji urządzeń użyj legalnie przydzielonych identyfikatorów przez `python build.py --vid 0x.... --pid 0x....`.

Firmware standardowo inicjalizuje i odpytuje pad. Publiczna aplikacja odczytuje raporty; nie steruje konfiguracją ani silnikami. Wbudowane funkcje serwisowe firmware nie stanowią publicznego interfejsu testera. Wibracji nie zweryfikowano.
