#include "ModuloBPlus.h"
#include "Entrada.h"
#include <iostream>
namespace dw { namespace bplus {
void ModuloBPlus::ejecutar(ContextoAplicacion& contexto, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Arbol B+ ===\nV2: raiz hoja, hasta 3 claves. Divisiones y rangos aun no disponibles.\n1. Enviar solicitud\n2. Indexar venta existente\n3. Buscar clave indexada\n0. Volver\n";
        if (!leerEntero(in, out, "Opcion: ", opcion) || opcion == 0) return;
        if (opcion < 1 || opcion > 3) { out << "Opcion no valida.\n"; continue; }
        int clave;
        if (!leerEntero(in, out, "Clave positiva: ", clave)) return;
        if (clave <= 0) { out << "La clave debe ser positiva.\n"; continue; }
        if (opcion == 1) { out << "Arbol B+: solicitud recibida [" << clave << "].\n"; continue; }
        std::size_t posicion;
        if (opcion == 2) {
            if (!contexto.hechos.posicionDe(clave, posicion)) { out << "No existe esa venta en la tabla.\n"; continue; }
            const auto resultado = indice_.insertar(clave, posicion);
            if (resultado == ArbolBPlus::Resultado::Insertado) out << "Clave indexada: " << clave << '\n';
            else if (resultado == ArbolBPlus::Resultado::Duplicado) out << "Clave duplicada; indice sin cambios.\n";
            else out << "Capacidad V2: 3 claves; indice sin cambios.\n";
        } else if (!indice_.buscar(clave, posicion)) out << "Clave no indexada.\n";
        else if (posicion >= contexto.hechos.ventas.size() || contexto.hechos.ventas[posicion].id != clave)
            out << "Referencia de indice inconsistente.\n";
        else {
            const auto& v = contexto.hechos.ventas[posicion];
            out << "Venta encontrada: " << v.id << "; sucursal: " << v.sucursal
                << "; importe: " << v.importeCentavos << " centavos\n";
        }
    }
}
} }
