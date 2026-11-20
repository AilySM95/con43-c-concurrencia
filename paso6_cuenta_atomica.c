/* paso6_cuenta_atomica.c
 * Objetivo: cumplir la regla CON43-C de OTRA forma: con una variable ATÓMICA de C11.
 * Una operación atómica es "indivisible": mientras un cajero la hace,
 * el otro no puede meterse en medio. No hace falta candado (mutex).
 *
 * Compilar: gcc -Wall -Wextra -std=c23 -pthread paso6_cuenta_atomica.c -o paso6
 */
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>                  /* biblioteca de variables atómicas de C11 */

#define RETIROS_POR_CAJERO 1000000      /* retiros que hace cada cajero */
#define SALDO_INICIAL      (2 * RETIROS_POR_CAJERO)

atomic_int saldo = SALDO_INICIAL;       /* un int ATÓMICO: la cuenta compartida */

void *cajero(void *arg) {
    (void)arg;                          /* no usamos el argumento (evita un warning) */
    for (int i = 0; i < RETIROS_POR_CAJERO; i++) {
        atomic_fetch_sub(&saldo, 1);    /* "restar 1" como UNA sola operación indivisible
                                           (leer, restar y escribir sin que nadie se cuele) */
    }
    return NULL;
}

int main(void) {
    pthread_t cajeroA, cajeroB;

    pthread_create(&cajeroA, NULL, cajero, NULL);   /* lanza el cajero A */
    pthread_create(&cajeroB, NULL, cajero, NULL);   /* lanza el cajero B */

    pthread_join(cajeroA, NULL);                    /* espera al cajero A */
    pthread_join(cajeroB, NULL);                    /* espera al cajero B */

    int saldo_final = atomic_load(&saldo);          /* leemos el valor atómico final */

    printf("Saldo inicial:   %d EUR\n", SALDO_INICIAL);
    printf("Dinero retirado: %d EUR\n", 2 * RETIROS_POR_CAJERO);
    printf("Saldo esperado:  0 EUR\n");
    printf("Saldo obtenido:  %d EUR\n", saldo_final);

    if (saldo_final == 0) {
        printf("-> CORRECTO: el saldo coincide con el dinero retirado\n");
    } else {
        printf("-> ERROR: el banco cree que aun quedan %d EUR\n", saldo_final);
    }
    return 0;
}
