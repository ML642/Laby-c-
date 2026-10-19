# Funkcje POSIX i C — pliki oraz system

[Spis dokumentacji](README.md) · [Obsługa `errno`](errno.md) · [Łańcuchy znaków](stringi.md)

## Funkcje biblioteczne użyte w kodzie

Każda funkcja poniżej występuje w co najmniej jednym pliku `.c`. Kolumna „Wynik / błąd” podaje warunek, który należy sprawdzić przed odczytaniem `errno`. Podane przykłady błędów są praktyczne, a nie pełną listą dopuszczalnych kodów.

Funkcje występujące wyłącznie na [stronie tutorialu L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/) są zebrane w osobnej tabeli poniżej. Funkcje tekstowe opisano w [Łańcuchach znaków](stringi.md).

### Katalogi, ścieżki, pliki i uprawnienia

| Funkcja lub makro | Co robi | Wynik / błąd i `errno` |
| --- | --- | --- |
| `opendir(path)` | Otwiera strumień katalogu. | `DIR *` albo `NULL`; przy `NULL` sprawdź `errno` (np. `ENOENT`, `EACCES`). |
| `readdir(dir)` | Pobiera następny wpis katalogu. | `struct dirent *` albo `NULL`. `NULL` oznacza koniec **lub** błąd: ustaw `errno = 0` przed wywołaniem, potem sprawdź `errno`. |
| `closedir(dir)` | Zamyka strumień katalogu. | `0` albo `-1`; po `-1` sprawdź `errno`. |
| `lstat(path, &st)` | Pobiera dane o obiekcie, bez podążania za końcowym dowiązaniem symbolicznym. | `0` albo `-1`; po `-1` sprawdź `errno`. |
| `S_ISDIR(mode)`, `S_ISREG(mode)`, `S_ISLNK(mode)` | Sprawdzają typ z `st_mode`. | Wynik logiczny; to **makra**, nie funkcje, i nie zgłaszają błędu przez `errno`. |
| `getcwd(buf, size)` | Zapisuje ścieżkę bieżącego katalogu. | Zwraca `buf` albo `NULL`; po `NULL` sprawdź `errno` (np. `ERANGE` przy zbyt małym buforze). |
| `chdir(path)` | Zmienia katalog roboczy procesu. | `0` albo `-1`; po `-1` sprawdź `errno`. |
| `mkdir(path, mode)` | Tworzy katalog. | `0` albo `-1`; `EEXIST` oznacza już istniejący obiekt. |
| `chmod(path, mode)` | Zmienia uprawnienia obiektu wskazanego **ścieżką**. | `0` albo `-1`; po `-1` sprawdź `errno`. Argumentem nie jest `FILE *`. |
| `access(path, mode)` | Sprawdza istnienie lub dostęp według wskazanych flag. | `0` albo `-1`; po `-1` `errno` wyjaśnia przyczynę. Wynik nie gwarantuje, że późniejsze otwarcie się powiedzie. |
| `symlink(target, linkpath)` | Tworzy dowiązanie symboliczne. | `0` albo `-1`; po `-1` sprawdź `errno` (np. `EEXIST`). |
| `unlink(path)` | Usuwa nazwę pliku lub dowiązania. | `0` albo `-1`; `ENOENT` oznacza brak wskazanej ścieżki. |
| `umask(mask)` | Ustawia maskę uprawnień nowych plików w procesie. | Zwraca **poprzednią maskę**; nie zgłasza błędu przez `errno`. |

### Strumienie `FILE *` i formatowany tekst

