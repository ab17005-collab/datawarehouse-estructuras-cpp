
#include <iostream>
#include <limits>

int main() {
    int opcion = -1;

    do {
        std::cout << "\n=== DATA WAREHOUSE ===\n";
        std::cout << "1. Cubo OLAP\n";
        std::cout << "2. Arbol B+\n";
        std::cout << "3. Grafo ETL\n";
        std::cout << "0. Salir\n";
        std::cout << "Seleccione una opcion: ";

        if (!(std::cin >> opcion)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );
            std::cout << "Entrada invalida.\n";
            continue;
        }

        switch (opcion) {
            case 1:
                std::cout << "Modulo OLAP pendiente.\n";
                break;
            case 2:
                std::cout << "Modulo B+ pendiente.\n";
                break;
            case 3:
                std::cout << "Modulo ETL pendiente.\n";
                break;
            case 0:
                std::cout << "Hasta luego.\n";
                break;
            default:
                std::cout << "Opcion no valida.\n";
        }
    } while (opcion != 0);

    return 0;
}
