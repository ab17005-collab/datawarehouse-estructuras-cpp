#pragma once
#include "Modelo.h"
#include <iosfwd>
namespace dw { namespace bplus {
class ModuloBPlus {
public:
    void ejecutar(ContextoAplicacion&, std::istream&, std::ostream&);
};
} }
