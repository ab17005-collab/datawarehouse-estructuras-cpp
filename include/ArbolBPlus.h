#pragma once
#include <cstddef>
#include <iosfwd>
#include <vector>
#include <string>
namespace dw { namespace bplus {
struct EntradaIndice { int clave; std::size_t posicion; };
class ArbolBPlus {
public:
    // CapacidadV2 se conserva por compatibilidad; V3 ya no retorna este estado.
    enum class Resultado { Insertado, Duplicado, CapacidadV2, ClaveInvalida };
    ArbolBPlus() = default;
    ~ArbolBPlus();
    ArbolBPlus(const ArbolBPlus&) = delete;
    ArbolBPlus& operator=(const ArbolBPlus&) = delete;
    Resultado insertar(int clave, std::size_t posicion);
    bool buscar(int clave, std::size_t& posicion) const;
    std::size_t cantidad() const { return cantidad_; }
    int altura() const;
    bool validar(std::string& error) const;
    void mostrar(std::ostream&) const;
    bool consultarRango(int desde, int hasta, std::vector<EntradaIndice>& resultado, std::string& error) const;
private:
    struct Nodo {
        bool hoja;
        int cantidad = 0;
        int claves[4] = {}; // Una celda extra durante desbordamiento.
        std::size_t posiciones[4] = {};
        Nodo* hijos[5] = {};
        Nodo* siguiente = nullptr;
        explicit Nodo(bool h) : hoja(h) {}
    };
    Nodo* raiz_ = nullptr;
    std::size_t cantidad_ = 0;
    static void liberar(Nodo*);
    // Devuelve hermano derecho al dividir y entrega separador para el padre.
    static Nodo* insertarEn(Nodo*, int clave, std::size_t posicion, int& separador, std::vector<Nodo*>& reserva);
    static Nodo* tomarNodo(std::vector<Nodo*>&, bool hoja);
    bool validarNodo(const Nodo*, bool raiz, int nivel, int& nivelHoja,
        std::vector<const Nodo*>& visitados, std::vector<const Nodo*>& hojas,
        int& minimo, int& maximo, std::size_t& registros, std::string& error) const;
    static void mostrarNodo(const Nodo*, int nivel, std::ostream&);
};
} }
