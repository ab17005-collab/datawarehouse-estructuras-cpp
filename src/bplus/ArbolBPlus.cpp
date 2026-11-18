#include "ArbolBPlus.h"
#include <ostream>
#include <string>
#include <algorithm>
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
ArbolBPlus::Nodo* ArbolBPlus::insertarEn(Nodo* nodo, int clave, std::size_t posicion, int& separador, std::vector<Nodo*>& reserva) {
    int lugar = 0;
    if (nodo->hoja) {
        while (lugar < nodo->cantidad && nodo->claves[lugar] < clave) ++lugar;
        for (int i = nodo->cantidad; i > lugar; --i) {
            nodo->claves[i] = nodo->claves[i-1]; nodo->posiciones[i] = nodo->posiciones[i-1];
        }
        nodo->claves[lugar] = clave; nodo->posiciones[lugar] = posicion;
        ++nodo->cantidad;
        if (nodo->cantidad <= 3) return nullptr;
        Nodo* derecho = tomarNodo(reserva, true);
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
    Nodo* nuevo = insertarEn(nodo->hijos[lugar], clave, posicion, promovida, reserva);
    if (!nuevo) return nullptr;
    for (int i = nodo->cantidad; i > lugar; --i) nodo->claves[i] = nodo->claves[i-1];
    for (int i = nodo->cantidad+1; i > lugar+1; --i) nodo->hijos[i] = nodo->hijos[i-1];
    nodo->claves[lugar] = promovida; nodo->hijos[lugar+1] = nuevo;
    ++nodo->cantidad;
    if (nodo->cantidad <= 3) return nullptr;
    Nodo* derecho = tomarNodo(reserva, false);
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
    std::vector<Nodo*> reserva;
    const int limite = altura()+2; // Como maximo una division por nivel y una nueva raiz.
    reserva.reserve(static_cast<std::size_t>(limite));
    try {
        for (int i = 0; i < limite; ++i) reserva.push_back(new Nodo(false));
    } catch (...) {
        for (Nodo* nodo : reserva) delete nodo;
        throw; // El arbol aun no fue modificado.
    }
    if (!raiz_) raiz_ = tomarNodo(reserva,true);
    int separador = 0;
    Nodo* derecho = insertarEn(raiz_, clave, posicion, separador, reserva);
    if (derecho) {
        Nodo* nuevaRaiz = tomarNodo(reserva,false);
        nuevaRaiz->cantidad = 1; nuevaRaiz->claves[0] = separador;
        nuevaRaiz->hijos[0] = raiz_; nuevaRaiz->hijos[1] = derecho;
        raiz_ = nuevaRaiz;
    }
    for (Nodo* nodo : reserva) delete nodo;
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

bool ArbolBPlus::consultarRango(int desde, int hasta, std::vector<EntradaIndice>& resultado, std::string& error) const {
    if (desde <= 0 || hasta <= 0 || desde > hasta) { error = "Rango invalido: use claves positivas y desde <= hasta."; return false; }
    const Nodo* hoja = raiz_;
    while (hoja && !hoja->hoja) {
        int i = 0;
        while (i < hoja->cantidad && desde >= hoja->claves[i]) ++i;
        hoja = hoja->hijos[i];
    }
    std::vector<EntradaIndice> temporal;
    bool terminado = false;
    while (hoja && !terminado) {
        for (int i = 0; i < hoja->cantidad; ++i) {
            const int clave = hoja->claves[i];
            if (clave > hasta) { terminado = true; break; }
            if (clave >= desde) temporal.push_back({clave, hoja->posiciones[i]});
        }
        hoja = hoja->siguiente;
    }
    resultado.swap(temporal);
    error.clear();
    return true;
}


ArbolBPlus::Nodo* ArbolBPlus::tomarNodo(std::vector<Nodo*>& reserva, bool hoja) {
    Nodo* nodo = reserva.back(); reserva.pop_back(); nodo->hoja = hoja;
    return nodo;
}
bool ArbolBPlus::validarNodo(const Nodo* nodo, bool raiz, int nivel, int& nivelHoja,
    std::vector<const Nodo*>& visitados, std::vector<const Nodo*>& hojas,
    int& minimo, int& maximo, std::size_t& registros, std::string& error) const {
    if (!nodo || std::find(visitados.begin(),visitados.end(),nodo) != visitados.end()) {
        error = "Hijo nulo, compartido o ciclo interno."; return false;
    }
    visitados.push_back(nodo);
    const int minimoClaves = raiz ? (nodo->hoja ? 0 : 1) : (nodo->hoja ? 2 : 1);
    if (nodo->cantidad < minimoClaves || nodo->cantidad > 3) { error = "Ocupacion invalida."; return false; }
    for (int i = 0; i < nodo->cantidad; ++i)
        if (nodo->claves[i] <= 0 || (i && nodo->claves[i-1] >= nodo->claves[i])) { error = "Claves sin orden estricto."; return false; }
    if (nodo->hoja) {
        for (const Nodo* hijo : nodo->hijos) if (hijo) { error = "Hoja con hijos."; return false; }
        if (nivelHoja == -1) nivelHoja = nivel;
        if (nivel != nivelHoja) { error = "Hojas a distinta profundidad."; return false; }
        hojas.push_back(nodo);
        if (nodo->cantidad) { minimo = nodo->claves[0]; maximo = nodo->claves[nodo->cantidad-1]; }
        registros = static_cast<std::size_t>(nodo->cantidad);
        return true;
    }
    if (nodo->siguiente) { error = "Nodo interno con enlace de hoja."; return false; }
    for (int i = nodo->cantidad+1; i < 5; ++i)
        if (nodo->hijos[i]) { error = "Cantidad de hijos invalida."; return false; }
    registros = 0;
    int anteriorMaximo = 0;
    for (int i = 0; i <= nodo->cantidad; ++i) {
        int hijoMinimo = 0, hijoMaximo = 0;
        std::size_t cuenta = 0;
        if (!validarNodo(nodo->hijos[i],false,nivel+1,nivelHoja,visitados,hojas,hijoMinimo,hijoMaximo,cuenta,error)) return false;
        if (i == 0) minimo = hijoMinimo;
        else if (nodo->claves[i-1] != hijoMinimo || anteriorMaximo >= hijoMinimo) {
            error = "Separador o limites de subarbol invalidos."; return false;
        }
        anteriorMaximo = hijoMaximo; maximo = hijoMaximo; registros += cuenta;
    }
    return true;
}
bool ArbolBPlus::validar(std::string& error) const {
    if (!raiz_) {
        if (cantidad_) { error = "Contador sin raiz."; return false; }
        error.clear(); return true;
    }
    std::vector<const Nodo*> visitados, hojas;
    int nivelHoja = -1, minimo = 0, maximo = 0;
    std::size_t registros = 0;
    if (!validarNodo(raiz_,true,0,nivelHoja,visitados,hojas,minimo,maximo,registros,error)) return false;
    if (registros != cantidad_) { error = "Contador de registros inconsistente."; return false; }
    for (std::size_t i = 0; i < hojas.size(); ++i) {
        const Nodo* esperado = i+1 < hojas.size() ? hojas[i+1] : nullptr;
        if (hojas[i]->siguiente != esperado) { error = "Cadena de hojas inconsistente."; return false; }
    }
    error.clear();
    return true;
}

} }
