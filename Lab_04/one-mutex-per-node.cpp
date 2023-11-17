#include <iostream>
#include <pthread.h>
#include <vector>

pthread_mutex_t outputMutex;

struct ListNode {
    int data;
    ListNode* next;
    pthread_mutex_t mutex;

    ListNode(int x) : data(x), next(nullptr) {
        pthread_mutex_init(&mutex, NULL);
    }

    ~ListNode() {
        pthread_mutex_destroy(&mutex);
    }
};

class LinkedList {
private:
    ListNode* head;

public:
    LinkedList() : head(nullptr) {}

    ~LinkedList() {
        clear();
    }

    void clear() {
        ListNode* current = head;
        while (current != nullptr) {
            ListNode* temp = current;
            current = current->next;
            delete temp;  // El destructor de ListNode se encargará de destruir el mutex
        }
        head = nullptr;
    }

    bool insert(int value) {
        ListNode* newNode = new ListNode(value);
        pthread_mutex_lock(&newNode->mutex);
        if (head != nullptr) {
            pthread_mutex_lock(&head->mutex);
        }

        newNode->next = head;
        head = newNode;

        if (newNode->next != nullptr) {
            pthread_mutex_unlock(&newNode->next->mutex);
        }
        pthread_mutex_unlock(&newNode->mutex);
        return true;
    }

    bool remove(int value) {
        ListNode *current = head, *prev = nullptr;

        if (current == nullptr) {
            return false;
        }

        pthread_mutex_lock(&current->mutex);

        while (current != nullptr && current->data != value) {
            if (prev != nullptr) {
                pthread_mutex_unlock(&prev->mutex);
            }
            prev = current;
            current = current->next;
            if (current != nullptr) {
                pthread_mutex_lock(&current->mutex);
            }
        }

        if (current == nullptr) {
            if (prev != nullptr) {
                pthread_mutex_unlock(&prev->mutex);
            }
            return false;
        }

        if (prev == nullptr) {  // Deleting the head node
            head = current->next;
        } else {
            prev->next = current->next;
            pthread_mutex_unlock(&prev->mutex);
        }

        delete current;  // current->mutex is destroyed in ListNode destructor
        return true;
    }


    bool search(int value) {
        ListNode* current = head;

        while (current != nullptr) {
            pthread_mutex_lock(&current->mutex);

            if (current->data == value) {
                pthread_mutex_unlock(&current->mutex);
                return true;
            }

            ListNode* temp = current;
            current = current->next;
            pthread_mutex_unlock(&temp->mutex);
        }

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