| Funkcja | Co robi | Wynik / błąd i `errno` |
| --- | --- | --- |
| `fopen(path, mode)` | Otwiera lub tworzy plik. | `FILE *` albo `NULL`; przy `NULL` sprawdź `errno`. Tryb `"w+"` skraca istniejący plik. |
| `fclose(file)` | Opróżnia bufor i zamyka strumień. | `0` albo `EOF`; po `EOF` sprawdź `errno`. Nie ignoruj tej kontroli po zapisie. |
| `fseek(file, offset, whence)` | Przesuwa pozycję w strumieniu. | `0` przy sukcesie; wartość różna od `0` przy błędzie (`-1` w POSIX), wtedy sprawdź `errno`. |
| `ftell(file)` | Odczytuje bieżącą pozycję. | `long` z pozycją albo `-1L`; po `-1L` sprawdź `errno`. |
| `fgets(buf, size, file)` | Czyta najwyżej `size-1` znaków i dodaje `'\0'`. | `buf` albo `NULL`; `NULL` to koniec lub błąd. Rozróżnij przez `feof()` i `ferror()`. |
| `fscanf(file, format, ...)` | Czyta i konwertuje dane według formatu. | Liczba przypisań, `0` przy niedopasowaniu albo `EOF` przy końcu/błędzie przed pierwszym przypisaniem. Przy `EOF` sprawdź `feof()`/`ferror()`. |
| `fread(buf, size, count, file)` | Czyta do `count` elementów po `size` bajtów. | Liczba **pełnych elementów**; wynik mniejszy niż `count` wymaga `feof()`/`ferror()`. Przy błędzie odczytu POSIX ustawia `errno`. |
| `fwrite(buf, size, count, file)` | Zapisuje `count` elementów. | Liczba zapisanych elementów; mniej niż `count` oznacza błąd zapisu, sprawdź `ferror()` i `errno`. |
| `fputc(ch, file)` | Zapisuje jeden znak. | Zapisany znak albo `EOF`; po `EOF` sprawdź `errno`. |
| `printf(format, ...)`, `fprintf(file, format, ...)` | Zapisują sformatowany tekst na stdout lub do strumienia. | Liczba zapisanych znaków albo wartość ujemna; po wyniku ujemnym sprawdź `errno`/stan strumienia. |
| `sprintf(buf, format, ...)` | Zapisuje sformatowany tekst do tablicy. | Liczba znaków albo wartość ujemna; **nie zna rozmiaru bufora**, więc za mała tablica powoduje przepełnienie. Użyj `snprintf()` z rozmiarem. |
| `feof(file)` | Sprawdza wskaźnik końca strumienia. | Niezerowe: osiągnięto koniec; `0`: nie. Samo `feof()` nie zgłasza błędu przez `errno`. |
| `ferror(file)` | Sprawdza wskaźnik błędu strumienia. | Niezerowe: wystąpił błąd I/O; `0`: brak ustawionego wskaźnika błędu. |
| `perror(prefix)` | Wypisuje na stderr `prefix` i tekst bieżącego `errno`. | `void`. Używaj bezpośrednio po funkcji, której wynik potwierdził błąd. |

Przykład `prog13.c` z tutorialu pokazuje, że `printf()` może zatrzymać dane w buforze, szczególnie gdy stdout jest przekierowane do pliku. Komunikaty diagnostyczne wypisuj przez `fprintf(stderr, ...)`; po zapisie do pliku sprawdzaj także wynik `fclose()`.

### Argumenty, pamięć i losowanie

| Funkcja | Co robi | Wynik / błąd i `errno` |
| --- | --- | --- |
| `getopt(argc, argv, spec)` | Pobiera kolejną opcję programu; używa `optarg` i `optind`. | Znak opcji; `-1` po końcu; zwykle `'?'` przy nieznanej opcji lub brakującym argumencie. To nie jest błąd `errno`. |
| `getenv(name)` | Odczytuje zmienną środowiskową. | Wskaźnik na wartość albo `NULL`, jeśli jej nie ma. `NULL` nie jest błędem `errno`. |
| `malloc(size)` | Przydziela pamięć. | Wskaźnik albo `NULL`; po `NULL` POSIX przewiduje `errno` (typowo `ENOMEM`). |
| `free(ptr)` | Zwalnia pamięć przydzieloną dynamicznie. | `void`; dla poprawnego wskaźnika nie zgłasza błędu. |
| `rand()` | Zwraca pseudolosową liczbę. | `int` w zakresie `0..RAND_MAX`; brak standardowego błędu `errno`. |
| `srand(seed)` | Ustawia ziarno generatora `rand()`. | `void`; brak standardowego błędu `errno`. |
| `time(NULL)` | Pobiera czas kalendarzowy. | `time_t` albo `(time_t)-1` przy błędzie; wtedy sprawdź `errno`. |
| `exit(status)` | Kończy proces z kodem `status`. | Nie wraca. `EXIT_SUCCESS` i `EXIT_FAILURE` są stałymi, nie funkcjami. |

`sizeof` jest operatorem języka C. `ERR(source)` jest makrem z plików źródłowych: wywołuje `perror(source)`, wypisuje nazwę pliku i linię, a potem `exit(EXIT_FAILURE)`. Dlatego `ERR` powinno otrzymywać tekstową nazwę **rzeczywiście nieudanej operacji** i nie nadaje się do komunikowania błędu składni.

## Dodatkowe funkcje z tutorialu L1

Poniższych funkcji bibliotecznych nie wywołuje żaden z lokalnych 11 plików `.c`. Są wymienione lub użyte na głównej stronie laboratorium. Tutorial odsyła również do stron manuala `fdopendir()` i `fstatat()`, chociaż objaśnia tam głównie `opendir()`, `stat()` i `lstat()`.

