#include "ArbolBPlus.h"
#include <ostream>
#include <string>
namespace dw { namespace bplus {
ArbolBPlus::~ArbolBPlus() { liberar(raiz_); }
void ArbolBPlus::liberar(Nodo* nodo) {
    if (!nodo) return;
    if (!nodo->hoja) for (int i = 0; i <= nodo->cantidad; ++i) liberar(nodo->hijos[i]);
    delete nodo; // siguiente no es propietario: no se libera por el enlace lateral.
}
bool ArbolBPlus::buscar(int clave, std::size_t& posicion) const {
    const Nodo* nodo = raiz_;
    while (nodo && !nodo->hoja) {
        int i = 0;
        while (i < nodo->cantidad && clave >= nodo->claves[i]) ++i;
        nodo = nodo->hijos[i];
    }
    if (!nodo) return false;
    for (int i = 0; i < nodo->cantidad; ++i) {
        if (nodo->claves[i] == clave) { posicion = nodo->posiciones[i]; return true; }
        if (nodo->claves[i] > clave) break;
    }
    return false;
}
ArbolBPlus::Nodo* ArbolBPlus::insertarEn(Nodo* nodo, int clave, std::size_t posicion, int& separador) {
    int lugar = 0;
    if (nodo->hoja) {
        while (lugar < nodo->cantidad && nodo->claves[lugar] < clave) ++lugar;
        for (int i = nodo->cantidad; i > lugar; --i) {
            nodo->claves[i] = nodo->claves[i-1]; nodo->posiciones[i] = nodo->posiciones[i-1];
        }
        nodo->claves[lugar] = clave; nodo->posiciones[lugar] = posicion;
        ++nodo->cantidad;
        if (nodo->cantidad <= 3) return nullptr;
        Nodo* derecho = new Nodo(true);
        derecho->cantidad = 2;
        for (int i = 0; i < 2; ++i) {
            derecho->claves[i] = nodo->claves[i+2]; derecho->posiciones[i] = nodo->posiciones[i+2];
        }
        nodo->cantidad = 2;
        derecho->siguiente = nodo->siguiente; nodo->siguiente = derecho;
        separador = derecho->claves[0]; // Copiar, no retirar: el registro sigue en la hoja.
        return derecho;
    }
    while (lugar < nodo->cantidad && clave >= nodo->claves[lugar]) ++lugar;
    int promovida = 0;
    Nodo* nuevo = insertarEn(nodo->hijos[lugar], clave, posicion, promovida);
    if (!nuevo) return nullptr;
    for (int i = nodo->cantidad; i > lugar; --i) nodo->claves[i] = nodo->claves[i-1];
    for (int i = nodo->cantidad+1; i > lugar+1; --i) nodo->hijos[i] = nodo->hijos[i-1];
    nodo->claves[lugar] = promovida; nodo->hijos[lugar+1] = nuevo;
    ++nodo->cantidad;
    if (nodo->cantidad <= 3) return nullptr;
    Nodo* derecho = new Nodo(false);
    separador = nodo->claves[2]; // En internos se retira el separador promovido.
    derecho->cantidad = 1; derecho->claves[0] = nodo->claves[3];
    derecho->hijos[0] = nodo->hijos[3]; derecho->hijos[1] = nodo->hijos[4];
    nodo->cantidad = 2;
    nodo->hijos[3] = nodo->hijos[4] = nullptr;
    return derecho;
}
ArbolBPlus::Resultado ArbolBPlus::insertar(int clave, std::size_t posicion) {
    if (clave <= 0) return Resultado::ClaveInvalida;
    std::size_t anterior;
    if (buscar(clave, anterior)) return Resultado::Duplicado;
    if (!raiz_) raiz_ = new Nodo(true);
    int separador = 0;
    Nodo* derecho = insertarEn(raiz_, clave, posicion, separador);
    if (derecho) {
        Nodo* nuevaRaiz = new Nodo(false);
        nuevaRaiz->cantidad = 1; nuevaRaiz->claves[0] = separador;
        nuevaRaiz->hijos[0] = raiz_; nuevaRaiz->hijos[1] = derecho;
        raiz_ = nuevaRaiz;
    }
    ++cantidad_;
    return Resultado::Insertado;
}
int ArbolBPlus::altura() const {
    int niveles = 0;
    for (const Nodo* nodo = raiz_; nodo; nodo = nodo->hoja ? nullptr : nodo->hijos[0]) ++niveles;
    return niveles; // Vacio=0, una hoja=1.
}
void ArbolBPlus::mostrarNodo(const Nodo* nodo, int nivel, std::ostream& out) {
    out << std::string(static_cast<std::size_t>(nivel)*2, ' ') << (nodo->hoja ? "Hoja:" : "Interno:");
    for (int i = 0; i < nodo->cantidad; ++i) out << ' ' << nodo->claves[i];
    out << '\n';
    if (!nodo->hoja) for (int i = 0; i <= nodo->cantidad; ++i) mostrarNodo(nodo->hijos[i], nivel+1, out);
}
void ArbolBPlus::mostrar(std::ostream& out) const {
    out << "Claves: " << cantidad_ << "; niveles: " << altura() << '\n';
    if (raiz_) mostrarNodo(raiz_, 0, out);
}
} }
