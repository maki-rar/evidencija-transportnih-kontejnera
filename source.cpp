#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "color.h"  // Dodano ukljucivanje za boje
#ifdef _WIN32
#include <windows.h>
#endif

#define RED 3
#define STUPAC 3
#define MAX_KAPACITET 3
#define NAZIV_DULJINA 15

typedef struct {
    char oznaka[NAZIV_DULJINA];
} Kontejner;

typedef struct {
    Kontejner* kontejneri;
    int brojKontejnera;
    int kapacitet;
} Celija;

static Celija mreza[RED][STUPAC];

// --- Funkcije za upravljanje memorijom i datotekama ---

static void spremiUDatoteku() {
    FILE* dat = fopen("kontejneri.bin", "wb");
    if (!dat) {
        perror("Greska pri otvaranju datoteke za pisanje");
        return;
    }
    // Spremi broj kontejnera i kapacitet za svaku celiju, pa onda kontejner oznake
    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            fwrite(&mreza[i][j].brojKontejnera, sizeof(int), 1, dat);
            fwrite(&mreza[i][j].kapacitet, sizeof(int), 1, dat);
            for (int k = 0; k < mreza[i][j].brojKontejnera; k++) {
                fwrite(mreza[i][j].kontejneri[k].oznaka, sizeof(char), NAZIV_DULJINA, dat);
            }
        }
    }
    fclose(dat);
}

static void ucitajIzDatoteke() {
    FILE* dat = fopen("kontejneri.bin", "rb");
    if (!dat) {
        // Inicijalizacija prazne mreze
        for (int i = 0; i < RED; i++) {
            for (int j = 0; j < STUPAC; j++) {
                mreza[i][j].kapacitet = MAX_KAPACITET;
                mreza[i][j].brojKontejnera = 0;
                mreza[i][j].kontejneri = (Kontejner*)malloc(sizeof(Kontejner) * mreza[i][j].kapacitet);
            }
        }
        return;
    }

    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            fread(&mreza[i][j].brojKontejnera, sizeof(int), 1, dat);
            fread(&mreza[i][j].kapacitet, sizeof(int), 1, dat);
            mreza[i][j].kontejneri = (Kontejner*)malloc(sizeof(Kontejner) * mreza[i][j].kapacitet);
            for (int k = 0; k < mreza[i][j].brojKontejnera; k++) {
                fread(mreza[i][j].kontejneri[k].oznaka, sizeof(char), NAZIV_DULJINA, dat);
            }
        }
    }
    fclose(dat);
}

// --- Pomocne funkcije ---

static int indeksStupca(char slovo) {
    slovo = toupper((unsigned char)slovo);
    if (slovo == 'A') return 0;
    if (slovo == 'B') return 1;
    if (slovo == 'C') return 2;
    return -1;
}

static int strcasecmp_custom(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
            return 1;
        s1++; s2++;
    }
    return *s1 || *s2;
}

static int provjeriDupliciraneKontejnere(const char* naziv) {
    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            for (int k = 0; k < mreza[i][j].brojKontejnera; k++) {
                if (strcasecmp_custom(mreza[i][j].kontejneri[k].oznaka, naziv) == 0)
                    return 1;
            }
        }
    }
    return 0;
}

static void ocistiKonzolu() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// --- Prikaz mreze s lijepim okvirima i oznakama ---

static void prikaziMrezu() {
    printf(CRVENA "\n     A       B       C   \n" RESET);
    printf(PLAVA "   +-------+-------+-------+\n" RESET);
    for (int i = 0; i < RED; i++) {
        printf(CRVENA " %d" RESET PLAVA " |" RESET, i + 1);
        for (int j = 0; j < STUPAC; j++) {
            if (mreza[i][j].brojKontejnera > 0) {
                printf(CRVENA "  %2d   " RESET PLAVA "|" RESET, mreza[i][j].brojKontejnera);
            } else {
                printf(PLAVA "       |" RESET);
            }
        }
        printf("\n");
        printf(PLAVA "   +-------+-------+-------+\n" RESET);
    }
    printf("\n");
}

