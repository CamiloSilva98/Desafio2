
#include "foto.h"

// Constructor por defecto
Foto::Foto() : tamanio(0), ruta(""), likes(0) {}

// Constructor con parámetros optimizado usando listas de inicialización
Foto::Foto(const Fecha& fechaFoto, int tamanio, const std::string& ruta, unsigned int likes)
    : fechaFoto(fechaFoto), tamanio(tamanio), ruta(ruta), likes(likes) {}

void Foto::darLike()
{
    likes++;
}
// Resta un like. Como likes es unsigned, restar desde 0 daria un numero enorme,
// por eso se valida antes.
void Foto::quitarLike()
{
    if (likes > 0)
    {
        likes--;
    }
}
//setters y gettes
const Fecha& Foto::getFechaFoto() const { return fechaFoto; }
int Foto::getTamanio() const { return tamanio; }
const std::string& Foto::getRuta() const { return ruta; }
unsigned int Foto::getLikes() const { return likes; }

// Setters
void Foto::setFechaFoto(const Fecha& nuevaFecha) { fechaFoto = nuevaFecha; }
void Foto::setTamanio(int nuevoTamanio) { tamanio = nuevoTamanio; }
void Foto::setRuta(const std::string& nuevaRuta) { ruta = nuevaRuta; }
void Foto::setLikes(unsigned int nuevosLikes) { likes = nuevosLikes; }
