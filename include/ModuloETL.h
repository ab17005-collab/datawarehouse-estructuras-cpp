#pragma once
#include "Modelo.h"
#include "GrafoETL.h"
#include <iosfwd>
namespace dw { namespace etl {
class ModuloETL {
    GrafoETL grafo_;
public:
    void ejecutar(ContextoAplicacion&, std::istream&, std::ostream&);
};
} }
