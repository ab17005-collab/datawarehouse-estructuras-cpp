#pragma once
#include "Modelo.h"
#include <iosfwd>
namespace dw { namespace etl {
class ModuloETL {
public:
    void ejecutar(ContextoAplicacion&, std::istream&, std::ostream&);
};
} }
