#include "ArbolBPlus.h"
namespace dw { namespace bplus {
ArbolBPlus::Resultado ArbolBPlus::insertar(int clave, std::size_t posicion) {
    if (clave <= 0) return Resultado::ClaveInvalida;
    std::size_t lugar = 0;
    while (lugar < raiz_.cantidad && raiz_.claves[lugar] < clave) ++lugar;
    if (lugar < raiz_.cantidad && raiz_.claves[lugar] == clave) return Resultado::Duplicado;
    if (raiz_.cantidad == 3) return Resultado::CapacidadV2;
    for (std::size_t i = raiz_.cantidad; i > lugar; --i) {
        raiz_.claves[i] = raiz_.claves[i-1];
        raiz_.posiciones[i] = raiz_.posiciones[i-1];
    }
    raiz_.claves[lugar] = clave;
    raiz_.posiciones[lugar] = posicion;
    ++raiz_.cantidad;
    return Resultado::Insertado;
}
bool ArbolBPlus::buscar(int clave, std::size_t& posicion) const {
    for (std::size_t i = 0; i < raiz_.cantidad; ++i) {
        if (raiz_.claves[i] == clave) { posicion = raiz_.posiciones[i]; return true; }
        if (raiz_.claves[i] > clave) break;
    }
    return false;
}
} }
