#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>

// Variables compartidas
int counter = 0;
int thread_count;
std::mutex barrier_mutex;

// Función de hilo
void ThreadWork() {
    // Barrera
    barrier_mutex.lock();
    counter++;
    barrier_mutex.unlock();

    // Espera activa
    while (true) {
        barrier_mutex.lock();
        if (counter >= thread_count) {
            barrier_mutex.unlock();
            break;
        }
        barrier_mutex.unlock();
        // Introducir un pequeño retraso para reducir la sobrecarga de la espera activa
        std::this_thread::yield();
    }

    // Sección crítica o código adicional va aquí
    std::stringstream ss;
    ss << "Hilo " << std::this_thread::get_id() << " ha pasado la barrera\n";
    std::cout << ss.str();
}

int main() {
    // Inicializar la cantidad de hilos
    thread_count = 5; // Ejemplo de cantidad de hilos

    // Crear hilos
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(ThreadWork);
    }

    // Unir hilos
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Todos los hilos han pasado la barrera.\n";

    return 0;
}
