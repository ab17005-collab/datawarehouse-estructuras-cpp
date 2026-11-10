#include "ModuloETL.h"
#include "Entrada.h"
#include <iostream>
namespace dw { namespace etl {
void ModuloETL::ejecutar(ContextoAplicacion&, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Grafo ETL ===\nV1: recepcion de solicitudes; operaciones de datos no disponibles.\n1. Enviar solicitud\n0. Volver\n";
        if (!leerEntero(in, out, "Opcion: ", opcion) || opcion == 0) return;
        if (opcion != 1) { out << "Opcion no valida.\n"; continue; }
        std::string valor;
        if (!leerTexto(in, out, "Nombre ETL: ", valor)) return;
        out << "Grafo ETL: solicitud recibida [" << valor << "].\n";
    }
}
} }
