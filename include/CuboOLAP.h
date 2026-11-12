#pragma once
#include "Modelo.h"
namespace dw { namespace olap {
struct Agregado { std::int64_t unidades = 0; std::int64_t importeCentavos = 0; };
class CuboOLAP {
public:
    // V2: agregado global; jerarquia temporal a partir de V3.
    // Si hay error, no modifica resultado.
    bool consultarTotal(const TablaHechos&, Agregado&, std::string& error) const;
};
} }
