/**
 * @file main.cpp
 * @brief Program obliczający przybliżenie liczby PI metodą całkowania numerycznego.
 * @author Twój Nick/Imię
 * @date 2024-01-20
 * @version 1.0
 */

#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <string>

/**
 * @brief Funkcja matematyczna podlegająca całkowaniu.
 * * Funkcja zdefiniowana wzorem:
 * \f$ f(x) = \frac{4}{1+x^2} \f$
 * * Całka tej funkcji w przedziale [0,1] wynosi PI.
 * * @param x Argument funkcji (liczba rzeczywista).
 * @return Wartość funkcji w punkcie x.
 */
double func(double x) {
    return 4.0 / (1.0 + x * x);
}

/**
 * @brief Główna funkcja programu.
 * * Odpowiada za pobranie argumentów, uruchomienie wątków i zmierzenie czasu.
 * * @param argc Liczba argumentów wywołania.
 * @param argv Tablica argumentów (argv[1] - kroki, argv[2] - wątki).
 * @return Kod wyjścia (0 oznacza sukces).
 */
int main(int argc, char* argv[]) {
    long long num_steps = 100000000; 
    int num_threads = 1;

    // Obsługa argumentów
    if (argc >= 3) {
        num_steps = std::stoll(argv[1]);
        num_threads = std::stoi(argv[2]);
    } else {
        std::cout << "Podaj liczbe krokow (N): ";
        std::cin >> num_steps;
        std::cout << "Podaj liczbe watkow: ";
        std::cin >> num_threads;
    }

    double step_width = 1.0 / (double)num_steps;
    double total_pi = 0.0;
    
    std::vector<double> partial_sums(num_threads, 0.0);
    std::vector<std::thread> threads;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Tworzenie wątków z wyrażeniem lambda
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            double local_sum = 0.0;
            long long items_per_thread = num_steps / num_threads;
            long long start_index = i * items_per_thread;
            long long end_index = (i == num_threads - 1) ? num_steps : start_index + items_per_thread;

            for (long long j = start_index; j < end_index; ++j) {
                double x = (j + 0.5) * step_width;
                local_sum += func(x);
            }
            partial_sums[i] = local_sum;
        });
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    for (double val : partial_sums) total_pi += val;
    total_pi *= step_width;

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << std::fixed << std::setprecision(15);
    std::cout << "Wynik PI: " << total_pi << std::endl;
    std::cout << "Czas: " << elapsed.count() << " s" << std::endl;
    std::cout << "Watki: " << num_threads << std::endl;
    std::cout << "Kroki: " << num_steps << std::endl;

    return 0;
}