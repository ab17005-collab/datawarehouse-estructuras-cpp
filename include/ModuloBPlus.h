#pragma once
#include "Modelo.h"
#include "ArbolBPlus.h"
#include <iosfwd>
namespace dw { namespace bplus {
class ModuloBPlus {
    ArbolBPlus indice_;
public:
    bool insertarVenta(ContextoAplicacion&, const Venta&, std::string& error);
    bool validarIndice(const ContextoAplicacion&, std::string& error) const;
    const ArbolBPlus& indice() const { return indice_; }
    void ejecutar(ContextoAplicacion&, std::istream&, std::ostream&);
};
} }
