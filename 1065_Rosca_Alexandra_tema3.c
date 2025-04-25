#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rezervare Rezervare;
typedef struct Nod Nod;
typedef struct HashTable HashTable;

struct Rezervare {
	char* numeClient;
	char* destinatie;
	char* dataRezervare;
	int nrPersoane;
	float pret;
	int durata;
	int codRezervare;
};

struct Nod {
	Rezervare info;
	Nod* next;
};

struct HashTable {
	int dim;
	Nod** vector;
};

void afisareRezervare(Rezervare r) {
	printf("\nRezervare #%d | Client: %s | Destinatie: %s | Data: %s | Persoane: %d | Pret: %.2f | Durata: %d zile",
		r.codRezervare, r.numeClient, r.destinatie, r.dataRezervare, r.nrPersoane, r.pret, r.durata);
}

Rezervare citireRezervare(FILE* f) {
	Rezervare r;
	char buffer[100];

	fgets(buffer, 100, f);
	buffer[strcspn(buffer, "\n")] = 0;
	r.numeClient = (char*)malloc(strlen(buffer) + 1);
	strcpy(r.numeClient, buffer);

	fgets(buffer, 100, f);
	buffer[strcspn(buffer, "\n")] = 0;
	r.destinatie = (char*)malloc(strlen(buffer) + 1);
	strcpy(r.destinatie, buffer);

	fgets(buffer, 100, f);
	buffer[strcspn(buffer, "\n")] = 0;
	r.dataRezervare = (char*)malloc(strlen(buffer) + 1);
	strcpy(r.dataRezervare, buffer);

	fgets(buffer, 100, f);
	r.nrPersoane = atoi(buffer);

	fgets(buffer, 100, f);
	r.pret = atof(buffer);

	fgets(buffer, 100, f);
	r.durata = atoi(buffer);

	fgets(buffer, 100, f);
	r.codRezervare = atoi(buffer);

	return r;
}

void inserareLaSfarsit(Nod** cap, Rezervare r) {
	Nod* nou = (Nod*)malloc(sizeof(Nod));
	nou->info = r;
	nou->next = NULL;

	if (*cap == NULL) {
		*cap = nou;
	}
	else {
		Nod* temp = *cap;
		while (temp->next)
			temp = temp->next;
		temp->next = nou;
	}
}

void traversareLista(Nod* cap) {
	while (cap) {
		printf("Rezervare %d: Client: %s  Destinatie: %s  Data: %s  Persoane: %d  Pret: %.2f  Durata: %d zile\n",
			cap->info.codRezervare, cap->info.numeClient, cap->info.destinatie, cap->info.dataRezervare,
			cap->info.nrPersoane, cap->info.pret, cap->info.durata);
		cap = cap->next;
	}
}


HashTable initHashTable(int dim) {
	HashTable tabela;
	tabela.dim = dim;
	tabela.vector = (Nod**)malloc(sizeof(Nod*) * dim);
	for (int i = 0; i < dim; i++)
		tabela.vector[i] = NULL;
	return tabela;
}

int hash(char litera, int dim) {
	return litera % dim;
}

void inserareInHashTable(HashTable tabela, Rezervare r) {
	int poz = hash(r.numeClient[0], tabela.dim);
	inserareLaSfarsit(&(tabela.vector[poz]), r);
}

void traversareHashTable(HashTable tabela) {
	for (int i = 0; i < tabela.dim; i++) {
		printf("Pozitia %d:", i);
		if (tabela.vector[i] == NULL) {
			printf(" (gol)\n");
		}
		else {
			printf("\n");
			traversareLista(tabela.vector[i]);
		}
	}
}


void dezalocareLista(Nod** cap) {
	while (*cap) {
		Nod* temp = *cap;
		free(temp->info.numeClient);
		free(temp->info.destinatie);
		free(temp->info.dataRezervare);
		*cap = (*cap)->next;
		free(temp);
	}
}

void dezalocareHashTable(HashTable tabela) {
	for (int i = 0; i < tabela.dim; i++) {
		dezalocareLista(&(tabela.vector[i]));
	}
	free(tabela.vector);
}

