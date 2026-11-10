#ifndef DW_MODELO_H
#define DW_MODELO_H
#include <cstdint>
#include <string>
#include <vector>
namespace dw {
// Contratos de datos; las operaciones de tabla se incorporan en V2 y V5.
struct Fecha { int anio = 0; int mes = 0; int dia = 0; };
struct Venta {
    int id = 0;
    Fecha fecha;
    std::string sucursal;
    std::string categoria;
    int unidades = 0;
    std::int64_t importeCentavos = 0;
};
struct TablaHechos { std::vector<Venta> ventas; std::uint64_t revision = 0; };
struct ContextoAplicacion { TablaHechos hechos; };
}
#endif