| Funkcja i nagłówek | Co robi | Zwraca; błąd i `errno` |
| --- | --- | --- |
| `stat(path, &st)` — `<sys/stat.h>` | Odczytuje dane obiektu; dla dowiązania symbolicznego opisuje jego **cel**. `lstat()` opisuje samo dowiązanie. | `0` albo `-1`; po `-1` sprawdź `errno`. |
| `fdopendir(fd)` — `<dirent.h>` | Tworzy strumień katalogu z otwartego deskryptora; po sukcesie strumień przejmuje deskryptor, który zamyka `closedir()`. | `DIR *` albo `NULL`; po `NULL` sprawdź `errno` i sam zamknij nadal posiadany deskryptor. |
| `fstatat(fd, path, &st, flags)` — `<sys/stat.h>` | Odczytuje atrybuty względem katalogu wskazanego deskryptorem; z `AT_SYMLINK_NOFOLLOW` opisuje sam symlink. | `0` albo `-1`; po `-1` sprawdź `errno`. |
| `fgetc(file)` — `<stdio.h>` | Czyta jeden znak ze strumienia. | Odczytany znak jako `unsigned char` zamieniony na `int` albo `EOF`; po `EOF` odróżnij koniec od błędu przez `feof()` i `ferror()`. |
| `fputs(text, file)` — `<stdio.h>` | Zapisuje łańcuch bez końcowego `'\0'`. | Wynik nieujemny albo `EOF`; po `EOF` sprawdź stan strumienia i `errno`. |
| `scanf(format, ...)` — `<stdio.h>` | Czyta i konwertuje dane ze standardowego wejścia; odpowiednik `fscanf(stdin, ...)`. | Liczba udanych przypisań, `0` przy niedopasowaniu albo `EOF` przy końcu/błędzie przed pierwszym przypisaniem; przy `EOF` sprawdź `feof(stdin)` lub `ferror(stdin)`. Nie myl ze źle nazwaną własną funkcją `scanf()` w [`l1-2.c`](../tutorial/attempts/l1-2.c). |
| `chown(path, uid, gid)` — `<unistd.h>` | Zmienia właściciela i grupę obiektu; zwykle wymaga odpowiednich uprawnień. | `0` albo `-1`; po `-1` sprawdź `errno`, np. `EPERM`. |
| `link(old, new)` — `<unistd.h>` | Tworzy twarde dowiązanie: drugą nazwę tego samego obiektu. | `0` albo `-1`; po `-1` sprawdź `errno`, np. `EEXIST` lub `EXDEV`. |
| `readlink(path, buf, size)` — `<unistd.h>` | Kopiuje treść dowiązania symbolicznego do bufora. | Liczba skopiowanych bajtów albo `-1`; po `-1` sprawdź `errno`. **Nie dodaje `'\0'`**; gdy wynik równa się `size`, wynik mógł być obcięty. |
| `sleep(seconds)` — `<unistd.h>` | Wstrzymuje proces na określoną liczbę sekund. | `0` po odczekaniu całości albo liczba sekund pozostałych po przerwaniu; ten wynik nie oznacza błędu `errno`. |

Własne funkcje z kodów tutorialu, takie jak `scan_dir()`, `usage()` i `make_file()`, opisano w [spisie funkcji z plików](README.md).

W [przykładowym zadaniu 4](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example4/) pojawiają się ponadto:

| Funkcja i nagłówek | Co robi | Zwraca; błąd i `errno` |
| --- | --- | --- |
| `getline(&line, &capacity, file)` — `<stdio.h>` | Czyta cały wiersz, w razie potrzeby przydzielając lub powiększając bufor. Właścicielem `line` pozostaje wywołujący, który zwalnia go przez `free()`. | Liczba przeczytanych bajtów, także końcowy znak nowej linii, albo `-1`. Po `-1` sprawdź `feof()` i `ferror()`, aby odróżnić koniec pliku od błędu. |
| `readv(fd, iov, count)` — `<sys/uio.h>` | Odczytuje dane z deskryptora do wielu buforów opisanych przez `struct iovec`. | Liczba przeczytanych bajtów, `0` przy końcu danych albo `-1` przy błędzie; po `-1` sprawdź `errno`. Krótki odczyt jest możliwy. |

## Źródła do sprawdzania szczegółów

- [POSIX: wykaz funkcji](https://pubs.opengroup.org/onlinepubs/9699919799/idx/functions.html) — sygnatury, wartości zwracane i pełne listy kodów błędów.
- [POSIX: `errno`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/errno.html) — kiedy wolno odczytywać `errno`.
- [POSIX: `readdir()`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/readdir.html) — rozróżnienie końca katalogu i błędu.
- [POSIX: `getopt()`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/getopt.html) — `'?'`, `':'`, `optarg` i `optind`.
- [Tutorial L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/) — kontekst zadań i przykłady z kursu.
