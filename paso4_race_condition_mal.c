/* paso4_race_condition_mal.c
 * Objetivo: mostrar una RACE CONDITION aunque NO haya data race.
 * Cada acceso a "saldo" está protegido con mutex (no hay data race),
 * pero "comprobar" y "retirar" son DOS operaciones separadas.
 * Entre ellas, el otro cajero puede colarse.
 *
 * Cuenta: 100 EUR. Cada cajero intenta retirar 10 EUR, 10 veces,
 * y SOLO retira si ve saldo suficiente. El saldo nunca debería bajar de 0.
 *
 * Compilar: gcc -Wall -Wextra -std=c23 -pthread paso4_race_condition_mal.c -o paso4_mal
 */
#include <stdio.h>
#include <pthread.h>
#include <sched.h>                      /* para sched_yield() */

#define SALDO_INICIAL 100
#define MONTO         10                /* euros que retira cada vez */
#define INTENTOS      10                /* intentos de retiro por cajero */

int saldo = SALDO_INICIAL;
pthread_mutex_t candado = PTHREAD_MUTEX_INITIALIZER;

/* Operación 1: consultar el saldo (protegida con candado). */
int consultar_saldo(void) {
    pthread_mutex_lock(&candado);
    int s = saldo;
    pthread_mutex_unlock(&candado);     /* ¡soltamos el candado aquí! */
    return s;
}

/* Operación 2: retirar dinero (protegida con candado). */
void retirar(int monto) {
    pthread_mutex_lock(&candado);
    saldo = saldo - monto;
    pthread_mutex_unlock(&candado);
}

void *cajero(void *arg) {
    (void)arg;
    for (int i = 0; i < INTENTOS; i++) {
        if (consultar_saldo() >= MONTO) {   /* 1) COMPROBAR: ¿hay dinero? */
            sched_yield();                  /* simula la espera de la red del cajero:
                                               cede el turno al otro hilo */
            retirar(MONTO);                 /* 2) ACTUAR: retirar (¡el saldo pudo cambiar!) */
        }
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
        printf("-> Esta vez no hubo sobregiro (pero el bug sigue ahi)\n");
    }
    return 0;
}
