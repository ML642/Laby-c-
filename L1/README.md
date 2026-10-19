# L1 — System plików 1

Poniższych pięć plików źródłowych odpowiada [czterem przykładowym zadaniom z laboratorium L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/). Numer zadania znajduje się w nazwie katalogu i pliku. Przyporządkowanie wynika z zawartości programów; nie potwierdza poprawności ani kompletności rozwiązań.

| Zadanie | Obecny plik | Poprzednia ścieżka | Uzasadnienie |
| --- | --- | --- | --- |
| [Zadanie 1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example1/) | `zadanie-1/przejsciekatalogu.c` | `sopy1_practice/main.c` | Wypisuje zawartość katalogów i rozmiary obiektów; obsługuje opcje `-p` i `-o`. |
| [Zadanie 2](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example2/) | `zadanie-2/zadanie-2-attempt1.c` | `sopy_1zadanie/main.c` | Przeszukuje katalogi rekurencyjnie; używa opcji `-e`, `-d` i `-o`. |
| [Zadanie 2](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example2/) | `zadanie-2/rekuranjzcnyodczyt.c` | `Lab3/sopy2.c` | Druga wersja programu do przeszukiwania katalogów z opcjami `-p`, `-e`, `-d` i `-o`. |
| [Zadanie 3](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example3/) | `zadanie-3/packetmanager.c` | `Lab3/program3.c` | Tworzy środowisko z plikiem `requirements` oraz dodaje i usuwa pakiety. |
| [Zadanie 4](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example4/) | `zadanie-4/biblioteka.c` | `sop-bib-stage0.c` | Przeszukuje `library`, odczytuje metadane oraz tworzy indeksy i dowiązania symboliczne. |

Pozostałe sześć plików `.c` pochodzi z tutorialu lub ćwiczeń własnych. Nie odpowiadają one zadaniom 1–4.

| Obecny plik | Poprzednia ścieżka | Temat |
| --- | --- | --- |
| `tutorial/attempts/l1-2.c` | `sopy_powtorka/main.c` | Tutorial: katalog roboczy i zliczanie obiektów |
| `tutorial/prog12.c` | `sopy1/fmain.c` | Tutorial: tworzenie pliku |
| `tutorial/attempts/prog12-attempt2.c` | `sopy_powtorka/program2.c` | Druga próba rozwiązania zadania z tworzeniem pliku |
| `practice/fseek.c` | `sopy1/fseek.c` | Ćwiczenia z operacji na strumieniach plikowych |
| `practice/prog1b.c` | `sopy1/prog1b.c` | Ćwiczenia z operacji na katalogach i plikach |
| `practice/vodd.c` | `sopy1/vodd.c` | Ćwiczenie z tworzenia katalogu i uprawnień |

Wśród plików źródłowych nie było osobnego `l1-1.c`. Plik `practice/prog1b.c` zawiera podobną funkcję `scan_dir()`, ale jego `main()` wykonuje inne ćwiczenia.

## Dokumentacja funkcji

Opis [funkcji zdefiniowanych w plikach](funkcje/README.md) jest oddzielony od dokumentów o [`errno` i obsłudze błędów](funkcje/errno.md), [funkcjach POSIX i C](funkcje/posix.md) oraz [łańcuchach znaków](funkcje/stringi.md). Dokumentacja obejmuje też funkcje wymienione na stronie tutorialu L1, których nie ma w lokalnych plikach.
