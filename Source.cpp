#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "boje.h"  // Boje
#ifdef _WIN32
#include <windows.h>
#endif

#define RED 3
#define STUPAC 3
#define MAX_KAPACITET 3
#define NAZIV_DULJINA 15

//Globalna varijabla :)
extern int GLOBALNA_BROJILO;  
int GLOBALNA_BROJILO = 0;    
//

typedef struct {
    char oznaka[NAZIV_DULJINA];
} kontejner;

typedef struct {
    kontejner* kontejneri;
    int broj_kontejnera;
    int kapacitet;
} celija;

static celija mreza[RED][STUPAC];

// Makro funkcije
#define indeks_stupca(slovo) \
    (!isalpha((unsigned char)(slovo)) ? -1 : \
    (toupper((unsigned char)(slovo)) == 'A' ? 0 : \
    (toupper((unsigned char)(slovo)) == 'B' ? 1 : \
    (toupper((unsigned char)(slovo)) == 'C' ? 2 : -1))))

#define strcasecmp_custom(s1, s2) \
    (!(s1) || !(s2) ? -1 : \
    ( \
        { \
        const char *_s1 = (s1), *_s2 = (s2); \
        while (*_s1 && *_s2) { \
            if (tolower((unsigned char)*_s1) != tolower((unsigned char)*_s2)) \
                break; \
            _s1++; _s2++; \
        } \
        (*_s1 || *_s2); \
        } \
    ))

#define provjeri_duplicirane_kontejnere(naziv) \
    (!(naziv) ? 0 : \
    ( \
        { \
        int _found = 0; \
        for (int i = 0; i < RED && !_found; i++) { \
            for (int j = 0; j < STUPAC && !_found; j++) { \
                for (int k = 0; k < mreza[i][j].broj_kontejnera && !_found; k++) { \
                    if (strcasecmp_custom(mreza[i][j].kontejneri[k].oznaka, (naziv)) == 0) \
                        _found = 1; \
                } \
            } \
        } \
        _found; \
        } \
    ))

// Funkcije za upravljanje memorijom i datotekama
static void spremi_u_datoteku(void) {
    FILE* dat = fopen("kontejneri.bin", "wb");
    if (!dat) {
        perror("Greska pri otvaranju datoteke za pisanje");
        return;
    }
    rewind(dat);
    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            if (mreza[i][j].broj_kontejnera < 0 || mreza[i][j].kapacitet <= 0) {
                continue;
            }
            fwrite(&mreza[i][j].broj_kontejnera, sizeof(int), 1, dat);
            fwrite(&mreza[i][j].kapacitet, sizeof(int), 1, dat);
            for (int k = 0; k < mreza[i][j].broj_kontejnera; k++) {
                fwrite(mreza[i][j].kontejneri[k].oznaka, sizeof(char), NAZIV_DULJINA, dat);
            }
        }
    }
    fclose(dat);
}

static void ucitaj_iz_datoteke(void) {
    FILE* dat = fopen("kontejneri.bin", "rb");
    if (!dat) {
        for (int i = 0; i < RED; i++) {
            for (int j = 0; j < STUPAC; j++) {
                mreza[i][j].kapacitet = MAX_KAPACITET;
                mreza[i][j].broj_kontejnera = 0;
                mreza[i][j].kontejneri = (kontejner*)malloc(sizeof(kontejner) * mreza[i][j].kapacitet);
                if (!mreza[i][j].kontejneri) {
                    perror("Greska pri alokaciji memorije");
                    exit(EXIT_FAILURE);
                }
            }
        }
        return;
    }

    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            if (fread(&mreza[i][j].broj_kontejnera, sizeof(int), 1, dat) != 1 ||
                fread(&mreza[i][j].kapacitet, sizeof(int), 1, dat) != 1) {
                break;
            }
            if (mreza[i][j].broj_kontejnera < 0 || mreza[i][j].kapacitet <= 0) {
                break;
            }
            mreza[i][j].kontejneri = (kontejner*)malloc(sizeof(kontejner) * mreza[i][j].kapacitet);
            if (!mreza[i][j].kontejneri) {
                perror("Greska pri alokaciji memorije");
                exit(EXIT_FAILURE);
            }
            for (int k = 0; k < mreza[i][j].broj_kontejnera; k++) {
                fread(mreza[i][j].kontejneri[k].oznaka, sizeof(char), NAZIV_DULJINA, dat);
            }
        }
    }
    fclose(dat);
}

