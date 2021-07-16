/// The MIT License (MIT)
/// Copyright (c) 2021 Aiman Hammou
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to
/// deal in the Software without restriction, including without limitation the
/// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
/// sell copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
/// IN THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>

#define BOOL int

/*
    Programma molto base dell'algoritmo di caching LRU (Least Recently Used)

    Prima di iniziare a leggere il codice (sebbene commentato) sarebbe
    meglio guardare qualche video che spieghi il funzionamento di essi

*/


// Double Linked List (DLL)
typedef struct cache_node
{
    struct cache_node *previous; // Referenza al valore precedente
    struct cache_node *next;     // Referenza al valore successivo

    unsigned int page_number; //Numero page dove risiede il valore cache-ato

}cache_node;



typedef struct CACHE
{
    unsigned int number_of_elements; // Conteggio degli elementi presenti nella cache
    unsigned int size_of_cache;      // Grandezza della cache

    cache_node *front;
    cache_node *rear;

}CACHE;

typedef struct MAP
{
    unsigned int capacity; // Dimensione della mappa di riferimento

    cache_node **references; // Puntatore al puntatore indicante il valore

}MAP;


// Funzione che alloca un nuovo nodo assegnandone la posizione (page_number)
cache_node *alloc_new_node(unsigned int page_number)
{
    cache_node *tmp = (cache_node *)malloc(sizeof(cache_node));

    tmp->previous = NULL;
    tmp->next = NULL;

    tmp->page_number = page_number;

    return tmp;

}

CACHE *create_cache(unsigned int size_of_cache)
{
    CACHE *cache = (CACHE *)malloc(sizeof(CACHE));

    cache->number_of_elements = 0;

    cache->size_of_cache = size_of_cache;

    cache->front = NULL;
    cache->rear = NULL;

    return cache;
}

MAP *create_map(unsigned int capacity)
{
    MAP *map = (MAP *)malloc(sizeof(MAP));

    map->capacity = capacity;

    map->references = (cache_node **)malloc(sizeof(cache_node *) * map->capacity);

    for (int i = 0; i < map->capacity; i++)
        map->references[i] = NULL;

    return map;
}

BOOL is_cache_full(CACHE *cache)
{
    return cache->number_of_elements == cache->size_of_cache;
}

BOOL is_cache_empty(CACHE *cache)
{
    return cache->number_of_elements == 0;
}


void pop_cache_element(CACHE *cache)
{

    if (is_cache_empty(cache))
        return;

    if(cache->front == cache->rear)
        cache->front = NULL;

    cache_node *tmp = cache->rear;
    cache->rear = cache->rear->previous;

    free (tmp);

    cache->number_of_elements -= 1;

    return;
}

void add_cache_element(CACHE *cache, MAP* map, unsigned int page_number)
{
    /*
        la cache è piena quindi liberi un posto.

        Nota che gli elementi meno recenti sono
        quelli più vicini al rear

    */

    if(is_cache_full(cache))
    {
        map->references[cache->rear->page_number] = NULL;
        pop_cache_element(cache);
    }

    cache_node *tmp = alloc_new_node(page_number);
    tmp->next = cache->front;

    if(is_cache_empty(cache))
    {
        cache->front = tmp;
        cache->rear = cache->front;
    }
    else
    {
        cache->front->previous = tmp;
        cache->front = tmp;
    }

    map->references[page_number] = tmp;

    cache->number_of_elements += 1;

    return;

}

void reference_page(CACHE *cache, MAP* map, unsigned int page_number)
{

    /* gettiamo l'elemento associato alla page_number */
    cache_node *requested_node = map->references[ page_number ];


    if ( requested_node == NULL )
        add_cache_element(cache, map, page_number);
    else if (requested_node != cache->front)
    {
        requested_node->previous->next = requested_node->next;

        if(requested_node->next)
            requested_node->next->previous = requested_node->previous;

        if (requested_node == cache->rear)
        {
            cache->rear = requested_node->previous;
            cache->rear->next = NULL;
        }

        requested_node->next = cache->front;
        requested_node->previous = NULL;

        requested_node->next->previous = requested_node;

        cache->front = requested_node;
    }
}

int
main()
{

    CACHE* q = create_cache( 4 );
    MAP* map = create_map( 10 );
    reference_page( q, map, 1);
    reference_page( q, map, 2);
    reference_page( q, map, 3);
    reference_page( q, map, 1);
    reference_page( q, map, 4);
    reference_page( q, map, 5);

    printf ("%d ", q->front->page_number);
    printf ("%d ", q->front->next->page_number);
    printf ("%d ", q->front->next->next->page_number);
    printf ("%d ", q->front->next->next->next->page_number);

    return 0;
}
