#include "CuboOLAP.h"
#include <limits>
#include <ostream>
#include <utility>
namespace dw { namespace olap {
bool CuboOLAP::consultarTotal(const TablaHechos& tabla, Agregado& resultado, std::string& error) const {
    Agregado total;
    for (const auto& venta : tabla.ventas) {
        if (!ventaValida(venta)) { error = "Venta invalida en la tabla."; return false; }
        if (venta.importeCentavos > std::numeric_limits<std::int64_t>::max() - total.importeCentavos
            || venta.unidades > std::numeric_limits<std::int64_t>::max() - total.unidades) {
            error = "Desbordamiento del agregado."; return false;
        }
        total.importeCentavos += venta.importeCentavos;
        total.unidades += venta.unidades;
    }
    resultado = total;
    error.clear();
    return true;
}

CuboOLAP::~CuboOLAP() { liberar(raiz_); }
void CuboOLAP::liberar(Nodo* nodo) {
    // Iterar hermanos evita profundidad recursiva proporcional al numero de fechas.
    while (nodo) {
        Nodo* siguiente = nodo->hermano;
        liberar(nodo->hijo);
        delete nodo;
        nodo = siguiente;
    }
}
CuboOLAP::Nodo* CuboOLAP::obtenerOCrear(Nodo* padre, int valor) {
    Nodo** enlace = &padre->hijo;
    while (*enlace && (*enlace)->valor < valor) enlace = &(*enlace)->hermano;
    if (*enlace && (*enlace)->valor == valor) return *enlace;
    Nodo* nuevo = new Nodo(valor);
    nuevo->hermano = *enlace;
    *enlace = nuevo;
    return nuevo;
}
bool CuboOLAP::reconstruir(const TablaHechos& tabla, std::string& error) {
    Agregado total;
    if (!consultarTotal(tabla, total, error)) return false;
    CuboOLAP temporal;
    temporal.raiz_ = new Nodo(0);
    temporal.raiz_->total = total;
    // Todas las medidas son no negativas y el total ya fue validado: ningun subtotal desborda.
    for (const auto& venta : tabla.ventas) {
        Nodo* padre = temporal.raiz_;
        for (int valor : {venta.fecha.anio, venta.fecha.mes, venta.fecha.dia}) {
            padre = obtenerOCrear(padre, valor);
            padre->total.unidades += venta.unidades;
            padre->total.importeCentavos += venta.importeCentavos;
        }
    }
    std::swap(raiz_, temporal.raiz_);
    error.clear();
    return true;
}
bool CuboOLAP::consultarPeriodo(int anio, int mes, int dia, Agregado& resultado, std::string& error) const {
    if (anio < 1 || anio > 9999 || mes < 0 || mes > 12 || dia < 0 || (mes == 0 && dia != 0)
        || (dia != 0 && !fechaValida({anio,mes,dia}))) {
        error = "Periodo invalido."; return false;
    }
    if (!raiz_) { error = "Primero reconstruya la jerarquia."; return false; }
    const Nodo* nodo = raiz_;
    for (int valor : {anio,mes,dia}) {
        if (valor == 0) break;
        nodo = nodo->hijo;
        while (nodo && nodo->valor < valor) nodo = nodo->hermano;
        if (!nodo || nodo->valor != valor) { resultado = {}; error.clear(); return true; }
    }
    resultado = nodo->total;
    error.clear();
    return true;
}
void CuboOLAP::mostrar(const Nodo* nodo, int nivel, std::ostream& out) {
    const char* etiquetas[] = {"Total", "Anio", "Mes", "Dia"};
    for (; nodo; nodo = nodo->hermano) {
        out << std::string(static_cast<std::size_t>(nivel)*2, ' ') << etiquetas[nivel];
        if (nivel) out << ' ' << nodo->valor;
        out << ": " << nodo->total.importeCentavos << " centavos; unidades: " << nodo->total.unidades << '\n';
        mostrar(nodo->hijo, nivel+1, out);
    }
}
void CuboOLAP::mostrarJerarquia(std::ostream& out) const {
    if (!raiz_) out << "Jerarquia no construida.\n";
    else mostrar(raiz_, 0, out);
}
} }
