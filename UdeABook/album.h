#ifndef ALBUM_H
#define ALBUM_H

#include "foto.h"
#include "listadinamica.h"
#include <string>

class Album
{
public:
    enum Privacidad {PUBLICO, PRIVADO,SOLO_AMIGOS};
    //creamos un tipo de dato privacidad simple (4bytes) q nos entregara 0,1,2 dependiendo del dato seleccionado
private:
    std::string nombre;
    std::string ruta;
    Privacidad Privacidad;
    ListaDinamica<Foto> fotos; //fotos del album
public:

    Album();
};

#endif // ALBUM_H
