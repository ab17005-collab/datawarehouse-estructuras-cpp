#include "GrafoETL.h"
#include <ostream>
namespace dw { namespace etl {
int GrafoETL::indice(const std::string& nombre) const {
    for (std::size_t i = 0; i < cantidad_; ++i)
        if (nodos_[i].nombre == nombre) return static_cast<int>(i);
    return -1;
}
bool GrafoETL::agregarNodo(const std::string& nombre, TipoNodo tipo, std::string& error) {
    if (nombre.find_first_not_of(" \t\r\n") == std::string::npos) error = "Nombre vacio.";
    else if (tipo != TipoNodo::Fuente && tipo != TipoNodo::Tabla && tipo != TipoNodo::Proceso) error = "Tipo invalido.";
    else if (indice(nombre) >= 0) error = "Nodo duplicado.";
    else if (cantidad_ == Capacidad) error = "Capacidad ETL: 64 nodos.";
    else { nodos_[cantidad_++] = {nombre, tipo}; error.clear(); return true; }
    return false;
}
bool GrafoETL::agregarArista(const std::string& origen, const std::string& destino, std::string& error) {
    const int a = indice(origen), b = indice(destino);
    if (a < 0 || b < 0) { error = "Nodo inexistente."; return false; }
    if (matriz_[a][b]) { error = "Arista duplicada."; return false; }
    matriz_[a][b] = true;
    error.clear();
    return true;
}
bool GrafoETL::hayArista(const std::string& origen, const std::string& destino) const {
    const int a = indice(origen), b = indice(destino);
    return a >= 0 && b >= 0 && matriz_[a][b];
}
void GrafoETL::mostrar(std::ostream& out) const {
    out << "Nodos (indice nombre tipo):\n";
    for (std::size_t i = 0; i < cantidad_; ++i) {
        const auto tipo = nodos_[i].tipo;
        out << i << ' ' << nodos_[i].nombre << ' '
            << (tipo == TipoNodo::Fuente ? "fuente" : tipo == TipoNodo::Tabla ? "tabla" : "proceso") << '\n';
    }
    out << "Matriz: fila origen -> columna destino (B depende de A si A->B).\n   ";
    for (std::size_t j = 0; j < cantidad_; ++j) out << j << ' ';
    out << '\n';
    for (std::size_t i = 0; i < cantidad_; ++i) {
        out << i << ": ";
        for (std::size_t j = 0; j < cantidad_; ++j) out << (matriz_[i][j] ? 1 : 0) << ' ';
        out << '\n';
    }
}
} }
