//THIS FILE IMPLEMENTS THE DYNAMIC LINKED LIST USED TO STORE BOOKS DATA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define DEBUG 0

enum ERRORS
{
    ERR_MALLOC_FAIL = 2,
    ERR_STORAGE_OUT_OF_RANGE,
    ERR_FILE_NOT_FOUND
};

typedef struct node
{
    char *title;
    char *author;
    double price;
    int amount;
    struct node *next;
} book;

//METHODS

bool pushBackBook(book **storage, char *title, char *author, double price, int amount);
bool removeBook(book **storage, char *title, char *author);
bool findBook(book **storage, char *title, char *author, book **result);
bool showBook(book *storage, int index);
bool showAllBooks(book **storage);


//HELPER FUNCTIONS
void _clearString(char *wejscie, int dlg)
{
    for (int i = 0; i < dlg; i++)
    {
        wejscie[i] = '\0';
    }
}

int my_split(const char *source, char *wynikowe[], const int dlugoscTablicy, const char delim)
{
    //wynikowe[] to tablica pointerów wskazujacych na poczatek stringa do ktorego zapisujemy wyniki
    int za_delimiterem = 0;
    int przesuniecia = 0;
    while (*source != '\0')
    {
        //printf("while");
        if (*source == delim)
        {
            source++;
            za_delimiterem++;
            continue;
        }

        *wynikowe[za_delimiterem] = *source;
        wynikowe[za_delimiterem]++;
        source++;
    }

    for (int i = 0; i < dlugoscTablicy; i++)
    {
        *wynikowe[i] = '\0';
    }

    //zla podana ilosc wynikowych - najprawdopodobniej zla dlugosc tabeli wynikowe[]
    if (za_delimiterem != dlugoscTablicy - 1)
    {
        return -1;
    }

    // nie napotkano zadnego delimitera - zly input do funkcji?
    if (za_delimiterem == 0)
    {
        return -2;
    }

    return 0;
}


//IMPLEMENTATION

/* 
Creates book with given values and inserts it at the end of storage.
Creates storage if it does not exist.

Args:
    storage - pointer to address of the storage
    title - title of the book
    author - author of the book
    price - price of the book
    amount - amount of given book in storage

Returns:
    EXIT_SUCCESS = 0 - if success
    ERR_MALLOC_FAIL = 2 - if memory allocation failed
*/
bool pushBackBook(book **storage, char *title, char *author, double price, int amount)
{
    book* temp = NULL;
    if (findBook(storage, title, author, &temp))
    {
        return false;
    }

    book *element;
    book *iterator = *storage;

    if ((element = (book *)malloc(sizeof(book))) == NULL)
    {
        // fprintf(stderr, "Za mało pamięci!\n");
        return ERR_MALLOC_FAIL;
    }

    // You have to create a completely new string everytime you create a new element.
    // Otherwise all the elements will have same title and author because
    // they will point to the same string.
    // To create new string you have to allocate new memory and then copy given string to 
    // the newly created one.
    char *newTitle = (char *)malloc(255);
    strcpy(newTitle, title);
    char *newAuthor = (char *)malloc(255);
    strcpy(newAuthor, author);

    element->title = newTitle;
    element->author = newAuthor;
    element->price = price;
    element->amount = amount;
    element->next = NULL;

    if (DEBUG == 1)
    {
        printf("CREATING BOOK: %s %s %f %d\n", element->title, element->author, element->price, element->amount);
    }

    if (iterator == NULL)
    {
        *storage = element;
        return true;
    }

    while (iterator->next != NULL)
    {
        // printf("CURRENT BOOK: %s %s %f %d\n", iterator->title, iterator->author, iterator->price, iterator->amount);
        iterator = iterator->next;
    }

    iterator->next = element;
    return true;
}

/*
Finds a book in storage, searching by title and author.
Saves address to found object in result.

Returns:
    EXIT_SUCCESS = 0 - if success
*/
bool findBook(book **storage, char *title, char *author, book **result)
{
    book *iterator = *storage;

    while (iterator != NULL)
    {
        if (!strcasecmp(iterator->author, author) && !strcasecmp(iterator->title, title))
        {
            if (DEBUG == 1)
            {
                printf("BOOK FOUND: author=%s;title=%s;price=%.2f;count=%d\n", iterator->author, iterator->title, iterator->price, iterator->amount);
            }
            *result = iterator;
            return true;
        }
        iterator = iterator->next;
    }
    return false;
    
}

