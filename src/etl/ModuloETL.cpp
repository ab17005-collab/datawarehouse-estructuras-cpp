#include "ModuloETL.h"
#include "Entrada.h"
#include <iostream>
namespace dw { namespace etl {
void ModuloETL::ejecutar(ContextoAplicacion&, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Grafo ETL ===\nV2: nodos y aristas; recorridos e impacto aun no disponibles.\n1. Enviar solicitud\n2. Agregar nodo\n3. Agregar arista\n4. Mostrar matriz\n0. Volver\n";
        if (!leerEntero(in, out, "Opcion: ", opcion) || opcion == 0) return;
        std::string nombre, error;
        if (opcion == 1) {
            if (!leerTexto(in, out, "Nombre ETL: ", nombre)) return;
            out << "Grafo ETL: solicitud recibida [" << nombre << "].\n";
        } else if (opcion == 2) {
            int tipo;
            if (!leerTexto(in, out, "Nombre: ", nombre)
                || !leerEntero(in, out, "Tipo (1 fuente, 2 tabla, 3 proceso): ", tipo)) return;
            if (tipo < 1 || tipo > 3) { out << "Tipo invalido.\n"; continue; }
            if (grafo_.agregarNodo(nombre, static_cast<TipoNodo>(tipo), error)) out << "Nodo agregado.\n";
            else out << error << '\n';
        } else if (opcion == 3) {
            std::string destino;
            if (!leerTexto(in, out, "Origen: ", nombre) || !leerTexto(in, out, "Destino: ", destino)) return;
            if (grafo_.agregarArista(nombre, destino, error)) out << "Arista agregada.\n";
            else out << error << '\n';
        } else if (opcion == 4) grafo_.mostrar(out);
        else out << "Opcion no valida.\n";
    }
}
} }
