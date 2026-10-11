# Jak działa adaptacyjna kalibracja w badanych padach PS1 i PS2

Kacper Lewandowski, znany też jako modyfikator Modi / modyfikator 89 · Modi PSX Lab · badania zamknięte 11 października 2026 r.

**W badanych kontrolerach odczyt środka zależał od wcześniejszych wychyleń. Po przejściu pełnego zakresu kolejne powroty stawały się znacznie bardziej zgodne.** Okrąg pozwalał przejść przez krańce obu osi przed serią kierunkową. Przy samych kierunkach duże przesunięcia występowały jeszcze w pierwszej serii, a następna dawała już skupione wyniki.

Takie zachowanie mocno wspiera model **adaptacyjnej kalibracji zależnej od poznanego zakresu**. Ustaliliśmy doświadczalnie, jak ten mechanizm objawia się na wyjściu badanych padów i jakie ma praktyczne ograniczenia. Nie odczytywaliśmy ich wewnętrznego programu, dlatego nie przypisujemy mu konkretnego wzoru ani struktury pamięci.

Osiągany środek pozostaje zależny od zamontowanych analogów. Po wymianie modułów w badanym PS2 wybrane wyniki po przejściu zakresu były bliższe środkowi nominalnemu. Jednocześnie ani wymiana, ani kolejne obroty nie gwarantowały identycznego 128/128 po każdym puszczeniu.

## Wniosek końcowy: adaptacyjne centrowanie w praktyce

W badanych egzemplarzach rozpoznaliśmy powtarzalną zasadę działania: po uruchomieniu wartości środka mogły być różne, a pełny obrót gałki lub przejście czterech kierunków prowadziły do stabilizacji kolejnych powrotów. Opisujemy ten efekt jako adaptacyjną kalibrację zależną od historii wychyleń i poznanego zakresu. Sam start nie wystarczał do uzyskania końcowego, powtarzalnego środka; korekta ujawniała się podczas ruchów.

W praktyce oznacza to, że po przejściu zakresu pad wracał do bardziej powtarzalnego punktu, w granicach możliwości zamontowanego modułu analogowego. „Wycentrowanie” nie oznacza tutaj gwarancji 128/128. Wymiana analogów wpływała na osiągane wyniki. Tym samym wyjaśniliśmy obserwowane zachowanie badanych padów i sposób jego odtworzenia; nie utożsamiamy tego z odczytaniem dokładnego algorytmu Sony ani dowodem całkowitej niezależności kalibracji od czynności wykonywanych przy starcie.

## Cel badania: zrozumieć, jak działa kalibracja

Głównym celem badań było doświadczalne zrozumienie, jak działa kalibracja w badanych kontrolerach PS1 i PS2: kiedy zmieniają się raportowane wartości środka, jakie znaczenie mają wcześniejsze ruchy i osiągnięte zakresy oraz jak na wynik wpływa wymiana modułów analogowych. Motywacją były niejasności w społeczności dotyczące pojęcia „autokalibracja”. Według prowadzącego dostępne mu opisy Sony nie wyjaśniały wystarczająco mechanizmu, aby przewidzieć obserwowane zachowanie padów; dlatego postanowił zbadać je bezpośrednio na sprzęcie i udostępnić pomiary oraz sposób ich odtworzenia.

Jednym z pytań w ramach tego celu było sprawdzenie spotykanego w internecie twierdzenia, że samo uruchomienie pada automatycznie ustawia prawidłowy, idealny środek analogów. W naszych próbach samo podłączenie nie gwarantowało takiego wyniku. Pierwsze wychylenia zmieniały wartości powrotu, a po przejściu zakresów — obrotem lub ruchami kierunkowymi — kolejne powroty stawały się bardziej powtarzalne. To właśnie ten obserwowalny wzorzec opisujemy jako zachowanie zgodne z adaptacyjną kalibracją. Słowo „automatyczna” samo w sobie nie oznacza „wykonywana wyłącznie przy starcie”; pomiary nie wykluczają wszystkich czynności inicjalizacyjnych kontrolera.

