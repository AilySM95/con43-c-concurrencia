/* paso4_race_condition_ok.c
 * Corrección de la race condition: COMPROBAR + RETIRAR dentro del MISMO candado.
 * Así nadie puede colarse entre la comprobación y el retiro.
 *
 * Cuenta: 100 EUR. Cada cajero intenta retirar 10 EUR, 10 veces,
 * y SOLO retira si hay saldo suficiente. El saldo nunca baja de 0.
 *
 * Compilar: gcc -Wall -Wextra -std=c23 -pthread paso4_race_condition_ok.c -o paso4_ok
 */
#include <stdio.h>
#include <pthread.h>
#include <sched.h>                      /* para sched_yield() */

#define SALDO_INICIAL 100
#define MONTO         10                /* euros que retira cada vez */
#define INTENTOS      10                /* intentos de retiro por cajero */

int saldo = SALDO_INICIAL;
pthread_mutex_t candado = PTHREAD_MUTEX_INITIALIZER;

/* UNA sola operación: comprobar y retirar juntas, con el candado cerrado todo el tiempo.
 * Devuelve 1 si retiró el dinero, 0 si no había saldo suficiente. */
int retirar_si_hay_saldo(int monto) {
    int retirado = 0;
    pthread_mutex_lock(&candado);           /* cerramos el candado ANTES de comprobar */
    if (saldo >= monto) {                   /* 1) COMPROBAR */
        sched_yield();                      /* misma "espera de red" de antes: ahora ya no importa,
                                               porque el otro cajero está bloqueado esperando */
        saldo = saldo - monto;              /* 2) ACTUAR (el saldo no pudo cambiar) */
        retirado = 1;
    }
    pthread_mutex_unlock(&candado);         /* abrimos el candado DESPUÉS de retirar */
    return retirado;
}

void *cajero(void *arg) {
    (void)arg;
    for (int i = 0; i < INTENTOS; i++) {
        retirar_si_hay_saldo(MONTO);
    }
    return NULL;
}

int main(void) {
    pthread_t cajeroA, cajeroB;

    pthread_create(&cajeroA, NULL, cajero, NULL);
    pthread_create(&cajeroB, NULL, cajero, NULL);
    pthread_join(cajeroA, NULL);
    pthread_join(cajeroB, NULL);

    printf("Saldo inicial:  %d EUR\n", SALDO_INICIAL);
    printf("Saldo final:    %d EUR\n", saldo);

    if (saldo < 0) {
        printf("-> ERROR: sobregiro. La cuenta quedo en negativo (%d EUR)\n", saldo);
    } else {
        printf("-> CORRECTO: no hubo sobregiro\n");
    }
    return 0;
}
