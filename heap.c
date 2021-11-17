#include "heap.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define CAPACIDAD_INICIAL 20

// -----------------------------------------------------------------
// ----------------------- Estructura Heap -------------------------
// -----------------------------------------------------------------

struct heap{
    void** arreglo;
    size_t capacidad;
    size_t cantidad;
    cmp_func_t cmp;
};


// -----------------------------------------------------------------
// ------------------ Funciones Auxiliares Heap --------------------
// -----------------------------------------------------------------

int hijo_izq(size_t pos, size_t cantidad){
    size_t pos_h_izq = (2*pos) + 1;
    return (pos_h_izq < cantidad) ? (int) pos_h_izq : -1;
}

int hijo_der(size_t pos, size_t cantidad){
    size_t pos_h_der = (2*pos) + 2;
    return (pos_h_der < cantidad) ? (int) pos_h_der : -1;
}

int padre(size_t pos){
    if(pos == 0){
        return -1;
    }
    return (int) (pos - 1) / 2;
}

void swap(void **arreglo, size_t posicion1, size_t posicion2){
    void *aux = arreglo[posicion1];
    arreglo[posicion1] = arreglo[posicion2];
    arreglo[posicion2] = aux;
}

bool heap_redimensionar(heap_t* heap, size_t nueva_capacidad){
    void** nuevo_arreglo = realloc(heap->arreglo, sizeof(void*) * nueva_capacidad);
    if (!nuevo_arreglo) return false;
    heap->arreglo = nuevo_arreglo;
    return true;
}

void upheap(void** arreglo, size_t posicion_elemento, size_t cantidad, cmp_func_t cmp){
    // Obtengo el elemento que tengo que hacerle upheap
    void* elemento_upheap = arreglo[posicion_elemento];

    // Obtengo al padre
    int posicion_padre = padre(posicion_elemento);
    if (posicion_padre == -1) return;
    void* padre_upheap = arreglo[posicion_padre];

    // Evaluo si es necesario hacer un swap
    if (cmp(padre_upheap, elemento_upheap) >= 0) return;

    // Realizo el swap
    swap(arreglo, posicion_elemento, (size_t) posicion_padre);

    // Llamo nuevamente a la funcion
    upheap(arreglo, (size_t) posicion_padre, cantidad, cmp);
}

int hijo_mayor(int posicion_hijo_izq, int posicion_hijo_der, void** arreglo, cmp_func_t cmp){
    // Si ambas posiciones son incorrectas devuelvo -1
    if (posicion_hijo_izq == -1 && posicion_hijo_der == -1) return -1;

    // Si alguna posicion es incorrecta devuelvo la otra
    if (posicion_hijo_izq == -1) return posicion_hijo_der;
    if (posicion_hijo_der == -1) return posicion_hijo_izq;

    // Obtengo ambos elementos
    void* elemento_hijo_izq = arreglo[posicion_hijo_izq];
    void* elemento_hijo_der = arreglo[posicion_hijo_der];
    
    // Devuelvo el maximo
    return (cmp(elemento_hijo_izq, elemento_hijo_der) >= 0) ? posicion_hijo_izq : posicion_hijo_der;
}

void downheap(void** arreglo, size_t posicion_elemento, size_t cantidad, cmp_func_t cmp){
    // Obtengo el elemento que tengo que hacerle downheap
    void* elemento_downheap = arreglo[posicion_elemento];

    // Obtengo a los hijos
    int posicion_hijo_izq = hijo_izq(posicion_elemento, cantidad);
    int posicion_hijo_der = hijo_der(posicion_elemento, cantidad);

    // Obtener la posicion del hijo mayor
    int posicion_hijo_mayor = hijo_mayor(posicion_hijo_izq, posicion_hijo_der, arreglo, cmp);
    if (posicion_hijo_mayor == -1) return;

    // Obtengo el elemento hijo mayor
    void* elemento_hijo_mayor = arreglo[posicion_hijo_mayor];
    
    // Verifico si es necesario hacer un swap
    if (cmp(elemento_downheap, elemento_hijo_mayor) >= 0) return;

    // Realizo el swap
    swap(arreglo, posicion_elemento, (size_t) posicion_hijo_mayor);

    // Llamo nuevamente a la funcion
    downheap(arreglo,(size_t) posicion_hijo_mayor, cantidad, cmp);
}

