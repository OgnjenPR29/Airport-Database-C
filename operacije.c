#include "operacije.h"
#include <string.h>
#include <io.h>
#include <stdbool.h>

FILE *otvoriDatoteku(char *filename) {
    FILE *fajl = fopen(filename, "rb+");
    if (fajl == NULL){
        printf("Doslo je do greske prilikom otvaranja %s fajla \n", filename);
    }
    else {
        printf("Datoteka %s je uspesno otvorena", filename);
    }
    return fajl;
}

void kreirajDatoteku(char *filename) {
	FILE *fajl = fopen(filename, "wb");
	if (fajl == NULL) {
		printf("Doslo je do greske prilikom kreiranja datoteke \"%s\"!\n", filename);
	} else {
		//upisi pocetni blok
		BLOK blok;
		blok.slogovi[0].sifraLeta = OZNAKA_KRAJA_DATOTEKE;
		fwrite(&blok, sizeof(BLOK), 1, fajl);
		printf("Datoteka \"%s\" uspesno kreirana.\n", filename);
		fclose(fajl);
	}
}


char *pronadjiTip(FILE *fajl){

	if (fajl == NULL) {
		return NULL;
	}

	fseek(fajl, 0, SEEK_SET);
	BLOK blok;
    //SLOG *max = (SLOG *)malloc(sizeof(SLOG));
    //SLOG max;
    float brzina = 0.00;
    char* najbrzi = malloc(sizeof(char)*7);

	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {

            if(blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE){
               // printf("%s\n", najbrzi);
                return najbrzi;
            }

			if ((blok.slogovi[i].udaljenostLeta*1.00)/(blok.slogovi[i].trajanjeLeta*1.00) > brzina && !blok.slogovi[i].deleted) {
               //printf("uslo je u if");
               /* memcpy(max, &blok.slogovi[i], sizeof(SLOG));
                brzina = (blok.slogovi[i].udaljenostLeta*1.00)/(blok.slogovi[i].trajanjeLeta*1.00);*/
                strcpy(najbrzi, blok.slogovi[i].tipAviona);
                //printf("%s  ", najbrzi);
                brzina = (blok.slogovi[i].udaljenostLeta*1.00)/(blok.slogovi[i].trajanjeLeta*1.00);
                //printf("%2f\n", brzina);

			}
		}
	}

	//najbrzi = max->tipAviona;
	return najbrzi;

}

SLOG *pronadjiSlog(FILE *fajl, int sifraLeta){

	if (fajl == NULL) {
		return NULL;
	}

	fseek(fajl, 0, SEEK_SET);
	BLOK blok;

	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {
			if (blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE) {
				//nema vise slogova
				return NULL;
			}

			if ((blok.slogovi[i].sifraLeta == sifraLeta) && !blok.slogovi[i].deleted) {
                //Ako se sifraLeta poklapa i slog NIJE logicki obrisan
                //(logicki obrisane slogove tretiramo kao da i ne
                //postoje u datoteci).

				SLOG *slog = (SLOG *)malloc(sizeof(SLOG));
				memcpy(slog, &blok.slogovi[i], sizeof(SLOG));
				return slog;
			}
		}
	}

	return NULL;

}

void dodajSlog(FILE *fajl, SLOG *slog) {
	if (fajl == NULL) {
		printf("Datoteka nije otvorena!\n");
		return;
	}

	SLOG *slogStari = pronadjiSlog(fajl, slog->sifraLeta);
	if (slogStari != NULL) {
        //U datoteci vec postoji slog sa tom sifrom leta,
        //pa ne mozemo upisati novi slog.
        printf("Vec postoji slog sa tom sifrom leta!\n");
        return;
    }

	BLOK blok;
	fseek(fajl, -sizeof(BLOK), SEEK_END); //u poslednji blok upisujemo novi slog
	fread(&blok, sizeof(BLOK), 1, fajl);

	int i;
	for (i = 0; i < FBLOKIRANJA; i++) {
		if (blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE) {
            //Ovo je mesto gde se nalazi slog sa oznakom
            //kraja datoteke; tu treba upisati novi slog.
			memcpy(&blok.slogovi[i], slog, sizeof(SLOG));
			break;
		}
	}

	i++; //(na to mesto u bloku cemo upisati krajDatoteke)

	if (i < FBLOKIRANJA) {
        //Jos uvek ima mesta u ovom bloku, mozemo tu smestiti slog
        //sa oznakom kraja datoteke.
		blok.slogovi[i].sifraLeta = OZNAKA_KRAJA_DATOTEKE;

		//Sada blok mozemo vratiti u datoteku.
		fseek(fajl, -sizeof(BLOK), SEEK_CUR);
		fwrite(&blok, sizeof(BLOK), 1, fajl);
		fflush(fajl);
	} else {
		//Nema vise mesta u tom bloku, tako da moramo
        //praviti novi blok u koji cemo upisati slog
        //sa oznakom kraja datoteke.

		//Prvo ipak moramo vratiti u datoteku blok
        //koji smo upravo popunili:
		fseek(fajl, -sizeof(BLOK), SEEK_CUR);
		fwrite(&blok, sizeof(BLOK), 1, fajl);

		//Okej, sad pravimo novi blok:
		BLOK noviBlok;
		noviBlok.slogovi[0].sifraLeta = OZNAKA_KRAJA_DATOTEKE;
		//(vec smo na kraju datoteke, nema potrebe za fseekom)
		fwrite(&noviBlok, sizeof(BLOK), 1, fajl);
	}

	if (ferror(fajl)) {
		printf("Greska pri upisu u fajl!\n");
	} else {
		printf("Upis novog sloga zavrsen.\n");
	}
}

