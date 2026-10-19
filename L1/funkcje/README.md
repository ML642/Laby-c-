# Funkcje użyte w plikach L1

Ten dokument opisuje funkcje zdefiniowane w aktualnych 11 plikach `.c`. Funkcje biblioteczne użyte w tych plikach oraz dodatkowe funkcje z laboratorium opisano w dokumentach tematycznych poniżej. Opisy dotyczą **obecnej wersji kodu**. „`void`” oznacza brak wartości zwracanej, a nie gwarancję sukcesu. Funkcje mogą wypisać wynik, zmienić plik lub katalog roboczy albo zakończyć proces przez makro `ERR`.


## Spis dokumentacji

- [errno i obsługa błędów](errno.md) — kiedy odczytywać `errno` i jak odróżniać koniec danych od błędu.
- [Funkcje POSIX i C](posix.md) — katalogi, pliki, uprawnienia, strumienie i pozostałe funkcje.
- [Łańcuchy znaków](stringi.md) — funkcje tekstowe oraz konwersja liczby z tekstu.

Funkcje z samych przykładów [tutorialu L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/) są oznaczone w dokumentach tematycznych.

## Funkcje napisane w tych plikach

W tabelach „błąd” opisuje to, **co robi obecny kod**, a nie jednolitą konwencję. Większość funkcji pomocniczych ma typ `void` i nie przekazuje wywołującemu statusu błędu.

### Zadanie 1 — [`przejsciekatalogu.c`](../zadanie-1/przejsciekatalogu.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `usage()` | Wypisuje krótką informację o ścieżce. | `void`; nie zgłasza statusu. Nie należy łączyć jej komunikatu z `errno`. |
| `listing(char *ch)` | `ch`: katalog do przeglądania. Rekurencyjnie wypisuje obiekty i rozmiary na stdout albo do pliku wybranego globalnymi zmiennymi. | `void`; przy nieudanym `opendir()` wypisuje `usage()` i wraca, przy części pozostałych błędów kończy proces. Dla `readdir()` trzeba rozróżnić `NULL` od końca katalogu. |
| `main(int argc, char **argv)` | Parsuje powtarzalne `-p` i opcjonalne `-o nazwa`; uruchamia `listing()`. | `int`: zwykle `EXIT_SUCCESS`, przy części błędów `EXIT_FAILURE`. `getopt()` sygnalizuje błędną opcję przez `'?'`, nie przez `errno`. |

### Zadanie 2, próba 1 — [`zadanie-2-attempt1.c`](../zadanie-2/zadanie-2-attempt1.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `usage()` | Wypisuje przykładowe wywołanie. | `void`; nie zgłasza błędu. |
| `scan_dir(char *s, char *s2, int depth, FILE *file)` | `s`: katalog; `s2`: opcjonalne rozszerzenie; `depth`: limit rekurencji; `file`: strumień wyniku lub `NULL` dla stdout. Wypisuje ścieżki i rozmiary. | `void`; `opendir()`, `lstat()` i `closedir()` mogą ustawić `errno`. Kod używa `ERR` przy części błędów, a nie zwraca statusu. |
| `main(int argc, char **argv)` | Parsuje `-p`, `-e`, `-d`, `-o`, pobiera `L1_OUTPUTFILE` i uruchamia `scan_dir()`. | `int`: `EXIT_SUCCESS` albo `EXIT_FAILURE` przy części błędów. `getenv() == NULL` oznacza brak zmiennej, nie błąd opisany przez `errno`. |

### Zadanie 2, próba 2 — [`rekuranjzcnyodczyt.c`](../zadanie-2/rekuranjzcnyodczyt.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `usage()` | Wypisuje informację o wymaganej ścieżce. | `void`; nie przekazuje błędu. |
| `listing(char *ch, char *restrict1[500], int depth, FILE *fl)` | `ch`: katalog; `restrict1`: rozszerzenia; `depth`: bieżąca głębokość; `fl`: opcjonalny plik wyniku. Przegląda katalogi i filtruje pliki. | `void`; brak `opendir()` powoduje komunikat i powrót, część innych błędów kończy proces. W pętli `readdir()` trzeba sprawdzać `errno` bezpośrednio po wyniku `NULL`. |
| `main(int argc, char **argv)` | Parsuje `-p`, `-e`, `-d`, `-o`; dla `-o` otwiera plik wskazany przez `L1_OUTPUTFILE`. | `int`: `EXIT_SUCCESS` lub `EXIT_FAILURE` przy części błędów. Błędy składni `getopt()` nie pochodzą z `errno`. |