Drugim celem było sprawdzenie, czy na wynik można wpłynąć sprzętowo, wymieniając moduły analogowe. Porównanie badanego PS2 przed i po wymianie pokazało zmianę zachowania środka. Dobór modułu jest więc praktycznym sposobem wpływania na wynik. Powtarzalność mechanicznego powrotu oraz zgodność charakterystyki elektrycznej modułu są istotnymi kryteriami doboru. Rezystancję, przebieg ścieżki i zależność sygnału od kąta należy przy tym odróżnić od osobno zmierzonych przyczyn: w tym badaniu nie izolowaliśmy wpływu każdego z tych parametrów i nie obiecujemy idealnego środka dla dowolnego zamiennika.

W tych doświadczeniach nie programowaliśmy wewnętrznej kalibracji Sony ani nie stosowaliśmy korekcji wartości RAW po stronie adaptera. Nie wykazaliśmy tym samym, że każda ingerencja programowa jest niemożliwa; wykazaliśmy wpływ historii ruchów i wymiany sprzętu na raportowane wartości.

## Co zbudowaliśmy i co mierzyliśmy

Tor badawczy wyglądał następująco:

**pad PS1 lub PS2 → Modi PSX Adapter na RP2040-Zero → USB → Modi PSX Lab na PC.**

Adapter odczytuje protokół kontrolera. Aplikacja pokazuje cztery oryginalne bajty osi w zakresie 0–255. W badaniu nie dodawaliśmy deadzone, wygładzania, przesuwania środka ani rozciągania zakresu. Parser kopiuje osie z odpowiedzi kontrolera; wartości w raportach USB porównywaliśmy z zachowaną ramką PSX. Kod tego toru jest w [psx_parser.c](../firmware/src/psx_parser.c).

**RAW oznacza tutaj wartości wysłane przez pad, już po ewentualnym przetwarzaniu wewnątrz jego elektroniki.** To nie jest bezpośredni pomiar napięcia potencjometru ani surowych próbek przetwornika ADC. Wynik pozwala więc badać zachowanie kontrolera widziane przez komputer.

Próbki zbieraliśmy przez WebHID oraz niezależny czytnik wejściowych raportów HID w Windows, zwykle około 222 razy na sekundę. Czytnik natywny nie wysyłał poleceń kalibracji. Adapter wykonywał zwykły polling i standardową inicjalizację po podłączeniu. Przycisk „Zapisz środek” w aplikacji tworzy wyłącznie lokalne odniesienie pomiarowe.

We wcześniejszym etapie usunęliśmy błąd obsługi przycisku ANALOG: parser odrzucał poprawny wariant nagłówka 00. Firmware 2.1 zachowuje dane i pozwala przełączać tryb cyfrowy oraz analogowy bez utraty przycisków. Ta naprawa dotyczyła transportu, nie korekcji osi. Opis i walidacja są w [ANALOG_BUTTON_FIX_PL.md](ANALOG_BUTTON_FIX_PL.md).

## Badane egzemplarze

| Egzemplarz | Stan podczas badań | Rola |
|---|---|---|
| PS1 z wymienionymi analogami | Otwarty, niezłożony kontroler; moduły wskazane przez właściciela jako specjalna wersja | Pierwszy wzorzec powrotów, próby startu i historii zakresu |
| PS2 przed wymianą | Dotychczasowe analogi; stopnia zużycia nie zmierzono niezależnie | Porównanie zachowania na innej elektronice i modułach |
| Ten sam PS2 po wymianie | Nowe analogi według właściciela | Próba wpływu hardware i szczegółowe porównanie pierwszych oraz kolejnych powrotów |

Właściciel określił zastosowane zamienniki jako wersję o kącie 60°. Dokładnych oznaczeń, charakterystyk rezystancyjnych i definicji tego kąta nie zmierzono w tej sesji. Identyfikatory robocze i pochodzenie danych znajdują się w [pakiecie dowodowym](research/center-2026-10-11/evidence.json).