// --- Operacije nad celijama ---
static void readCelija(int red, int stupac) {
    printf(PLAVA "\nSadrzaj celije" RESET ZUTA " %c%d" RESET PLAVA " :: \n" RESET, 'A' + stupac, red + 1);
    if (mreza[red][stupac].brojKontejnera == 0) {
        printf(ZELENA "[ PRAZNO ]\n" RESET);
        return;
    }

    // Ispis kontejnere od dna prema vrhu (kontejner na vrhu je zadnji u nizu)
    for (int i = mreza[red][stupac].brojKontejnera - 1; i >= 0; i--) {
        char* oznaka = mreza[red][stupac].kontejneri[i].oznaka;

        // ASCII art brodskog kontejnera, širina je fiksirana na 11 znakova (15 max oznaka, pa rezamo ako treba)
        char oznaka_prikaz[12];
        snprintf(oznaka_prikaz, sizeof(oznaka_prikaz), "%.10s", oznaka); // max 10 znakova, ostatak rezemo

        printf(ZUTA "  +------------+\n" RESET);
        printf(ZUTA "  | %-10s |\n" RESET, oznaka_prikaz);
        printf(ZUTA "  +------------+\n" RESET);
    }
}

static void addCelija(int red, int stupac) {
    if (mreza[red][stupac].brojKontejnera >= mreza[red][stupac].kapacitet) {
        printf(CRVENA "\nCelija je puna!\n" RESET);
        return;
    }
    char novi[NAZIV_DULJINA];
    printf(PLAVA "Unesi oznaku kontejnera (npr. K813) :: " RESET, NAZIV_DULJINA - 1);
    scanf("%14s", novi);

    if (provjeriDupliciraneKontejnere(novi)) {
        printf(CRVENA "Kontejner s tim nazivom vec postoji u mrezi!\n" RESET);
        return;
    }

    // Dodavanje kontejnera
    strcpy(mreza[red][stupac].kontejneri[mreza[red][stupac].brojKontejnera].oznaka, novi);
    mreza[red][stupac].brojKontejnera++;
    spremiUDatoteku();
    printf(ZELENA "Kontejner dodan.\n" RESET);
}

static void deleteCelija(int red, int stupac) {
    readCelija(red, stupac);
    if (mreza[red][stupac].brojKontejnera == 0) return;

    char oznaka[NAZIV_DULJINA];
    printf(PLAVA "Unesi oznaku kontejnera za brisanje :: " RESET);
    scanf("%14s", oznaka);

    int nasao = 0;
    for (int i = 0; i < mreza[red][stupac].brojKontejnera; i++) {
        if (strcasecmp_custom(mreza[red][stupac].kontejneri[i].oznaka, oznaka) == 0) {
            nasao = 1;
            for (int j = i; j < mreza[red][stupac].brojKontejnera - 1; j++) {
                strcpy(mreza[red][stupac].kontejneri[j].oznaka, mreza[red][stupac].kontejneri[j + 1].oznaka);
            }
            mreza[red][stupac].brojKontejnera--;
            spremiUDatoteku();
            printf(ZELENA "Obrisano.\n" RESET);
            break;
        }
    }
    if (!nasao) printf(CRVENA "Kontejner nije pronadjen.\n" RESET);
}

static void moveCelija(int red, int stupac) {
    readCelija(red, stupac);
    if (mreza[red][stupac].brojKontejnera == 0) {
        printf(ZELENA "Celija je prazna.\n" RESET);
        return;
    }

    char oznaka[NAZIV_DULJINA];
    printf(PLAVA "Unesi oznaku kontejnera za premjestanje (samo gornji dozvoljen) :: " RESET);
    scanf("%14s", oznaka);

    if (strcasecmp_custom(mreza[red][stupac].kontejneri[mreza[red][stupac].brojKontejnera - 1].oznaka, oznaka) != 0) {
        printf(CRVENA "Mozes premjestiti samo gornji kontejner.\n" RESET);
        return;
    }

    char novaCelija[3];
    printf(PLAVA "Unesi oznaku ciljne celije (npr. B2) :: " RESET);
    scanf("%2s", novaCelija);
    int noviStupac = indeksStupca(novaCelija[0]);
    int noviRed = novaCelija[1] - '1';

    if (noviStupac < 0 || noviRed < 0 || noviRed >= RED || noviStupac >= STUPAC) {
        printf(CRVENA "Neispravna celija.\n" RESET);
        return;
    }
    if (mreza[noviRed][noviStupac].brojKontejnera >= mreza[noviRed][noviStupac].kapacitet) {
        printf(CRVENA "Ciljna celija je puna.\n" RESET);
        return;
    }

    // Premjestanje
    strcpy(mreza[noviRed][noviStupac].kontejneri[mreza[noviRed][noviStupac].brojKontejnera].oznaka,
           oznaka);
    mreza[noviRed][noviStupac].brojKontejnera++;
    mreza[red][stupac].brojKontejnera--;
    spremiUDatoteku();
    printf(ZELENA "Premjesteno.\n" RESET);
}

