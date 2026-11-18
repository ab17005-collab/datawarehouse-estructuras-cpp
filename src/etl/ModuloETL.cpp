#include "ModuloETL.h"
#include "Entrada.h"
#include <iostream>
#include <fstream>
#include <new>
namespace dw { namespace etl {
void ModuloETL::ejecutar(ContextoAplicacion&, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Grafo ETL ===\nV6: carga CSV de escenario ETL y analisis completo.\n1. Enviar solicitud\n2. Agregar nodo\n3. Agregar arista\n4. Mostrar matriz\n5. Recorrido BFS\n6. Recorrido DFS\n7. Dependencias directas\n8. Dependencias transitivas\n9. Impacto de un cambio\n10. Detectar ciclos\n11. Importar escenario CSV\n0. Volver\n";
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
        else if (opcion == 5) {
            if (!leerTexto(in, out, "Nodo inicial (ejemplo fuente): ", nombre)) return;
            std::vector<std::string> recorrido;
            if (!grafo_.bfs(nombre, recorrido, error)) out << error << '\n';
            else {
                out << "BFS:";
                for (const auto& nodo : recorrido) out << ' ' << nodo;
                out << '\n';
            }
        } else if (opcion >= 6 && opcion <= 8) {
            if (!leerTexto(in, out, opcion == 6 ? "Nodo inicial (ejemplo fuente): " : "Nodo consultado (ejemplo hechos): ", nombre)) return;
            std::vector<std::string> resultado;
            const bool ok = opcion == 6 ? grafo_.dfs(nombre, resultado, error)
                : grafo_.dependencias(nombre, opcion == 8, resultado, error);
            if (!ok) out << error << '\n';
            else {
                out << (opcion == 6 ? "DFS:" : opcion == 7 ? "Dependencias directas:" : "Dependencias transitivas:");
                for (const auto& nodo : resultado) out << ' ' << nodo;
                if (resultado.empty()) out << " (ninguna)";
                out << '\n';
            }
        } else if (opcion == 9) {
            if (!leerTexto(in,out,"Nodo que cambia (ejemplo fuente): ",nombre)) return;
            std::vector<std::string> afectados;
            if (!grafo_.impacto(nombre,afectados,error)) out << error << '\n';
            else {
                out << "Impacto:";
                for (const auto& afectado : afectados) out << ' ' << afectado;
                if (afectados.empty()) out << " (ninguno)";
                out << '\n';
            }
        } else if (opcion == 10) {
            std::vector<std::string> ciclo;
            if (!grafo_.detectarCiclo(ciclo)) out << "Sin ciclos: flujo ETL aciclico.\n";
            else {
                out << "Ciclo detectado: ";
                for (std::size_t i = 0; i < ciclo.size(); ++i) out << (i ? " -> " : "") << ciclo[i];
                out << '\n';
            }
        } else if (opcion == 11) {
            std::string ruta;
            if (!leerTexto(in,out,"Archivo ETL (ejemplo data/etl.csv): ",ruta)) return;
            std::ifstream archivo(ruta);
            if (!archivo) { out << "No se pudo abrir el archivo ETL; grafo sin cambios.\n"; continue; }
            const auto antes = grafo_.cantidad();
            try {
                if (grafo_.importarCSV(archivo,error)) out << "CSV ETL cargado: " << grafo_.cantidad()-antes
                    << " nodos agregados; total: " << grafo_.cantidad() << '\n';
                else out << "Carga rechazada; grafo sin cambios. " << error << '\n';
            } catch (const std::bad_alloc&) { out << "Memoria insuficiente; grafo sin cambios.\n"; }
        } else out << "Opcion no valida.\n";
    }
}
} }
