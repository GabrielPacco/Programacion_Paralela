#include <pthread.h>
#include <iostream>
#include <vector>
#include <unistd.h> // Para usar usleep

// Variables globales compartidas
int counter = 0;
int thread_count;
pthread_mutex_t mutex;
pthread_cond_t cond_var;

// Función que ejecutará cada hilo
void* ThreadWork(void* rank) {
    long my_rank = (long)rank;

    // Sección de trabajo del hilo...

    // Inicio de la barrera
    pthread_mutex_lock(&mutex);
    counter++;
    if (counter == thread_count) {
        counter = 0; // Reseteamos el contador para la próxima barrera
        pthread_cond_broadcast(&cond_var); // Despertamos a todos los hilos
    } else {
        // Esperamos hasta que el contador alcance thread_count
        while (pthread_cond_wait(&cond_var, &mutex) != 0);
    }
    pthread_mutex_unlock(&mutex);
    // Fin de la barrera

    // Retardo para evitar solapamiento en la salida
    usleep(100000 * my_rank); // Retardo basado en el rango del hilo

    // Sección de salida
    pthread_mutex_lock(&mutex); // Protegemos la salida para evitar solapamiento
    std::cout << "Hilo " << my_rank << " ha pasado la barrera" << std::endl;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char* argv[]) {
    // Inicialización de mutex y variable de condición
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_var, NULL);

    // Creación de hilos
    thread_count = 5; // Número de hilos a crear
    std::vector<pthread_t> threads(thread_count);
    for (long i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, ThreadWork, (void*)i);
    }

    // Esperar a que todos los hilos terminen
    for (pthread_t& thread : threads) {
        pthread_join(thread, NULL);
    }

    // Destrucción de mutex y variable de condición
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);

    std::cout << "Todos los hilos han pasado la barrera." << std::endl;

    return 0;
}
