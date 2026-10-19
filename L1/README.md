# L1 — System plików 1

Пять исходников ниже относятся к [четырём примерным заданиям L1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/). Номер задания указан и в папке, и в имени файла. Это сопоставление по содержимому программы; оно не означает, что решение завершено или правильно работает.

| Задание | Файл | Было | Почему соответствует |
| --- | --- | --- | --- |
| [Zadanie 1](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example1/) | `zadanie-1/zadanie-1.c` | `sopy1_practice/main.c` | Листинг каталогов с размером объектов, параметры `-p` и `-o` |
| [Zadanie 2](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example2/) | `zadanie-2/zadanie-2-attempt1.c` | `sopy_1zadanie/main.c` | Рекурсивное сканирование, расширение `-e`, глубина `-d`, вывод `-o` |
| [Zadanie 2](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example2/) | `zadanie-2/zadanie-2-attempt2.c` | `Lab3/sopy2.c` | Второй вариант сканирования с параметрами `-p`, `-e`, `-d`, `-o` |
| [Zadanie 3](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example3/) | `zadanie-3/zadanie-3-sop-venv.c` | `Lab3/program3.c` | Создание окружения, файл `requirements`, установка и удаление пакетов |
| [Zadanie 4](https://sop.mini.pw.edu.pl/pl/sop1/lab/l1/example4/) | `zadanie-4/zadanie-4-sop-bib-stage0.c` | `sop-bib-stage0.c` | Обход `library`, чтение метаданных, создание индексов и ссылок |

Остальные семь `.c` файлов относятся к туториалу или самостоятельным упражнениям, а не к Zadanie 1–4:

| Файл | Было | Тема |
| --- | --- | --- |
| `tutorial/attempts/l1-2.c` | `sopy_powtorka/main.c` | Туториал: рабочий каталог и подсчёт объектов |
| `tutorial/prog12.c` | `sopy1/fmain.c` | Туториал: создание файла |
| `tutorial/attempts/prog12-attempt2.c` | `sopy_powtorka/program2.c` | Второй черновик создания файла |
| `tutorial/prog13.c` | `sopy1/main.c` | Туториал: буферизация stdout |
| `practice/fseek.c` | `sopy1/fseek.c` | Упражнения с файловыми потоками |
| `practice/prog1b.c` | `sopy1/prog1b.c` | Смешанные упражнения с каталогами и файлами |
| `practice/vodd.c` | `sopy1/vodd.c` | Упражнение с каталогом и правами доступа |

Отдельного `l1-1.c` среди исходников не было. `practice/prog1b.c` содержит похожую функцию `scan_dir()`, но его `main()` выполняет другие упражнения.
