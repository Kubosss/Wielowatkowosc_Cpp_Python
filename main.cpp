#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <string>

// Funkcja matematyczna, którą całkujemy: f(x) = 4 / (1 + x^2)
// Całka z tego w przedziale [0,1] daje PI.
double func(double x) {
    return 4.0 / (1.0 + x * x);
}

int main(int argc, char* argv[]) {
    // Domyślne wartości (gdybyś uruchamiał program ręcznie bez argumentów)
    long long num_steps = 100000000; 
    int num_threads = 1;

    // 1. Obsługa argumentów wejściowych (To pozwoli Pythonowi sterować programem)
    // argv[1] to liczba kroków, argv[2] to liczba wątków
    if (argc >= 3) {
        num_steps = std::stoll(argv[1]);
        num_threads = std::stoi(argv[2]);
    } else {
        // Jeśli uruchomisz ręcznie, program zapyta o dane
        std::cout << "Podaj liczbe krokow (N): ";
        std::cin >> num_steps;
        std::cout << "Podaj liczbe watkow: ";
        std::cin >> num_threads;
    }

    double step_width = 1.0 / (double)num_steps;
    double total_pi = 0.0;
    
    // Wektor na wyniki cząstkowe (żeby wątki nie nadpisywały sobie zmiennych)
    std::vector<double> partial_sums(num_threads, 0.0);
    std::vector<std::thread> threads;

    // Start pomiaru czasu
    auto start_time = std::chrono::high_resolution_clock::now();

    // 2. Tworzenie wątków i zrównoleglenie
    for (int i = 0; i < num_threads; ++i) {
        
        // --- WYMÓG ZADANIA: Wyrażenie Lambda ---
        // [&, i] oznacza: przekaż wszystko przez referencję, ale 'i' (indeks wątku) przez wartość
        threads.emplace_back([&, i]() {
            double local_sum = 0.0;
            
            // Dzielimy pracę: każdy wątek bierze swój kawałek przedziału
            long long items_per_thread = num_steps / num_threads;
            long long start_index = i * items_per_thread;
            // Ostatni wątek bierze wszystko co zostało (żeby nie zgubić końcówki przy dzieleniu)
            long long end_index = (i == num_threads - 1) ? num_steps : start_index + items_per_thread;

            for (long long j = start_index; j < end_index; ++j) {
                // Metoda prostokątów (punkt środkowy)
                double x = (j + 0.5) * step_width;
                local_sum += func(x);
            }
            
            // Zapisz wynik tego wątku do jego "przegródki"
            partial_sums[i] = local_sum;
        });
    }

    // 3. Czekanie na zakończenie wszystkich wątków
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Sumowanie wyników cząstkowych
    for (double val : partial_sums) {
        total_pi += val;
    }
    total_pi *= step_width;

    // Stop pomiaru czasu
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // 4. Wypisanie wyników (Format ważny dla skryptu Pythona!)
    std::cout << std::fixed << std::setprecision(15);
    std::cout << "Wynik PI: " << total_pi << std::endl;
    std::cout << "Czas: " << elapsed.count() << " s" << std::endl; // Python szuka słowa "Czas:"
    std::cout << "Watki: " << num_threads << std::endl;
    std::cout << "Kroki: " << num_steps << std::endl;

    return 0;
}