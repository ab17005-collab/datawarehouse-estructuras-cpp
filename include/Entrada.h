#ifndef DW_ENTRADA_H
#define DW_ENTRADA_H
#include <iosfwd>
#include <string>
#include <cstdint>
namespace dw {
bool leerEntero(std::istream&, std::ostream&, const std::string&, int&);
bool leerEntero64(std::istream&, std::ostream&, const std::string&, std::int64_t&);
bool leerTexto(std::istream&, std::ostream&, const std::string&, std::string&);
}
#endif
