#include "CuboOLAP.h"
#include <limits>
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
} }
