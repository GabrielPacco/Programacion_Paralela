#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::cout << "Hola desde el proceso " << rank << std::endl;
    MPI_Finalize();
    return 0;
}
