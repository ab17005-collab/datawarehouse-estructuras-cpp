#include "ModuloOLAP.h"
#include "CuboOLAP.h"
#include "Entrada.h"
#include <iostream>
namespace dw { namespace olap {
void ModuloOLAP::ejecutar(ContextoAplicacion& contexto, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Cubo OLAP ===\nV2: total global; jerarquia y navegacion aun no disponibles.\n1. Enviar solicitud\n2. Listar ventas\n3. Total global\n0. Volver\n";
        if (!leerEntero(in, out, "Opcion: ", opcion) || opcion == 0) return;
        if (opcion == 1) {
            std::string valor;
            if (!leerTexto(in, out, "Solicitud OLAP: ", valor)) return;
            out << "Cubo OLAP: solicitud recibida [" << valor << "].\n";
        } else if (opcion == 2) {
            out << "ID Fecha Sucursal Categoria Unidades Centavos\n";
            for (const auto& v : contexto.hechos.ventas)
                out << v.id << ' ' << v.fecha.anio << '-' << v.fecha.mes << '-' << v.fecha.dia << ' '
                    << v.sucursal << ' ' << v.categoria << ' ' << v.unidades << ' ' << v.importeCentavos << '\n';
        } else if (opcion == 3) {
            Agregado total;
            std::string error;
            if (CuboOLAP().consultarTotal(contexto.hechos, total, error))
                out << "Total: " << total.importeCentavos << " centavos; unidades: " << total.unidades << '\n';
            else out << error << '\n';
        } else out << "Opcion no valida.\n";
    }
}
} }
