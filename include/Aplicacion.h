#ifndef DW_APLICACION_H
#define DW_APLICACION_H
#include "Modelo.h"
#include "Entrada.h"
#include "ModuloOLAP.h"
#include "ModuloBPlus.h"
#include "ModuloETL.h"
#include <iostream>
namespace dw {
inline int ejecutarAplicacion(std::istream& in, std::ostream& out) {
    ContextoAplicacion contexto;
    contexto.hechos = crearDatosEjemplo();
    olap::ModuloOLAP olap;
    bplus::ModuloBPlus bplus;
    etl::ModuloETL etl;
    int opcion;
    while (true) {
        out << "\n=== DATA WAREHOUSE V2 ===\n1. Cubo OLAP\n2. Arbol B+\n3. Grafo ETL\n0. Salir\n";
        if (!leerEntero(in, out, "Opcion: ", opcion)) break;
        if (opcion == 0) break;
        switch (opcion) {
            case 1: olap.ejecutar(contexto, in, out); break;
            case 2: bplus.ejecutar(contexto, in, out); break;
            case 3: etl.ejecutar(contexto, in, out); break;
            default: out << "Opcion no valida.\n";
        }
        if (!in) break;
    }
    out << "Hasta luego.\n";
    return 0;
}
}
#endif
