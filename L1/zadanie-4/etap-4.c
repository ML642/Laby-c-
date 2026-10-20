#define _POSIX_C_SOURCE 200809L

#include "etap-4.h"

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

enum { TITLE_BYTES = 64, RECORD_BYTES = 68 };

_Static_assert(sizeof(uint32_t) == 4, "Rozmiar pola rozmiaru musi wynosic 4 bajty");

struct book_record {
    uint32_t expected_size;
    char title[TITLE_BYTES + 1];
};

/* readv() może zwrócić mniej bajtów niż żądano, także w środku rekordu. */
static int read_record(int fd, struct book_record *record)
{
    size_t offset = 0;
    memset(record, 0, sizeof(*record));

    while (offset < RECORD_BYTES) {
        struct iovec parts[2];
        int count = 0;

        if (offset < sizeof(record->expected_size)) {
            parts[count].iov_base = (char *)&record->expected_size + offset;
            parts[count].iov_len = sizeof(record->expected_size) - offset;
            count++;
            parts[count].iov_base = record->title;
            parts[count].iov_len = TITLE_BYTES;
            count++;
        } else {
            parts[count].iov_base = record->title + offset - sizeof(record->expected_size);
            parts[count].iov_len = RECORD_BYTES - offset;
            count++;
        }

        ssize_t n = readv(fd, parts, count);
        if (n == -1) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (n == 0)
            return -2; /* niepełny rekord, np. baza skrócona podczas czytania */
        offset += (size_t)n;
    }

    record->title[TITLE_BYTES] = '\0';
    return 0;
}

static int valid_title(const char title[TITLE_BYTES + 1])
{
    size_t len = strnlen(title, TITLE_BYTES);
    if (len == 0 || strchr(title, '/') || !strcmp(title, ".") || !strcmp(title, ".."))
        return 0;
    for (size_t i = len; i < TITLE_BYTES; i++) {
        if (title[i] != '\0')
            return 0; /* pole krótsze niż 64 bajty musi być dopełnione zerami */
    }
    return 1;
}

static char *index_path(const char *dir, const char *title)
{
    size_t dir_len = strlen(dir);
    size_t title_len = strlen(title);
    if (title_len > SIZE_MAX - 2 || dir_len > SIZE_MAX - title_len - 2) {
        errno = ENOMEM;
        return NULL;
    }
    char *path = malloc(dir_len + title_len + 2);
    if (path == NULL)
        return NULL;
    memcpy(path, dir, dir_len);
    path[dir_len] = '/';
    memcpy(path + dir_len + 1, title, title_len + 1);
    return path;
}

int verify_database(const char *database_path, const char *by_title_dir)
{
    int fd = open(database_path, O_RDONLY);
    if (fd == -1) {
        perror(database_path);
        return -1;
    }

    struct stat database_stat;
    if (fstat(fd, &database_stat) == -1) {
        perror("fstat");
        close(fd);
        return -1;
    }
    if (!S_ISREG(database_stat.st_mode) || database_stat.st_size % RECORD_BYTES != 0) {
        fprintf(stderr, "Niepoprawny format bazy: oczekiwano rekordów po %d bajtów\n",
                RECORD_BYTES);
        close(fd);
        return -1;
    }

    int result = 0;
    off_t records = database_stat.st_size / RECORD_BYTES;
    for (off_t i = 0; i < records; i++) {
        struct book_record book;
        int read_result = read_record(fd, &book);
        if (read_result != 0) {
            if (read_result == -1)
                perror("readv");
            else
                fprintf(stderr, "Niepełny rekord bazy na pozycji %jd\n", (intmax_t)i);
            result = -1;
            break;
        }
        if (!valid_title(book.title)) {
            fprintf(stderr, "Niepoprawny tytuł w rekordzie %jd\n", (intmax_t)i);
            result = -1;
            break;
        }

        char *path = index_path(by_title_dir, book.title);
        if (path == NULL) {
            perror("malloc");
            result = -1;
            break;
        }

        struct stat book_stat;
        if (stat(path, &book_stat) == -1) {
            if (errno == ENOENT || errno == ENOTDIR) {
                if (printf("Book \"%s\" is missing\n", book.title) < 0)
                    result = -1;
                else if (result == 0)
                    result = 1;
            } else {
                perror(path);
                result = -1;
            }
        } else if (!S_ISREG(book_stat.st_mode)) {
            if (printf("Book \"%s\" is missing\n", book.title) < 0)
                result = -1;
            else if (result == 0)
                result = 1;
        } else if (book_stat.st_size != (off_t)book.expected_size) {
            if (printf("Book \"%s\" size mismatch (%" PRIu32 " vs %jd)\n",
                       book.title, book.expected_size, (intmax_t)book_stat.st_size) < 0)
                result = -1;
            else if (result == 0)
                result = 1;
        }
        free(path);
        if (result == -1)
            break;
    }

    if (close(fd) == -1) {
        perror("close");
        result = -1;
    }
    if (fflush(stdout) == EOF) {
        perror("fflush");
        result = -1;
    }
    return result;
}
