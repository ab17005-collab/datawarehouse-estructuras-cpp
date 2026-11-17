#pragma once
#include "Modelo.h"
#include "CuboOLAP.h"
#include <iosfwd>
namespace dw { namespace olap {
class ModuloOLAP {
    CuboOLAP cubo_;
    FiltroVentas filtro_;
public:
    void ejecutar(ContextoAplicacion&, std::istream&, std::ostream&);
};
} }
