#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rezervare Rezervare;
typedef struct Nod Nod;

struct Rezervare {
    int id;
    char* nume_client;
    char* data;
    char* ora;
    char* tip_camera;
    int numar_persoane;
    char* comentarii;
};

struct Nod {
    Rezervare info;
    struct Nod* next;
};

char* my_strdup(const char* str) {
    if (!str) return NULL;
    char* copie = malloc(strlen(str) + 1);
    if (copie) strcpy(copie, str);
    return copie;
}

void citireDinFisiere(const char* nume_fisier, Nod** cap) {
    FILE* f = fopen(nume_fisier, "r");
    if (!f) {
        printf("Eroare la deschiderea fisierului!\n");
        return;
    }

    while (!feof(f)) {
        Rezervare temp;
        char nume[100], data[20], ora[20], tip_camera[50], comentarii[200];

        int n = fscanf(f, "%d %s %s %s %s %d", &temp.id, nume, data, ora, tip_camera, &temp.numar_persoane);
        fgetc(f);
        fgets(comentarii, 200, f);

        if (n == 6) {
            temp.nume_client = my_strdup(nume);
            temp.data = my_strdup(data);
            temp.ora = my_strdup(ora);
            temp.tip_camera = my_strdup(tip_camera);
            temp.comentarii = my_strdup(comentarii);

            Nod* newNode = malloc(sizeof(Nod));
            newNode->info = temp;
            newNode->next = NULL;

            if (*cap == NULL) {
                *cap = newNode;
            }
            else {
                Nod* tempNode = *cap;
                while (tempNode->next) {
                    tempNode = tempNode->next;
                }
                tempNode->next = newNode;
            }
        }
    }
    fclose(f);
}

void inserareLaSfarsit(Nod** cap, Rezervare newRezervare) {
    Nod* newNode = malloc(sizeof(Nod));
    newNode->info.id = newRezervare.id;
    newNode->info.numar_persoane = newRezervare.numar_persoane;
    newNode->info.nume_client = my_strdup(newRezervare.nume_client);
    newNode->info.data = my_strdup(newRezervare.data);
    newNode->info.ora = my_strdup(newRezervare.ora);
    newNode->info.tip_camera = my_strdup(newRezervare.tip_camera);
    newNode->info.comentarii = my_strdup(newRezervare.comentarii);
    newNode->next = NULL;

    if (*cap == NULL) {
        *cap = newNode;
    }
    else {
        Nod* temp = *cap;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    printf("\n");
}

void afisareLista(Nod* cap) {
    if (!cap) {
        printf("Lista este goala.\n\n");
        return;
    }
    Nod* temp = cap;
    while (temp) {
        printf("ID: %d, Nume: %s, Data: %s, Ora: %s, Tip camera: %s, Nr persoane: %d, Comentarii: %s\n\n",
            temp->info.id, temp->info.nume_client, temp->info.data, temp->info.ora,
            temp->info.tip_camera, temp->info.numar_persoane, temp->info.comentarii);
        temp = temp->next;
    }
    printf("\n");
}

Rezervare* cautaRezervare(Nod* cap, int id) {
    while (cap) {
        if (cap->info.id == id) return &cap->info;
        cap = cap->next;
    }
    return NULL;
}

void stergerePrimulElement(Nod** cap) {
    if (*cap == NULL) return;

    Nod* temp = *cap;
    *cap = (*cap)->next;

    free(temp->info.nume_client);
    free(temp->info.data);
    free(temp->info.ora);
    free(temp->info.tip_camera);
    free(temp->info.comentarii);
    free(temp);
}

void stergereRezervare(Nod** cap, int id) {
    Nod* temp = *cap, * prev = NULL;
    while (temp && temp->info.id != id) {
        prev = temp;
        temp = temp->next;
    }
    if (!temp) return;

    if (!prev) *cap = temp->next;
    else prev->next = temp->next;

    free(temp->info.nume_client);
    free(temp->info.data);
    free(temp->info.ora);
    free(temp->info.tip_camera);
    free(temp->info.comentarii);
    free(temp);
}

void sortareLista(Nod** cap) {
    if (*cap == NULL) return;

    Nod* i = *cap;
    Nod* j = NULL;
    int temp_id;
    char* temp_nume_client;
    char* temp_data;
    char* temp_ora;
    char* temp_tip_camera;
    char* temp_comentarii;
    int temp_numar_persoane;

    while (i) {
        j = i->next;
        while (j) {
            if (i->info.id > j->info.id) {
                temp_id = i->info.id;
                temp_nume_client = i->info.nume_client;
                temp_data = i->info.data;
                temp_ora = i->info.ora;
                temp_tip_camera = i->info.tip_camera;
                temp_comentarii = i->info.comentarii;
                temp_numar_persoane = i->info.numar_persoane;

                i->info.id = j->info.id;
                i->info.nume_client = j->info.nume_client;
                i->info.data = j->info.data;
                i->info.ora = j->info.ora;
                i->info.tip_camera = j->info.tip_camera;
                i->info.comentarii = j->info.comentarii;
                i->info.numar_persoane = j->info.numar_persoane;

                j->info.id = temp_id;
                j->info.nume_client = temp_nume_client;
                j->info.data = temp_data;
                j->info.ora = temp_ora;
                j->info.tip_camera = temp_tip_camera;
                j->info.comentarii = temp_comentarii;
                j->info.numar_persoane = temp_numar_persoane;
            }
            j = j->next;
        }
        i = i->next;
    }
}

void dezalocare(Nod* cap) {
    Nod* temp;
    while (cap) {
        temp = cap;
        cap = cap->next;
        free(temp->info.nume_client);
        free(temp->info.data);
        free(temp->info.ora);
        free(temp->info.tip_camera);
        free(temp->info.comentarii);
        free(temp);
    }
}

int main() {
    Nod* cap = NULL;

    citireDinFisiere("rezervari.txt", &cap);

    Rezervare rezervareNoua = { 10, "Popescu", "19/03/2025", "18:00", "Single", 1, "Fumator\n" };
    inserareLaSfarsit(&cap, rezervareNoua);

    printf("\nLista rezervarilor:\n");
    afisareLista(cap);

    int id_cautat = 10;
    Rezervare* rez = cautaRezervare(cap, id_cautat);
    if (rez) printf("Rezervare gasita: %s la data %s ora %s\n", rez->nume_client, rez->data, rez->ora);
    else printf("Rezervare cu ID %d nu a fost gasita!\n\n", id_cautat);

    printf("Lista dupa stergerea primului element:\n");
    stergerePrimulElement(&cap);
    afisareLista(cap);

    printf("Lista dupa sortare:\n");
    sortareLista(&cap);
    afisareLista(cap);

    stergereRezervare(&cap, 10);
    printf("Lista dupa stergere:\n");
    afisareLista(cap);

    dezalocare(cap);
    return 0;
}

//autoevaluare: 7.5/9 :) 
