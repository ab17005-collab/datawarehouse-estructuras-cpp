#pragma once
#include "Modelo.h"
#include <iosfwd>
namespace dw { namespace olap {
class ModuloOLAP {
public:
    void ejecutar(ContextoAplicacion&, std::istream&, std::ostream&);
};
} }
