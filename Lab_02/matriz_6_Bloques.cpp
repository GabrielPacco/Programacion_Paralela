#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace std;

void blockMultiplyMatrices(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int n, int blockSize) {
    for (int i = 0; i < n; i += blockSize) {
        for (int j = 0; j < n; j += blockSize) {
            for (int k = 0; k < n; k += blockSize) {
                for (int ii = i; ii < min(i + blockSize, n); ++ii) {
                    for (int jj = j; jj < min(j + blockSize, n); ++jj) {
                        for (int kk = k; kk < min(k + blockSize, n); ++kk) {
                            C[ii][jj] += A[ii][kk] * B[kk][jj];
                        }
                    }
                }
            }
        }
    }
}

int main() {
    srand(time(0));
    int n = 30;
    int blockSize = min(n, 10); 
    vector<vector<int>> A(n, vector<int>(n));
    vector<vector<int>> B(n, vector<int>(n));
    vector<vector<int>> C(n, vector<int>(n, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }

    auto start = chrono::high_resolution_clock::now();
    blockMultiplyMatrices(A, B, C, n, blockSize);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

    cout << "Matriz de tamaño " << n << "\n";
    cout << "Tiempo de ejecución: " << duration << " microsegundos\n";

    return 0;
}