static void ocisti_konzolu(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Prikaz mreže 
static void prikazi_mrezu(void) {
    printf(CRVENA "\n     A       B       C   \n" RESET);
    printf(PLAVA "   +-------+-------+-------+\n" RESET);
    for (int i = 0; i < RED; i++) {
        printf(CRVENA " %d" RESET PLAVA " |" RESET, i + 1);
        for (int j = 0; j < STUPAC; j++) {
            if (mreza[i][j].broj_kontejnera > 0) {
                printf(CRVENA "  %2d   " RESET PLAVA "|" RESET, mreza[i][j].broj_kontejnera);
            }
            else {
                printf(PLAVA "       |" RESET);
            }
        }
        printf("\n");
        printf(PLAVA "   +-------+-------+-------+\n" RESET);
    }
    printf("\n");
}

// Operacije nad celijama
static void read_celija(const int red, const int stupac) {
    if (red < 0 || red >= RED || stupac < 0 || stupac >= STUPAC) {
        return;
    }
    printf(PLAVA "\nSadrzaj celije" RESET ZUTA " %c%d" RESET PLAVA " :: \n" RESET, 'A' + stupac, red + 1);
    if (mreza[red][stupac].broj_kontejnera == 0) {
        printf(ZELENA "[ PRAZNO ]\n" RESET);
        return;
    }

    for (int i = mreza[red][stupac].broj_kontejnera - 1; i >= 0; i--) {
        char* oznaka = mreza[red][stupac].kontejneri[i].oznaka;

        char oznaka_prikaz[12];
        snprintf(oznaka_prikaz, sizeof(oznaka_prikaz), "%.10s", oznaka);

        printf(ZUTA "  +------------+\n" RESET);
        printf(ZUTA "  | %-10s |\n" RESET, oznaka_prikaz);
        printf(ZUTA "  +------------+\n" RESET);
    }
}

static void add_celija(const int red, const int stupac) {
    GLOBALNA_BROJILO++;
    if (red < 0 || red >= RED || stupac < 0 || stupac >= STUPAC) {
        return;
    }
    if (mreza[red][stupac].broj_kontejnera >= mreza[red][stupac].kapacitet) {
        printf(CRVENA "\nCelija je puna!\n" RESET);
        return;
    }
    char novi[NAZIV_DULJINA];
    printf(PLAVA "Unesi oznaku kontejnera (npr. K813) :: " RESET, NAZIV_DULJINA - 1);
    scanf("%14s", novi);

    if (provjeri_duplicirane_kontejnere(novi)) {
        printf(CRVENA "Kontejner s tim nazivom vec postoji u mrezi!\n" RESET);
        return;
    }

    strcpy(mreza[red][stupac].kontejneri[mreza[red][stupac].broj_kontejnera].oznaka, novi);
    mreza[red][stupac].broj_kontejnera++;
    spremi_u_datoteku();
    printf(ZELENA "Kontejner dodan.\n" RESET);
}

static void delete_celija(const int red, const int stupac) {
    if (red < 0 || red >= RED || stupac < 0 || stupac >= STUPAC) {
        return;
    }
    read_celija(red, stupac);
    if (mreza[red][stupac].broj_kontejnera == 0) return;

    char oznaka[NAZIV_DULJINA];
    printf(PLAVA "Unesi oznaku kontejnera za brisanje :: " RESET);
    scanf("%14s", oznaka);

    int nasao = 0;
    for (int i = 0; i < mreza[red][stupac].broj_kontejnera; i++) {
        if (strcasecmp_custom(mreza[red][stupac].kontejneri[i].oznaka, oznaka) == 0) {
            nasao = 1;
            for (int j = i; j < mreza[red][stupac].broj_kontejnera - 1; j++) {
                strcpy(mreza[red][stupac].kontejneri[j].oznaka, mreza[red][stupac].kontejneri[j + 1].oznaka);
            }
            mreza[red][stupac].broj_kontejnera--;
            spremi_u_datoteku();
            printf(ZELENA "Obrisano.\n" RESET);
            break;
        }
    }
    if (!nasao) {
        printf(CRVENA "Kontejner nije pronadjen.\n" RESET);
    }
}

static void move_celija(const int red, const int stupac) {
    if (red < 0 || red >= RED || stupac < 0 || stupac >= STUPAC) {
        return;
    }
    read_celija(red, stupac);
    if (mreza[red][stupac].broj_kontejnera == 0) {
        printf(ZELENA "Celija je prazna.\n" RESET);
        return;
    }

    char oznaka[NAZIV_DULJINA];
    printf(PLAVA "Unesi oznaku kontejnera za premjestanje (samo gornji dozvoljen) :: " RESET);
    scanf("%14s", oznaka);

    if (strcasecmp_custom(mreza[red][stupac].kontejneri[mreza[red][stupac].broj_kontejnera - 1].oznaka, oznaka) != 0) {
        printf(CRVENA "Mozes premjestiti samo gornji kontejner.\n" RESET);
        return;
    }

    char nova_celija[3];
    printf(PLAVA "Unesi oznaku ciljne celije (npr. B2) :: " RESET);
    scanf("%2s", nova_celija);
    int novi_stupac = indeks_stupca(nova_celija[0]);
    int novi_red = nova_celija[1] - '1';

    if (novi_stupac < 0 || novi_red < 0 || novi_red >= RED || novi_stupac >= STUPAC) {
        printf(CRVENA "Neispravna celija.\n" RESET);
        return;
    }
    if (mreza[novi_red][novi_stupac].broj_kontejnera >= mreza[novi_red][novi_stupac].kapacitet) {
        printf(CRVENA "Ciljna celija je puna.\n" RESET);
        return;
    }

    strcpy(mreza[novi_red][novi_stupac].kontejneri[mreza[novi_red][novi_stupac].broj_kontejnera].oznaka, oznaka);
    mreza[novi_red][novi_stupac].broj_kontejnera++;
    mreza[red][stupac].broj_kontejnera--;
    spremi_u_datoteku();
    printf(ZELENA "Premjesteno.\n" RESET);
}

// Binarna pretraga kontejnera rekurzivno
static int b_search(kontejner* kontejneri, const int l, const int r, const char* trazeni) {
    if (l > r) return -1;  // nije pronadjeno
    
    int mid = l + (r - l) / 2;
    
    if (strcasecmp_custom(kontejneri[mid].oznaka, trazeni) == 0) {
        return mid;
    }
    
    int lijevo = b_search(kontejneri, l, mid - 1, trazeni);
    if (lijevo != -1) return lijevo;
    
    return b_search(kontejneri, mid + 1, r, trazeni);
}

static void pretrazi_kontejnere(void) {
    char trazeni[NAZIV_DULJINA];
    printf(PLAVA "Unesi oznaku kontejnera za pretragu :: " RESET);
    scanf("%14s", trazeni);

    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            int index = b_search(mreza[i][j].kontejneri, 0, mreza[i][j].broj_kontejnera - 1, trazeni);
            if (index != -1) {
                printf(ZELENA"Kontejner" RESET ZUTA" %s" ZELENA" pronadjen u celiji" RESET ZUTA " %c%d" RESET ZELENA " na poziciji" RESET ZUTA" %d\n" RESET,
                    trazeni, 'A' + j, i + 1, index + 1);
                return;
            }
        }
    }
    printf(CRVENA "Kontejner nije pronadjen.\n" RESET);
}

