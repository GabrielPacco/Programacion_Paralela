#include <iostream>
#include <pthread.h>
#include <vector>

pthread_mutex_t outputMutex;

struct ListNode {
    int data;
    ListNode* next;

    ListNode(int x) : data(x), next(nullptr) {}
};

class LinkedList {
private:
    ListNode* head;
    pthread_mutex_t mutex;

public:
    LinkedList() : head(nullptr) {
        pthread_mutex_init(&mutex, NULL);
    }

    ~LinkedList() {
        clear();
        pthread_mutex_destroy(&mutex);
    }

    void clear() {
        pthread_mutex_lock(&mutex);
        ListNode* current = head;
        while (current != nullptr) {
            ListNode* temp = current;
            current = current->next;
            delete temp;
        }
        head = nullptr;
        pthread_mutex_unlock(&mutex);
    }

    bool insert(int value) {
        pthread_mutex_lock(&mutex);
        ListNode* newNode = new ListNode(value);
        newNode->next = head;
        head = newNode;
        pthread_mutex_unlock(&mutex);
        return true;
    }

    bool remove(int value) {
        pthread_mutex_lock(&mutex);
        ListNode *current = head, *prev = nullptr;
        while (current != nullptr) {
            if (current->data == value) {
                if (prev != nullptr) {
                    prev->next = current->next;
                } else {
                    head = current->next;
                }
                delete current;
                pthread_mutex_unlock(&mutex);
                return true;
            }
            prev = current;
            current = current->next;
        }
        pthread_mutex_unlock(&mutex);
        return false;
    }

    bool search(int value) {
        pthread_mutex_lock(&mutex);
        ListNode* current = head;
        while (current != nullptr) {
            if (current->data == value) {
                pthread_mutex_unlock(&mutex);
                return true;
            }
            current = current->next;
        }
        pthread_mutex_unlock(&mutex);
        return false;
    }
};


// Prototipos de las funciones para los hilos
void* insertFunction(void* arg);
void* deleteFunction(void* arg);
void* searchFunction(void* arg);

// Estructura para pasar datos a los hilos
struct ThreadData {
    LinkedList* list;
    int value;
};

int main() {
    pthread_mutex_init(&outputMutex, NULL);

    LinkedList myList;

    // Crear hilos
    pthread_t insertThread, deleteThread, searchThread;
    ThreadData insertData{&myList, 10};
    ThreadData deleteData{&myList, 5};
    ThreadData searchData{&myList, 10};

    // Iniciar hilos
    pthread_create(&insertThread, nullptr, insertFunction, &insertData);
    pthread_create(&deleteThread, nullptr, deleteFunction, &deleteData);
    pthread_create(&searchThread, nullptr, searchFunction, &searchData);

    // Esperar a que los hilos terminen
    pthread_join(insertThread, nullptr);
    pthread_join(deleteThread, nullptr);
    pthread_join(searchThread, nullptr);

    pthread_mutex_destroy(&outputMutex);

    return 0;
}

void* insertFunction(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    data->list->insert(data->value);

    pthread_mutex_lock(&outputMutex);
    std::cout << "Insertado: " << data->value << std::endl;
    pthread_mutex_unlock(&outputMutex);

    return nullptr;
}

void* deleteFunction(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    pthread_mutex_lock(&outputMutex);
    if (data->list->remove(data->value)) {
        std::cout << "Eliminado: " << data->value << std::endl;
    } else {
        std::cout << "Valor no encontrado para eliminar: " << data->value << std::endl;
    }
    pthread_mutex_unlock(&outputMutex);
    return nullptr;
}

void* searchFunction(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    pthread_mutex_lock(&outputMutex);
    if (data->list->search(data->value)) {
        std::cout << "Encontrado: " << data->value << std::endl;
    } else {
        std::cout << "No se ha encontrado: " << data->value << std::endl;
    }
    pthread_mutex_unlock(&outputMutex);
    return nullptr;
}