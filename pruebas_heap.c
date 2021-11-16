#include <stdlib.h>
#include "testing.h"

void pruebas_heap_estudiante(){
    return;
}

#ifndef CORRECTOR  // Para que no dé conflicto con el main() del corrector.

int main(void) {
    pruebas_heap_estudiante();
    return failure_count() > 0;  // Indica si falló alguna prueba.
}

#endif