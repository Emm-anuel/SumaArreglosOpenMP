#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>

using namespace std;

/*
 Función que imprime una muestra de un arreglo.
 - v: arreglo a imprimir
 - name: nombre del arreglo (A, B o R)
 - maxPrint: número máximo de elementos a mostrar
*/
void printSample(const vector<int> &v, const string &name, int maxPrint = 20)
{
    cout << name << " (primeros " << min((int)v.size(), maxPrint) << " elementos): ";
    for (int i = 0; i < (int)v.size() && i < maxPrint; i++)
    {
        cout << v[i] << (i < maxPrint - 1 ? ", " : "");
    }
    cout << "\n";
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cout << "Cantidad de elementos (ej. 1000 o 10000000): " << flush;
    cin >> n;

    vector<int> A(n), B(n), R(n);

    // Llenado con valores aleatorios
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 100);

    for (int i = 0; i < n; i++)
    {
        A[i] = dist(gen);
        B[i] = dist(gen);
    }

    int threads;
    cout << "Numero de hilos (ej. 1, 2, 4, 8): " << flush;
    cin >> threads;

    // Configuración del número de hilos OpenMP
    omp_set_num_threads(threads);

#pragma omp parallel
    {
#pragma omp single
        {
            cout << "\nHilos en ejecucion: "
                 << omp_get_num_threads() << endl;
        }
    }

    // Inicio de medición de tiempo
    auto t1 = chrono::high_resolution_clock::now();

/*
 For paralelo con OpenMP.
 Cada iteración es independiente:
 R[i] = A[i] + B[i]
 No hay condiciones de carrera.
*/
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        R[i] = A[i] + B[i];
    }

    auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> ms = t2 - t1;

    cout << "\n--- Resultados ---\n";
    cout << "Threads usados (omp_get_max_threads): "
         << omp_get_max_threads() << "\n";
    cout << "Tiempo suma paralela: "
         << ms.count() << " ms\n\n";

    printSample(A, "Arreglo 1");
    printSample(B, "Arreglo 2");
    printSample(R, "Arreglo Resultado");

    // Validación de resultados (primeros 20 elementos)
    bool ok = true;
    int checkCount = min(n, 20);
    for (int i = 0; i < checkCount; i++)
    {
        if (R[i] != A[i] + B[i])
        {
            ok = false;
            break;
        }
    }

    cout << "\nValidacion (primeros " << checkCount
         << " elementos del arreglo): " << (ok ? "OK" : "ERROR") << "\n";

    return 0;
}
