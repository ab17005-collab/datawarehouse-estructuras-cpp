#include "ModuloBPlus.h"
#include "Entrada.h"
#include <iostream>
namespace dw { namespace bplus {
void ModuloBPlus::ejecutar(ContextoAplicacion& contexto, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Arbol B+ ===\nV3: divisiones de hojas e internos. Rangos aun no disponibles.\n1. Enviar solicitud\n2. Indexar venta existente\n3. Buscar clave indexada\n4. Demo independiente con 40 ventas\n5. Mostrar indice actual\n0. Volver\n";
        if (!leerEntero(in, out, "Opcion: ", opcion) || opcion == 0) return;
        if (opcion == 4) {
            // Tabla e indice exclusivos de la demostracion: no alteran los datos compartidos.
            TablaHechos tabla;
            ArbolBPlus demo;
            for (int id = 1; id <= 40; ++id) {
                tabla.ventas.push_back({id, {2026,1,1}, "Demo", "Alimentos", 1, 100});
                demo.insertar(id, tabla.ventas.size()-1);
            }
            out << "Demo independiente: 40 ventas de 100 centavos; tabla compartida sin cambios.\n";
            demo.mostrar(out);
            for (int id = 1; id <= 40; ++id) {
                std::size_t posicion;
                if (demo.buscar(id, posicion) && posicion < tabla.ventas.size() && tabla.ventas[posicion].id == id)
                    out << "Demo encontrada: " << id << '\n';
                else out << "Error de demostracion: " << id << '\n';
            }
            continue;
        }
        if (opcion == 5) { indice_.mostrar(out); continue; }
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
