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
    ruta_.clear();
    origen_ = nullptr;
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

const CuboOLAP::Nodo* CuboOLAP::nodoActual() const {
    const Nodo* actual = raiz_;
    for (int valor : ruta_) {
        if (!actual) return nullptr;
        actual = actual->hijo;
        while (actual && actual->valor != valor) actual = actual->hermano;
    }
    return actual;
}
bool CuboOLAP::vistaActual(VistaTemporal& resultado, std::string& error) const {
    const Nodo* actual = nodoActual();
    if (!actual) { error = "Primero reconstruya la jerarquia."; return false; }
    VistaTemporal temporal;
    temporal.nivel = static_cast<int>(ruta_.size());
    temporal.valor = actual->valor;
    temporal.total = actual->total;
    for (const Nodo* hijo = actual->hijo; hijo; hijo = hijo->hermano)
        temporal.hijos.push_back({hijo->valor, hijo->total});
    resultado = temporal;
    error.clear();
    return true;
}
bool CuboOLAP::drillDown(int valorHijo, std::string& error) {
    const Nodo* actual = nodoActual();
    if (!actual) { error = "Primero reconstruya la jerarquia."; return false; }
    if (ruta_.size() == 3) { error = "El dia es el nivel mas detallado."; return false; }
    for (const Nodo* hijo = actual->hijo; hijo; hijo = hijo->hermano) {
        if (hijo->valor == valorHijo) { ruta_.push_back(valorHijo); error.clear(); return true; }
    }
    error = "No existe ese hijo en la vista actual.";
    return false;
}
bool CuboOLAP::rollUp(std::string& error) {
    if (!raiz_) { error = "Primero reconstruya la jerarquia."; return false; }
    if (ruta_.empty()) { error = "Ya esta en el total general."; return false; }
    ruta_.pop_back();
    error.clear();
    return true;
}


bool CuboOLAP::actualizar(const TablaHechos& tabla, const FiltroVentas& filtro, std::string& error) {
    if (raiz_ && origen_ == &tabla && revision_ == tabla.revision
        && filtroConstruido_.sucursal == filtro.sucursal && filtroConstruido_.categoria == filtro.categoria) {
        error.clear(); return true;
    }
    Agregado validacion;
    if (!consultarTotal(tabla,validacion,error)) return false;
    FiltroVentas copia = filtro;
    TablaHechos seleccion;
    for (const auto& venta : tabla.ventas)
        if ((filtro.sucursal.empty() || filtro.sucursal == venta.sucursal)
            && (filtro.categoria.empty() || filtro.categoria == venta.categoria)) seleccion.ventas.push_back(venta);
    if (!reconstruir(seleccion,error)) return false;
    filtroConstruido_ = std::move(copia);
    origen_ = &tabla; revision_ = tabla.revision;
    return true;
}
bool CuboOLAP::consultarTotalFiltrado(Agregado& resultado, std::string& error) const {
    if (!raiz_) { error = "Primero reconstruya la jerarquia."; return false; }
    resultado = raiz_->total; error.clear(); return true;
}

} }
