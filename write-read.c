#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <poll.h>

/*
 * SOP1: READ / WRITE - NOTATKI
 *
 * 1. READ
 * ssize_t read(int fd, void *buf, size_t count);
 *
 * - Próbujemy odczytać maksymalnie count bajtów.
 * - read() NIE gwarantuje odczytania count bajtów.
 *
 * Zwraca:
 *   > 0  liczba faktycznie odczytanych bajtów
 *   = 0  EOF (koniec danych)
 *   = -1 błąd, sprawdzamy errno
 *
 * Przykład:
 *   read(fd, buf, 128) może zwrócić 20.
 *   Przetwarzamy TYLKO buf[0..19].
 *
 * 2. WRITE
 * ssize_t write(int fd, const void *buf, size_t count);
 *
 * - Próbujemy zapisać count bajtów.
 * - write() NIE gwarantuje zapisania count bajtów.
 *
 * Zwraca:
 *   > 0  liczba faktycznie zapisanych bajtów
 *   = 0  brak postępu (trzeba obsłużyć)
 *   = -1 błąd, sprawdzamy errno
 *
 * 3. KIEDY UŻYWAMY PĘTLI?
 *
 * READ:
 * - Gdy chcemy zebrać określoną liczbę bajtów.
 * - Gdy czytamy strumień aż do EOF.
 * - Gdy zbieramy wiadomość aż do '\n'.
 *
 * WRITE:
 * - Gdy musimy zapisać WSZYSTKIE bajty,
 *   powtarzamy write(), aż total == count.
 *
 * 4. TYPOWE BŁĘDY
 *
 * EINTR:
 *   Operację przerwał sygnał przed przesłaniem danych.
 *   Ponawiamy read()/write().
 *
 * EAGAIN / EWOULDBLOCK:
 *   O_NONBLOCK: operacja wymagałaby oczekiwania.
 *   Nie kończymy programu.
 *   Czekamy na odpowiednie zdarzenie w poll().
 *
 * 5. FIFO + POLL
 *
 * poll() NIE czyta danych.
 * poll() informuje, który fd jest gotowy.
 *
 * POLLIN  -> można próbować read().
 * POLLOUT -> można próbować write().
 * POLLHUP -> rozłączenie; mogą zostać dane do odczytu.
 *
 * Przy FIFO odczytanym z O_NONBLOCK:
 * - read() > 0: przetwarzamy dane.
 * - read() == 0: brak pisarzy i brak danych (EOF).
 * - EAGAIN: dane jeszcze nie są dostępne,
 *   ale istnieje otwarty deskryptor piszący.
 *
 * 6. PIPE_BUF
 *
 * Zapis do FIFO <= PIPE_BUF jest atomowy:
 * dane różnych pisarzy nie przeplatają się.
 * Nie gwarantuje to powodzenia operacji.
 */



int main(){
        size_t total = 0;

        while (1) {
        ssize_t rd = read(fd, buf, sizeof(buf));

        if (rd > 0) {
            // Przetwarzamy dokładnie rd bajtów.
            // Przy FIFO możemy zbierać wiadomość do '\n'.
        }
        else if (rd == 0) {
            // EOF: w FIFO brak pisarzy i danych.
            break;
        }
        else if (errno == EINTR) {
            continue;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Nie ma teraz danych.
            // Wracamy do poll(), nie czekamy aktywnie.
            break;
        }
        else {
            ERR("read");
        }
    }

    size_t total = 0;
    size_t count = 64;

    while (total < count) {
        ssize_t wr = write(fd, buf + total, count - total);

        if (wr > 0) {
            total += wr;
        }
        else if (wr == 0) {
            // Brak postępu: nie zapętlamy się bez końca.
            ERR("write");
        }
        else if (errno == EINTR) {
            continue;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Dla O_NONBLOCK: poczekaj na POLLOUT,
            // zachowując wartość total.
            // Nie resetuj total do 0!
            break;
        }
        else {
            ERR("write");
        }
    }
}


// READ + WRITE
while (1) {
    ssize_t rd = read(fd_in, buf, sizeof(buf));

    if (rd == -1) {
        if (errno == EINTR)
            continue;
        ERR("read");
    }

    if (rd == 0)
        break;  // EOF

    size_t written = 0;

    while (written < (size_t)rd) {
        ssize_t wr = write(
            fd_out,
            buf + written,
            (size_t)rd - written
        );

        if (wr == -1) {
            if (errno == EINTR)
                continue;
            ERR("write");
        }

        if (wr == 0) {
            fprintf(stderr, "Brak postępu zapisu\n");
            exit(EXIT_FAILURE);
        }

        written += wr;
    }
}