### Zadanie 3 — [`packetmanager.c`](../zadanie-3/packetmanager.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `install(char *s1, char *s2, int size)` | `s1`: nazwa pakietu, `s2`: wersja, `size`: liczba losowych liter. Tworzy plik pakietu i dopisuje wpis do `requirements`. | `void`; przy duplikacie tylko wypisuje komunikat i wraca. Wywołanie `perror()` w tej gałęzi może użyć niepowiązanego `errno`. Wyniki części operacji plikowych nie są sprawdzane. |
| `deleteq(char *s1)` | Usuwa plik pakietu `s1` i przepisuje `requirements` bez jego wpisu. | `void`; część błędów kończy proces przez `ERR`. Warunek `unlink(s1) && errno == ENOENT` jest błędny: należy sprawdzić `unlink(...) == -1` i zdecydować, czy `ENOENT` wolno pominąć. |
| `main(int argc, char **argv)` | Obsługuje `-c`, `-v`, `-i` oraz użyte w kodzie `-d`; zmienia katalog roboczy i wywołuje funkcje pakietów. | `int`: zwraca `EXIT_FAILURE` przy źle zapisanym pakiecie i `EXIT_SUCCESS` na końcu; wiele błędów `mkdir()`, `chdir()` i `fopen()` nie jest sprawdzanych. |

### Zadanie 4 — [`biblioteka.c`](../zadanie-4/biblioteka.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `join_paths(const char *path1, const char *path2)` | Łączy dwie części ścieżki, wstawiając `/`, jeśli potrzeba. | Nowy `char *`; wywołujący musi użyć `free()`. Przy nieudanym `malloc()` makro `ERR` kończy proces; funkcja nie zwraca `NULL`. |
| `usage(int argc, char **argv)` | Wypisuje składnię programu. | `void`, ale kończy proces przez `exit(EXIT_FAILURE)`; nie wraca. Nie jest wywoływana z `main()`. |
| `seek_(char *dirname, char *title2, char *genre2)` | Czyta metadane z pliku `dirname`, wypisuje autora, tytuł i gatunek oraz kopiuje dwa ostatnie do buforów. | `void`; `fopen()` lub `fclose()` może doprowadzić do `ERR`. Brak pola nie jest błędem `errno`. Bufory wyjściowe nie mają przekazanych rozmiarów. |
| `recursivesearch(char *dir, char *linpath, char *pathtofile, char *linpathtitle, char *linpathgenre)` | Rekurencyjnie przegląda `dir`, buduje ścieżki i tworzy dowiązania w indeksach. | `void`; przy błędach `opendir()`, `lstat()`, `symlink()` lub `closedir()` kończy proces przez `ERR`. Pojedyncze `readdir() == NULL` wymaga rozróżnienia końca od błędu. |
| `Recursie_search()` | Tworzy `index` i podkatalogi indeksów, po czym wywołuje `recursivesearch()`. | `void`; niektóre błędy `mkdir()` i `chdir()` kończą proces. Istniejący `index` jest tu błędem (`EEXIST`). |
| `main(int argc, char **argv)` | Sprawdza możliwość wejścia do `library` i uruchamia tworzenie indeksów. | `int`: `EXIT_SUCCESS` lub zakończenie przez `ERR`. Argumenty `argc` i `argv` obecnie nie sterują działaniem. |

### Tutorial — [`l1-2.c`](../tutorial/attempts/l1-2.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `scanf()` | Przegląda bieżący katalog i zlicza typy obiektów. | `void`; przy błędach wypisuje `perror()` i wraca bez statusu. Nazwa i deklaracja kolidują ze standardową `scanf()` z `<stdio.h>`. Zwykłe pliki błędnie zwiększają licznik katalogów. |
| `main(int argc, char **argv)` | Zapamiętuje bieżącą ścieżkę; dla każdego argumentu zmienia katalog, wywołuje lokalną `scanf()` i wraca. | `int`: `EXIT_FAILURE` po nieudanym `getcwd()` lub `chdir()`, inaczej `EXIT_SUCCESS`. Konflikt nazwy `scanf` uniemożliwia poprawną kompilację tej wersji. |

