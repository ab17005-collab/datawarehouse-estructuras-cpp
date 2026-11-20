#include "ModuloBPlus.h"
#include "Entrada.h"
#include <iostream>
#include <limits>
#include <utility>
#include <new>
#include <fstream>
#include "CSV.h"
namespace dw { namespace bplus {
namespace {
void mostrarRango(const ArbolBPlus& indice, const TablaHechos& tabla, std::istream& in, std::ostream& out) {
    int desde, hasta;
    if (!leerEntero(in, out, "Desde (ejemplo 10): ", desde)
        || !leerEntero(in, out, "Hasta inclusive (ejemplo 15): ", hasta)) return;
    std::vector<EntradaIndice> resultado;
    std::string error;
    if (!indice.consultarRango(desde, hasta, resultado, error)) { out << error << '\n'; return; }
    for (const auto& entrada : resultado) {
        if (entrada.posicion >= tabla.ventas.size() || tabla.ventas[entrada.posicion].id != entrada.clave) {
            out << "Referencia de indice inconsistente.\n"; return;
        }
    }
    out << "Rango [" << desde << ',' << hasta << "]: " << resultado.size() << " registros\n";
    for (const auto& entrada : resultado) {
        const auto& venta = tabla.ventas[entrada.posicion];
        out << "  ID " << entrada.clave << "; sucursal: " << venta.sucursal
            << "; importe: " << venta.importeCentavos << " centavos\n";
    }
    if (resultado.empty()) out << "  (sin claves indexadas en el rango)\n";
}
}
void ModuloBPlus::ejecutar(ContextoAplicacion& contexto, std::istream& in, std::ostream& out) {
    int opcion;
    while (true) {
        out << "\n=== Arbol B+ ===\nV6: carga CSV por lote, indice B+ y ventas compartidas.\n1. Enviar solicitud\n2. Indexar venta existente\n3. Buscar clave indexada\n4. Demo independiente con 40 ventas\n5. Mostrar indice actual\n6. Rango del indice actual\n7. Demo de rango con 40 ventas\n8. Insertar venta nueva\n9. Validar indice actual\n10. Importar ventas CSV\n0. Volver\n";
        if (!leerEntero(in, out, "Opcion: ", opcion) || opcion == 0) return;
        if (opcion == 4 || opcion == 7) {
            // Tabla e indice exclusivos de la demostracion: no alteran los datos compartidos.
            TablaHechos tabla;
            ArbolBPlus demo;
            for (int id = 1; id <= 40; ++id) {
                tabla.ventas.push_back({id, {2026,1,1}, "Demo", "Alimentos", 1, 100});
                demo.insertar(id, tabla.ventas.size()-1);
            }
            out << "Demo independiente: 40 ventas de 100 centavos; tabla compartida sin cambios.\n";
            if (opcion == 7) { mostrarRango(demo, tabla, in, out); if (!in) return; continue; }
            demo.mostrar(out);
            for (int id = 1; id <= 40; ++id) {
                std::size_t posicion;
                if (demo.buscar(id, posicion) && posicion < tabla.ventas.size() && tabla.ventas[posicion].id == id)
                    out << "Demo encontrada: " << id << '\n';
                else out << "Error de demostracion: " << id << '\n';
            }
            continue;
        }
        if (opcion == 10) {
            std::string ruta, error;
            if (!leerTexto(in,out,"Archivo de ventas (ejemplo data/ventas.csv): ",ruta)) return;
            std::ifstream archivo(ruta);
            if (!archivo) { out << "No se pudo abrir el archivo de ventas; datos sin cambios.\n"; continue; }
            const auto antes = contexto.hechos.ventas.size();
            try {
                if (importarVentas(archivo,contexto,error)) out << "CSV ventas: " << contexto.hechos.ventas.size()-antes
                    << " ventas agregadas e indexadas; revision: " << contexto.hechos.revision << '\n';
                else out << "Carga rechazada; datos sin cambios. " << error << '\n';
            } catch (const std::bad_alloc&) { out << "Memoria insuficiente; lote sin cambios.\n"; }
            continue;
        }
        if (opcion == 8) {
            Venta venta;
            if (!leerEntero(in,out,"ID nuevo (ejemplo 4): ",venta.id)
                || !leerEntero(in,out,"Anio (ejemplo 2026): ",venta.fecha.anio)
                || !leerEntero(in,out,"Mes (ejemplo 1): ",venta.fecha.mes)
                || !leerEntero(in,out,"Dia (ejemplo 25): ",venta.fecha.dia)
                || !leerTexto(in,out,"Sucursal (ejemplo Norte): ",venta.sucursal)
                || !leerTexto(in,out,"Categoria (ejemplo Limpieza): ",venta.categoria)
                || !leerEntero(in,out,"Unidades (ejemplo 2): ",venta.unidades)
                || !leerEntero64(in,out,"Importe total en centavos (ejemplo 500): ",venta.importeCentavos)) return;
            std::string error;
            try {
                if (insertarVenta(contexto,venta,error)) out << "Venta insertada e indexada: " << venta.id
                    << "; revision: " << contexto.hechos.revision << '\n';
                else out << error << '\n';
            } catch (const std::bad_alloc&) { out << "Memoria insuficiente; venta no insertada.\n"; }
            continue;
        }
        if (opcion == 9) {
            std::string error;
            if (validarIndice(contexto,error)) out << "Indice valido: " << indice_.cantidad()
                << " claves; ventas sin indexar: " << contexto.hechos.ventas.size()-indice_.cantidad() << '\n';
            else out << error << '\n';
            continue;
        }
        if (opcion == 5) { indice_.mostrar(out); continue; }
        if (opcion == 6) { mostrarRango(indice_, contexto.hechos, in, out); if (!in) return; continue; }
        if (opcion < 1 || opcion > 3) { out << "Opcion no valida.\n"; continue; }
        int clave;
        if (!leerEntero(in, out, "Clave positiva: ", clave)) return;
        if (clave <= 0) { out << "La clave debe ser positiva.\n"; continue; }
        if (opcion == 1) { out << "Arbol B+: solicitud recibida [" << clave << "].\n"; continue; }
        std::size_t posicion;
        if (opcion == 2) {
            if (!contexto.hechos.posicionDe(clave, posicion)) { out << "No existe esa venta en la tabla.\n"; continue; }
            const auto resultado = indice_.insertar(clave, posicion);
            if (resultado == ArbolBPlus::Resultado::Insertado) out << "Clave indexada: " << clave << '\n';
            else if (resultado == ArbolBPlus::Resultado::Duplicado) out << "Clave duplicada; indice sin cambios.\n";
            else out << "Capacidad V2: 3 claves; indice sin cambios.\n";
        } else if (!indice_.buscar(clave, posicion)) out << "Clave no indexada.\n";
        else if (posicion >= contexto.hechos.ventas.size() || contexto.hechos.ventas[posicion].id != clave)
            out << "Referencia de indice inconsistente.\n";
        else {
            const auto& v = contexto.hechos.ventas[posicion];
            out << "Venta encontrada: " << v.id << "; sucursal: " << v.sucursal
                << "; importe: " << v.importeCentavos << " centavos\n";
        }
    }
}

bool ModuloBPlus::insertarVenta(ContextoAplicacion& contexto, const Venta& venta, std::string& error) {
    auto& tabla = contexto.hechos;
    if (!tabla.validarInsercion(venta,error)) return false;
    std::size_t posicion;
    if (indice_.buscar(venta.id,posicion)) { error = "ID ya presente en indice; datos sin cambios."; return false; }
    Venta preparada = venta; // Copiar textos antes de publicar cualquier cambio.
    tabla.ventas.reserve(tabla.ventas.size()+1);
    const auto resultado = indice_.insertar(preparada.id,tabla.ventas.size());
    if (resultado != ArbolBPlus::Resultado::Insertado) { error = "Insercion de indice rechazada."; return false; }
    // No hay concurrencia: validacion sigue siendo cierta y ya existe capacidad.
    // Mover Venta (strings estandar) y agregar al vector no requieren nuevas reservas.
    return tabla.insertar(std::move(preparada),error);
}
bool ModuloBPlus::validarIndice(const ContextoAplicacion& contexto, std::string& error) const {
    if (!indice_.validar(error)) return false;
    std::vector<EntradaIndice> entradas;
    if (!indice_.consultarRango(1,std::numeric_limits<int>::max(),entradas,error)) return false;
    for (const auto& entrada : entradas)
        if (entrada.posicion >= contexto.hechos.ventas.size()
            || contexto.hechos.ventas[entrada.posicion].id != entrada.clave) {
            error = "Referencia de indice inconsistente."; return false;
        }
    error.clear(); return true;
}


bool ModuloBPlus::importarVentas(std::istream& in, ContextoAplicacion& contexto, std::string& error) {
    std::vector<Venta> nuevas;
    if (!leerVentasCSV(in,nuevas,error) || !validarIndice(contexto,error)) return false;
    if (nuevas.empty()) { error.clear(); return true; }
    TablaHechos tabla = contexto.hechos;
    for (const auto& venta : nuevas) {
        if (!tabla.insertar(venta,error)) { error = "Venta ID " + std::to_string(venta.id) + ": " + error; return false; }
    }
    ArbolBPlus candidato;
    std::vector<EntradaIndice> actuales;
    if (!indice_.consultarRango(1,std::numeric_limits<int>::max(),actuales,error)) return false;
    for (const auto& entrada : actuales) candidato.insertar(entrada.clave,entrada.posicion);
    const auto inicio = contexto.hechos.ventas.size();
    for (std::size_t i = 0; i < nuevas.size(); ++i) candidato.insertar(nuevas[i].id,inicio+i);
    if (!candidato.validar(error)) return false;
    // Publicacion sin nuevas asignaciones: ambas estructuras candidatas ya son validas.
    indice_.intercambiar(candidato);
    contexto.hechos.ventas.swap(tabla.ventas);
    contexto.hechos.revision = tabla.revision;
    error.clear(); return true;
}

} }
