#ifndef OPERACIJE_H_INCLUDED
#define OPERACIJE_H_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include "struktura.h"

FILE *otvoriDatoteku(char *filename);
void obrisiSlogLogicki(FILE *fajl,int sifraLeta);
SLOG *pronadjiSlog(FILE *fajl, int sifraLeta);
char *pronadjiTip(FILE *fajl);
void dodajSlog(FILE *fajl, SLOG *slog);
void ispisiSveSlogove(FILE *fajl);
void obrisiSlogFizicki(FILE *fajl, int sifraLeta);
void kreirajDatoteku(char *filename);
void azurirajSlog(FILE *fajl);
void ispisiSlog(SLOG *slog);
void MestoTip(FILE *fajl);
void MestoTip2(FILE *fajl);
void Ispisi(FILE* fajl, char* mesto[], int zadnji);
void Ispis2(FILE* fajl, char* str);
#endif // OPERACIJE_H_INCLUDED
