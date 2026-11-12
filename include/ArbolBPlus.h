#pragma once
#include <cstddef>
namespace dw { namespace bplus {
class ArbolBPlus {
public:
    enum class Resultado { Insertado, Duplicado, CapacidadV2, ClaveInvalida };
    ArbolBPlus() = default;
    ArbolBPlus(const ArbolBPlus&) = delete;
    ArbolBPlus& operator=(const ArbolBPlus&) = delete;
    Resultado insertar(int clave, std::size_t posicion);
    bool buscar(int clave, std::size_t& posicion) const;
    std::size_t cantidad() const { return raiz_.cantidad; }
private:
    // V2: la raiz es la unica hoja. Orden 4, maximo 3 claves.
    // No se aceptan inserciones que requieran una division hasta V3.
    struct Nodo {
        int claves[3] = {};
        std::size_t posiciones[3] = {};
        std::size_t cantidad = 0;
        Nodo* siguiente = nullptr;
    };
    Nodo raiz_;
};
} }
