#pragma once
#include "Modelo.h"
#include "ArbolBPlus.h"
#include <iosfwd>
namespace dw { namespace bplus {
class ModuloBPlus {
    ArbolBPlus indice_;
public:
    void ejecutar(ContextoAplicacion&, std::istream&, std::ostream&);
};
} }
