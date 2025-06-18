#ifndef MAKROF_H
#define MAKROF_H
//Provjerava je li znak slovo, ako nije, znaci nevazeci unos i vraca -1
//Ako je, pretvara u veliko slovo, usporedjuje a s A, B i C i vraca 
//odgovarajuci indeks, tj 0, 1 ili 2. Ako nije A, B ili C onda je nevazece pa -1.
#define indeks_stupca(slovo) \
    (!isalpha((unsigned char)(slovo)) ? -1 : \
    (toupper((unsigned char)(slovo)) == 'A' ? 0 : \
    (toupper((unsigned char)(slovo)) == 'B' ? 1 : \
    (toupper((unsigned char)(slovo)) == 'C' ? 2 : -1))))

//Usporedjuje dva stringa neosjetljivo na velika i mala slova.
//Ako su s1 ili s2 NULL, vraca -1
//Uglavnom pretvara svaki znak u malo slovo i usporedjuje znak
//po znak i ako se razlikuju, prekida se petlja.
//Vraca 0 ako su stringovi jednaki i 1 ako su razliciti.
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
    
//Provjerava postoji li kontejner s istom oznakom u mrezi.
//Ako je naziv NULL, vraca 0 znaci nema duplikata.
//Prolazi kroz svaku celiju kao red x stupac i u svakoj
//celiji provjerava sve kontejnere s funkcijom iznad.
//Ako se pronadje isti, _found se postavlja da je 1 i prekida se pretraga.
//Vraca 1 ako postoji duplikat i 0 ako ne postoji.
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
    
#endif