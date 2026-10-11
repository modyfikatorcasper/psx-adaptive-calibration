# Sprawdziliśmy działanie adaptacyjnej kalibracji w padach PS1 i PS2

Dlaczego po pierwszych ruchach analog wraca do innych wartości, a po przejściu całego zakresu zaczyna centrować znacznie bardziej powtarzalnie? Sprawdziliśmy to na fizycznych kontrolerach, z własnym adapterem Modi PSX Adapter i aplikacją Modi PSX Lab.

Odczytywaliśmy oryginalne bajty wysyłane przez pad: bez deadzone, wygładzania i poprawiania środka w testerze. Badanie objęło PS1 z wymienionymi analogami oraz PS2 przed i po wymianie modułów. Rejestrowaliśmy starty, pełne wychylenia, obroty, powtórki i mikroruchy.

Najważniejsze okazało się porównanie każdego powrotu, a nie tylko ostatniego odczytu z całego testu. Jedna gałka wykonywała obrót, a następnie cztery kierunki. Druga wykonywała cztery kierunki i powtarzała je bez wcześniejszego obrotu.

W końcowej próbie prawa gałka po pierwszych kierunkach wracała kolejno do **107/134, 105/148, 105/121 i 132/121**. Po powtórzeniu kierunków jej powroty skupiały się już na **131–133/122**. Rozrzut spadł z **27 jednostek na każdej osi do 2 na X i 0 na Y**. Lewa po wcześniejszym obrocie wracała po kierunkach do **126–127/125**.

W badanych padach odtworzyliśmy więc zachowanie mocno wskazujące na **adaptacyjną kalibrację zależną od poznanego zakresu ruchu**. Okrąg pozwala przejść krańce obu osi przed kolejnymi ruchami. Przy samych kierunkach etap dostosowania widać w pierwszej serii, a następna daje bardziej zgodne powroty. Nie oznacza to idealnego 128/128 ani poprawy po każdym kolejnym obrocie.

Wymiana analogów w tym samym PS2 poprawiła wybrane pomiary po przejściu zakresu. Przykładowo lewa po okręgach wracała wcześniej do **138/119**, a po wymianie do **129/126**; prawa odpowiednio do **134/108** i **134/124**. To pokazuje znaczenie dobranego hardware, choć historie ruchu tych prób nie były identyczne.

Mechanizm nie usuwa wszystkich różnic wynikających z powrotu analogów. Po mikroruchach widzieliśmy różne wartości w kolejnych przerwach. Po ostatnim puszczeniu odczyty przez **28,3 sekundy** pozostawały nieruchome — w tym oknie nie było dalszego dociągania do 128/128. Sam zapis nie rozdziela wpływu mechaniki i wewnętrznej korekcji.

Autokalibracja i adaptacja nie wykluczają się. Sony już w dokumentacji DualShocka opisywało kalibrację środka przy podłączeniu oraz wykrywanie maksymalnego wychylenia. Nasze pomiary pokazują praktycznie, dlaczego sam start nie gwarantuje idealnego centrum i dlaczego historia ruchu ma znaczenie. Dokumentacja Sony, §11.10.1.

Dla napraw oznacza to, że **dobór analogu ma znaczenie**: jego mechaniczny powrót, charakterystyka ścieżki i zgodność z elektroniką kontrolera wpływają na wynik. Sama rezystancja w kΩ lub oznaczenie kąta nie opisują całego modułu. Raport nadal ma 256 wartości, a 10 jednostek RAW to 3,92% pełnej rozpiętości 0–255.

W zbadanym protokole nie znaleźliśmy udokumentowanej komendy zapisu własnej kalibracji środka osi do pada. Korekcję można dodać po stronie komputera lub adaptera, lecz nie jest to zmiana wewnętrznego algorytmu kontrolera. Hardware możemy zmieniać bezpośrednio — i jego wpływ zobaczyliśmy w pomiarach.

**Ustaliliśmy doświadczalnie, jak adaptacja objawia się w badanych PS1 i PS2: po poznaniu zakresu powroty stają się bardziej zgodne, a końcowy rezultat nadal zależy od analogów.** To opis zmierzonego zachowania, nie deklaracja odczytania kodu Sony ani gwarancja identycznego działania wszystkich rewizji i zamienników.

[Pełny raport, metodologia i dane](ADAPTIVE_CALIBRATION_PL.md).

## PS1 a PS2 w tym badaniu

W naszych próbach zmodyfikowany pad PS1 dawał mniej powtarzalne środki niż PS2 po wymianie analogów. Przykładowo LY w wybranych powrotach PS1 przyjmowała 130–137, podczas gdy końcowa seria kierunkowa lewej gałki PS2 po obrocie utrzymywała LY 125 i LX 126–127. To porównanie tych egzemplarzy i prób, nie wszystkich padów PS1 i PS2; PS1 był niezłożony, a warunki oraz historie ruchów różniły się.

Badania i pomiary: **Kacper Lewandowski, znany też jako modyfikator Modi / modyfikator 89**.
