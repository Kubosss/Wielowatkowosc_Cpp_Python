import subprocess
import matplotlib.pyplot as plt
import re
import os

# Parametry testu (zgodne z treścią zadania)
# Uwaga: 3 mld kroków (3_000_000_000) może trwać długo na słabszym komputerze.
steps_list = [100_000_000, 1_000_000_000, 3_000_000_000] 
max_threads = 50  # Testujemy od 1 do 50 wątków
threads_list = list(range(1, max_threads + 1))

# Nazwa pliku wykonywalnego C++
executable = "main.exe"

# Sprawdzenie czy plik exe istnieje
if not os.path.exists(executable):
    print("BŁĄD: Nie znaleziono pliku main.exe! Skompiluj najpierw kod C++.")
    exit()

results = {}

print(f"Rozpoczynam testy wydajności. Maksymalna liczba wątków: {max_threads}")

for steps in steps_list:
    print(f"\n--- Rozpoczynam serię dla {steps} kroków ---")
    times = []
    
    for t in threads_list:
        try:
            # Uruchomienie procesu C++: main.exe <liczba_krokow> <liczba_watkow>
            process = subprocess.run(
                [executable, str(steps), str(t)],
                capture_output=True,
                text=True
            )
            
            # Odczytanie czasu z wyjścia programu
            output = process.stdout
            # Szukamy linii w stylu "Czas: 1.2345 s"
            match = re.search(r"Czas: ([0-9\.]+) s", output)
            
            if match:
                elapsed_time = float(match.group(1))
                times.append(elapsed_time)
                # Wypisujemy postęp w jednej linii, żeby nie śmiecić
                print(f"\rWątki: {t}/{max_threads} | Czas: {elapsed_time:.4f}s", end="")
            else:
                print(f"\nBłąd odczytu dla {t} wątków. Wyjście programu:\n{output}")
                times.append(None)
                
        except Exception as e:
            print(f"\nBłąd uruchomienia: {e}")
            times.append(None)
            
    results[steps] = times
    print() # Nowa linia po zakończeniu pętli

# Rysowanie wykresu
print("\nRysowanie wykresu...")
plt.figure(figsize=(10, 6))

for steps, times in results.items():
    if times and len(times) == len(threads_list):
        plt.plot(threads_list, times, label=f'Liczba podziałów: {steps}')

plt.title('Czas obliczeń PI w zależności od liczby wątków')
plt.xlabel('Liczba wątków')
plt.ylabel('Czas wykonania (sekundy)')
plt.legend()
plt.grid(True)

# Zapis wykresu do pliku (ważne do dokumentacji)
plt.savefig("wykres_wyniki.png")
print("Gotowe! Wykres zapisano jako 'wykres_wyniki.png'.")
plt.show()