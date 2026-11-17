#pragma once
#include <cstddef>
#include <string>
#include <iosfwd>
#include <vector>
namespace dw { namespace etl {
enum class TipoNodo { Fuente = 1, Tabla = 2, Proceso = 3 };
class GrafoETL {
public:
    static constexpr std::size_t Capacidad = 64;
    bool agregarNodo(const std::string&, TipoNodo, std::string& error);
    bool agregarArista(const std::string& origen, const std::string& destino, std::string& error);
    bool hayArista(const std::string& origen, const std::string& destino) const;
    std::size_t cantidad() const { return cantidad_; }
    void mostrar(std::ostream&) const;
    bool bfs(const std::string& origen, std::vector<std::string>& recorrido, std::string& error) const;
    bool dfs(const std::string& origen, std::vector<std::string>& recorrido, std::string& error) const;
    // Dependencias excluyen al nodo consultado incluso cuando hay ciclos.
    // Orden de indice de alta; transitivas=true incluye todos los antecesores.
    bool dependencias(const std::string& destino, bool transitivas, std::vector<std::string>& resultado, std::string& error) const;
    bool impacto(const std::string& origen, std::vector<std::string>& resultado, std::string& error) const;
    bool detectarCiclo(std::vector<std::string>& testigo) const;
private:
    bool cicloDesde(int actual, int colores[], std::vector<int>& pila, std::vector<std::string>& testigo) const;
    void dfsDesde(int actual, bool visitados[], std::vector<std::string>& recorrido) const;
    void marcarAntecesores(int actual, bool visitados[]) const;
    int indice(const std::string&) const;
    struct Nodo { std::string nombre; TipoNodo tipo = TipoNodo::Fuente; };
    Nodo nodos_[Capacidad];
    bool matriz_[Capacidad][Capacidad] = {};
    std::size_t cantidad_ = 0;
};
} }
