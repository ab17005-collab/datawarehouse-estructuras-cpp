#pragma once
#include "Modelo.h"
#include <iosfwd>
namespace dw { namespace olap {
struct FiltroVentas { std::string sucursal; std::string categoria; };
struct Agregado { std::int64_t unidades = 0; std::int64_t importeCentavos = 0; };
struct DetalleTemporal { int valor = 0; Agregado total; };
struct VistaTemporal {
    int nivel = 0; // 0 total, 1 anio, 2 mes, 3 dia.
    int valor = 0;
    Agregado total;
    std::vector<DetalleTemporal> hijos;
};
class CuboOLAP {
public:
    CuboOLAP() = default;
    ~CuboOLAP();
    CuboOLAP(const CuboOLAP&) = delete;
    CuboOLAP& operator=(const CuboOLAP&) = delete;
    bool consultarTotal(const TablaHechos&, Agregado&, std::string& error) const;
    bool reconstruir(const TablaHechos&, std::string& error);
    bool actualizar(const TablaHechos&, const FiltroVentas&, std::string& error);
    bool consultarTotalFiltrado(Agregado&, std::string& error) const;
    // mes=0 agrega el anio; dia=0 agrega el mes. Periodo valido ausente suma cero.
    bool consultarPeriodo(int anio, int mes, int dia, Agregado&, std::string& error) const;
    void mostrarJerarquia(std::ostream&) const;
    bool vistaActual(VistaTemporal&, std::string& error) const;
    bool drillDown(int valorHijo, std::string& error);
    bool rollUp(std::string& error);
private:
    struct Nodo {
        int valor;
        Agregado total;
        Nodo* hijo = nullptr;
        Nodo* hermano = nullptr;
        explicit Nodo(int v) : valor(v) {}
    };
    Nodo* raiz_ = nullptr;
    std::vector<int> ruta_;
    const TablaHechos* origen_ = nullptr; // Solo identidad; no se desreferencia.
    std::uint64_t revision_ = 0;
    FiltroVentas filtroConstruido_;
    const Nodo* nodoActual() const;
    static void liberar(Nodo*);
    static Nodo* obtenerOCrear(Nodo*, int);
    static void mostrar(const Nodo*, int nivel, std::ostream&);
};
} }