int nrRezervariByClient(HashTable tabela, const char* client) {
	int contor = 0;
	int poz = hash(client[0], tabela.dim);
	Nod* cap = tabela.vector[poz];
	while (cap) {
		if (strcmp(cap->info.numeClient, client) == 0)
			contor++;
		cap = cap->next;
	}
	return contor;
}

void stergereDupaPret(HashTable* tabela, float pragPret) {
	for (int i = 0; i < tabela->dim; i++) {
		Nod* curent = tabela->vector[i];
		Nod* anterior = NULL;

		while (curent) {
			if (curent->info.pret < pragPret) {
				Nod* deSters = curent;
				if (anterior == NULL) {
					tabela->vector[i] = curent->next;
				}
				else {
					anterior->next = curent->next;
				}
				curent = curent->next;

				free(deSters->info.numeClient);
				free(deSters->info.destinatie);
				free(deSters->info.dataRezervare);
				free(deSters);
			}
			else {
				anterior = curent;
				curent = curent->next;
			}
		}
	}
}

Rezervare* rezervariPePozitie(HashTable tabela, int pozitie, int* nrRezervari) {
	*nrRezervari = 0;
	if (pozitie < 0 || pozitie >= tabela.dim) {
		return NULL;
	}

	Nod* temp = tabela.vector[pozitie];
	Rezervare* vector = NULL;

	while (temp) {
		vector = (Rezervare*)realloc(vector, (*nrRezervari + 1) * sizeof(Rezervare));

		vector[*nrRezervari].numeClient = (char*)malloc(strlen(temp->info.numeClient) + 1);
		strcpy(vector[*nrRezervari].numeClient, temp->info.numeClient);

		(*nrRezervari)++;
		temp = temp->next;
	}

	return vector;
}

void afisareVectorRezervari(Rezervare* vector, int nr) {
	for (int i = 0; i < nr; i++) {
		afisareRezervare(vector[i]);
	}
}


int main() {
	FILE* file = fopen("rezervari3.txt", "r");
	if (!file) {
		printf("Eroare la deschiderea fisierului.\n");
		return -1;
	}

	Rezervare* vectorRez = NULL;
	int nrRez = 0;

	while (!feof(file)) {
		vectorRez = (Rezervare*)realloc(vectorRez, (nrRez + 1) * sizeof(Rezervare));
		vectorRez[nrRez++] = citireRezervare(file);
	}

	HashTable tabela = initHashTable(6);
	for (int i = 0; i < nrRez; i++) {
		inserareInHashTable(tabela, vectorRez[i]);
	}

	printf("\n=== AFISARE HASH TABLE ===\n");
	traversareHashTable(tabela);
	printf("\n");

	printf("Popescu are %d rezervari.\n", nrRezervariByClient(tabela, "Popescu"));

	int pozitieCautata = 3;  
	int nrGasite = 0;

	Rezervare* vector = rezervariPePozitie(tabela, pozitieCautata, &nrGasite);
	if (vector) {
		printf("\n=== VECTOR DE REZERVARI PE POZITIA %d ===\n", pozitieCautata);
		afisareVectorRezervari(vector, nrGasite);

		for (int i = 0; i < nrGasite; i++) {
			free(vector[i].numeClient);
			free(vector[i].destinatie);
			free(vector[i].dataRezervare);
		}
		free(vector);
	}
	else {
		printf("\nPozitia %d nu contine rezervari.\n", pozitieCautata);
	}


	printf("\n=== STERGERE REZERVARI CU PRET < 500 ===\n");
	stergereDupaPret(&tabela, 500.0f);

	printf("\n=== HASH TABLE DUPA STERGERE ===\n");
	traversareHashTable(tabela);

	dezalocareHashTable(tabela);
	free(vectorRez);
	fclose(file);
	

	return 0;
}


//Autoevaluare
//am atins toate punctele dar tema am dat-o tarziu imi cer scuze, eu mi-as da un 8/10 :)
