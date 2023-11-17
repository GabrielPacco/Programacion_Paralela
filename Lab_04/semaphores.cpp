#include <iostream>
#include <thread>
#include <vector>
#include <semaphore.h>
#include <mutex>

// Variables compartidas
int counter = 0;
int thread_count;
sem_t count_sem;
sem_t barrier_sem;
std::mutex cout_mutex; // Mutex para proteger la salida estándar

void ThreadWork(int thread_id) {
    // Barrera
    sem_wait(&count_sem);
    if (counter == thread_count - 1) {
        // Último hilo en llegar a la barrera
        counter = 0; // Reiniciar contador para la próxima vez
        sem_post(&count_sem); // Liberar count_sem
        for (int j = 0; j < thread_count - 1; j++)
            sem_post(&barrier_sem); // Liberar todos los otros hilos
    } else {
        counter++; // Incrementar contador
        sem_post(&count_sem); // Liberar count_sem
        sem_wait(&barrier_sem); // Esperar en la barrera
    }
    // Sección crítica para imprimir
    cout_mutex.lock();
    std::cout << "Hilo " << thread_id << " ha pasado la barrera" << std::endl;
    cout_mutex.unlock();
}

int main() {
    std::cout << "Enter the number of threads: ";
    std::cin >> thread_count;

    // Inicializar semáforos
    sem_init(&count_sem, 0, 1);
    sem_init(&barrier_sem, 0, 0);

    std::vector<std::thread> threads;

    // Crear y comenzar hilos
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(ThreadWork, i);
    }

    // Esperar a que todos los hilos terminen
    for (auto& t : threads) {
        t.join();
    }

    // Limpiar semáforos
    sem_destroy(&count_sem);
    sem_destroy(&barrier_sem);

    std::cout << "All threads have passed the barrier.\n";

    return 0;
}
