#include "Modelo.h"
namespace dw {
bool fechaValida(const Fecha& f) {
    if (f.anio < 1 || f.anio > 9999 || f.mes < 1 || f.mes > 12) return false;
    const int dias[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int limite = dias[f.mes-1];
    if (f.mes == 2 && f.anio % 4 == 0 && (f.anio % 100 != 0 || f.anio % 400 == 0)) ++limite;
    return f.dia >= 1 && f.dia <= limite;
}
bool ventaValida(const Venta& v) {
    return v.id > 0 && fechaValida(v.fecha) && v.unidades > 0 && v.importeCentavos >= 0
        && v.sucursal.find_first_not_of(" \t\r\n") != std::string::npos
        && v.categoria.find_first_not_of(" \t\r\n") != std::string::npos;
}
bool TablaHechos::posicionDe(int id, std::size_t& posicion) const {
    for (std::size_t i = 0; i < ventas.size(); ++i) {
        if (ventas[i].id == id) { posicion = i; return true; }
    }
    return false;
}
const Venta* TablaHechos::obtener(int id) const {
    std::size_t posicion;
    return posicionDe(id, posicion) ? &ventas[posicion] : nullptr;
}
TablaHechos crearDatosEjemplo() {
    TablaHechos tabla;
    tabla.ventas = {
        {1, {2026,1,5}, "Centro", "Alimentos", 2, 1000},
        {2, {2026,1,20}, "Norte", "Limpieza", 3, 2000},
        {3, {2026,2,10}, "Centro", "Alimentos", 4, 3000}
    };
    tabla.revision = 1;
    return tabla;
}
}