/*
removes the item (found by title and author) from storage, freeing the memory.

Returns:
    EXIT_SUCCESS = 0 - if success
*/
bool removeBook(book **storage, char *title, char *author)
{
    book *previous = NULL;
    book *current = *storage;
    bool found = false;

    // finding book
    while (current != NULL)
    {
        if (DEBUG == 1)
        {
            printf("CHECKING BOOK: %s %s\n", current->author, current->title);
            printf("COMPARING TO: %s %s\n", author, title);
        }

        if (!strcasecmp(current->author, author) && !strcasecmp(current->title, title)){
            found = true;
            break;
        }
        previous = current;
        current = current->next;
    }

    if (!found)
    {
        return false;
    }

    // If found book is first
    if (previous == NULL)
    {
        *storage = current->next; // point the storage variable to next book
        free(current); // free the memory (aka. delete the book)
        return true;
    }

    // If found book is last
    else if (current->next == NULL)
    {
        previous->next = NULL; // set 'previous' to be the new last element
        free(current);
        return true;
    }

    previous->next = current->next; // 'skip' the element
    free(current);
    return true;
}


bool loadFromCsv(const char *nazwaPliku, book **storage)
{
    FILE *fp;
    fp = fopen(nazwaPliku, "r");
    if (fp == NULL)
    {
        if (DEBUG == 1)
        {
            printf("Brak pliku z danymi\n");
        }
        return 0;
    }

    char wejscie[255];
    while (fgets(wejscie, 255, fp))
    {
        char author[255];
        char title[255];
        char amount[255];
        char price[255];

        // dodatkowe miejsce do dzielenia tablicy - potrzebne poniewaz moja funkcja
        // my_split zaklada ze na koncu jest "\n", a nie ";" tak jest w lista.txt
        char last[2];

        //printf("%d: Zczytano linie: %s\n", i ,wejscie);
        char *tablica[5] = {author, title, amount, price, last};

        if (my_split(wejscie, tablica, 5, ';') == 0)
        {
            double price1 = atof(price);
            int amount1 = atoi(amount);
            if (DEBUG == 1)
            {
                printf("DODAJE KSIAZKE: %s %s %d %.2f\n", title, author, price1, amount1);
            }
            pushBackBook(storage, title, author, price1, amount1);
        }
        else
        {
            if (DEBUG == 1)
            {
                printf("Nie udalo sie podzielic linii\n");
            }
            return 0;
        }
        _clearString(wejscie, 255);
    }

    return 1;
}



bool showBook(book *storage, int index)
{
/*
Searches storage for given book and shows its info.

Returns:
    EXIT_SUCCES = 0 - if success
    ERR_STORAGE_OUT_OF_RANGE = 3 - if given index is outside of storage
*/
    if (index < 0) {
        return false;
    }

    book *element = storage->next;
    for (int i = 0; i < index - 1; i++)
    {
        if (element->next == NULL){
            return false;
        }
        element = element->next;
    }

    printf("Book no. %d: %s %s %d %.2f\n",
           index + 1, element->author, element->title,
           element->amount, element->price);
    return true;
}

bool showAllBooks(book **storage)
{
    if (*storage == NULL)
    {
        return false;
    }
    book *iterator = *storage;


    int i = 0;
    while (iterator != NULL)
    {
        printf("Book no. %d: %s %s %d %.2f\n",
           ++i, iterator->author, iterator->title,
           iterator->amount, iterator->price);
        iterator = iterator->next;
    }
    return true;
    
}

// int main(int argc, char const *argv[])
// {
//     book *storage = NULL;
//     pushBackBook(&storage, "TYTUL1", "AUTOR1", 1, 1);
//     pushBackBook(&storage, "TYTUL2", "AUTOR2", 2, 2);
//     pushBackBook(&storage, "TYTUL3", "AUTOR3", 3, 3);
//     pushBackBook(&storage, "TYTUL4", "AUTOR4", 4, 4);
//     showAllBooks(&storage);
//     removeBook(&storage, "TYTUL3", "AUTOR3");
//     showAllBooks(&storage);
//     pushBackBook(&storage, "TYTUL5", "AUTOR5", 4, 4);
//     showBook(storage, 3);
    
//     return 0;
// }
