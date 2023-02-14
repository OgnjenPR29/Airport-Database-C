#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operacije.h"

int main()
{
    int running = 1;
	int userInput;

	FILE *fajl = NULL;

	while (running) {
		printf("\n\nOdaberite opciju:\n");
		printf("1 - Otvaranje datoteke\n");
		printf("2 - Formiranje datoteke\n");
		printf("3 - Pretraga datoteke\n");
		printf("4 - Unos sloga\n");
		printf("5 - Ispis svih slogova\n");
		printf("6 - Dodavanje zvezdice\n");
		printf("7 - Brisanje sloga (logicko)\n");
		printf("8 - Brisanje sloga (fizicko)\n");
		printf("9 - Najbrzi let\n");
		printf("10 - Mesto i tip\n");
		printf("0 - Izlaz\n");
		if (fajl == NULL) {
			printf("!!! PAZNJA: datoteka jos uvek nije otvorena !!!\n");
		}
		scanf("%d", &userInput);
		getc(stdin);
				switch(userInput) {
			case 1:
				{
					char filename[20];
					printf("Unesite ime datoteke za otvaranje: ");
					scanf("%s", &filename[0]);
					fajl = otvoriDatoteku(filename);
					printf("\n");
					break;
				}
			case 2:
				{
					char filename[20];
					printf("Unesite ime datoteke za kreiranje: ");
					scanf("%s", filename);
					kreirajDatoteku(filename);
					printf("\n");
					break;
				}
			case 3:
				{
					int sifraLeta;
					printf("Unesite sifruLeta trazenog prijema: ");
					scanf("%d", &sifraLeta);
					SLOG *slog = pronadjiSlog(fajl, sifraLeta);
					if (slog == NULL) {
                        printf("Nema tog sloga u datoteci.\n");
					} else {
                        printf("Sif.  Datum            TipAviona   Traj.Leta  Udaljenost   MestoDol\n");
                        ispisiSlog(slog);
                        printf("\n");
					}
					free(slog);
					printf("\n");
					break;
				}
			case 4:
				{
					SLOG slog;

					printf("Sifra Leta: ");
					scanf("%d", &slog.sifraLeta);

					printf("Datum (!!! YYYY-MM-dd-HH:mm !!!)(16 karaktera): ");
					scanf("%s", slog.datum);
					//OVDE UNESITE DATUM U OVOM  OBLIKU YYYY-MM-dd-HH:mm
                    //BEZ RAZMAKA JER NISAM USPEO DA IMPLEMENTIRAM DA MI UZIMA RAZMAK
                    //KAO DEO STRINGA, IZ NEKOG RAZLOGA NE RADI FGETS NITI "%[^\n]s"
                    //fgets(slog.datum, 20, stdin);

					printf("Tip aviona (6 karaktera): ");
					scanf("%s", slog.tipAviona);

					printf("Trajanje leta: ");
					scanf("%d", &slog.trajanjeLeta);

					printf("Udaljenost leta: ");
					scanf("%d", &slog.udaljenostLeta)
					;
					printf("Mesto dolaska: ");
					scanf("%s", slog.mestoDolaska);

					slog.deleted = 0;

					dodajSlog(fajl, &slog);
					printf("\n");
					break;
				}
			case 5:
				{
					ispisiSveSlogove(fajl);
					printf("\n");
					break;
				}
			case 6:
				{
					azurirajSlog(fajl);
                    printf("Fajl azuriran uspesno\n");
					break;
				}
			case 7:
				{
					int sifraLeta;
					printf("Unesite sifru leta sloga za logicko brisanje: ");
					scanf("%d", &sifraLeta);
					obrisiSlogLogicki(fajl, sifraLeta);
					printf("\n");
					break;
				}
			case 8:
				{
					int sifraLeta;
					printf("Unesite sifru leta sloga za FIZICKO brisanje: ");
					scanf("%d", &sifraLeta);
					obrisiSlogFizicki(fajl, sifraLeta);
					printf("\n");
					break;
				}
            case 9:
                {
                    char* str = pronadjiTip(fajl);
                    printf("Najbrzi tip je %s\n", str);
                    break;
                }
            case 10:
                {
                    MestoTip2(fajl);
                    break;
                }

			case 0:
				{
					running = 0;
					if (fajl != NULL) {
						fclose(fajl);
					}
				}
		}
	}


    return 0;
}
