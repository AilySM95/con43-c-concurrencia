/* paso3_cuenta_con_mutex.c
 * Objetivo: CORREGIR el data race del paso 2 siguiendo la regla CON43-C.
 * Solución: usar un MUTEX (un "candado"). Solo un cajero a la vez
 * puede tocar el saldo; el otro espera su turno.
 *
 * Compilar: gcc -Wall -Wextra -std=c23 -pthread paso3_cuenta_con_mutex.c -o paso3
 */
#include <stdio.h>
#include <pthread.h>

#define RETIROS_POR_CAJERO 1000000      /* retiros que hace cada cajero */
#define SALDO_INICIAL      (2 * RETIROS_POR_CAJERO)

int saldo = SALDO_INICIAL;              /* la cuenta: variable GLOBAL compartida */

/* El candado. Se crea una sola vez y lo comparten los dos cajeros. */
pthread_mutex_t candado = PTHREAD_MUTEX_INITIALIZER;

void *cajero(void *arg) {
    (void)arg;                          /* no usamos el argumento (evita un warning) */
    for (int i = 0; i < RETIROS_POR_CAJERO; i++) {
        pthread_mutex_lock(&candado);   /* CERRAR el candado: si otro cajero lo tiene, espero */
        saldo = saldo - 1;              /* zona protegida: solo UN hilo a la vez pasa por aquí */
        pthread_mutex_unlock(&candado); /* ABRIR el candado: ahora el otro cajero puede entrar */
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

    if (saldo == 0) {
        printf("-> CORRECTO: el saldo coincide con el dinero retirado\n");
    } else {
        printf("-> ERROR: el saldo no coincide (quedan %d EUR de más en la cuenta)\n", saldo);
    }
    return 0;
}
