#include "foto.h"

// Constructor por defecto
Foto::Foto() : tamanio(0), ruta(""), likes(0), ultimoUsuarioLike(-1) {}

// Constructor con parámetros optimizado usando listas de inicialización
Foto::Foto(const Fecha& fechaFoto, int tamanio, const std::string& ruta, unsigned int likes)
    : fechaFoto(fechaFoto), tamanio(tamanio), ruta(ruta), likes(likes), ultimoUsuarioLike(-1) {}

void Foto::darLike()
{
    likes++;
}
void Foto::quitarLike()
{
    if (likes > 0)
    {
        likes--;
    }
}
// Da el like y recuerda quien fue. Solo se guarda UN usuario por foto (el ultimo): es lo minimo
// para que no pueda sumar likes repetidos mientras el programa siga abierto, sin crear listas
// nuevas ni cambiar los .txt.
void Foto::darLike(int indiceUsuario)
{
    likes++;
    ultimoUsuarioLike = indiceUsuario;
}
// Solo quita el like si es de ese usuario; asi nunca se borra el like de otra persona
void Foto::quitarLike(int indiceUsuario)
{
    if (ultimoUsuarioLike == indiceUsuario && likes > 0)
    {
        likes--;
        ultimoUsuarioLike = -1;
    }
}
bool Foto::leDioLike(int indiceUsuario) const
{
    return ultimoUsuarioLike == indiceUsuario;
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

