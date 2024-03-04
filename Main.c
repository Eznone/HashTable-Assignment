// Enzo Tres mediano 2211731
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 7
#define INIT_SIZE 1031

// structs------------------------------------------------------------------------------------------
typedef struct plate Plate;
struct plate {
    char* lics;
    int check;
};
typedef struct plateList PlateList;
struct hashTable {
    Plate* HashArray;
    int maxSize;
};
typedef struct hashTable HashTable;

// Prototipos---------------------------------------------------------------------------------------
unsigned int hash(char* plate, int sizeHT);
int insertHash(HashTable* HashTable, char* liscensePlate);
void initHashTable(HashTable* ht, int size);
char** loadLicensePlates(int numPlacas);
void freeHashTable(HashTable* HashList);
int Search(HashTable* ht, char* lic);
void initPlate(Plate* plate);
void deletePlate(HashTable* ht, char* lics);

// Main---------------------------------------------------------------------------------------------
int main(void) {
    int numPlacas;
    printf("How many license plates? (Maximum input acceptable is 1000) : \n");
    scanf("%d", &numPlacas);
#define COUNT numPlacas
    printf("Amount of license plates: %d\n", COUNT);
    int i;
    int count = 0;

    // Making array from the file
    char** licenseArray = loadLicensePlates(numPlacas);

    // Setting up Time
    clock_t start, end;
    double cpuTimeUsed;

    // Making and Initalizaing the HashTable
    HashTable ht;
    initHashTable(&ht, INIT_SIZE);

    // Comeco do test vvv
    // -----------------------------------------------------------------------------
    // Test de inserir
    start = clock();
    for (i = 0; i < COUNT; i++) {
        count += insertHash(&ht, licenseArray[i]);
    }
    end = clock();
    cpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\n\n------------------------\nNumero de colisoes: "
        "%d\n------------------------\n",
        count);
    printf("\nTempo para completar insert: %.10f segundos\n", cpuTimeUsed);

    // Search just gets the position
    start = clock();
    for (i = 0; i < COUNT; i++) {
        Search(&ht, licenseArray[i]);
    }
    end = clock();
    cpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nTempo para completar buscar: %.10f segundos\n", cpuTimeUsed);

    start = clock();
    for (i = 0; i < COUNT; i++) {
        deletePlate(&ht, licenseArray[i]);
    }
    end = clock();
    cpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\nTempo para completar deletar: %.10f segundos\n", cpuTimeUsed);

    freeHashTable(&ht);
}
// Functions
// ---------------------------------------------------------------------------------------

unsigned int hash(char* plate, int sizeHT) {
    unsigned int sum = 0;

    for (int i = 0; i < SIZE; i++) {
        sum ^= (sum * sum) + plate[i] << (i % 4);
        /*The ^= is the power of using bits, the << means the value on the left is
        being multiplied by 2 the amount of times determined by the number on the
        left*/
    }
    return sum % sizeHT;
    /*Here it is %MAX because the %MAX will return the remainder which should be
     * in the range of 0 to MAX*/
}

void initHashTable(HashTable* ht, int size) {
    ht->HashArray = (Plate*)malloc(size * sizeof(Plate));
    if (ht->HashArray == NULL) {
        printf("Error allocating");
        exit(2);
    }
    ht->maxSize = size;
    for (int i = 0; i < ht->maxSize; i++) {
        initPlate(&ht->HashArray[i]);
    }
}

int insertHash(HashTable* ht, char* liscensePlate) {
    // Return is for the counting of collisions
    int count = 0;
    // Making new HashTable
    unsigned int hashedNum = hash(liscensePlate, ht->maxSize);

    // Inserting values to new HashTable
    while (ht->HashArray[hashedNum].lics != NULL) {
        hashedNum = (hashedNum + 1) % ht->maxSize;
        count++;
    }
    ht->HashArray[hashedNum].lics = strdup(liscensePlate);
    return count;
}

void initPlate(Plate* plate) {
    plate->lics = NULL;
    plate->check = 0;
}

void deletePlate(HashTable* ht, char* lics) {
    int loopNum = 0;
    int hashedNum = hash(lics, ht->maxSize);
    for (;;) {
        Plate* plate = &ht->HashArray[hashedNum];
        if (plate->lics == NULL) {
            if (plate->check == 1) {
                hashedNum = (hashedNum + 1) % ht->maxSize;
                continue;
            }
            printf("Error, Plate never existed");
            exit(2);
        }
        if (strcmp(plate->lics, lics) == 0) {
            free(plate->lics);
            plate->lics = NULL;
            plate->check = 1;
            return;
        }
        hashedNum = (hashedNum + 1) % ht->maxSize;
    }
}

int Search(HashTable* ht, char* lic) {
    int hashedNum = hash(lic, ht->maxSize);
    for (;;) {
        Plate* plate = &(ht->HashArray[hashedNum]);
        if (plate->lics == NULL) {
            hashedNum = (hashedNum + 1) % ht->maxSize;
            //printf("don't match");
            continue;
        }
        if (strcmp(plate->lics, lic) == 0) {
            return hashedNum;
        }
        //printf("don't match");
        hashedNum = (hashedNum + 1) % ht->maxSize;
    }
}

void freeHashTable(HashTable* ht) {
    if (ht == NULL) {
        return;
    }
    for (int i = 0; i < ht->maxSize; i++) {
        if (ht->HashArray[i].lics != NULL) {
            free(ht->HashArray[i].lics);
        }
    }
    free(ht->HashArray);
}

char** loadLicensePlates(int numPlacas) {
    FILE* fptr;
    fptr = fopen("licensePlates.txt", "r");
    if (fptr == NULL) {
        printf("Error opening file!");
        exit(1);
    }
    // Allocamento de um array dos licensePlates
    char** licensePlate = (char**)malloc(numPlacas * sizeof(char*));
    if (licensePlate == NULL) {
        printf("Error allocating space");
        exit(2);
    }
    // Buffer to get the correct size of the storage needed for the string
    char buffer[SIZE + 1];
    for (int i = 0; i < numPlacas; i++) {
        // Esse scanf vai ser como a programa vai saber ler e pular cada \n para que
        // pode ir pra proxima linha
        fscanf(fptr, "%[^\n]\n", buffer);
        // Instead of strdup, can also be done with strcpy method
        licensePlate[i] = strdup(buffer);
    }

    fclose(fptr);
    return licensePlate;
}