### Tutorial — [`prog12.c`](../tutorial/prog12.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `make_file(char *name, int size, mode_t permissions, int t)` | Ustawia `umask`, otwiera plik `name` i wykonuje próby zapisu liter w losowych pozycjach; `t` jest procentem prób. | `void`; przy błędach `fopen()`, `fseek()` lub `fclose()` wywołuje `ERR`. Wynik `fprintf()` nie jest sprawdzany. `umask()` zwraca poprzednią maskę i nie zgłasza błędu. |
| `main(int argc, char **argv)` | Czyta `-n`, `-p`, `-s`, inicjuje generator przez `srand(time(NULL))` i wywołuje `make_file()`. | `int`; przy brakującej nazwie lub niedodatnim rozmiarze zwraca `EXIT_FAILURE`, a dojście do końca `main()` bez `return` oznacza sukces (`0`). |

### Tutorial, druga próba — [`prog12-attempt2.c`](../tutorial/attempts/prog12-attempt2.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `usage()` | Wypisuje składnię i wywołuje `ERR("wrong usage")`. | `void`, ale kończy proces. `perror()` w `ERR` może tu pokazać stare, niepowiązane `errno`. |
| `record(int size, char *name, mode_t params)` | Próbuje utworzyć plik i zapisać litery w losowych miejscach. | `void`; kod nie kompiluje się poprawnie: `chmod(fl, 0777)` dostaje `FILE *` zamiast ścieżki, a `ERR(fl)` przekazuje niepoprawny typ do `perror()`. Poprawny wzorzec to `chmod(name, mode) == -1` i sprawdzenie `errno`. |
| `main(int argc, char **argv)` | Odczytuje `-n`, `-p`, `-s`, częściowo sprawdza argumenty i wywołuje `record()`. | `int`: na końcu `EXIT_SUCCESS`; nie wszystkie błędne argumenty są odrzucane. |

### Ćwiczenia — [`fseek.c`](../practice/fseek.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `get(char *s)` | Czyta plik tekstowy `s` i wypisuje wiersze z numerami. | `void`; przy błędzie `fopen()` lub `fclose()` kończy proces. Po `fgets() == NULL` należałoby sprawdzić `ferror()`, aby odróżnić błąd od końca. |
| `fseek_(char *c)` | Otwiera `c`, wypisuje jego rozmiar i znak odczytany po przesunięciu pozycji. | `void`; sprawdza część wyników `fopen()`, `fseek()`, `ftell()`, `fclose()`. `ftell() == -1L` oznacza błąd; `errno` ma sens po tym wyniku. |
| `create(char *s)` | Zapisuje pięć liczb całkowitych i próbuje odczytać dziesięć elementów. | `void`; bufor ma miejsce tylko na trzy liczby, więc `fread()` może go nadpisać. Przed analizą `errno` trzeba naprawić rozmiar bufora i sprawdzać liczby elementów zwrócone przez `fwrite()` i `fread()`. |
| `rawscan()` | Tworzy `NEW_FILE`, zapisuje dane, wywołuje `unlink()` przy nadal otwartym pliku i próbuje czytać. | `void`; wyniki wielu operacji są ignorowane. `strlen(some)` jest niebezpieczne, bo `some` nie musi zawierać końcowego `'\0'`. |

`get()`, `fseek_()` i `create()` są w tym samym pliku zdefiniowane **dwukrotnie**. Plik nie kompiluje się bez usunięcia duplikatów.

### Ćwiczenia — [`prog1b.c`](../practice/prog1b.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `scan_dir(void)` | Wypisuje nazwę i typ każdego obiektu w bieżącym katalogu. | `void`; przy nieudanym `opendir()`, `lstat()` lub `closedir()` kończy proces przez `ERR`. Dla `readdir()` zeruje i sprawdza `errno`. |
| `tellsize()` | Odczytuje pozycję na końcu `numbers.txt` przez `ftell()` i wypisuje rozmiar. | `void`; przy części błędów kończy proces. `ftell() == -1L` oznacza błąd, a `fseek()` powinno zwrócić `0`. |
| `main()` | Czyta i modyfikuje `numbers.txt`, wywołuje `get()`, `tellsize()` i `create()`. | `int`: zwykle `EXIT_SUCCESS`, przy części błędów `EXIT_FAILURE`. Ponieważ plik dołącza `fseek.c` przez `#include`, powtórne definicje uniemożliwiają kompilację. |

