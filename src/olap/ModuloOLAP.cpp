#include "ModuloOLAP.h"
#include "CuboOLAP.h"
#include "Entrada.h"
#include <iostream>
namespace dw { namespace olap {
void ModuloOLAP::ejecutar(ContextoAplicacion& contexto, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Cubo OLAP ===\nV3: jerarquia temporal; roll-up/drill-down interactivos y filtros aun no disponibles.\n1. Enviar solicitud\n2. Listar ventas\n3. Total global\n4. Mostrar jerarquia temporal\n5. Consultar periodo\n0. Volver\n";
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
        } else if (opcion == 4 || opcion == 5) {
            CuboOLAP cubo;
            std::string error;
            if (!cubo.reconstruir(contexto.hechos, error)) { out << error << '\n'; continue; }
            if (opcion == 4) cubo.mostrarJerarquia(out);
            else {
                int anio, mes, dia;
                if (!leerEntero(in, out, "Anio (ejemplo 2026): ", anio)
                    || !leerEntero(in, out, "Mes (0 para todo el anio, ejemplo 1): ", mes)
                    || !leerEntero(in, out, "Dia (0 para todo el mes, ejemplo 5): ", dia)) return;
                Agregado total;
                if (cubo.consultarPeriodo(anio, mes, dia, total, error))
                    out << "Periodo: " << total.importeCentavos << " centavos; unidades: " << total.unidades << '\n';
                else out << error << '\n';
            }
        } else out << "Opcion no valida.\n";
    }
}
} }
