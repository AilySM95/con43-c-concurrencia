/* paso2_cuenta_bancaria.c
 * Objetivo: MOSTRAR el error de la regla CON43-C (data race)
 * con el ejemplo de una cuenta bancaria compartida.
 *
 * Dos cajeros automáticos (hilos) retiran 1 euro, 1.000.000 de veces cada uno,
 * de la MISMA cuenta, SIN ninguna protección.
 * Saldo inicial: 2.000.000 euros -> el saldo final correcto es 0.
 *
 * Compilar: gcc -Wall -Wextra -std=c23 -pthread paso2_cuenta_bancaria.c -o paso2
 */
#include <stdio.h>
#include <pthread.h>

#define RETIROS_POR_CAJERO 1000000      /* retiros que hace cada cajero */
#define SALDO_INICIAL      (2 * RETIROS_POR_CAJERO)

int saldo = SALDO_INICIAL;              /* la cuenta: variable GLOBAL compartida */

/* Función que ejecuta cada cajero (hilo): retira 1 euro, muchas veces. */
void *cajero(void *arg) {
    (void)arg;                          /* no usamos el argumento (evita un warning) */
    for (int i = 0; i < RETIROS_POR_CAJERO; i++) {
        saldo = saldo - 1;              /* ¡AQUÍ está el data race! leer, restar, escribir */
    }
    return NULL;
}

int main(void) {
    pthread_t cajeroA, cajeroB;

    pthread_create(&cajeroA, NULL, cajero, NULL);   /* lanza el cajero A */
    pthread_create(&cajeroB, NULL, cajero, NULL);   /* lanza el cajero B */

    pthread_join(cajeroA, NULL);                    /* espera al cajero A */
    pthread_join(cajeroB, NULL);                    /* espera al cajero B */

    printf("Saldo inicial:   %d EUR\n", SALDO_INICIAL);
    printf("Dinero retirado: %d EUR\n", 2 * RETIROS_POR_CAJERO);
    printf("Saldo esperado:  0 EUR\n");
    printf("Saldo obtenido:  %d EUR\n", saldo);

    if (saldo != 0) {
        printf("-> ERROR: el banco cree que aun quedan %d EUR, pero los cajeros entregaron todo el saldo inicial\n", saldo);
    } else {
        printf("-> Esta vez no se notó el error (pero el bug sigue ahí)\n");
    }
    return 0;
}
