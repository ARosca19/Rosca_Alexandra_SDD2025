#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef struct Examen Examen;
typedef struct NodLista NodLista;
typedef struct NodGraf NodGraf;
typedef struct NodArb NodArb;

struct Examen {
    int id;
    char* materie;      
    float notaScris;
    float notaOral;
    char* numeStudent;  
    char* data;   
    int durataMinute;
};

struct NodLista {
    int index;
    NodLista* next;
};

struct NodGraf {
    Examen info;
    NodLista* vecini;
};

struct NodArb {
    Examen info;
    NodArb* stanga;
    NodArb* dreapta;
    int inaltime;
};

Examen citireExamenDinFisier(FILE* f) {
    Examen e;
    char buffer[100];
    fscanf(f, "%d", &e.id);

    fscanf(f, "%s", buffer);
    e.materie = malloc(strlen(buffer) + 1);
    strcpy(e.materie, buffer);

    fscanf(f, "%f %f", &e.notaScris, &e.notaOral);

    fscanf(f, "%s", buffer);
    e.numeStudent = malloc(strlen(buffer) + 1);
    strcpy(e.numeStudent, buffer);

    fscanf(f, "%s", buffer);
    e.data = malloc(strlen(buffer) + 1);
    strcpy(e.data, buffer);

    fscanf(f, "%d", &e.durataMinute);

    return e;
}

void afisareExamen(Examen e) {
    printf("ID: %d, Materie: %s, Note: %.2f/%.2f, Student: %s, Data: %s, Durata: %d min\n",
        e.id, e.materie, e.notaScris, e.notaOral, e.numeStudent, e.data, e.durataMinute);
}

void inserareLista(NodLista** cap, int index) {
    NodLista* nou = (NodLista*)malloc(sizeof(NodLista));
    nou->index = index;
    nou->next = *cap;
    *cap = nou;
}

void afisareGraf(NodGraf* graf, int nr) {
    for (int i = 0; i < nr; i++) {
        afisareExamen(graf[i].info);
        printf("Vecini: ");
        NodLista* p = graf[i].vecini;
        while (p) {
            printf("%d ", p->index);
            p = p->next;
        }
        printf("\n");
    }
}

float calculeazaMediaNote(NodGraf* graf, int nr) {
    float suma = 0;
    for (int i = 0; i < nr; i++) {
        suma += (graf[i].info.notaScris + graf[i].info.notaOral) / 2.0f;
    }
    return suma / nr;
}

int maxim(int a, int b) {
    return (a > b) ? a : b;
}

int inaltime(NodArb* rad) {
    if (rad == NULL)
        return 0;
    return rad->inaltime;
}

NodArb* rotatieDreapta(NodArb* y) {
    NodArb* x = y->stanga;
    NodArb* T2 = x->dreapta;

    x->dreapta = y;
    y->stanga = T2;

    y->inaltime = maxim(inaltime(y->stanga), inaltime(y->dreapta)) + 1;
    x->inaltime = maxim(inaltime(x->stanga), inaltime(x->dreapta)) + 1;

    return x;
}

NodArb* rotatieStanga(NodArb* x) {
    NodArb* y = x->dreapta;
    NodArb* T2 = y->stanga;

    y->stanga = x;
    x->dreapta = T2;

    x->inaltime = maxim(inaltime(x->stanga), inaltime(x->dreapta)) + 1;
    y->inaltime = maxim(inaltime(y->stanga), inaltime(y->dreapta)) + 1;

    return y;
}

int getFactorEchilibru(NodArb* rad) {
    if (rad == NULL)
        return 0;
    return inaltime(rad->stanga) - inaltime(rad->dreapta);
}

NodArb* inserareArboreAVL(NodArb* rad, Examen e) {
    if (rad == NULL) {
        NodArb* nou = (NodArb*)malloc(sizeof(NodArb));
        nou->info = e;
        nou->stanga = nou->dreapta = NULL;
        nou->inaltime = 1;
        return nou;
    }

    if (e.id < rad->info.id)
        rad->stanga = inserareArboreAVL(rad->stanga, e);
    else if (e.id > rad->info.id)
        rad->dreapta = inserareArboreAVL(rad->dreapta, e);
    else
        return rad;

    rad->inaltime = 1 + maxim(inaltime(rad->stanga), inaltime(rad->dreapta));

    int balans = getFactorEchilibru(rad);
    if (balans > 1 && e.id < rad->stanga->info.id)
        return rotatieDreapta(rad);
    if (balans < -1 && e.id > rad->dreapta->info.id)
        return rotatieStanga(rad);
    if (balans > 1 && e.id > rad->stanga->info.id) {
        rad->stanga = rotatieStanga(rad->stanga);
        return rotatieDreapta(rad);
    }

    if (balans < -1 && e.id < rad->dreapta->info.id) {
        rad->dreapta = rotatieDreapta(rad->dreapta);
        return rotatieStanga(rad);
    }

    return rad;
}

