#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rezervare Rezervare;
typedef struct LD LD;
typedef struct LS LS;

struct Rezervare {
    int codRezervare;
    char* numeClient;
    char* destinatie;
    char* dataRezervare;
    int nrPersoane;
    float pret;
    int durata;
};

struct LD {
    Rezervare info;
    struct LD* next;
    struct LD* prev;
};

struct LS {
    Rezervare info;
    struct LS* next;
};

Rezervare citireRezervareDinFisier(FILE* f) {
    Rezervare r;
    char buffer[100];

    fgets(buffer, sizeof(buffer), f);
    r.codRezervare = atoi(buffer);

    if (fgets(buffer, sizeof(buffer), f) == NULL) {
        r.numeClient = NULL;
        return r;
    }
    r.numeClient = _strdup(strtok(buffer, "\n"));

    fgets(buffer, sizeof(buffer), f);
    r.destinatie = _strdup(strtok(buffer, "\n"));

    fgets(buffer, sizeof(buffer), f);
    r.dataRezervare = _strdup(strtok(buffer, "\n"));

    fgets(buffer, sizeof(buffer), f);
    r.nrPersoane = atoi(buffer);

    fgets(buffer, sizeof(buffer), f);
    r.pret = (float)atof(buffer);

    fgets(buffer, sizeof(buffer), f);
    r.durata = atoi(buffer);

    

    return r;
}

void afisareRezervare(Rezervare r) {
    printf("Cod rezervare: %d \nClient: %s \nDestinatie: %s \nData: %s \nPersoane: %d \nPret: %.2f \nDurata rezervare: %d zile \n\n",
        r.codRezervare, r.numeClient, r.destinatie, r.dataRezervare, r.nrPersoane, r.pret, r.durata);
}

void inserareLaInceputLD(LD** cap, LD** coada, Rezervare r) {
    LD* nou = (LD*)malloc(sizeof(LD));
    nou->info = r;
    nou->next = *cap;
    nou->prev = NULL;

    if (*cap)
        (*cap)->prev = nou;
    else
        *coada = nou;

    *cap = nou;
}

void traversareLD(LD* cap) {
    while (cap) {
        afisareRezervare(cap->info);
        cap = cap->next;
    }
}

void citireListaDinFisier(const char* numeFisier, LD** cap, LD** coada) {
    FILE* f = fopen(numeFisier, "r");
    if (!f) {
        printf("Eroare la deschiderea fisierului!\n");
        return;
    }

    while (!feof(f)) {
        Rezervare r = citireRezervareDinFisier(f);
        if (r.numeClient != NULL) {
            inserareLaInceputLD(cap, coada, r);
        }
    }

    fclose(f);
}

LD* sortareLista(LD* cap, LD** coadaNoua) {
    LD* capNou = NULL;
    *coadaNoua = NULL;

    while (cap) {
        if (cap->info.nrPersoane <= 3) {
            Rezervare copie;
            copie.codRezervare = cap->info.codRezervare;
            copie.numeClient = _strdup(cap->info.numeClient);
            copie.destinatie = _strdup(cap->info.destinatie);
            copie.dataRezervare = _strdup(cap->info.dataRezervare);
            copie.nrPersoane = cap->info.nrPersoane;
            copie.pret = cap->info.pret;
            copie.durata = cap->info.durata;

            inserareLaInceputLD(&capNou, coadaNoua, copie);
        }
        cap = cap->next;
    }

    return capNou;
}

void transformaInListaCirculara(LD* cap, LD* coada) {
    if (cap && coada) {
        coada->next = cap;
        cap->prev = coada;
    }
}

void traversareCirculara(LD* cap) {
    if (cap == NULL) return;
    LD* temp = cap;
    do {
        afisareRezervare(temp->info);
        temp = temp->next;
    } while (temp != cap);
}