void ispisiSlog(SLOG *slog){
    printf("%d   %16s  %6s  %d          %d        %s",
           slog->sifraLeta,
           slog->datum,
           slog->tipAviona,
           slog->trajanjeLeta,
           slog->udaljenostLeta,
           slog->mestoDolaska
           );
}

void MestoTip(FILE *fajl){

    if (fajl == NULL){
        printf("Datoteka nije otvorena\n");
        return;
    }

    fseek(fajl, 0, SEEK_SET);
    BLOK blok;
    char mesto[22];

    while(fread(&blok, sizeof(BLOK), 1, fajl)){

        for(int i = 0; i < FBLOKIRANJA; i++){
            if(blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE){
                break;
            }
            if(blok.slogovi[i].mestoDolaska[strlen(blok.slogovi[i].mestoDolaska)-1] == '*'){
                strcpy(mesto, blok.slogovi[i].mestoDolaska);
                mesto[strlen(mesto)-1] = '\0';
                printf("\nZa mesto %s: ", mesto);


            }
            else{
                strcpy(mesto, blok.slogovi[i].mestoDolaska);
                printf("\nZa mesto %s: ", mesto);
            }

            for(int j = 0; j < FBLOKIRANJA; j++){
               /* if(blok.slogovi[j].sifraLeta == OZNAKA_KRAJA_DATOTEKE){
                break;
            }*/
                if(strcmp(blok.slogovi[j].mestoDolaska, mesto) == 0){
                    printf("%s  ", blok.slogovi[j].tipAviona);

                }
            }
        }
    }
}

void Ispisi(FILE *fajl, char* mesto[], int zadnji){

    if (fajl == NULL){
        printf("Datoteka nije otvorena\n");
        return;
    }

    fseek(fajl, 0, SEEK_SET);
    BLOK blok;

    int length = 0;
    char str[22];
    char zvezdica = '*';
    char str2[22];

while(fread(&blok, sizeof(BLOK), 1, fajl)){

    for(int p = 0; p < FBLOKIRANJA; p++){

        if(blok.slogovi[p].sifraLeta == OZNAKA_KRAJA_DATOTEKE){
            break;
        }
       // printf("BBB\n");
        //printf("%d\n", zadnji);
        //printf("%s", mesto[0]);
        for(int q = 0; q < zadnji; q++){

            length = strlen(blok.slogovi[p].mestoDolaska)-1;
            strcpy(str2, blok.slogovi[p].mestoDolaska);

            if(str2[length] == zvezdica){
                strcpy(str, blok.slogovi[p].mestoDolaska);
                str[length] = '\0';
            }

            else{
                strcpy(str, blok.slogovi[p].mestoDolaska);
            }
            //printf("uslo je");
            printf("%s \n", &mesto[q]);

            /*if(strcmp(&mesto[q], str) == 0){
                printf("%s ", blok.slogovi[p].tipAviona);
            }*/

        }
    }

}

}

void Ispis2(FILE *fajl, char* mesto){

fseek(fajl, 0, SEEK_SET);
BLOK blok;

  int length = 0;
    char str[22];
    char zvezdica = '*';
    char str2[22];
    char str3[100][22];
    int count = 0;


  while(fread(&blok, sizeof(BLOK), 1, fajl)){


          for(int i = 0; i < FBLOKIRANJA; i++){
                if(blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE){
                    break;
                }
            length = strlen(blok.slogovi[i].mestoDolaska)-1;
            strcpy(str2, blok.slogovi[i].mestoDolaska);

            if(str2[length] == zvezdica){
                strcpy(str, blok.slogovi[i].mestoDolaska);
                str[length] = '\0';
            }

            else{
                strcpy(str, blok.slogovi[i].mestoDolaska);
            }

            if(strcmp(mesto, str)==0 && !blok.slogovi[i].deleted){
                //strcat(str3, blok.slogovi[i].tipAviona);
                //printf("\n Za mesto %s", mesto);
                printf("%s ", blok.slogovi[i].tipAviona);


            }

          }


}
}

