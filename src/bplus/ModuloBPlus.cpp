#include "ModuloBPlus.h"
#include "Entrada.h"
#include <iostream>
namespace dw { namespace bplus {
void ModuloBPlus::ejecutar(ContextoAplicacion&, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Arbol B+ ===\nV1: recepcion de solicitudes; operaciones de datos no disponibles.\n1. Enviar solicitud\n0. Volver\n";
        if (!leerEntero(in, out, "Opcion: ", opcion) || opcion == 0) return;
        if (opcion != 1) { out << "Opcion no valida.\n"; continue; }
        int valor;
        if (!leerEntero(in, out, "Clave positiva: ", valor)) return;
        if (valor <= 0) { out << "La clave debe ser positiva.\n"; continue; }
        out << "Arbol B+: solicitud recibida [" << valor << "].\n";
    }
}
} }
