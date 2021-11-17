#include <stdlib.h>
#include "testing.h"
#include "heap.h"
#include <stdbool.h>

int comparar_elementos(const void *a, const void *b){
    if(*(int*) a == *(int*) b){
        return 0;
    }

    return *(int*) a < *(int*) b ? -1 : 1;
}

void prueba_heapsort(){
    void **elementos = malloc(sizeof(void*) * 14);
    int numeros[] = {8,5,6,9,10,20,1,3,14,12,5,4,7,18,15};
    int numeros_check[] = {1,3,4,5,5,6,7,8,9,10,12,14,15,18,20};
    for(int i = 0; i < 15; i++){
        elementos[i] = &numeros[i];
    }
    heap_sort(elementos,15, comparar_elementos);

    bool ok = true;
    for(int i = 0; i < 15; i++){
        if(*(int*)elementos[i] != numeros_check[i]){
            ok = false;
            break;
        }
    }
    print_test("Prueba heapsort ordena los elementos", ok);
}

void pruebas_heap_estudiante(){
    prueba_heapsort();
}

#ifndef CORRECTOR  // Para que no dé conflicto con el main() del corrector.

int main(void) {
    pruebas_heap_estudiante();
    return 0;
    //return failure_count() > 0;  // Indica si falló alguna prueba.
}

#endif