### Ćwiczenia — [`vodd.c`](../practice/vodd.c)

| Funkcja | Parametry i działanie | Zwraca; błędy i `errno` |
| --- | --- | --- |
| `mk(char *path)` | Tworzy katalog `path`, przechodzi do niego, tworzy `file.txt`, zmienia uprawnienia i sprawdza dostęp. | `void`; nie sprawdza wyników `mkdir()`, `chdir()`, `fopen()`, `chmod()`, `access()` ani `fclose()`. `errno` należy czytać dopiero po sprawdzeniu właściwego wyniku. W pliku nie ma `main()`. |

## Funkcje własne w przykładach tutorialu L1

Te funkcje są pokazane na [głównej stronie laboratorium](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/) w kodach `l1-1.c`, `l1-2.c`, `prog12.c` i `prog13.c`. Poniżej opisano **wersje ze strony**, które mogą się różnić od funkcji o tej samej nazwie w lokalnych plikach.

| Przykład i funkcja | Co robi | Zwraca; błędy |
| --- | --- | --- |
| `l1-1.c` / `scan_dir()` | Zlicza typy wpisów w bieżącym katalogu i wypisuje podsumowanie. | `void`; błędy `opendir()`, `lstat()`, `readdir()` i `closedir()` kończą proces przez `ERR`. Przed `readdir()` zeruje `errno`. |
| `l1-1.c` / `main()` | Wywołuje `scan_dir()`. | `int`: `EXIT_SUCCESS`, jeżeli skanowanie się uda; błędy kończą proces przez `ERR`. |
| `l1-2.c` / `scan_dir()` | Jak wyżej: wypisuje liczbę plików, katalogów, dowiązań i innych obiektów w katalogu roboczym. | `void`; przy błędzie wywołuje `ERR`. |
| `l1-2.c` / `main(argc, argv)` | Zapamiętuje katalog roboczy, kolejno odwiedza katalogi podane jako argumenty, skanuje je i wraca do katalogu początkowego. | `int`: `EXIT_SUCCESS` po przetworzeniu argumentów; `getcwd()` i `chdir()` przy błędzie prowadzą do `ERR`. |
| `prog12.c` / `usage(pname)` | Wypisuje składnię programu. | Formalnie `void`, ale kończy proces przez `exit(EXIT_FAILURE)`. Błąd składni nie pochodzi z `errno`. |
| `prog12.c` / `make_file(name, size, perms, percent)` | Ustawia `umask`, tworzy plik i zapisuje litery w losowych miejscach. | `void`; przy błędach `fopen()`, `fseek()` i `fclose()` wywołuje `ERR`. Nie sprawdza wyniku `fprintf()`. |
| `prog12.c` / `main(argc, argv)` | Parsuje nazwę, prawa i rozmiar, usuwa poprzedni plik i uruchamia `make_file()`. | `int`: `EXIT_SUCCESS`; błędne argumenty prowadzą do `usage()`, a nieudane operacje systemowe do `ERR`. |
| `prog13.c` / `main()` | Wypisuje liczby od 0 do 14 w odstępach około jednej sekundy, pokazując buforowanie wyjścia. | `int`: `EXIT_SUCCESS`; przykładowy kod nie sprawdza wyników `printf()` ani `sleep()`. |

## Źródła do sprawdzania szczegółów

- [POSIX: wykaz funkcji](https://pubs.opengroup.org/onlinepubs/9699919799/idx/functions.html) — sygnatury, wartości zwracane i pełne listy kodów błędów.
- [POSIX: `errno`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/errno.html) — kiedy wolno odczytywać `errno`.
- [POSIX: `readdir()`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/readdir.html) — rozróżnienie końca katalogu i błędu.
- [POSIX: `getopt()`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/getopt.html) — `'?'`, `':'`, `optarg` i `optind`.
- [Tutorial L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/) — kontekst zadań i przykłady z kursu.
