#define _CRT_SECURE_NO_WARNINGS
#include<malloc.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

typedef struct Produs Produs;
typedef struct Nod Nod;
typedef struct Heap Heap;

struct Produs {
	int idProdus;
	char* denumire;
	char* categorie;
	float pret;
	int* stocuriLunare;
	int nrLuni;
	char* furnizor;
};

struct Nod {
	Produs info;
	Nod* stanga;
	Nod* dreapta;
};

struct Heap{
	Produs* vector;
	int dim;
	int capacitate;
};

Produs initProdus(int id, const char* denumire, const char* categorie, float pret, int* stocuri, int nrLuni, const char* furnizor) {
	Produs p;
	p.idProdus = id;
	p.pret = pret;
	p.nrLuni = nrLuni;

	p.denumire = (char*)malloc(strlen(denumire) + 1);
	strcpy(p.denumire, denumire);

	p.categorie = (char*)malloc(strlen(categorie) + 1);
	strcpy(p.categorie, categorie);

	p.furnizor = (char*)malloc(strlen(furnizor) + 1);
	strcpy(p.furnizor, furnizor);

	p.stocuriLunare = (int*)malloc(sizeof(int) * nrLuni);
	for (int i = 0; i < nrLuni; i++) {
		p.stocuriLunare[i] = stocuri[i];
	}

	return p;
}

void afisareProdus(Produs p) {
	printf("\nProdus ID: %d | Denumire: %s | Categorie: %s | Pret: %.2f | Furnizor: %s | Stocuri lunare: ",
		p.idProdus, p.denumire, p.categorie, p.pret, p.furnizor);
	for (int i = 0; i < p.nrLuni; i++) {
		printf("%d ", p.stocuriLunare[i]);
	}
}

void inserareBST(Nod** radacina, Produs p) {
	if (*radacina == NULL) {
		Nod* nou = (Nod*)malloc(sizeof(Nod));
		nou->info = p;
		nou->stanga = NULL;
		nou->dreapta = NULL;
		*radacina = nou;
	}
	else if (p.idProdus < (*radacina)->info.idProdus) {
		inserareBST(&(*radacina)->stanga, p);
	}
	else {
		inserareBST(&(*radacina)->dreapta, p);
	}
}

void citireDinFisier(const char* numeFisier, Nod** radacina) {
	FILE* f = fopen(numeFisier, "r");
	if (!f) {
		printf("Eroare la deschiderea fisierului.\n");
		return;
	}

	while (!feof(f)) {
		int id, nrLuni;
		char denumire[100], categorie[100], furnizor[100];
		float pret;

		int rezultat = fscanf(f, "%d %s %s %f %d", &id, denumire, categorie, &pret, &nrLuni);
		if (rezultat != 5) {
			break;
		}

		int* stocuri = (int*)malloc(sizeof(int) * nrLuni);
		int ok = 1;
		for (int i = 0; i < nrLuni && ok; i++) {
			if (fscanf(f, "%d", &stocuri[i]) != 1)
				ok = 0;
		}

		if (fscanf(f, "%s", furnizor) != 1)
			ok = 0;

		if (ok == 0) {
			printf("Eroare: linie invalida in fisier pentru produs cu ID: %d\n", id);
		}

		if (ok) {
			Produs p = initProdus(id, denumire, categorie, pret, stocuri, nrLuni, furnizor);
			inserareBST(radacina, p);
		}

		free(stocuri); 
	}

	fclose(f);
}

void parcurgereInordine(Nod* radacina) {
	if (radacina) {
		parcurgereInordine(radacina->stanga);
		afisareProdus(radacina->info);
		parcurgereInordine(radacina->dreapta);
	}
}

int esteFrunza(Nod* nod) {
	return nod && nod->stanga == NULL && nod->dreapta == NULL;
}

int numaraFrunze(Nod* radacina) {
	if (radacina == NULL)
		return 0;
	if (esteFrunza(radacina))
		return 1;
	return numaraFrunze(radacina->stanga) + numaraFrunze(radacina->dreapta);
}

int numarFrunzeSubarboreDrept(Nod* radacina) {
	if (radacina && radacina->dreapta)
		return numaraFrunze(radacina->dreapta);
	return 0;
}

Heap initHeap(int capacitate) {
	Heap h;
	h.vector = (Produs*)malloc(sizeof(Produs) * capacitate);
	h.dim = 0;
	h.capacitate = capacitate;
	return h;
}

void swap(Produs* a, Produs* b) {
	Produs aux = *a;
	*a = *b;
	*b = aux;
}

