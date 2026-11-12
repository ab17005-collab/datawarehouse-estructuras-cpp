#pragma once
#include "Modelo.h"
#include <iosfwd>
namespace dw { namespace olap {
struct Agregado { std::int64_t unidades = 0; std::int64_t importeCentavos = 0; };
class CuboOLAP {
public:
    CuboOLAP() = default;
    ~CuboOLAP();
    CuboOLAP(const CuboOLAP&) = delete;
    CuboOLAP& operator=(const CuboOLAP&) = delete;
    bool consultarTotal(const TablaHechos&, Agregado&, std::string& error) const;
    bool reconstruir(const TablaHechos&, std::string& error);
    // mes=0 agrega el anio; dia=0 agrega el mes. Periodo valido ausente suma cero.
    bool consultarPeriodo(int anio, int mes, int dia, Agregado&, std::string& error) const;
    void mostrarJerarquia(std::ostream&) const;
private:
    struct Nodo {
        int valor;
        Agregado total;
        Nodo* hijo = nullptr;
        Nodo* hermano = nullptr;
        explicit Nodo(int v) : valor(v) {}
    };
    Nodo* raiz_ = nullptr;
    static void liberar(Nodo*);
    static Nodo* obtenerOCrear(Nodo*, int);
    static void mostrar(const Nodo*, int nivel, std::ostream&);
};
} }
