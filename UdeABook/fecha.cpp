#include "fecha.h"
#include <iostream>
// Constructor parametrizado con inicializadores
Fecha::Fecha(unsigned int dia, unsigned int mes, unsigned int anio, unsigned int hora)
    : Anio(anio), Mes(mes), Dia(dia), Hora(hora) {}
// se empaqueta en formato AAAAMMDDHH
unsigned long long Fecha::convertirAEntero() const {
    return (static_cast<unsigned long long>(Anio) * 1000000) +
           (Mes * 10000) +
           (Dia * 100) +
           Hora;
}

// Sobrecarga del operador de menor que (<)
bool Fecha::operator<(const Fecha& otra) const {
    return this->convertirAEntero() < otra.convertirAEntero();
}
// Sobrecarga del operador de igualdad (==)
bool Fecha ::operator==(const Fecha& otra )const{
    //comprivar si fecha es igual a 'otra'
    return(Anio==otra.Anio && Mes==otra.Mes && Dia==otra.Dia && Hora==otra.Hora );
}
//metodos de acceso
unsigned int Fecha::getAnio() const{return Anio; }
unsigned int Fecha::getMes() const{return Mes; }
unsigned int Fecha::getDia() const{return Dia; }
unsigned int Fecha::getHora() const{return Hora; }
void Fecha::setDia(unsigned int nuevoDia) {
    // Validación básica: los días deben estar entre 1 y 31
    if (nuevoDia >= 1 && nuevoDia <= 31) {
        Dia = nuevoDia;
    } else {
        std::cout << "Error: Dia invalido (" << nuevoDia << "). No se realizo el cambio.\n";
    }
}

void Fecha::setMes(unsigned int nuevoMes) {
    // Los meses del año van estrictamente del 1 al 12
    if (nuevoMes >= 1 && nuevoMes <= 12) {
        Mes = nuevoMes;
    } else {
        std::cout << "Error: Mes invalido (" << nuevoMes << "). No se realizo el cambio.\n";
    }
}

void Fecha::setAnio(unsigned int nuevoAnio) {
    // Para el año usualmente no hay restricción superior, solo que sea un año lógico
    if (nuevoAnio > 0) {
        Anio = nuevoAnio;
    } else {
        std::cout << "Error: Anio invalido. No se realizo el cambio.\n";
    }
}

void Fecha::setHora(unsigned int nuevaHora) {
    // El formato de hora militar va de 0 a 23
    if (nuevaHora <= 23) {
        Hora = nuevaHora;
    } else {
        std::cout << "Error: Hora invalida (" << nuevaHora << "). Debe estar entre 0 y 23.\n";
    }
}
