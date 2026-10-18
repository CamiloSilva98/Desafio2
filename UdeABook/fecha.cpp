#include "fecha.h"
#include <iostream>
#include <iomanip>

std::ostream& operator<<(std::ostream& salida, const Fecha& fecha)
{
    char relleno = salida.fill('0');   // se guarda el relleno anterior para devolverlo despues
    salida << std::setw(2) << fecha.Dia << "/" << std::setw(2) << fecha.Mes << "/" << fecha.Anio
           << " " << std::setw(2) << fecha.Hora << ":" << std::setw(2) << fecha.Minuto;
    salida.fill(relleno);
    return salida;
}

// Constructor parametrizado con inicializadores
Fecha::Fecha(unsigned int dia, unsigned int mes, unsigned int anio, unsigned int hora, unsigned int minuto)
    : Anio(anio), Mes(mes), Dia(dia), Hora(hora), Minuto(minuto) {}
// se empaqueta en formato AAAAMMDDHHMM
unsigned long long Fecha::convertirAEntero() const {
    return Anio * 100000000ULL +
           Mes * 1000000ULL +
           Dia * 10000ULL +
           Hora * 100ULL +
           Minuto;
}


// Sobrecarga del operador de menor que (<)
bool Fecha::operator<(const Fecha& otra) const {
    return this->convertirAEntero() < otra.convertirAEntero();
}
// Sobrecarga del operador de igualdad (==)
bool Fecha ::operator==(const Fecha& otra )const{
    //comprivar si fecha es igual a 'otra'
    return(Anio==otra.Anio && Mes==otra.Mes && Dia==otra.Dia && Hora==otra.Hora&& Minuto==otra.Minuto);
}
//metodos de acceso
unsigned int Fecha::getAnio() const{return Anio; }
unsigned int Fecha::getMes() const{return Mes; }
unsigned int Fecha::getDia() const{return Dia; }
unsigned int Fecha::getHora() const{return Hora; }
unsigned int Fecha::getMinuto() const{return Minuto; }
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
void Fecha::setMinuto(unsigned int nuevoMinuto) {
    // Los minutos van de 0 a 59
    if (nuevoMinuto <= 59) {
        Minuto = nuevoMinuto;
    } else {
        std::cout << "Error: Minuto invalido (" << nuevoMinuto << "). Debe estar entre 0 y 59.\n";
    }
}