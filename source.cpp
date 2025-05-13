#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define RED 3
#define STUPAC 3
#define MAX_KONTEJNERA 3
#define NAZIV_DULJINA 10

struct Celija {
    char kontejneri[MAX_KONTEJNERA][NAZIV_DULJINA];
    int broj;
};

struct Celija mreza[RED][STUPAC];

void spremiUDatoteku() {
    FILE* dat = fopen("kontejneri.bin", "wb");
    if (dat != NULL) {
        fwrite(mreza, sizeof(mreza), 1, dat);
        fclose(dat);
    }
}

void ucitajIzDatoteke() {
    FILE* dat = fopen("kontejneri.bin", "rb");
    if (dat != NULL) {
        fread(mreza, sizeof(mreza), 1, dat);
        fclose(dat);
    }
    else {
        for (int i = 0; i < RED; i++) {
            for (int j = 0; j < STUPAC; j++) {
                mreza[i][j].broj = 0;
            }
        }
    }
}

void prikaziMrezu() {
    printf("MAKSIMALNA VISINA STOGA :: %d", MAX_KONTEJNERA);
    printf("\n");
    printf("\n    A     B     C\n");
    for (int i = 0; i < RED; i++) {
        printf("%d ", i + 1);
        for (int j = 0; j < STUPAC; j++) {
            if (mreza[i][j].broj > 0) {
                printf("[ X ] ");
            }
            else {
                printf("[   ] ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

int indeksStupca(char slovo) {
    if (slovo == 'A') return 0;
    if (slovo == 'B') return 1;
    if (slovo == 'C') return 2;
    return -1;
}

void readCelija(int red, int stupac) {
    printf("\nSadrzaj celije ::\n");
    for (int i = MAX_KONTEJNERA - 1; i >= 0; i--) {
        if (i < mreza[red][stupac].broj) {
            printf("[ %s ]\n", mreza[red][stupac].kontejneri[i]);
        }
        else {
            printf("[     ]\n");
        }
    }
}

int strcasecmp_custom(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        if (tolower((unsigned char)*str1) != tolower((unsigned char)*str2)) {
            return 1;
        }
        str1++;
        str2++;
    }
    return *str1 || *str2;
}

int provjeriDupliciraneKontejnere(char* naziv) {
    for (int i = 0; i < RED; i++) {
        for (int j = 0; j < STUPAC; j++) {
            for (int k = 0; k < mreza[i][j].broj; k++) {
                if (strcasecmp_custom(mreza[i][j].kontejneri[k], naziv) == 0) {
                    return 1; // Kontejner vec postoji
                }
            }
        }
    }
    return 0; // Kontejner ne postoji
}

void addCelija(int red, int stupac) {
    if (mreza[red][stupac].broj >= MAX_KONTEJNERA) {
        printf("\nCelija je puna!\n");
        return;
    }

    char novi[NAZIV_DULJINA];
    printf("Unesi oznaku kontejnera :: ");
    scanf("%s", novi);

    // Provjera postoji li već kontejner s istim nazivom u cijeloj mrezi
    if (provjeriDupliciraneKontejnere(novi)) {
        printf("Kontejner s tim nazivom vec postoji u mrezi!\n");
        return;
    }

    strcpy(mreza[red][stupac].kontejneri[mreza[red][stupac].broj], novi);
    mreza[red][stupac].broj++;
    spremiUDatoteku();
}

void deleteCelija(int red, int stupac) {
    readCelija(red, stupac);
    if (mreza[red][stupac].broj == 0) return;
    char oznaka[NAZIV_DULJINA];
    printf("Unesi oznaku kontejnera za brisanje :: ");
    scanf("%s", oznaka);
    int nasao = 0;
    for (int i = 0; i < mreza[red][stupac].broj; i++) {
        if (strcasecmp_custom(mreza[red][stupac].kontejneri[i], oznaka) == 0) {
            nasao = 1;
            for (int j = i; j < mreza[red][stupac].broj - 1; j++) {
                strcpy(mreza[red][stupac].kontejneri[j], mreza[red][stupac].kontejneri[j + 1]);
            }
            mreza[red][stupac].broj--;
            spremiUDatoteku();
            printf("Obrisano.\n");
            break;
        }
    }
    if (!nasao) printf("Kontejner nije pronadjen.\n");
}

void moveCelija(int red, int stupac) {
    readCelija(red, stupac);
    if (mreza[red][stupac].broj == 0) {
        printf("Celija je prazna.\n");
        return;
    }

    char oznaka[NAZIV_DULJINA];
    printf("Unesi oznaku kontejnera za premjestanje (samo gornji dozvoljen) :: ");
    scanf("%s", oznaka);

    // Dozvoljeno samo ako je kontejner na vrhu
    if (strcasecmp_custom(mreza[red][stupac].kontejneri[mreza[red][stupac].broj - 1], oznaka) != 0) {
        printf("Mozes premjestiti samo kontejner koji je na vrhu stoga.\n");
        return;
    }

    char novaCelija[3];
    printf("Unesi oznaku ciljne celije (npr. B2) :: ");
    scanf("%s", novaCelija);
    int noviStupac = indeksStupca(novaCelija[0]);
    int noviRed = novaCelija[1] - '1';
    if (noviStupac < 0 || noviRed < 0 || noviRed >= RED || noviStupac >= STUPAC) {
        printf("Neispravna celija.\n");
        return;
    }
    if (mreza[noviRed][noviStupac].broj >= MAX_KONTEJNERA) {
        printf("Ciljna celija je puna.\n");
        return;
    }

    // Premjesti kontejner
    strcpy(mreza[noviRed][noviStupac].kontejneri[mreza[noviRed][noviStupac].broj], oznaka);
    mreza[noviRed][noviStupac].broj++;
    mreza[red][stupac].broj--;
    spremiUDatoteku();
    printf("Premjesteno.\n");
}

void ocistiKonzolu() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    ucitajIzDatoteke();
    while (1) {
        ocistiKonzolu();
        prikaziMrezu();
        printf("Odaberi operaciju (READ, ADD, DELETE, MOVE, EXIT) :: ");
        char operacija[10];
        scanf("%s", operacija);

        // Ignoriranje razlike izmedju velikih i malih slova
        for (int i = 0; operacija[i]; i++) {
            operacija[i] = tolower(operacija[i]);
        }

        if (strcmp(operacija, "exit") == 0) break;

        printf("Unesi oznaku celije (npr. A2) :: ");
        char celija[3];
        scanf("%s", celija);
        int stupac = indeksStupca(celija[0]);
        int red = celija[1] - '1';

        if (stupac < 0 || red < 0 || red >= RED || stupac >= STUPAC) {
            printf("Neispravna celija.\n");
            continue;
        }

        if (strcmp(operacija, "read") == 0) readCelija(red, stupac);
        else if (strcmp(operacija, "add") == 0) addCelija(red, stupac);
        else if (strcmp(operacija, "delete") == 0) deleteCelija(red, stupac);
        else if (strcmp(operacija, "move") == 0) moveCelija(red, stupac);
        else printf("Nepoznata operacija.\n");

        printf("\nPritisni ENTER za nastavak...");
        getchar(); getchar();
    }
    return 0;
}