void filtrareHeapBottomUp(Heap* h, int index) {
	if (index == 0) return;
	int parinte = (index - 1) / 2;
	if (h->vector[index].pret < h->vector[parinte].pret) {
		swap(&h->vector[index], &h->vector[parinte]);
		filtrareHeapBottomUp(h, parinte);
	}
}

Produs copiereProdus(Produs p) {
	Produs copie;
	copie.idProdus = p.idProdus;
	copie.pret = p.pret;
	copie.nrLuni = p.nrLuni;

	copie.denumire = (char*)malloc(strlen(p.denumire) + 1);
	strcpy(copie.denumire, p.denumire);

	copie.categorie = (char*)malloc(strlen(p.categorie) + 1);
	strcpy(copie.categorie, p.categorie);

	copie.furnizor = (char*)malloc(strlen(p.furnizor) + 1);
	strcpy(copie.furnizor, p.furnizor);

	copie.stocuriLunare = (int*)malloc(sizeof(int) * p.nrLuni);
	for (int i = 0; i < p.nrLuni; i++) {
		copie.stocuriLunare[i] = p.stocuriLunare[i];
	}

	return copie;
}

void inserareHeap(Heap* h, Produs p) {
	if (h->dim < h->capacitate) {
		h->vector[h->dim] = copiereProdus(p);  
		filtrareHeapBottomUp(h, h->dim);
		h->dim++;
	}
}

void copiereInHeap(Nod* radacina, Heap* h) {
	if (radacina) {
		inserareHeap(h, radacina->info);
		copiereInHeap(radacina->stanga, h);
		copiereInHeap(radacina->dreapta, h);
	}
}

void filtrareHeapPeCategorie(Heap* h, const char* categorie) {
	for (int i = 0; i < h->dim; i++) {
		if (strcmp(h->vector[i].categorie, categorie) != 0) {
			printf("\n[Filtrat] %s eliminat (nu e %s)", h->vector[i].denumire, categorie);
		}
	}
}

void afisareFrunzeHeap(Heap h) {
	printf("\nFrunze din heap (produse finale):\n");
	for (int i = h.dim / 2; i < h.dim; i++) {
		afisareProdus(h.vector[i]);
	}
}

void afisareHeap(Heap h) {
	printf("\nHeap complet:\n");
	for (int i = 0; i < h.dim; i++) {
		afisareProdus(h.vector[i]);
	}
}

void stergerePeCriteriu(Heap* h, float prag) {
	int i = 0;
	while (i < h->dim) {
		if (h->vector[i].pret < prag) {
			h->vector[i] = h->vector[h->dim - 1];
			h->dim--;
			filtrareHeapBottomUp(h, i); 
		}
		else {
			i++;
		}
	}
}

void dezalocareBST(Nod** radacina) {
	if (*radacina) {
		dezalocareBST(&(*radacina)->stanga);
		dezalocareBST(&(*radacina)->dreapta);
		free((*radacina)->info.denumire);
		free((*radacina)->info.categorie);
		free((*radacina)->info.furnizor);
		free((*radacina)->info.stocuriLunare);
		free(*radacina);
	}
}

void dezalocareHeap(Heap* h) {
	for (int i = 0; i < h->dim; i++) {
		free(h->vector[i].denumire);
		free(h->vector[i].categorie);
		free(h->vector[i].furnizor);
		free(h->vector[i].stocuriLunare);
	}
	free(h->vector);
}

int main() {
	Nod* radacina = NULL;

	citireDinFisier("produse.txt", &radacina);

	printf("\n========== PARCURGERE INORDINE A ARBORELUI ==========\n");
	parcurgereInordine(radacina);

	printf("\n\n========== NUMAR FRUNZE DIN SUBARBORELE DREPT ==========\n");
	int nrFrunzeDreapta = numarFrunzeSubarboreDrept(radacina);
	printf("Numarul de frunze din subarborele drept este: %d\n", nrFrunzeDreapta);

	Heap h = initHeap(10);
	copiereInHeap(radacina, &h);

	printf("\n========== AFISARE HEAP COMPLET ==========\n");
	afisareHeap(h);

	printf("\n\n========== AFISARE NODURI FRUNZA DIN HEAP ==========\n");
	afisareFrunzeHeap(h);

	printf("\n\n==========  FILTRARE HEAP (AFISEAZA CE NU E 'Alimentar') ==========\n");
	filtrareHeapPeCategorie(&h, "Alimentar");

	printf("\n\n========== STERGERE PRODUSE CU PRET < 100 DIN HEAP ==========\n");
	stergerePeCriteriu(&h, 100);

	afisareHeap(h);
	printf("\n");
	dezalocareBST(&radacina);
	dezalocareHeap(&h);

	return 0;
}

//autoevaluare: un 8.75/10 cred ca este bine :) multumesc!
