# Zadanie 4 — biblioteka, etap 4

Program tworzy indeksy z katalogu `library/`. Opcjonalny pierwszy argument uruchamia etap 4: porównanie binarnej bazy książek z indeksem `index/by-title/`. Wymagania pochodzą z [przykładowego zadania 4 laboratorium L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example4/).

## Pliki

- `biblioteka.c` — tworzenie indeksów i wywołanie etapu 4.
- `etap-4.c`, `etap-4.h` — osobny moduł odczytu bazy i porównywania książek.
- `Makefile` — kompilacja obu plików w systemie POSIX.

## Kompilacja i uruchomienie

Uruchom w Linuksie albo WSL, w katalogu `L1/zadanie-4`:

```sh
make
cd /sciezka/do/katalogu/z/library
/sciezka/do/L1/zadanie-4/biblioteka database_correct
```

Program oczekuje `library/` w bieżącym katalogu i tworzy `index/` obok niego. Jeżeli `index/` już istnieje, zgodnie z etapem 2 kończy się błędem. Uruchomienie bez argumentu tworzy tylko indeksy. Argument bazy jest ścieżką względem bieżącego katalogu lub ścieżką bezwzględną.

Każdy rekord bazy ma 68 bajtów: 4 bajty rozmiaru (`unsigned int` w formacie dostarczonej bazy) i 64 bajty tytułu dopełnionego zerami. Moduł używa `readv()` i obsługuje krótsze odczyty. Dla każdego rekordu sprawdza plik przez `index/by-title/<tytuł>`. Wypisuje `Book "<tytuł>" is missing` albo `Book "<tytuł>" size mismatch (<oczekiwany> vs <rzeczywisty>)`. Kod zakończenia wynosi `0` przy pełnej zgodności i `1` przy różnicach lub błędach.

W archiwum startowym znajdują się pliki `database_correct` i `database_wrong`. Drugi zawiera dodatkową książkę i różnicę rozmiaru. Nazwa `database_missing` podana na stronie zadania nie występuje w aktualnym archiwum.
