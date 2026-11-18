#include "GrafoETL.h"
#include <ostream>
#include "ColaIndices.h"
#include "CSV.h"
#include <utility>
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

bool GrafoETL::bfs(const std::string& origen, std::vector<std::string>& recorrido, std::string& error) const {
    const int inicio = indice(origen);
    if (inicio < 0) { error = "Nodo inexistente."; return false; }
    bool visitados[Capacidad] = {};
    ColaIndices cola;
    std::vector<std::string> temporal;
    cola.insertar(inicio); visitados[inicio] = true;
    int actual;
    while (cola.remover(actual)) {
        temporal.push_back(nodos_[actual].nombre);
        for (std::size_t j = 0; j < cantidad_; ++j) {
            if (matriz_[actual][j] && !visitados[j]) {
                visitados[j] = true; // Marcar al encolar evita duplicados en convergencias.
                cola.insertar(static_cast<int>(j));
            }
        }
    }
    recorrido.swap(temporal);
    error.clear();
    return true;
}

void GrafoETL::dfsDesde(int actual, bool visitados[], std::vector<std::string>& recorrido) const {
    visitados[actual] = true;
    recorrido.push_back(nodos_[actual].nombre);
    for (std::size_t i = 0; i < cantidad_; ++i)
        if (matriz_[actual][i] && !visitados[i]) dfsDesde(static_cast<int>(i), visitados, recorrido);
}
bool GrafoETL::dfs(const std::string& origen, std::vector<std::string>& recorrido, std::string& error) const {
    const int inicio = indice(origen);
    if (inicio < 0) { error = "Nodo inexistente."; return false; }
    bool visitados[Capacidad] = {};
    std::vector<std::string> temporal;
    dfsDesde(inicio, visitados, temporal);
    recorrido.swap(temporal);
    error.clear();
    return true;
}
void GrafoETL::marcarAntecesores(int actual, bool visitados[]) const {
    visitados[actual] = true;
    for (std::size_t i = 0; i < cantidad_; ++i)
        if (matriz_[i][actual] && !visitados[i]) marcarAntecesores(static_cast<int>(i), visitados);
}
bool GrafoETL::dependencias(const std::string& destino, bool transitivas, std::vector<std::string>& resultado, std::string& error) const {
    const int fin = indice(destino);
    if (fin < 0) { error = "Nodo inexistente."; return false; }
    bool visitados[Capacidad] = {};
    if (transitivas) marcarAntecesores(fin, visitados);
    else for (std::size_t i = 0; i < cantidad_; ++i) visitados[i] = matriz_[i][fin];
    std::vector<std::string> temporal;
    for (std::size_t i = 0; i < cantidad_; ++i)
        if (visitados[i] && static_cast<int>(i) != fin) temporal.push_back(nodos_[i].nombre);
    resultado.swap(temporal);
    error.clear();
    return true;
}


bool GrafoETL::impacto(const std::string& origen, std::vector<std::string>& resultado, std::string& error) const {
    std::vector<std::string> recorrido;
    if (!bfs(origen,recorrido,error)) return false;
    recorrido.erase(recorrido.begin()); // BFS siempre incluye primero al origen existente.
    resultado.swap(recorrido);
    error.clear(); return true;
}
bool GrafoETL::cicloDesde(int actual, int colores[], std::vector<int>& pila, std::vector<std::string>& testigo) const {
    colores[actual] = 1; pila.push_back(actual);
    for (std::size_t i = 0; i < cantidad_; ++i) {
        if (!matriz_[actual][i]) continue;
        if (colores[i] == 1) {
            bool copiar = false;
            for (int vertice : pila) {
                if (vertice == static_cast<int>(i)) copiar = true;
                if (copiar) testigo.push_back(nodos_[vertice].nombre);
            }
            testigo.push_back(nodos_[i].nombre);
            return true;
        }
        if (colores[i] == 0 && cicloDesde(static_cast<int>(i),colores,pila,testigo)) return true;
    }
    pila.pop_back(); colores[actual] = 2;
    return false;
}
bool GrafoETL::detectarCiclo(std::vector<std::string>& testigo) const {
    int colores[Capacidad] = {};
    std::vector<int> pila;
    std::vector<std::string> temporal;
    for (std::size_t i = 0; i < cantidad_; ++i)
        if (colores[i] == 0 && cicloDesde(static_cast<int>(i),colores,pila,temporal)) {
            testigo.swap(temporal); return true;
        }
    testigo.clear(); return false;
}


bool GrafoETL::importarCSV(std::istream& in, std::string& error) {
    std::vector<FilaCSV> filas;
    if (!leerCSV(in,{"clase","nombre","tipo","origen","destino"},filas,error)) return false;
    GrafoETL temporal = *this;
    for (const auto& fila : filas) {
        const auto& c = fila.campos;
        if (c[0] == "nodo") {
            TipoNodo tipo;
            if (c[2] == "fuente") tipo = TipoNodo::Fuente;
            else if (c[2] == "tabla") tipo = TipoNodo::Tabla;
            else if (c[2] == "proceso") tipo = TipoNodo::Proceso;
            else { error = "Linea " + std::to_string(fila.linea) + ": tipo ETL invalido."; return false; }
            if (!c[3].empty() || !c[4].empty()) { error = "Linea " + std::to_string(fila.linea) + ": nodo con extremos de arista."; return false; }
            if (!temporal.agregarNodo(c[1],tipo,error)) { error = "Linea " + std::to_string(fila.linea) + ": " + error; return false; }
        } else if (c[0] != "arista" || !c[1].empty() || !c[2].empty()) {
            error = "Linea " + std::to_string(fila.linea) + ": clase o campos ETL invalidos."; return false;
        }
    }
    // Dos pasadas: se permite declarar una arista antes que sus nodos.
    for (const auto& fila : filas) {
        const auto& c = fila.campos;
        if (c[0] == "arista" && !temporal.agregarArista(c[3],c[4],error)) {
            error = "Linea " + std::to_string(fila.linea) + ": " + error; return false;
        }
    }
    *this = std::move(temporal);
    error.clear(); return true;
}

} }
