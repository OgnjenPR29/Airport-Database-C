#ifndef STRUKTURA_H_INCLUDED
#define STRUKTURA_H_INCLUDED

#define FBLOKIRANJA 3
#define OZNAKA_KRAJA_DATOTEKE -1

typedef struct {
    int sifraLeta;
    char datum[17];
    char tipAviona[7];
    int trajanjeLeta;
    int udaljenostLeta;
    char mestoDolaska[22];
    int deleted;
} SLOG;

typedef struct Blok {
    SLOG slogovi[FBLOKIRANJA];
} BLOK;

#endif // STRUKTURA_H_INCLUDED