void afisareArbore(NodArb* rad) {
    if (rad) {
        afisareArbore(rad->stanga);
        afisareExamen(rad->info);
        afisareArbore(rad->dreapta);
    }
}

typedef struct NodListaRez {
    Examen info;
    struct NodListaRez* next;
} NodListaRez;

void inserareListaRez(NodListaRez** cap, Examen e) {
    NodListaRez* nou = (NodListaRez*)malloc(sizeof(NodListaRez));
    nou->info = e;
    nou->next = *cap;
    *cap = nou;
}
void cautaExamenePesteNota(NodArb* rad, NodListaRez** cap, float prag) {
    if (rad) {
        float medie = (rad->info.notaScris + rad->info.notaOral) / 2.0f;
        if (medie >= prag)
            inserareListaRez(cap, rad->info);
        cautaExamenePesteNota(rad->stanga, cap, prag);
        cautaExamenePesteNota(rad->dreapta, cap, prag);
    }
}

void afisareListaRez(NodListaRez* cap) {
    while (cap) {
        afisareExamen(cap->info);
        cap = cap->next;
    }
}

float sumaDurataMaterie(NodArb* rad, const char* materie, float prag) {
    if (rad == NULL) return 0;
    float suma = 0;
    float medie = (rad->info.notaScris + rad->info.notaOral) / 2.0f;
    if (strcmp(rad->info.materie, materie) == 0 && medie > prag)
        suma += rad->info.durataMinute;
    suma += sumaDurataMaterie(rad->stanga, materie, prag);
    suma += sumaDurataMaterie(rad->dreapta, materie, prag);
    return suma;
}

void BFS(NodGraf* graf, int nr, int start) {
    int* viz = (int*)calloc(nr, sizeof(int));
    int coada[1000], prim = 0, ultim = 0;
    coada[ultim++] = start;
    viz[start] = 1;

    while (prim < ultim) {
        int nod = coada[prim++];
        afisareExamen(graf[nod].info);
        NodLista* p = graf[nod].vecini;
        while (p) {
            if (!viz[p->index]) {
                viz[p->index] = 1;
                coada[ultim++] = p->index;
            }
            p = p->next;
        }
    }
    free(viz);
}

NodArb* nodMinim(NodArb* nod) {
    NodArb* curent = nod;
    while (curent && curent->stanga != NULL)
        curent = curent->stanga;
    return curent;
}

NodArb* stergereNodAVL(NodArb* rad, int id) {
    if (rad == NULL)
        return rad;

    if (id < rad->info.id)
        rad->stanga = stergereNodAVL(rad->stanga, id);
    else if (id > rad->info.id)
        rad->dreapta = stergereNodAVL(rad->dreapta, id);
    else {
        if (rad->stanga == NULL || rad->dreapta == NULL) {
            NodArb* temp = rad->stanga ? rad->stanga : rad->dreapta;

            if (temp == NULL) {
                free(rad->info.materie);
                free(rad->info.numeStudent);
                free(rad->info.data);
                free(rad);
                return NULL;
            }
            else {
                free(rad->info.materie);
                free(rad->info.numeStudent);
                free(rad->info.data);

                *rad = *temp;
                free(temp);
            }
        }
        else {
            NodArb* temp = nodMinim(rad->dreapta);

            free(rad->info.materie);
            free(rad->info.numeStudent);
            free(rad->info.data);

            rad->info.id = temp->info.id;

            rad->info.materie = malloc(strlen(temp->info.materie) + 1);
            strcpy(rad->info.materie, temp->info.materie);

            rad->info.notaScris = temp->info.notaScris;
            rad->info.notaOral = temp->info.notaOral;

            rad->info.numeStudent = malloc(strlen(temp->info.numeStudent) + 1);
            strcpy(rad->info.numeStudent, temp->info.numeStudent);

            rad->info.data = malloc(strlen(temp->info.data) + 1);
            strcpy(rad->info.data, temp->info.data);

            rad->info.durataMinute = temp->info.durataMinute;

            rad->dreapta = stergereNodAVL(rad->dreapta, temp->info.id);
        }
    }

    if (rad == NULL)
        return rad;

    rad->inaltime = 1 + maxim(inaltime(rad->stanga), inaltime(rad->dreapta));

    int balans = getFactorEchilibru(rad);

    if (balans > 1 && getFactorEchilibru(rad->stanga) >= 0)
        return rotatieDreapta(rad);

    if (balans > 1 && getFactorEchilibru(rad->stanga) < 0) {
        rad->stanga = rotatieStanga(rad->stanga);
        return rotatieDreapta(rad);
    }

    if (balans < -1 && getFactorEchilibru(rad->dreapta) <= 0)
        return rotatieStanga(rad);

    if (balans < -1 && getFactorEchilibru(rad->dreapta) > 0) {
        rad->dreapta = rotatieDreapta(rad->dreapta);
        return rotatieStanga(rad);
    }

    return rad;
}


