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
private:
    int indice(const std::string&) const;
    struct Nodo { std::string nombre; TipoNodo tipo = TipoNodo::Fuente; };
    Nodo nodos_[Capacidad];
    bool matriz_[Capacidad][Capacidad] = {};
    std::size_t cantidad_ = 0;
};
} }