// -----------------------------------------------------------------
// ----------------------- Primitivas Heap -------------------------
// -----------------------------------------------------------------

heap_t *_heap_crear(cmp_func_t cmp, size_t capacidad_inicial){
    // Solicito memoria para alojar el heap
    heap_t* heap = malloc(sizeof(heap_t));
    if (!heap) return NULL;

    // Solicito memoria para alojar el arreglo
    heap->arreglo = malloc(sizeof(void*) * capacidad_inicial);
    if (!heap->arreglo){
        free(heap);
        return NULL;
    }
    // Inicializo los parametros del heap
    heap->capacidad = capacidad_inicial;
    heap->cantidad = 0;
    heap->cmp = cmp;

    return heap;
}

heap_t *heap_crear(cmp_func_t cmp){
    return _heap_crear(cmp, CAPACIDAD_INICIAL);
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
    // Creo un nuevo heap con capacidad 2n
    heap_t* heap = _heap_crear(cmp, n * 2);
    if (!heap) return NULL;

    // Copio los valores del arreglo al heap
    for (size_t i = 0; i < n; i++){
        heap->arreglo[i] = arreglo[i];
        heap->cantidad++;
    }
    // Hago downheap del ultimo elemento al primero
    for (int j = (int) heap->cantidad - 1; j >= 0; j--){
        downheap(heap->arreglo,(size_t) j, heap->cantidad - (size_t) j, heap->cmp);
    }
    return heap;
}

void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e)){
    // Si pasan una funcion de destruir, recorro todos los elementos y le aplico la funcion
    if (destruir_elemento){
        for (size_t i = 0; i < heap->cantidad; i++){
        destruir_elemento(heap->arreglo[i]);
        }
    }
    // Libero la memoria de las estructuras
    free(heap->arreglo);
    free(heap);
}

size_t heap_cantidad(const heap_t *heap){
    return heap->cantidad;
}

bool heap_esta_vacio(const heap_t *heap){
    return (heap->cantidad == 0) ? true : false;
}

bool heap_encolar(heap_t *heap, void *elem){
    // Redimensiono si es que no hay espacio
    if (heap->cantidad == heap->capacidad - 1){
        if (!heap_redimensionar(heap, heap->capacidad * 2)) return false;
    }
    // Agrego el elemento en la ultima posicion del arreglo, aumento la capacidad en 1 y le hago upheap.
    heap->arreglo[heap->cantidad] = elem;
    heap->cantidad++;
    upheap(heap->arreglo, heap->cantidad - 1, heap->cantidad, heap->cmp);
    return true;
}

void *heap_ver_max(const heap_t *heap){
    return (heap_esta_vacio(heap)) ? NULL : heap->arreglo[0];
}

void *heap_desencolar(heap_t *heap){
    // Verifico si el heap esta vacio
    if (heap_esta_vacio(heap)) return NULL;

    // Me guardo una referencia del elemento desencolado
    void* elemento_desencolado = heap->arreglo[0];

    // Hago un swap del primer elemento con el ultimo, disminuyo la cant en uno y le hago down heap a la raiz
    swap(heap->arreglo, 0,  heap->cantidad - 1);
    heap->cantidad--;
    downheap(heap->arreglo, 0, heap->cantidad, heap->cmp);

    // Redimensiono si es que hay demasiado espacio libre
    if (heap->capacidad / heap->cantidad >= 4) heap_redimensionar(heap, heap->capacidad / 2);

    return elemento_desencolado;
}


// -----------------------------------------------------------------
// -------------------------- Heap Sort ----------------------------
// -----------------------------------------------------------------

void heapify(void *elementos[], size_t cant, cmp_func_t cmp){
    for(size_t i =  cant; i > 0; i--){
        downheap(elementos, i-1, cant, cmp);
    }
}

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
    heapify(elementos, cant, cmp);
    for(size_t i = 0; i < cant; i++){
        swap(elementos, 0, cant - 1 - i);
        downheap(elementos, 0, cant - i -1, cmp);
    }
}
