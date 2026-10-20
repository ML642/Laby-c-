#ifndef SOP_L1_ETAP_4_H
#define SOP_L1_ETAP_4_H

/* Zwraca 0, gdy baza zgadza się z indeksem; 1 przy brakach lub różnicach,
 * -1 przy błędzie odczytu albo niepoprawnym formacie bazy. */
int verify_database(const char *database_path, const char *by_title_dir);

#endif
