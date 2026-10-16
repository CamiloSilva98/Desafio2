#ifndef ALBUM_H
#define ALBUM_H

#include "foto.h"
#include "listadinamica.h"
#include <string>

class Album
{
public:
    enum Privacidad {PUBLICO = 0, SOLO_AMIGOS = 1, PRIVADO = 2};
    //creamos un tipo de dato privacidad simple (4bytes) q nos entregara 0,1,2 dependiendo del dato seleccionado
private:
    std::string nombre;
    std::string ruta;
    Privacidad privacidad;
    ListaDinamica<Foto> fotos; //fotos del album
public:
    //constructor por defecto.
    Album();
    Album(const std::string& nombre, const std::string& ruta, Privacidad privacidad);

    //agregar foto al final del album
    void agregarFoto(const Foto& foto);
    // Carrusel: muestra una foto a la vez y deja ir a la siguiente/previa de forma circular
    // dar/quitar like, o salir. Devuelve las iteraciones usadas
    // IMPORTANTE: debe llamarse sobre el album original (no sobre una copia),
    // porque los likes se guardan en las fotos de este objeto.
    long verCarrusel();

    //TRUE si un amigo del dueño puede ver el album
    bool esVsisibleParaAmigo() const;

    //GETTERS
    const std::string& getNombre() const;
    const std::string& getRuta() const;
    Privacidad getPrivacidad() const;
    int getCantidadFotos() const;
    const Foto& getFoto(int indice) const;   // sirve para guardar los likes en el archivo

    // Bytes que ocupa el album (objeto + arreglo de fotos + textos), para MedidorRecursos.
    long memoriaBytes() const;
};

#endif // ALBUM_H