static void pretraziKontejnere() {
    char trazeni[NAZIV_DULJINA];
    printf(PLAVA "Unesi oznaku kontejnera za pretragu :: " RESET);
    scanf("%14s", trazeni);

    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            for (int k = 0; k < mreza[i][j].brojKontejnera; k++) {
                if (strcasecmp_custom(mreza[i][j].kontejneri[k].oznaka, trazeni) == 0) {
                    printf(ZELENA"Kontejner" RESET ZUTA" %s" ZELENA" pronadjen u celiji" RESET ZUTA " %c%d" RESET ZELENA " na poziciji" RESET ZUTA" %d\n" RESET,
                           trazeni, 'A' + j, i + 1, k + 1);
                    return;
                }
            }
        }
    }
    printf(CRVENA "Kontejner nije pronadjen.\n" RESET);
}

static void popisKontejnera() {
    int ukupno = 0;
    printf(ZUTA "\nPopis svih kontejnera u mrezi :: \n" RESET);
    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            for (int k = 0; k < mreza[i][j].brojKontejnera; k++) {
                printf(ZELENA "[%s]\n" RESET, mreza[i][j].kontejneri[k].oznaka);
                ukupno++;
            }
        }
    }
    if (ukupno == 0) {
        printf(CRVENA "[Nema kontejnera u mrezi]\n" RESET);
    }
}

typedef enum {
    IZLAZ = 0,
    DODAJ = 1,
    ISPIS = 2,
    BRISE = 3,
    PREMIJESTI = 4,
    PRETRAZI = 5,
    POPIS = 6
} Opcija;

static void prikaziIzbornik() {
    printf(ZUTA "Izbornik:\n" RESET);
    printf(ZELENA "1 - Dodaj kontejner\n" RESET);
    printf(ZELENA "2 - Sadrzaj celije\n" RESET);
    printf(ZELENA "3 - Obrisi kontejner\n" RESET);
    printf(ZELENA "4 - Premjesti kontejner\n" RESET);
    printf(ZELENA "5 - Trazi kontejner\n" RESET);
    printf(ZELENA "6 - Popis svih kontejnera\n" RESET);
    printf(ZELENA "0 - Izlaz\n" RESET);
}

int main() {
	#ifdef _WIN32
	    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	    if (hOut == INVALID_HANDLE_VALUE) return 1;
	
	    DWORD dwMode = 0;
	    if (!GetConsoleMode(hOut, &dwMode)) return 1;
	
	    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	    if (!SetConsoleMode(hOut, dwMode)) return 1;
	#endif

    ucitajIzDatoteke();

    int red, stupac;
    char tmp[3];
    Opcija opcija;

    do {
        ocistiKonzolu();
        prikaziMrezu();

        prikaziIzbornik();
        printf(ZUTA "Odaberi opciju :: " CRVENA);
        scanf("%d", (int*)&opcija);
        printf(RESET);

        if (opcija == IZLAZ) break;

        if (opcija == DODAJ || opcija == ISPIS || opcija == BRISE || opcija == PREMIJESTI) {
            printf(PLAVA "Unesi celiju (npr. A1) :: " RESET);
            scanf("%2s", tmp);
            stupac = indeksStupca(tmp[0]);
            red = tmp[1] - '1';
            if (stupac < 0 || red < 0 || red >= RED || stupac >= STUPAC) {
                printf(CRVENA "Neispravna celija.\n" RESET);
                printf(PLAVA "Pritisni enter za nastavak..." RESET);
                getchar(); getchar();
                continue;
            }
        }

        switch (opcija) {
            case DODAJ:
                addCelija(red, stupac);
                break;
            case ISPIS:
                readCelija(red, stupac);
                break;
            case BRISE:
                deleteCelija(red, stupac);
                break;
            case PREMIJESTI:
                moveCelija(red, stupac);
                break;
            case PRETRAZI:
                pretraziKontejnere();
                break;
            case POPIS:
        		popisKontejnera();
        break;
            default:
                printf(CRVENA "Nepoznata opcija.\n" RESET);
        }

        printf(PLAVA "Pritisni enter za nastavak..." RESET);
        getchar(); getchar();

    } while (opcija != IZLAZ);

    // Oslobodi memoriju
    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            free(mreza[i][j].kontejneri);
            mreza[i][j].kontejneri = NULL;
        }
    }

    printf(ZELENA "Dovidjenja!\n" RESET);
    return 0;
}