To badanie konkretnych egzemplarzy. Nie przypisujemy wyniku każdej rewizji Sony, każdemu zamiennikowi ani podróbkom wspominanym przed rozpoczęciem pomiarów. Niezłożony PS1 i PS2 w innych warunkach montażu nie stanowią identycznych stanowisk mechanicznych.

## Jak prowadziliśmy próby

Najpierw rejestrowaliśmy spoczynek, uruchomienia z gałką puszczoną lub wychyloną oraz ruchy przy nieprzerwanym zasilaniu. Następnie wykonywaliśmy kierunki, obroty w obie strony, wymianę analogów w PS2, powtórki bez restartu i mikroruchy.

Automat dawał 3 sekundy na ruch kierunkowy i 2 sekundy na puszczenie. Wynik był medianą z ostatniej sekundy tego postoju. Serie okręgów miały własne odliczanie; faktyczna liczba obrotów bywała większa niż zadana i pozostaje zapisana. W próbach ręcznych użytkownik sam ustalał tempo, a analizowaliśmy kolejne odcinki ustalonego odczytu po ruchu.

Najważniejszą miarą okazał się **każdy powrót po każdym kierunku**. Sam ostatni środek z całej serii pomija etap dostosowywania, który zachodzi wcześniej. Osobno liczymy:

- przesunięcie powrotu względem nominalnego 128;
- rozrzut między powrotami po różnych kierunkach;
- wahania podczas pojedynczego postoju.

Te wielkości opisują różne zjawiska. Stały odczyt 133/122 może mieć zerowy szum w czasie, mimo że jest przesunięty względem 128/128.

## PS1 i PS2 przed wymianą pokazały wpływ historii zakresu

W PS1, przy ciągłym zasilaniu, początkowy LX wynosił 128. Pierwsze pełne wychylenie w prawo i puszczenie dało LX 62. Po pełnym wychyleniu w lewo odczyt wrócił do 126. Kolejne wychylenia w prawo i w lewo również kończyły się na 126. Duża zmiana nastąpiła bez restartu USB.

Późniejsze okręgi zachowywały LX 126, ale LY wynosiło kolejno 130 i 137. Pokazuje to zarówno stabilizację jednej osi po przejściu zakresu, jak i brak gwarancji coraz lepszego środka przy każdym następnym obrocie. Pierwsze przytrzymanie trwało dłużej niż kolejne; nie traktujemy tej próby jako porównania identycznych czasów ruchu.

W PS2 przed wymianą sekwencja LX wyglądała tak:

| Etap | LX po puszczeniu |
|---|---:|
| Początek | 117 |
| Pierwsze prawo | 114 |
| Pierwsze lewo | 143 |
| Powtórzone prawo | 139 |
| Powtórzone lewo | 144 |

Różnica między pierwszymi powrotami z prawej i lewej wyniosła 29 jednostek; między powtórzonymi — 5. Po poznaniu obu końców osi wyniki były bardziej zgodne, choć nadal odsunięte od nominalnego środka.

## Wymiana analogów w PS2

Po wymianie modułów zachowanie zależne od historii ruchu pozostało widoczne. W pierwszej pełnej serii nowa lewa gałka po wczesnych jednostronnych wychyleniach wracała m.in. do 105/156. Po przeciwległych krańcach i okręgach wracała do 128/125 oraz 129/126. W powtórce bez odłączania USB duże przesunięcia już nie wystąpiły; największa zmiana względem początku serii wynosiła 2 jednostki na osi.

Wybrane porównywalne rodzaje pomiarów po okręgach dały następujące wyniki:

| Gałka i kierunek okręgów | Przed wymianą X/Y | Po wymianie X/Y |
|---|---:|---:|
| Lewa, zgodnie z zegarem | 143/123 | 128/125 |
| Lewa, przeciwnie | 138/119 | 129/126 |
| Prawa, zgodnie z zegarem | 125/107 | 131/121 |
| Prawa, przeciwnie | 134/108 | 134/124 |

