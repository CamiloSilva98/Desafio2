
#include "foto.h"

// Constructor por defecto
Foto::Foto() : size(0), ruta(""), likes(0) {
    // La fecha se inicializa sola con su propio constructor por defecto
}

// Constructor con parámetros optimizado usando listas de inicialización
Foto::Foto(const Fecha& Date, int Size, const std::string& Ruta, unsigned int Likes)
    : date(Date), size(Size), ruta(Ruta), likes(Likes) {
}
void Foto::quitarlike() {
    if (likes > 0) { // Validación lógica de seguridad para evitar que un unsigned int desborde bajo 0
        likes--;
    }
}
//setters y gettes
int Foto::getsize() const { return size; }
unsigned int Foto::getlikes() const { return likes; }
const std::string& Foto::getruta() const { return ruta; }
const Fecha& Foto::getDate() const { return date; }

// Implementación de Setters corregidos
void Foto::setsize(int nuevoSize) {
    size = nuevoSize;
}

void Foto::setlikes(unsigned int nuevolike) {
    likes = nuevolike;
}

void Foto::setruta(const std::string& nuevaRuta) {
    ruta = nuevaRuta; // Ahora sí asigna un string a un string
}

void Foto::setDate(const Fecha& nuevaFecha) {
    date = nuevaFecha; // Asignación limpia utilizando el operador de copia por defecto de Fecha
}