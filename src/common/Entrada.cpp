#include "Entrada.h"
#include <iostream>
#include <sstream>
namespace dw {
bool leerEntero(std::istream& in, std::ostream& out, const std::string& mensaje, int& valor) {
    std::string linea;
    while (true) {
        out << mensaje;
        if (!std::getline(in, linea)) return false;
        std::istringstream parser(linea);
        int candidato;
        if ((parser >> candidato) && (parser >> std::ws).eof()) {
            valor = candidato;
            return true;
        }
        out << "Entrada invalida: ingrese un entero.\n";
    }
}
bool leerTexto(std::istream& in, std::ostream& out, const std::string& mensaje, std::string& texto) {
    while (true) {
        out << mensaje;
        if (!std::getline(in, texto)) return false;
        const auto inicio = texto.find_first_not_of(" \t\r");
        if (inicio != std::string::npos) {
            texto = texto.substr(inicio, texto.find_last_not_of(" \t\r") - inicio + 1);
            return true;
        }
        out << "Entrada invalida: el texto no puede estar vacio.\n";
    }
}
}