void MestoTip2(FILE *fajl){

    //typedef enum {false, true} bool;

    if (fajl == NULL){
        printf("Datoteka nije otvorena\n");
        return;
    }

    fseek(fajl, 0, SEEK_SET);
    BLOK blok;

    char mesto[100][22];
    int zadnji = 0;
    bool found;
    int length = 0;
    char str[22];
    char zvezdica = '*';
    char str2[22];

    while(fread(&blok, sizeof(BLOK), 1, fajl)){

          for(int i = 0; i < FBLOKIRANJA; i++){
                if(blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE){
                   // printf("%s\n", mesto[0]);
                    //Ispisi(fajl, mesto, zadnji);
                    //fseek(fajl, 0, SEEK_SET);
                    for(int f = 0; f < zadnji; f++){
                        printf("\nZa %s: ", mesto[f]);
                        Ispis2(fajl, mesto[f]);
                    }
                    //Ispisi(fajl, mesto, zadnji);
                    //printf("\n");
                }

                found = false;

                length = strlen(blok.slogovi[i].mestoDolaska)-1;
                //printf("%d ", length);
                strcpy(str2, blok.slogovi[i].mestoDolaska);
                //printf("%s\n", str2);

                if(str2[length] == zvezdica){
                    strcpy(str, blok.slogovi[i].mestoDolaska);
                    str[length] = '\0';
                }

                else{
                    strcpy(str, blok.slogovi[i].mestoDolaska);
                }

                for(int j = 0; j < zadnji; j++){//provera da li ima neki grad sa istim imenom

                    if(strcmp(mesto[j], str) == 0){
                        found = true;
                    }
                }
                if(!found){//u slucaju da nije nasao nijedan koji se podudara
                    strcpy(mesto[zadnji], str); //smestamo u matricu mesto
                    zadnji++;
                }

          }

    }
  /*  for(int z = 0; z < zadnji; z++){
        printf("%s, ", mesto[z]);
    }*/

    }



void ispisiSveSlogove(FILE *fajl) {
	if (fajl == NULL) {
		printf("Datoteka nije otvorena!\n");
		return;
	}

	fseek(fajl, 0, SEEK_SET);
	BLOK blok;
	int rbBloka = 0;
	printf("BL SL Sif   Datum             Tip     Traj.Leta   Udaljenost  MestoDol\n");
	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {
			if (blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE) {
				printf("B%d S%d *\n", rbBloka, i);
                break; //citaj sledeci blok
			}


			if (!blok.slogovi[i].deleted) {
                printf("B%d S%d ", rbBloka, i);
                ispisiSlog(&blok.slogovi[i]);
                printf("\n");
            }
		}

		rbBloka++;
	}
}

void obrisiSlogLogicki(FILE *fajl,int sifraLeta) {
	if (fajl == NULL) {
		printf("Datoteka nije otvorena!\n");
		return;
	}

	fseek(fajl, 0, SEEK_SET);
	BLOK blok;
	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {

            if (blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE) {
                printf("Nema tog sloga u datoteci\n");
                return;
            }

			if (blok.slogovi[i].sifraLeta == sifraLeta && !blok.slogovi[i].deleted) {

				blok.slogovi[i].deleted = 1;
				fseek(fajl, -sizeof(BLOK), SEEK_CUR);
				fwrite(&blok, sizeof(BLOK), 1, fajl);
				fflush(fajl);

                printf("Slog je logicki obrisan.\n");
				return;
			}
		}
	}
}

