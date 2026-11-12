#pragma once
#include <cstddef>
namespace dw { namespace etl {
// Cola circular propia. A lo sumo 64 indices, cada vertice se encola una vez.
class ColaIndices {
public:
    bool insertar(int valor) {
        if (cantidad_ == 64) return false;
        datos_[(frente_+cantidad_) % 64] = valor;
        ++cantidad_; return true;
    }
    bool remover(int& valor) {
        if (cantidad_ == 0) return false;
        valor = datos_[frente_]; frente_ = (frente_+1) % 64;
        --cantidad_; return true;
    }
private:
    int datos_[64] = {};
    std::size_t frente_ = 0, cantidad_ = 0;
};
} }
