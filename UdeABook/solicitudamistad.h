#ifndef SOLICITUDAMISTAD_H
#define SOLICITUDAMISTAD_H

#include "MatrizAmistad.h"

// solicitud pendiente entre dos usuarios; guarda la posicion de cada uno
// en el arreglo de usuarios (la misma que usa la matriz de amistad)
class SolicitudAmistad
{
private:
    int indiceEmisor;
    int indiceReceptor;

public:
    SolicitudAmistad(int indiceEmisor, int indiceReceptor);

    int getIndiceEmisor() const;
    int getIndiceReceptor() const;

    // crea la amistad en la matriz; devuelve false si ya eran amigos o los indices no sirven
    bool aceptar(MatrizAmistad& matriz);
};

#endif