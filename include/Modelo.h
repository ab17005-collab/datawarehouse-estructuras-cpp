#ifndef DW_MODELO_H
#define DW_MODELO_H
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace dw {
struct Fecha { int anio = 0; int mes = 0; int dia = 0; };
bool fechaValida(const Fecha&);
struct Venta {
    int id = 0;
    Fecha fecha;
    std::string sucursal;
    std::string categoria;
    int unidades = 0;
    std::int64_t importeCentavos = 0;
};
bool ventaValida(const Venta&);
struct TablaHechos {
    std::vector<Venta> ventas;
    std::uint64_t revision = 0;
    // Sin borrado: una posicion permanece estable aunque vector se realoque.
    bool posicionDe(int id, std::size_t& posicion) const;
    const Venta* obtener(int id) const;
};
TablaHechos crearDatosEjemplo();
struct ContextoAplicacion { TablaHechos hechos; };
}
#endif
