# CON43-C: No permitir data races en código multihilo

Proyecto de presentación sobre la regla **CON43-C** del estándar de codificación **SEI CERT C** (sección de concurrencia).

## Datos del estudiante

| | |
|---|---|
| **Nombre** | Aily |
| **Máster** | Máster en Investigación en Ciberseguridad |
| **Asignatura** | Diseño de Programación Segura |


## ¿De qué trata?

La regla CON43-C dice que dos hilos no deben acceder a la misma variable al mismo tiempo sin protección, porque eso produce un **data race** (comportamiento indefinido en C).

Lo explico con una **cuenta bancaria compartida**: dos cajeros (hilos) retiran dinero de la misma cuenta. Sin protección, el saldo final sale mal; con protección, sale siempre bien.

## Archivos

| Archivo | Qué muestra | Resultado esperado |
|---|---|---|
| `paso2_cuenta_bancaria.c` | **El error:** data race, sin protección | Saldo final distinto de 0, y distinto en cada ejecución |
| `paso3_cuenta_con_mutex.c` | **Solución 1:** mutex (candado) | Saldo final siempre 0 |
| `paso6_cuenta_atomica.c` | **Solución 2:** variable atómica de C11 | Saldo final siempre 0 |
| `paso4_race_condition_mal.c` | **Extra:** candado mal usado (mirar y retirar por separado) | A veces sobregiro (`-10 EUR`) |
| `paso4_race_condition_ok.c` | **Extra:** corregido (mirar y retirar con el mismo candado) | Saldo final 0, sin sobregiro |

## Cómo compilar y ejecutar

Requisitos: Linux con `gcc` (probado con gcc 16.2.0 y `-std=c23`).

```bash
# Ejemplo con el programa que tiene el error
gcc -Wall -Wextra -std=c23 -pthread paso2_cuenta_bancaria.c -o paso2
./paso2
```

Para los demás, se cambia el nombre del archivo `.c` y el nombre del programa de salida (`-o`).

### Comprobar con ThreadSanitizer

```bash
gcc -Wall -Wextra -std=c23 -g -fsanitize=thread -pthread paso2_cuenta_bancaria.c -o paso2_tsan
./paso2_tsan
```

- Con `paso2_cuenta_bancaria.c` avisa de un **data race** (variable `saldo`, línea 23).
- Con `paso3_cuenta_con_mutex.c` y `paso6_cuenta_atomica.c` **no avisa nada**.
- Con `paso4_race_condition_mal.c` **no avisa**, porque ahí no hay data race sino un error de lógica.

## Fuentes

- SEI CERT C, regla CON43-C: https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/rules/concurrency-con/con43-c/
- Aviso de seguridad de Xen XSA-155 (CVE-2015-8550): https://xenbits.xen.org/xsa/advisory-155.html


## Uso de inteligencia artificial

Para este proyecto usé Claude (asistente de IA de Anthropic) en un chat, como apoyo para aprender y para preparar el material.

**Qué hizo Claude:**

- Propuso la regla CON43-C y un plan de trabajo por pasos.
- Escribió y comentó el código en C de los cinco programas, a partir de mi idea de la cuenta bancaria con dos cajeros.
- Probó los programas en su propio entorno de pruebas antes de pasármelos (gcc 13 con -std=c2x, un solo núcleo, distinto al mío).
- Me explicó los conceptos paso a paso: hilos, data race, mutex, variables atómicas y ThreadSanitizer.
- Buscó y resumió fuentes (página de CMU de CON43-C y aviso de seguridad de Xen XSA-155).
- Me ayudó a redactar y simplificar el guion del video y este README.

**Qué hice yo:**

- Elegí el enfoque (el ejemplo del cajero) y decidí qué incluir y qué quitar del proyecto.
- Compilé y ejecuté todos los programas en mi equipo (Kali Linux, gcc 16.2.0, -std=c23), incluyendo ThreadSanitizer y la medición de tiempos.
- Los resultados que se muestran en el video son los de mi ejecución.
- Pregunté mis dudas, revisé el guion y lo adapté a mis palabras.
- Grabé y presenté el video.
