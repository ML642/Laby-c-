# Łańcuchy znaków i konwersja tekstu

[Spis dokumentacji](README.md) · [Obsługa `errno`](errno.md) · [Funkcje POSIX i C](posix.md)

Funkcje łańcuchowe z `<string.h>` operują na tekstach zakończonych bajtem `'\0'`, a `memcpy()`, `memcmp()` i `memset()` na określonej liczbie dowolnych bajtów. Wskaźnik na zbyt mały bufor lub brak zakończenia łańcucha może prowadzić do niezdefiniowanego działania. Takiego błędu nie wykrywa się przez `errno`. Funkcja `strtol()` pochodzi z `<stdlib.h>`; jest tu, ponieważ konwertuje tekst na liczbę i występuje w kodzie oraz [tutorialu L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/).

## Funkcje użyte w lokalnych plikach

| Funkcja | Co robi | Zwraca; błędy |
| --- | --- | --- |
| `strlen(s)` | Liczy bajty łańcucha przed `'\0'`. | `size_t`. `s` musi wskazywać na poprawnie zakończony łańcuch; `errno` nie służy do wykrywania błędu. |
| `strnlen(s, max)` | Liczy bajty przed `'\0'`, ale czyta najwyżej `max` bajtów; użyta do sprawdzania 64-bajtowego tytułu. | `size_t`: długość lub `max`, gdy zera nie znaleziono; brak `errno` dla takiego przypadku. |
| `strcpy(dst, src)` | Kopiuje `src` wraz z `'\0'` do `dst`. | `dst`. Bufor musi mieć co najmniej `strlen(src) + 1` bajtów; zbyt mały bufor powoduje niezdefiniowane działanie. |
| `strncpy(dst, src, n)` | Kopiuje najwyżej `n` bajtów i dopełnia zerami, gdy `src` jest krótszy. | `dst`. Jeśli pierwsze `n` bajtów `src` nie zawierają `'\0'`, wynik nie będzie zakończony; nie polegaj na `errno`. |
| `strcat(dst, src)` | Dopisuje `src` na końcu `dst`. | `dst`. `dst` musi już zawierać `'\0'` i mieć miejsce na obie części oraz końcowe zero. |
| `strcmp(a, b)` | Porównuje dwa łańcuchy leksykograficznie. | Liczba ujemna, `0` przy równości albo dodatnia. Wartości poza zerem nie mają ustalonej wielkości; `errno` nie służy do sprawdzania wyniku. |
| `strchr(s, ch)` | Szuka pierwszego wystąpienia znaku, także końcowego `'\0'`. | Wskaźnik na znak w `s` albo `NULL`, jeśli brak znaku; `NULL` nie jest błędem `errno`. |
| `strrchr(s, ch)` | Szuka ostatniego wystąpienia znaku, także `'\0'`. | Wskaźnik na znak w `s` albo `NULL`; `NULL` oznacza brak dopasowania. |
| `strstr(s, pattern)` | Szuka pierwszego wystąpienia podłańcucha. | Wskaźnik do `s` albo `NULL`; pusty `pattern` daje wskaźnik na początek `s`. Brak dopasowania nie ustawia `errno` jako błędu. |
| `strtol(text, &end, base)` — `<stdlib.h>` | Zamienia początkową część tekstu na `long` w bazie `base`, np. `8` dla uprawnień ósemkowych. | Liczba. Po `errno = 0` sprawdź `end == text` (brak cyfr), `*end != '\0'` (dodatkowy tekst) i `errno == ERANGE` (przekroczenie zakresu). |
| `memcpy(dst, src, n)` | Kopiuje dokładnie `n` bajtów, także bajty zerowe; etap 4 używa tego przy budowie ścieżki. | `dst`. Zakresy muszą być poprawne i nie mogą się nakładać; błędy pamięci nie są sygnalizowane przez `errno`. |
| `memcmp(a, b, n)` | Porównuje `n` bajtów; parser metadanych rozpoznaje nim klucze. | Liczba ujemna, `0` lub dodatnia; nie wymaga zakończenia `'\0'`. Nie używa `errno`. |
| `memset(dst, byte, n)` | Wypełnia `n` bajtów podaną wartością; etap 4 zeruje strukturę rekordu. | `dst`. Zbyt mały bufor daje niezdefiniowane działanie, a nie błąd `errno`. |

## Funkcja z przykładowego zadania 4

W [przykładowym zadaniu 4](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example4/) pojawia się również `strdup(s)` z `<string.h>`. Tworzy dynamiczną kopię łańcucha. Zwraca nowy `char *` albo `NULL` przy błędzie alokacji (wtedy `errno`, zwykle `ENOMEM`). Kopię należy zwolnić przez `free()`. Nie ma jej w lokalnych plikach `.c`.

## Dwa częste wzorce

Do łączenia ścieżek oblicz długość przed przydzieleniem pamięci i uwzględnij separator oraz `'\0'`. Funkcja `join_paths()` w [`biblioteka.c`](../zadanie-4/biblioteka.c) robi to przez `malloc()`, `strlen()` i `memcpy()`. Sprawdzaj też, czy dodawanie długości nie przekroczy `SIZE_MAX`, jeśli operujesz na niepewnych danych.

Przy `strtol()` sam wynik `0` nie oznacza błędu: tekst `"0"` jest poprawny. Wykorzystaj wskaźnik `end` i `errno`, jak pokazano w [dokumencie o błędach](errno.md#konwersja-argumentu-liczbowego).

## Źródła

- [Tutorial L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/) — konwersja liczb i kontekst ćwiczeń.
- [POSIX: `<string.h>`](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/string.h.html) — deklaracje funkcji łańcuchowych.
- [POSIX: `strtol()`](https://pubs.opengroup.org/onlinepubs/9699919799/functions/strtol.html) — wynik, `endptr` i `ERANGE`.