W tych pomiarach nowe moduły dawały środki bliższe 128/128. To wspiera praktyczny wniosek o wpływie hardware. Historie poprzedzających ruchów i rzeczywiste liczby obrotów różniły się, więc tabela nie wyznacza uniwersalnej poprawy procentowej po wymianie ani samego wpływu kąta lub rezystancji.

## Kluczowe porównanie każdego powrotu

W końcowej próbie jedna gałka najpierw wykonywała obrót obejmujący zakres obu osi, a następnie cztery kierunki. Druga wykonywała cztery kierunki, a później ponownie cztery kierunki, bez obrotu. We wcześniejszej próbie zamieniliśmy role gałek. Celem było porównanie przebiegu dostosowania oraz powtarzalności powrotów.

Końcowa próba lewej gałki dała po zgłoszonym obrocie 127/125. Po kierunkach było:

| Powrót lewej po obrocie | X/Y |
|---|---:|
| Góra | 127/125 |
| Prawo | 126/125 |
| Dół | 126/125 |
| Lewo | 127/125 |

Prawa, wykonująca same kierunki, pokazała różnicę między pierwszym a następnym przebiegiem:

| Powrót prawej | Pierwsza seria X/Y | Druga seria X/Y |
|---|---:|---:|
| Prawo | 107/134 | 133/122 |
| Góra | 105/148 | 133/122 |
| Dół | 105/121 | 131/122 |
| Lewo | 132/121 | 133/122 |

Ruchy pierwszej serii następowały w kolejności prawo, góra, dół, lewo; drugiej — góra, prawo, dół, lewo. Tabela zestawia odpowiadające sobie kierunki.

**Rozrzut między powrotami prawej zmniejszył się z 27 do 2 jednostek na X oraz z 27 do 0 na Y. Największe odchylenie pojedynczej osi od 128 zmalało z 23 do 6 jednostek.** Lewa po obrocie miała rozrzut 1 na X i 0 na Y już w pierwszej serii kierunkowej. Zapis obejmuje 26 330 raportów bez wykrytych luk i błędów, z zachowaną zgodnością osi z ramkami kontrolera.

To jest główny wynik: **po uprzednim przejściu zakresu kolejne powroty skupiają się w wąskim obszarze**. Okrąg oraz seria pełnych kierunków są dwoma sposobami dostarczenia informacji o krańcach. Wynik nie wymaga idealnego 128/128. Nie wyznacza też wymaganej liczby obrotów dla wszystkich padów.

## Co pokazały mikroruchy

Po wcześniejszych pełnych wychyleniach wykonaliśmy drobne ruchy obu gałek. Żadna oś nie osiągnęła w tym zapisie 0 ani 255.

| Gałka | Początek tego zapisu X/Y | Końcowy postój X/Y |
|---|---:|---:|
| Lewa | 122/125 | 131/126 |
| Prawa | 130/122 | 133/121 |

W stabilnych przerwach lewa pokazywała m.in. 121/125, 136/126, 121/124 i 137/126. Prawa również miała różne wartości w kolejnych przerwach. Indywidualne puszczenia nie były oznaczane osobnymi komunikatami, dlatego ustalony odczyt traktujemy jako plateau, a nie niezależnie potwierdzone mechaniczne zero.

Po ostatnim puszczeniu wszystkie cztery osie pozostawały dokładnie niezmienne przez **28,3 sekundy**. Nie zaobserwowaliśmy dalszego przesuwania odczytu do 128/128 podczas tego postoju. Ta obserwacja dotyczy konkretnego okna, nie dowodzi braku jakichkolwiek wewnętrznych aktualizacji.

Różnice po mikroruchach są zgodne z niedoskonałą powtarzalnością powrotu mechanizmu i potencjometrów. Sam pomiar wartości wysyłanych przez pad nie pozwala przypisać ich wyłącznie mechanice ani wyłącznie elektronicznej korekcji. Zapis mikroruchów zawiera 14 489 raportów bez wykrytych luk i błędów.

