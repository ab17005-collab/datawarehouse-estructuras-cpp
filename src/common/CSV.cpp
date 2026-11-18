#include "CSV.h"
#include <istream>
#include <sstream>
#include <utility>
namespace dw {
namespace {
std::string limpiar(const std::string& texto) {
    const auto inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return {};
    return texto.substr(inicio,texto.find_last_not_of(" \t\r\n")-inicio+1);
}
std::vector<std::string> separar(const std::string& linea) {
    std::vector<std::string> campos;
    std::size_t inicio = 0;
    while (true) {
        const auto coma = linea.find(',',inicio);
        campos.push_back(limpiar(linea.substr(inicio,coma == std::string::npos ? coma : coma-inicio)));
        if (coma == std::string::npos) break;
        inicio = coma+1;
    }
    return campos;
}
}
bool leerCSV(std::istream& in, const std::vector<std::string>& encabezado,
             std::vector<FilaCSV>& resultado, std::string& error) {
    std::vector<FilaCSV> temporal;
    std::string linea;
    std::size_t numero = 0;
    bool cabecera = false;
    while (std::getline(in,linea)) {
        ++numero;
        if (numero == 1 && linea.compare(0,3,"\xEF\xBB\xBF") == 0) linea.erase(0,3);
        if (limpiar(linea).empty()) continue;
        if (linea.find('"') != std::string::npos) {
            error = "Linea " + std::to_string(numero) + ": comillas no admitidas en CSV simple."; return false;
        }
        auto campos = separar(linea);
        if (!cabecera) {
            if (campos != encabezado) { error = "Linea " + std::to_string(numero) + ": encabezado CSV incorrecto."; return false; }
            cabecera = true;
        } else {
            if (campos.size() != encabezado.size()) { error = "Linea " + std::to_string(numero) + ": cantidad de columnas incorrecta."; return false; }
            temporal.push_back({numero,std::move(campos)});
        }
    }
    if (in.bad() || (in.fail() && !in.eof())) { error = "Error de lectura del CSV."; return false; }
    if (!cabecera) { error = "CSV vacio: falta encabezado."; return false; }
    resultado.swap(temporal); error.clear(); return true;
}
bool convertirEntero(const std::string& texto, int& resultado) {
    std::istringstream in(texto); int valor;
    if (!(in >> valor) || !(in >> std::ws).eof()) return false;
    resultado = valor; return true;
}
bool convertirEntero64(const std::string& texto, std::int64_t& resultado) {
    std::istringstream in(texto); std::int64_t valor;
    if (!(in >> valor) || !(in >> std::ws).eof()) return false;
    resultado = valor; return true;
}
bool leerVentasCSV(std::istream& in, std::vector<Venta>& resultado, std::string& error) {
    std::vector<FilaCSV> filas;
    if (!leerCSV(in,{"id","anio","mes","dia","sucursal","categoria","unidades","importe_centavos"},filas,error)) return false;
    TablaHechos temporal;
    for (const auto& fila : filas) {
        const auto& c = fila.campos;
        Venta venta;
        if (!convertirEntero(c[0],venta.id) || !convertirEntero(c[1],venta.fecha.anio)
            || !convertirEntero(c[2],venta.fecha.mes) || !convertirEntero(c[3],venta.fecha.dia)
            || !convertirEntero(c[6],venta.unidades) || !convertirEntero64(c[7],venta.importeCentavos)) {
            error = "Linea " + std::to_string(fila.linea) + ": entero invalido en ventas."; return false;
        }
        venta.sucursal = c[4]; venta.categoria = c[5];
        if (!temporal.insertar(std::move(venta),error)) {
            error = "Linea " + std::to_string(fila.linea) + ": " + error; return false;
        }
    }
    resultado.swap(temporal.ventas); error.clear(); return true;
}
}
