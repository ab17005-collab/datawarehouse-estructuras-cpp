#include "ModuloOLAP.h"
#include "CuboOLAP.h"
#include "Entrada.h"
#include <iostream>
#include <utility>
namespace dw { namespace olap {
namespace {
void navegar(CuboOLAP& cubo, std::istream& in, std::ostream& out) {
    const char* niveles[] = {"Total", "Anio", "Mes", "Dia"};
    std::string error;
    while (true) {
        VistaTemporal vista;
        if (!cubo.vistaActual(vista, error)) { out << error << '\n'; return; }
        out << "Vista: " << niveles[vista.nivel];
        if (vista.nivel) out << ' ' << vista.valor;
        out << ": " << vista.total.importeCentavos << " centavos; unidades: " << vista.total.unidades << '\n';
        out << "Hijos inmediatos:\n";
        for (const auto& hijo : vista.hijos)
            out << "  " << niveles[vista.nivel+1] << ' ' << hijo.valor << ": "
                << hijo.total.importeCentavos << " centavos; unidades: " << hijo.total.unidades << '\n';
        if (vista.hijos.empty()) out << "  (sin hijos)\n";
        out << "1. Drill-down (bajar a un hijo)\n2. Roll-up (subir al padre)\n3. Mostrar vista\n0. Volver a OLAP\n";
        int opcion;
        if (!leerEntero(in, out, "Navegacion: ", opcion) || opcion == 0) return;
        if (opcion == 1) {
            // En dia no se pide un valor que no puede utilizarse.
            if (vista.nivel == 3) { out << "El dia es el nivel mas detallado.\n"; continue; }
            int valor;
            if (!leerEntero(in, out, "Valor del hijo (ejemplo 2026, 1 o 5 segun el nivel): ", valor)) return;
            if (!cubo.drillDown(valor, error)) out << error << '\n';
        } else if (opcion == 2) {
            if (!cubo.rollUp(error)) out << error << '\n';
        } else if (opcion != 3) out << "Opcion no valida.\n";
    }
}
}
void ModuloOLAP::ejecutar(ContextoAplicacion& contexto, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Cubo OLAP ===\nV6: filtros persistentes y actualizacion tras insercion o CSV. Opcion 3 siempre es total global.\n1. Enviar solicitud\n2. Listar ventas\n3. Total global\n4. Mostrar jerarquia temporal\n5. Consultar periodo\n6. Navegar roll-up/drill-down\n7. Configurar filtros\n8. Total filtrado\n0. Volver\n";
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
        } else if (opcion == 7) {
            FiltroVentas candidato;
            if (!leerTexto(in,out,"Sucursal (Centro, Norte o * para todas): ",candidato.sucursal)
                || !leerTexto(in,out,"Categoria (Alimentos, Limpieza o * para todas): ",candidato.categoria)) return;
            if (candidato.sucursal == "*") candidato.sucursal.clear();
            if (candidato.categoria == "*") candidato.categoria.clear();
            std::string error;
            if (!cubo_.actualizar(contexto.hechos,candidato,error)) out << error << '\n';
            else { filtro_ = std::move(candidato); out << "Filtros aplicados.\n"; }
        } else if (opcion == 4 || opcion == 5 || opcion == 6 || opcion == 8) {
            CuboOLAP& cubo = cubo_;
            std::string error;
            if (!cubo.actualizar(contexto.hechos, filtro_, error)) { out << error << '\n'; continue; }
            out << "Filtro sucursal=" << (filtro_.sucursal.empty() ? "*" : filtro_.sucursal)
                << "; categoria=" << (filtro_.categoria.empty() ? "*" : filtro_.categoria)
                << "; revision=" << contexto.hechos.revision << '\n';
            if (opcion == 8) {
                Agregado total;
                if (cubo.consultarTotalFiltrado(total,error)) out << "Total filtrado: " << total.importeCentavos
                    << " centavos; unidades: " << total.unidades << '\n';
                else out << error << '\n';
            }
            else if (opcion == 4) cubo.mostrarJerarquia(out);
            else if (opcion == 6) { while (cubo.rollUp(error)) {} navegar(cubo, in, out); }
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