## Uruchomienie i znaczenie nazwy autokalibracja

Autokalibracja oznacza kalibrację wykonywaną automatycznie; nazwa nie ogranicza jej do chwili startu. Adaptacyjna opisuje jej dostosowanie do obserwowanego sygnału. W naszym opisie używamy obu pojęć zgodnie z tym znaczeniem.

Sony w *Technical Requirements Checklist* 1.3, §11.10.1, opisuje wbudowaną kalibrację środka i wykrywanie maksymalnego wychylenia DualShocka. Dokument mówi również o wykorzystaniu wartości pobranej przy podłączeniu kontrolera. To dokument dotyczący DS1; nie jest opisem dokładnej implementacji każdej wersji DS2. [Źródło Sony, strona dokumentu 14](https://psx.arthus.net/sdk/Psy-Q/DOCS/TECHNOTE/mtrc13.pdf#page=19).

Nasze próby także nie pozwalają uznać, że start niczego nie zmienia. W PS1 po uruchomieniu z gałką wychyloną i późniejszym puszczeniu LX wynosił 64; po kolejnym uruchomieniu z gałką puszczoną wynosił 128. Osobna próba przy ciągłym zasilaniu wykazała jednak podobną dużą zmianę po samym ruchu, więc tych obserwacji nie wolno przypisać wyłącznie startowi.

W PS2 przed wymianą restart USB zmienił lewą z 138/128 na 126/131, a prawą z 134/108 na 149/141. Jedna zbliżyła się do środka, druga oddaliła. W siedmiu zestawionych uruchomieniach z gałkami deklarowanymi jako puszczone ani razu wszystkie cztery osie nie wynosiły jednocześnie 128.

**Sam start nie gwarantuje idealnego środka, a istotne dostosowanie widać podczas kolejnych ruchów.** Nie ma podstaw, by na tej podstawie ogłaszać brak kalibracji przy podłączeniu lub błędność całej wcześniejszej dokumentacji. Badanie koryguje uproszczenie, że automatyczna kalibracja musi od razu ustawić każdą oś idealnie albo stale dociągać ją do zera w spoczynku.

## Dlaczego analog i jego ścieżka mają znaczenie

Końcowy efekt zależy od współpracy mechanizmu, charakterystyki elektrycznej czujnika i elektroniki kontrolera. Ważne są powtarzalność mechanicznego powrotu, położenie suwaka na ścieżce, jej charakterystyka, dostępny zakres, kontakt elektryczny oraz zgodność modułu z płytą.

Potencjometr w typowym układzie dzielnika daje napięcie zależne od proporcji rezystancji po obu stronach suwaka. Dla idealnego nieobciążonego dzielnika: `Vout / Vcc = R_dolna / (R_górna + R_dolna)`. Sama rezystancja całkowita, np. 10 kΩ, nie opisuje centrowania. Dwa moduły o tej samej rezystancji mogą mieć różne charakterystyki i powroty; inna rezystancja nie musi automatycznie oznaczać gorszego środka, choć musi pasować do rzeczywistego układu wejściowego. [Zasada odczytu potencjometru — Analog Devices](https://wiki.analog.com/university/tools/python-tutorial/tutorial3-potentiometer).

Producenci rozdzielają te parametry. Przykładowy moduł Alps RKJXV1224005 ma osobno określoną rezystancję 10 kΩ i dokładność mechanicznego powrotu ±5°. Nie identyfikujemy go jako modułu użytego w naszych próbach; przykład pokazuje, dlaczego jedna liczba w kΩ nie opisuje jakości centrowania. [Karta produktu Alps Alpine](https://tech.alpsalpine.com/e/products/detail/RKJXV1224005/).

Oznaczenie 60° może mieć znaczenie dla zależności między ruchem a sygnałem, ale **nie zwiększa liczby wartości dostępnych w 8-bitowym raporcie**. Nadal są to 256 kodów. Wpływ kąta elektrycznego lub mechanicznego na czułość w pobliżu środka wymaga charakterystyki konkretnego modułu i uwzględnienia skalowania kontrolera.

Praktyczny wniosek serwisowy jest mocny: dobór i montaż odpowiedniego analogu mają znaczenie, a korekcja wewnętrzna nie gwarantuje usunięcia każdego błędu mechanicznego. Nasza wymiana poprawiła wybrane pomiary na tej samej płycie. Nie ustaliliśmy jednak, który pojedynczy parametr zamiennika odpowiada za całą poprawę ani matematycznie najlepszego możliwego środka.

## Ile oznacza jednostka RAW

Raport ma 256 wartości od 0 do 255, czyli rozpiętość 255 kroków. Geometryczny środek tej skali to 127,5; do porównań w tabelach używamy umownego 128.

Procent pełnego zakresu liczymy jako `100 × różnica / 255`. Dla odchylenia od środka do krańca spotyka się inną normalizację: `100 × różnica / 127,5`. Trzeba wskazać, którą stosujemy.

| Różnica RAW | Procent pełnej rozpiętości 0–255 |
|---:|---:|
| 1 | 0,39% |
| 6 | 2,35% |
| 10 | 3,92% |
| 16 | 6,27% |
| 23 | 9,02% |
| 27 | 10,59% |

W końcowej próbie kierunkowej maksymalne odchylenie osi zmalało więc z 9,02% do 2,35% pełnej rozpiętości. Rozrzut między powrotami oraz odchylenie od nominalnego środka pozostają osobnymi miarami. Sam fakt użycia 8 bitów nie dowodzi, że gry musiały mieć dużą deadzone. Deadzone może ukrywać przesunięcia, ale nie naprawia mechanicznego powrotu.

## Co możemy zmienić przez software

W zbadanym protokole nie zidentyfikowaliśmy udokumentowanego polecenia, które pozwalałoby z PC zapisać własny środek lub trwałą tabelę kalibracji osi badanego pada. Komend konfiguracyjnych przycisków pressure, w tym 0x40, nie należy utożsamiać z zapisem centrum gałki. Ich opis i wcześniejszy przegląd kodu są w [RESEARCH_PL.md](RESEARCH_PL.md), a opis protokołu pressure w [PSX-SPX](https://psx-spx.consoledev.net/ps1/sio/controllersandmemorycards/controllers-analog-buttons-dualshock2/).

Na komputerze lub w adapterze można dodać własną korekcję, deadzone i skalowanie. Zmieni to wartości przekazywane grze, nie będzie jednak dowodem przeprogramowania wewnętrznej kalibracji oryginalnego kontrolera. W torze badawczym zachowaliśmy oryginalne bajty.

Hardware możemy zmieniać bezpośrednio: wymienić moduły, dobrać właściwą charakterystykę i zadbać o powtarzalny powrót. Nasze badanie pokazuje sens tego podejścia. Nie dowodzi absolutnej niemożliwości ingerencji programowej żadną inną metodą.

## USB i identyfikacja kontrolera

Nazwa **Modi PSX Adapter** oraz USB VID/PID należą do firmware adaptera. Nie są automatycznie odczytanym identyfikatorem producenta podłączonego pada. Badany firmware używa testowego 1209:0001.

Identyfikatory PSX 0x41, 0x73 i 0x79 opisują obserwowany format lub tryb odpowiedzi: cyfrowy, analogowy i pressure. Nie są numerem seryjnym ani dowodem oryginalności. Podobnie rozpoznanie rodziny protokołu nie potwierdza fizycznego typu zamontowanych analogów. [Opis formatu i ograniczeń projektu](../README_PL.md).

## Zakres końcowego wniosku

**W badanych egzemplarzach potwierdziliśmy zmianę i późniejszą stabilizację raportowanych środków zależną od historii ruchu. Wyniki mocno wspierają adaptacyjną kalibrację zakresu.** Przejście obu krańców osi — przez okrąg albo kierunki — pozwalało uzyskać bardziej zgodne kolejne powroty. Jakość i dopasowanie analogów wpływały na wynik, a niewielkie różnice powrotu pozostawały również po dostosowaniu.

Nie utożsamiamy tego z odczytaniem wewnętrznego algorytmu. Nie ustaliliśmy jego wzoru, sposobu przechowywania parametrów ani udziału mechaniki w każdej pojedynczej zmianie. Nie badaliśmy w tej sesji analogicznego mechanizmu Xboxa ani trwałości współczesnych padów Sony. Takie porównania wymagają własnych pomiarów.

Główna lekcja pomiarowa: **oceniać wszystkie powroty przed i po poznaniu zakresu, zamiast wyciągać wniosek z jednego końcowego odczytu lub samego restartu.** To właśnie ujawniło zależność w naszych danych.

## Dane i odtwarzalność

[Pakiet dowodowy JSON](research/center-2026-10-11/evidence.json) zawiera zestawienia pomiarów, dokładne okna, parametry prób, ograniczenia, źródłowe nazwy plików oraz ich SHA-256. [Indeks archiwum](research/center-2026-10-11/archive-index.json) opisuje zachowane pliki badania. Pełne zachowane logi RAW, zestawienia i sesje udostępniamy w [archiwach danych](../data/README.md). Pakiet podsumowań nie zastępuje logów. Archiwa zachowują nazwy i bajty oryginalnych plików; indeks umożliwia odtworzenie ich położenia.

| Zapis | Raporty wykorzystane w danym sprawdzeniu | Weryfikacja |
|---|---:|---|
| PS2 po wymianie, pierwsza pełna seria obu gałek | 20 905 | Wszystkie zgodne z niezależnym odczytem i ramką PSX |
| PS2 po wymianie, powtórka obu gałek | 20 903 | Wszystkie zgodne z niezależnym odczytem i ramką PSX |
| Pierwsza ręczna próba okrąg kontra kierunki | 15 774 | Natywny zapis wejściowy, zgodność osi z ramką PSX |
| Ręczna próba z zamienionymi stronami | 16 077 | Natywny zapis wejściowy, zgodność osi z ramką PSX |
| Końcowe porównanie każdego powrotu | 26 330 | Natywny zapis wejściowy, zgodność osi z ramką PSX |
| Mikroruchy | 14 489 | Natywny zapis wejściowy, zgodność osi z ramką PSX |

W powyższych zapisach nie stwierdzono luk sekwencji ani zgłoszonych błędów. Nie rozszerzamy tego stwierdzenia na całe wielogodzinne archiwum: wcześniejsze próby zawierają luki, przerwane serie i powtórki. Zachowano je wraz z opisami. Protokół PSX nie ma sumy kontrolnej, a ACK nie był mierzony, więc zerowe liczniki nie są dowodem wykrycia każdego możliwego błędu elektrycznego.

Niepełne próby nie zostały potraktowane jako pełne serie. Fizycznego położenia gałki nie mierzyliśmy niezależnym czujnikiem. Rejestracja po enumeracji USB nie obejmuje wszystkich pierwszych transakcji startowych; restart USB obejmuje adapter, kontroler i standardową inicjalizację. Dane pochodzą z badania przy komputerze, nie z podsłuchu pracującej konsoli PS2.

Krótki tekst przeznaczony do publikacji znajduje się w [ADAPTIVE_CALIBRATION_POST_PL.md](ADAPTIVE_CALIBRATION_POST_PL.md).

## PS1 a PS2 w tym badaniu

W naszych próbach zmodyfikowany pad PS1 dawał mniej powtarzalne środki niż PS2 po wymianie analogów. Przykładowo LY w wybranych powrotach PS1 przyjmowała 130–137, podczas gdy końcowa seria kierunkowa lewej gałki PS2 po obrocie utrzymywała LY 125 i LX 126–127. To porównanie tych egzemplarzy i prób, nie wszystkich padów PS1 i PS2; PS1 był niezłożony, a warunki oraz historie ruchów różniły się.

