#pragma once
#include "Modelo.h"
#include <iosfwd>
namespace dw {
struct FilaCSV { std::size_t linea = 0; std::vector<std::string> campos; };
// CSV simple: coma, encabezado exacto, sin comillas ni campos multilinea.
// Acepta CRLF, BOM UTF-8 y lineas vacias. Error conserva resultado previo.
bool leerCSV(std::istream&, const std::vector<std::string>& encabezado,
             std::vector<FilaCSV>& resultado, std::string& error);
bool convertirEntero(const std::string&, int&);
bool convertirEntero64(const std::string&, std::int64_t&);
bool leerVentasCSV(std::istream&, std::vector<Venta>& resultado, std::string& error);
}
