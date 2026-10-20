# `errno` i obsługa błędów

[Spis dokumentacji](README.md) · [Funkcje POSIX i C](posix.md) · [Łańcuchy znaków](stringi.md)

## Jak sprawdzać `errno`

1. Najpierw sprawdź **wartość zwróconą** przez funkcję. Samo `errno != 0` nie dowodzi błędu: poprawne wywołanie nie musi zerować `errno`.
2. Gdy wynik wskazuje błąd, odczytaj `errno` od razu, przed innym wywołaniem funkcji. `perror("nazwa")` wypisuje opis aktualnego `errno`.
3. Gdy `NULL` lub liczba może oznaczać zarówno błąd, jak i zwykły koniec danych, użyj dodatkowego wskaźnika: `readdir()` wymaga wyzerowania `errno` przed wywołaniem, a operacje na `FILE *` wymagają `feof()` lub `ferror()`.
4. Nie używaj `perror()` dla błędu składni argumentów, jeśli żadna funkcja nie ustawiła właśnie `errno`. Wtedy wypisz własny komunikat przez `fprintf(stderr, ...)`.

### Koniec katalogu a błąd

```c
errno = 0;
struct dirent *entry = readdir(dir);
if (entry == NULL) {
    if (errno != 0) {
        perror("readdir");  /* błąd */
    } else {
        /* koniec katalogu */
    }
}
```

Jeżeli między `readdir()` a sprawdzeniem `errno` wykonujesz inne operacje, zapisz wartość `errno` natychmiast po `readdir()`. W pętli wyzeruj je **przed każdym** wywołaniem, dla którego trzeba rozróżnić koniec od błędu.

### Nieistniejący plik jako dopuszczalny przypadek

```c
if (unlink(path) == -1 && errno != ENOENT) {
    perror("unlink");
    return EXIT_FAILURE;
}
```

`ENOENT` można pominąć tylko wtedy, gdy brak pliku jest dopuszczalny. Analogicznie `EEXIST` przy `mkdir()` można zaakceptować tylko wtedy, gdy istniejący katalog jest poprawnym wynikiem.

### Konwersja argumentu liczbowego

```c
char *end;
errno = 0;
long value = strtol(text, &end, 10);
if (end == text || *end != '\0' || errno == ERANGE) {
    fprintf(stderr, "Niepoprawna liczba: %s\n", text);
    return EXIT_FAILURE;
}
```

Po tym sprawdź także zakres docelowego typu, np. `int` lub `mode_t`. W obecnych plikach `strtol()` jest zwykle wywoływane bez `endptr`, więc błędny tekst może przejść niezauważony.

### Strumień pliku

```c
size_t n = fread(buffer, sizeof buffer[0], count, file);
if (n < count) {
    if (ferror(file)) {
        perror("fread");
    } else if (feof(file)) {
        /* zwykły koniec pliku */
    }
}
```

`fgets()`, `fscanf()` i `fread()` mogą zakończyć odczyt bez błędu. Przy zapisie sprawdzaj wyniki `fprintf()`, `fputc()` i `fwrite()` oraz wynik `fclose()`, bo błąd zapisu może ujawnić się dopiero przy zamykaniu strumienia.

### Przykład z tutorialu: `stat()` i `chdir()`

Funkcje takie jak `stat()`, `lstat()`, `chdir()`, `mkdir()`, `chmod()`, `link()` i `symlink()` zwracają `0` przy powodzeniu i `-1` przy błędzie. Sprawdzaj konkretny wynik:

```c
struct stat st;
if (stat(path, &st) == -1) {
    perror("stat");
    return EXIT_FAILURE;
}
```

`access()` także zwraca `0` lub `-1`, ale wynik jest tylko bieżącą informacją o dostępie. Między `access()` a późniejszym `fopen()` stan pliku może się zmienić; zawsze sprawdzaj również wynik otwarcia.

### Odczyt bazy w etapie 4

`readv()` zwraca liczbę odczytanych bajtów. `-1` oznacza błąd i pozwala sprawdzić `errno`; `0` oznacza koniec danych, a dodatni wynik mniejszy od żądanej liczby bajtów wymaga kolejnego odczytu. W [`etap-4.c`](../zadanie-4/etap-4.c) pętla składa pełny 68-bajtowy rekord. Przedwczesny koniec rekordu jest błędem **formatu bazy**, a nie kodem `errno`.

Przy sprawdzaniu `index/by-title` wynik `stat() == -1` z `errno == ENOENT` oznacza brak książki lub zerwane dowiązanie. Pozostałe błędy `stat()` są zgłaszane jako błędy operacji na pliku.

### Błędy argumentów i błędy systemowe

`getopt()` sygnalizuje nieznaną opcję najczęściej znakiem `'?'`, a nie przez `errno`. Podobnie `getenv() == NULL` oznacza brak zmiennej. W takich przypadkach wypisz własny komunikat, np. `fprintf(stderr, "Brak opcji -p\n")`.

| Kod | Typowe znaczenie w L1 | Przykład właściwej decyzji |
| --- | --- | --- |
| `ENOENT` | Nie ma wskazanego pliku lub składnika ścieżki. | Po `unlink()` można zaakceptować brak pliku, jeśli celem było zapewnienie, że pliku nie ma. |
| `EEXIST` | Nazwa już istnieje. | Po `mkdir()` można użyć istniejącego katalogu, jeżeli jego istnienie jest dopuszczalne; sprawdź także jego typ. |
| `EACCES` | Brak odpowiednich praw dostępu do ścieżki. | Zgłoś, dla której operacji brakło dostępu. |
| `EPERM` | Operacja zabroniona dla bieżącego procesu. | Przy `chown()` lub `chmod()` zgłoś odmowę uprawnień. |
| `ERANGE` | Za mały bufor albo wynik konwersji poza zakresem. | Przy `getcwd()` zwiększ bufor, przy `strtol()` odrzuć liczbę. |
| `ENOMEM` | Nie udało się przydzielić pamięci. | Zwolnij posiadane zasoby i zakończ albo zwróć błąd. |

To są przykłady, a nie pełna lista kodów dla każdej funkcji. Po otrzymaniu `-1` lub `NULL` sprawdź dokumentację danej funkcji. Zwykły brak dopasowania w `strcmp()`, `strchr()` czy `strstr()` nie jest błędem `errno`; szczegóły są w [dokumencie o łańcuchach](stringi.md).

## Źródła do sprawdzania szczegółów

- [POSIX: wykaz funkcji](https://pubs.opengroup.org/onlinepubs/9699919799/idx/functions.html) — sygnatury, wartości zwracane i pełne listy kodów błędów.
- [POSIX: `errno`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/errno.html) — kiedy wolno odczytywać `errno`.
- [POSIX: `readdir()`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/readdir.html) — rozróżnienie końca katalogu i błędu.
- [POSIX: `getopt()`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/getopt.html) — `'?'`, `':'`, `optarg` i `optind`.
- [Tutorial L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/) — kontekst zadań i przykłady z kursu.