// READ + WRITE + POLL



int main(int argc, char **argv)
{
    if (argc < 3 || argc > 7) {
        fprintf(stderr,
                "Uzycie: %s recipe fifo1 [fifo2 ... fifo5]\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    /* Przygotowanie pliku przepisu */
    int recipe = open(argv[1], O_RDWR | O_CREAT, 0644);

    if (recipe == -1)
        ERR("open recipe");

    struct stat st;

    if (fstat(recipe, &st) == -1)
        ERR("fstat recipe");

    if (st.st_size == 0) {
        char empty_line[64];

        for (int j = 0; j < 63; j++)
            empty_line[j] = ' ';

        empty_line[63] = '\n';

        for (int j = 0; j < 32; j++)
            write_all(recipe, empty_line, 64);
    }
    else if (st.st_size != 2048) {
        fprintf(stderr, "Ta ksiega to nie przepis!\n");
        return EXIT_FAILURE;
    }

    /* Przygotowanie FIFO i poll */
    int count = argc - 2;
    int active = count;

    struct pollfd fds[MAX_GRANNIES];

    char messages[MAX_GRANNIES][MSG_CAP];
    size_t positions[MAX_GRANNIES] = {0};
    int too_long[MAX_GRANNIES] = {0};

    for (int i = 0; i < count; i++) {
        int fd = open(argv[i + 2], O_RDONLY | O_NONBLOCK);

        if (fd == -1)
            ERR("open FIFO");

        struct stat fifo_st;

        if (fstat(fd, &fifo_st) == -1)
            ERR("fstat FIFO");

        if (!S_ISFIFO(fifo_st.st_mode)) {
            fprintf(stderr,
                    "Nie ma babci na wiezy %s\n",
                    argv[i + 2]);
            return EXIT_FAILURE;
        }

        fds[i].fd = fd;
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }

    /* GLOWNA PETLA POLL */
    while (active > 0) {

        int ready = poll(fds, count, 3000);

        if (ready == -1) {
            if (errno == EINTR)
                continue;

            ERR("poll");
        }

        if (ready == 0) {
            printf("Babcie dolewaja herbatki...\n");
            continue;
        }

        /* Sprawdzamy wszystkie FIFO */
        for (int i = 0; i < count; i++) {

            if (fds[i].fd == -1)
                continue;

            if (fds[i].revents & (POLLERR | POLLNVAL)) {
                fprintf(stderr, "Blad FIFO\n");
                exit(EXIT_FAILURE);
            }

            if (!(fds[i].revents & (POLLIN | POLLHUP)))
                continue;

            /* Odczytujemy dostepne dane z FIFO i */
            while (1) {
                char buf[128];

                ssize_t rd = read(
                    fds[i].fd,
                    buf,
                    sizeof(buf)
                );

                if (rd > 0) {

                    for (ssize_t j = 0; j < rd; j++) {

                        if (buf[j] == '\n') {

                            if (too_long[i]) {
                                fprintf(stderr,
                                        "Babcia sie zagadala: za dluga wiadomosc\n");
                            }
                            else {
                                store_step(
                                    recipe,
                                    messages[i],
                                    positions[i]
                                );
                            }

                            positions[i] = 0;
                            too_long[i] = 0;
                        }
                        else if (!too_long[i]) {

                            if (positions[i] < MSG_CAP) {
                                messages[i][positions[i]] = buf[j];
                                positions[i]++;
                            }
                            else {
                                too_long[i] = 1;
                            }
                        }
                    }

                    continue;
                }

                if (rd == 0) {
                    /* EOF - babcia skonczyla */
                    if (positions[i] > 0 || too_long[i]) {
                        fprintf(stderr,
                                "Babcia sie zagadala: brak konca linii\n");
                    }

                    if (close(fds[i].fd) == -1)
                        ERR("close FIFO");

                    fds[i].fd = -1;
                    active--;

                    printf(
                        "Babcia z wiezy %s skonczyla dyktowac\n",
                        argv[i + 2]
                    );

                    break;
                }

                if (errno == EINTR)
                    continue;

                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;

                ERR("read");
            }
        }
    }

    if (close(recipe) == -1)
        ERR("close recipe");

    return EXIT_SUCCESS;
}