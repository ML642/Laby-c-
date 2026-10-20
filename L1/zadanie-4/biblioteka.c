#define _POSIX_C_SOURCE 200809L

#include "etap-4.h"

#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

enum { MAX_TITLE = 64 };

/* Wynik jest nowym blokiem pamięci; wywołujący zwalnia go przez free(). */
char *join_paths(const char *path1, const char *path2)
{
    size_t first = strlen(path1);
    size_t second = strlen(path2);
    int separator = first != 0 && path1[first - 1] != '/';

    if (second > SIZE_MAX - 2 || first > SIZE_MAX - second - 2) {
        errno = ENOMEM;
        ERR("join_paths");
    }
    char *result = malloc(first + second + 2);
    if (result == NULL)
        ERR("malloc");
    memcpy(result, path1, first);
    if (separator)
        result[first++] = '/';
    memcpy(result + first, path2, second + 1);
    return result;
}

void usage(const char *program)
{
    fprintf(stderr, "USAGE: %s [database]\n", program);
    exit(EXIT_FAILURE);
}

/* Czyta tytuł i gatunek. Wartości są obcinane do 64 bajtów, jak nazwy w indeksie. */
void seek_(const char *pathname, char title[MAX_TITLE + 1], char genre[MAX_TITLE + 1])
{
    FILE *file = fopen(pathname, "r");
    if (file == NULL)
        ERR("fopen");

    char *line = NULL;
    size_t capacity = 0;
    ssize_t length;
    title[0] = '\0';
    genre[0] = '\0';

    while ((length = getline(&line, &capacity, file)) != -1) {
        char *colon = strchr(line, ':');
        if (colon == NULL)
            continue;
        size_t key_length = (size_t)(colon - line);
        char *value = colon + 1;
        size_t value_length = (size_t)(line + length - value);
        while (value_length > 0 &&
               (value[value_length - 1] == '\n' || value[value_length - 1] == '\r'))
            value_length--;
        if (value_length > MAX_TITLE)
            value_length = MAX_TITLE;

        char *destination = NULL;
        if (key_length == 5 && memcmp(line, "title", 5) == 0)
            destination = title;
        else if (key_length == 5 && memcmp(line, "genre", 5) == 0)
            destination = genre;
        if (destination != NULL) {
            memcpy(destination, value, value_length);
            destination[value_length] = '\0';
        }
    }

    if (ferror(file))
        ERR("getline");
    free(line);
    if (fclose(file) == EOF)
        ERR("fclose");
}

void seek_fgets(const char *pathname, char title[MAX_TITLE + 1], char genre[MAX_TITLE + 1])
{
    FILE *file = fopen(pathname, "r");
    if (file == NULL)
        ERR("fopen");

    char line[1024];
    title[0] = '\0';
    genre[0] = '\0';

    while (fgets(line, sizeof(line), file) != NULL) {
        int has_non_cr_suffix = 0;
        if (strchr(line, '\n') == NULL) {
            /* Discard the rest of this line, preserving trailing-CR semantics. */
            int character;
            while ((character = fgetc(file)) != EOF && character != '\n') {
                if (character != '\r')
                    has_non_cr_suffix = 1;
            }
        }

        char *colon = strchr(line, ':');
        if (colon == NULL)
            continue;
        size_t key_length = (size_t)(colon - line);
        char *value = colon + 1;
        size_t value_length = strlen(value);
        while (!has_non_cr_suffix && value_length > 0 &&
               (value[value_length - 1] == '\n' || value[value_length - 1] == '\r'))
            value_length--;
        if (value_length > MAX_TITLE)
            value_length = MAX_TITLE;

        char *destination = NULL;
        if (key_length == 5 && memcmp(line, "title", 5) == 0)
            destination = title;
        else if (key_length == 5 && memcmp(line, "genre", 5) == 0)
            destination = genre;
        if (destination != NULL) {
            strncpy(destination, value, value_length);
            destination[value_length] = '\0';
        }
    }

    if (ferror(file))
        ERR("fgets");
    if (fclose(file) == EOF)
        ERR("fclose");
}

void recursivesearch(const char *directory_path, const char *relative_path)
{
    DIR *directory = opendir(directory_path);
    if (directory == NULL)
        ERR("opendir");

    for (;;) {
        errno = 0;
        struct dirent *entry = readdir(directory);
        if (entry == NULL) {
            if (errno != 0)
                ERR("readdir");
            break;
        }
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char *source = join_paths(directory_path, entry->d_name);
        char *relative = join_paths(relative_path, entry->d_name);
        struct stat info;
        if (lstat(source, &info) == -1)
            ERR("lstat");

        if (S_ISDIR(info.st_mode)) {
            recursivesearch(source, relative);
        } else if (S_ISREG(info.st_mode)) {
            char title[MAX_TITLE + 1];
            char genre[MAX_TITLE + 1];
            seek_(source, title, genre);

            char *visible_target = join_paths("../../library", relative);
            char *visible_link = join_paths("index/by-visible-title", entry->d_name);
            if (symlink(visible_target, visible_link) == -1)
                ERR("symlink");
            free(visible_target);
            free(visible_link);

            if (title[0] != '\0') {
                if (strchr(title, '/') != NULL || !strcmp(title, ".") ||
                    !strcmp(title, "..")) {
                    fprintf(stderr, "Niepoprawny tytuł w pliku: %s\n", source);
                    exit(EXIT_FAILURE);
                }
                char *title_link = join_paths("index/by-title", title);
                char *title_target = join_paths("../../library", relative);
                if (symlink(title_target, title_link) == -1)
                    ERR("symlink");
                free(title_link);
                free(title_target);

                if (genre[0] != '\0') {
                    if (strchr(genre, '/') != NULL || !strcmp(genre, ".") ||
                        !strcmp(genre, "..")) {
                        fprintf(stderr, "Niepoprawny gatunek w pliku: %s\n", source);
                        exit(EXIT_FAILURE);
                    }
                    char *genre_dir = join_paths("index/by-genre", genre);
                    if (mkdir(genre_dir, 0777) == -1 && errno != EEXIST)
                        ERR("mkdir");
                    char *genre_link = join_paths(genre_dir, title);
                    char *genre_target = join_paths("../../../library", relative);
                    if (symlink(genre_target, genre_link) == -1)
                        ERR("symlink");
                    free(genre_dir);
                    free(genre_link);
                    free(genre_target);
                }
            }
        }
        free(source);
        free(relative);
    }
    if (closedir(directory) == -1)
        ERR("closedir");
}


void Recursie_search(void)
{
    if (mkdir("index", 0777) == -1)
        ERR("mkdir index");
    if (mkdir("index/by-visible-title", 0777) == -1)
        ERR("mkdir by-visible-title");
    if (mkdir("index/by-title", 0777) == -1)
        ERR("mkdir by-title");
    if (mkdir("index/by-genre", 0777) == -1)
        ERR("mkdir by-genre");
    recursivesearch("library", "");
}

int main(int argc, char **argv)
{
    if (argc > 2)
        usage(argv[0]);

    struct stat library_info;
    if (stat("library", &library_info) == -1)
        ERR("stat library");
    if (!S_ISDIR(library_info.st_mode)) {
        fprintf(stderr, "library nie jest katalogiem\n");
        return EXIT_FAILURE;
    }

    Recursie_search();
    if (argc == 2)
        return verify_database(argv[1], "index/by-title") == 0
                   ? EXIT_SUCCESS : EXIT_FAILURE;
    return EXIT_SUCCESS;
}