void obrisiSlogFizicki(FILE *fajl, int sifraLeta) {

    SLOG *slog = pronadjiSlog(fajl, sifraLeta);
    if (slog == NULL) {
        printf("Slog koji zelite obrisati ne postoji!\n");
        return;
    }

    //Trazimo slog sa odgovarajucom vrednoscu kljuca, i potom sve
    //slogove ispred njega povlacimo jedno mesto unazad.

    BLOK blok, naredniBlok;

    int sifraZaBrisanje;
    sifraZaBrisanje = sifraLeta;

    fseek(fajl, 0, SEEK_SET);
    while (fread(&blok, 1, sizeof(BLOK), fajl)) {
        for (int i = 0; i < FBLOKIRANJA; i++) {

            if (blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE) {

                if (i == 0) {
                    //Ako je oznaka kraja bila prvi slog u poslednjem bloku,
                    //posle brisanja onog sloga, ovaj poslednji blok nam
                    //vise ne treba;
                    printf("(skracujem fajl...)\n");
                    fseek(fajl, -sizeof(BLOK), SEEK_END);
					long bytesToKeep = ftell(fajl);
					_chsize(fileno(fajl), bytesToKeep);
					fflush(fajl); //(da bi se primenile promene usled poziva truncate)
                }

                printf("Slog je fizicki obrisan.\n");
                return;
            }

            if (blok.slogovi[i].sifraLeta == sifraZaBrisanje) {

                //Obrisemo taj slog iz bloka tako sto sve slogove ispred njega
                //povucemo jedno mesto unazad.
                for (int j = i+1; j < FBLOKIRANJA; j++) {
                    memcpy(&(blok.slogovi[j-1]), &(blok.slogovi[j]), sizeof(SLOG));
                }

                //Jos bi hteli na poslednju poziciju u tom bloku upisati prvi
                //slog iz narednog bloka, pa cemo zato ucitati naredni blok...
                int podatakaProcitano = fread(&naredniBlok, sizeof(BLOK), 1, fajl);

                //...i pod uslovom da uopste ima jos blokova posle trenutnog...
                if (podatakaProcitano) {

                    //(ako smo nesto procitali, poziciju u fajlu treba vratiti nazad)
                    fseek(fajl, -sizeof(BLOK), SEEK_CUR);

                    //...prepisati njegov prvi slog na mesto poslednjeg sloga u trenutnom bloku.
                    memcpy(&(blok.slogovi[FBLOKIRANJA-1]), &(naredniBlok.slogovi[0]), sizeof(SLOG));

                    //U narednoj iteraciji while loopa, brisemo prvi slog iz narednog bloka.
                    sifraZaBrisanje = naredniBlok.slogovi[0].sifraLeta;
                }

                //Vratimo trenutni blok u fajl.
                fseek(fajl, -sizeof(BLOK), SEEK_CUR);
                fwrite(&blok, sizeof(BLOK), 1, fajl);
                fflush(fajl);

                if (!podatakaProcitano) {
                    //Ako nema vise blokova posle trentnog, mozemo prekinuti algoritam.
                    printf("Slog je fizicki obrisan.\n");
                    return;
                }

                //To je to, citaj sledeci blok
                break;
            }

        }
    }
}



void azurirajSlog(FILE *fajl) {

	if (fajl == NULL) {
		printf("Datoteka nije otvorena!\n");
		return;
	}


	fseek(fajl, 0, SEEK_SET);
	BLOK blok;

	while (fread(&blok, sizeof(BLOK), 1, fajl)) {

		for (int i = 0; i < FBLOKIRANJA; i++) {
                if(blok.slogovi[i].sifraLeta == OZNAKA_KRAJA_DATOTEKE){
                    return;
                }
                char c = '*';

                int len = strlen(blok.slogovi[i].mestoDolaska)-1;
                char b = blok.slogovi[i].mestoDolaska[len];
                if(b == c){
                    continue;
                }
                char *text = blok.slogovi[i].datum ;

                char subtext[3];
                char subtext2[3];

                //strncpy(subtext,&text[11],2);//YYYY-MM-DD HH:mm
                memcpy( subtext, &text[11], 2);
                subtext[2] = '\0';
                //strncpy(subtext,&text[14],2);
                memcpy( subtext2, &text[14], 2);
                subtext2[2] = '\0';
               // printf("OVO SU STRINGOVI za %d %s %s   ",blok.slogovi[i].sifraLeta, subtext, subtext2);
                int sati = atoi(subtext);
                int minuti = atoi(subtext2);
               // printf("OVO INTEGERI %d %d\n", sati, minuti);
                char* mesto = blok.slogovi[i].mestoDolaska;

                char* str = "2021-12-31";

                int trajanje = blok.slogovi[i].trajanjeLeta;
                int pom1 = trajanje / 60;
                int trajanjeSati = pom1;
                int trajanjeMin = trajanje - trajanjeSati*60;

                if ((strstr(blok.slogovi[i].datum, str) != NULL) && ((sati + trajanjeSati) >= 23) && !blok.slogovi[i].deleted) {
                        if(sati + trajanjeSati == 23){
                            if(minuti + trajanjeMin >= 60){
                               strncat(blok.slogovi[i].mestoDolaska, &c, 1);
                               	fseek(fajl, -sizeof(BLOK), SEEK_CUR);
                                fwrite(&blok, sizeof(BLOK), 1, fajl);
                                fflush(fajl);

                                printf("Uspesno uradjeno.\n");
                                return;
                            }
                        }
                        else{
                            strncat(blok.slogovi[i].mestoDolaska, &c, 1);
                            	fseek(fajl, -sizeof(BLOK), SEEK_CUR);
                                fwrite(&blok, sizeof(BLOK), 1, fajl);
                                fflush(fajl);

                                printf("Uspesno uradjeno.\n");
                                return;
                        }
            }



		}
	}
}