void inserareDupaPrimulCuPret(LD** cap, float pragPret, Rezervare nouaRezervare) {
    if (*cap == NULL) return;
    LD* temp = *cap;
    do {
        if (temp->info.pret > pragPret) {
            LD* nou = (LD*)malloc(sizeof(LD));
            nou->info = nouaRezervare;
            nou->next = temp->next;
            nou->prev = temp;
            temp->next->prev = nou;
            temp->next = nou;
            break;
        }
        temp = temp->next;
    } while (temp != *cap);
}

LS* conversieLDinLS(LD* capLD) {
    if (capLD == NULL) return NULL;

    LS* capLS = NULL;
    LS* ultim = NULL;
    LD* temp = capLD;

    LD* coada = capLD;
    while (coada->next != capLD && coada->next != NULL) {
        coada = coada->next;
    }
    coada->next = NULL;
    capLD->prev = NULL;

    while (temp) {
        if (temp->info.numeClient == NULL) break;

        LS* nou = (LS*)malloc(sizeof(LS));
        nou->info.codRezervare = temp->info.codRezervare;
        nou->info.numeClient = _strdup(temp->info.numeClient);
        nou->info.destinatie = _strdup(temp->info.destinatie);
        nou->info.dataRezervare = _strdup(temp->info.dataRezervare);
        nou->info.nrPersoane = temp->info.nrPersoane;
        nou->info.pret = temp->info.pret;
        nou->info.durata = temp->info.durata;
        nou->next = NULL;

        if (capLS == NULL) {
            capLS = nou;
            ultim = nou;
        }
        else {
            ultim->next = nou;
            ultim = nou;
        }

        temp = temp->next;
    }

    return capLS;
}

void traversareLS(LS* cap) {
    while (cap) {
        afisareRezervare(cap->info);
        cap = cap->next;
    }
}

void dezalocareLD(LD** cap, LD** coada) {
    if (*coada && *cap && (*coada)->next == *cap) {
        (*coada)->next = NULL;
        (*cap)->prev = NULL;
    }

    while (*cap) {
        free((*cap)->info.numeClient);
        free((*cap)->info.destinatie);
        free((*cap)->info.dataRezervare);
        LD* temp = *cap;
        *cap = (*cap)->next;
        free(temp);
    }
    *coada = NULL;
}

void dezalocareLS(LS** cap) {
    while (*cap) {
        free((*cap)->info.numeClient);
        free((*cap)->info.destinatie);
        free((*cap)->info.dataRezervare);
        LS* temp = *cap;
        *cap = (*cap)->next;
        free(temp);
    }
}

int main() {
    LD* cap = NULL, * coada = NULL;
    citireListaDinFisier("rezervari2.txt", &cap, &coada);

    printf("\n--- Lista Dublu Inlantuita ---\n");
    traversareLD(cap);

    printf("\n--- Lista sortata dupa nrPersoane <= 3 ---\n");
    LD* capSortat = NULL;
    LD* coadaSortata = NULL;
    capSortat = sortareLista(cap, &coadaSortata);
    traversareLD(capSortat);

    printf("\n--- Lista Circulara ---\n");
    transformaInListaCirculara(cap, coada);
    traversareCirculara(cap);

    printf("\n--- Inserare dupa primul cu pret > 2000  ---\n");
    Rezervare rNou;
    rNou.codRezervare = 111;
    rNou.numeClient = _strdup("Eduard Andrei");
    rNou.destinatie = _strdup("Atlantida");
    rNou.dataRezervare = _strdup("01/01/2030");
    rNou.nrPersoane = 2;
    rNou.pret = 3000;
    rNou.durata = 5;
    inserareDupaPrimulCuPret(&cap, 2000, rNou);

    transformaInListaCirculara(cap, coada);
    traversareCirculara(cap);

    printf("\n--- Lista Simplu Inlantuita ---\n");
    LS* capLS = conversieLDinLS(cap);
    traversareLS(capLS);

    dezalocareLD(&cap, &coada);
    dezalocareLD(&capSortat, &coadaSortata);
    dezalocareLS(&capLS);

    return 0;
}
//autoevaluare: 8.5/10 :) 
