#include "solicitudamistad.h"

SolicitudAmistad::SolicitudAmistad(int indiceEmisor, int indiceReceptor)
    : indiceEmisor(indiceEmisor), indiceReceptor(indiceReceptor) {}

int SolicitudAmistad::getIndiceEmisor() const
{
    return indiceEmisor;
}

int SolicitudAmistad::getIndiceReceptor() const
{
    return indiceReceptor;
}

bool SolicitudAmistad::aceptar(MatrizAmistad& matriz)
{
    // si ya son amigos no se hace nada
    if (matriz.sonAmigos(indiceEmisor, indiceReceptor))
    {
        return false;
    }

    // se prenden las dos casillas de la matriz
    matriz.establecer(indiceEmisor, indiceReceptor);

    // si los indices no eran validos la matriz no cambia y se devuelve false
    return matriz.sonAmigos(indiceEmisor, indiceReceptor);
}
void SolicitudAmistad::setIndiceEmisor(int nuevoIndice){indiceEmisor = nuevoIndice;}
void SolicitudAmistad::setIndiceReceptor(int nuevoIndice){indiceReceptor = nuevoIndice;}