void eliberareLista(NodLista* cap) {
    while (cap) {
        NodLista* temp = cap;
        cap = cap->next;
        free(temp);
    }
}

void eliberareGraf(NodGraf* graf, int nr) {
    for (int i = 0; i < nr; i++) {
        free(graf[i].info.materie);
        free(graf[i].info.numeStudent);
        free(graf[i].info.data);
        eliberareLista(graf[i].vecini);
    }
    free(graf);
}

void eliberareArbore(NodArb* rad) {
    if (rad) {
        eliberareArbore(rad->stanga);
        eliberareArbore(rad->dreapta);
        free(rad->info.materie);
        free(rad->info.numeStudent);
        free(rad->info.data);
        free(rad);
    }
}

void eliberareListaRez(NodListaRez* cap) {
    while (cap) {
        NodListaRez* temp = cap;
        cap = cap->next;
        free(temp);
    }
}

int main() {
    FILE* f = fopen("examene.txt", "r");
    if (!f) {
        printf("Nu s-a putut deschide fisierul examene.txt\n");
        return 1;
    }

    int n;
    fscanf(f, "%d", &n);

    NodGraf* graf = (NodGraf*)malloc(sizeof(NodGraf) * n);
    for (int i = 0; i < n; i++) {
        graf[i].info = citireExamenDinFisier(f);
        graf[i].vecini = NULL;
    }

    int m;
    fscanf(f, "%d", &m);
    for (int i = 0; i < m; i++) {
        int x, y;
        fscanf(f, "%d %d", &x, &y);
        inserareLista(&graf[x].vecini, y);
        inserareLista(&graf[y].vecini, x);
    }
    fclose(f);

    printf("--------------Graful citit--------------\n");
    afisareGraf(graf, n);

    float medie = calculeazaMediaNote(graf, n);
    printf("\n--------------Media notelor tuturor examenelor este: %.2f--------------\n", medie);

    NodArb* radAVL = NULL;
    for (int i = 0; i < n; i++) {
        radAVL = inserareArboreAVL(radAVL, graf[i].info);
    }

    printf("\n--------------Arbore AVL (inordine)--------------\n");
    afisareArbore(radAVL);

    NodListaRez* listaRez = NULL;

    cautaExamenePesteNota(radAVL, &listaRez, 8.0f);

    printf("\n--------------Examene cu media peste sau egala cu 8--------------\n");
    if (listaRez == NULL)
        printf("Nu exista examene cu media peste sau egala cu 8\n");
    else
        afisareListaRez(listaRez);

    float sumaDurata = sumaDurataMaterie(radAVL, "Mate", 7.0f);
    printf("\n--------------Suma duratelor examenelor de Mate cu media peste 7 este: %.2f--------------\n", sumaDurata);

    printf("\n--------------Parcurgere BFS a grafului incepand de la nodul 0--------------\n");
    BFS(graf, n, 0);

    printf("\n--------------Parcurgere arbore AVL (inordine)--------------\n");
    afisareArbore(radAVL);

    printf("\n--------------Stergem nodul cu id 2 din arborele AVL--------------\n");
    radAVL = stergereNodAVL(radAVL, 2);
    afisareArbore(radAVL);

    eliberareListaRez(listaRez);
    return 0;
}


//Autoevaluare: am trecut de deadline imi pare rau dar am incercat sa fac cat mai bine 6/10 :)