static int usporedi_kontejnere(const void* a, const void* b) {
    const kontejner* ka = (const kontejner*)a;
    const kontejner* kb = (const kontejner*)b;
    return strcasecmp(ka->oznaka, kb->oznaka);
}

static void popis_kontejnera(void) {
    kontejner svi_kontejneri[RED * STUPAC * MAX_KAPACITET];
    int ukupno = 0;
    
    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            for (int k = 0; k < mreza[i][j].broj_kontejnera; k++) {
                strcpy(svi_kontejneri[ukupno].oznaka, mreza[i][j].kontejneri[k].oznaka);
                ukupno++;
            }
        }
    }

    qsort(svi_kontejneri, ukupno, sizeof(kontejner), usporedi_kontejnere);

    printf(ZUTA "\nPopis svih kontejnera u mrezi (sortirano) :: \n" RESET);
    for (int i = 0; i < ukupno; i++) {
        printf(ZELENA "[%s]\n" RESET, svi_kontejneri[i].oznaka);
    }
    
    if (ukupno == 0) {
        printf(CRVENA "[Nema kontejnera u mrezi]\n" RESET);
    }
}

static void obrisi_datoteku(void) {
    char potvrda;
    printf(CRVENA "\nJesi li siguran da zelis obrisati datoteku? (d/n) :: " RESET);
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    potvrda = getchar();
    
    if (tolower(potvrda) == 'd') {
        if (remove("kontejneri.bin") == 0) {
            printf(ZELENA "Datoteka uspjesno obrisana.\n" RESET);
            
            // Resetiranje mreže
            for (int i = 0; i < RED; i++) {
                for (int j = 0; j < STUPAC; j++) {
                    if (mreza[i][j].kontejneri) {
                        free(mreza[i][j].kontejneri);
                    }
                    mreza[i][j].kapacitet = MAX_KAPACITET;
                    mreza[i][j].broj_kontejnera = 0;
                    mreza[i][j].kontejneri = (kontejner*)malloc(sizeof(kontejner) * mreza[i][j].kapacitet);
                    if (!mreza[i][j].kontejneri) {
                        perror("Greska pri alokaciji memorije");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            
        } else {
            printf(CRVENA "Greska pri brisanju datoteke!\n" RESET);
        }
    } else {
        printf(ZUTA "Brisanje otkazano.\n" RESET);
    }
    
    while ((c = getchar()) != '\n' && c != EOF);
}

typedef enum {
    IZLAZ = 0,
    DODAJ = 1,
    ISPIS = 2,
    BRISE = 3,
    PREMJESTI = 4,
    PRETRAZI = 5,
    POPIS = 6,
    BRISI_DATOTEKU = 7
} opcija;

static void prikazi_izbornik(void) {
    printf(PLAVA "EXTERN :: Dodano ukupno %d novih kontejnera!\n" RESET, GLOBALNA_BROJILO);
    printf(ZUTA "Izbornik:\n" RESET);
    printf(ZELENA "1 - Dodaj kontejner (CREATE/INSERT)\n" RESET);
    printf(ZELENA "2 - Sadrzaj celije (READ)\n" RESET);
    printf(ZELENA "3 - Obrisi kontejner (DELETE)\n" RESET);
    printf(ZELENA "4 - Premjesti kontejner (UPDATE)\n" RESET);
    printf(ZELENA "5 - Trazi kontejner\n" RESET);
    printf(ZELENA "6 - Popis svih kontejnera\n" RESET);
    printf(CRVENA "7 - Obrisi datoteku kontejneri.bin\n" RESET);
    printf(ZELENA "0 - Izlaz\n" RESET);
}

typedef void (*operacija_celija)(int, int);  // Pokazivac na funkciju

int main(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return 1;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return 1;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode)) return 1;
#endif

    ucitaj_iz_datoteke();

    int red, stupac;
    char tmp[3];
    opcija opcija;

    do {
        ocisti_konzolu();
        prikazi_mrezu();

        prikazi_izbornik();
        printf(ZUTA "Odaberi opciju :: " CRVENA);
        scanf("%d", (int*)&opcija);
        printf(RESET);

        if (opcija == IZLAZ) break;

        if (opcija == DODAJ || opcija == ISPIS || opcija == BRISE || opcija == PREMJESTI) {
            printf(PLAVA "Unesi celiju (npr. A1) :: " RESET);
            scanf("%2s", tmp);
            stupac = indeks_stupca(tmp[0]);
            red = tmp[1] - '1';
            if (stupac < 0 || red < 0 || red >= RED || stupac >= STUPAC) {
                printf(CRVENA "Neispravna celija.\n" RESET);
                printf(PLAVA "Pritisni enter za nastavak..." RESET);
                getchar(); getchar();
                continue;
            }
        }

        switch (opcija) {
            case DODAJ: {
                operacija_celija operacija = add_celija; 
                operacija(red, stupac);
                break;
            }
            case ISPIS: {
                operacija_celija operacija = read_celija;
                operacija(red, stupac);
                break;
            }
            case BRISE: {
                operacija_celija operacija = delete_celija;
                operacija(red, stupac);
                break;
            }
            case PREMJESTI: {
                operacija_celija operacija = move_celija;
                operacija(red, stupac);
                break;
            }
            case PRETRAZI:
                pretrazi_kontejnere();
                break;
            case POPIS:
                popis_kontejnera();
                break;
            case BRISI_DATOTEKU:
                obrisi_datoteku();
                break;
            default:
                printf(CRVENA "Nepoznata opcija.\n" RESET);
        }

        printf(PLAVA "Pritisni enter za nastavak..." RESET);
        getchar(); getchar();

    } while (opcija != IZLAZ);

    // Sigurno brisanje memorije
    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            if (mreza[i][j].kontejneri) {
                free(mreza[i][j].kontejneri);
                mreza[i][j].kontejneri = NULL;
                mreza[i][j].broj_kontejnera = 0;
                mreza[i][j].kapacitet = 0;
            }
        }
    }

    printf(ZELENA "Dovidjenja!\n" RESET);
    return 0;
}