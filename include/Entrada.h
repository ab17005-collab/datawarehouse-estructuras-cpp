#ifndef DW_ENTRADA_H
#define DW_ENTRADA_H
#include <iosfwd>
#include <string>
namespace dw {
bool leerEntero(std::istream&, std::ostream&, const std::string&, int&);
bool leerTexto(std::istream&, std::ostream&, const std::string&, std::string&);
}
#endif
