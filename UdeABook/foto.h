#ifndef FOTO_H
#define FOTO_H
#include "fecha.h"
#include <string>
using namespace std;

class Foto
{
private:
    Fecha fechaFoto;        // fecha de captura
    int tamanio;            // tamano de la foto
    std::string ruta;       // ruta absoluta del .png en el almacenamiento
    unsigned int likes;     // unsigned: un conteo de likes nunca es negativo

public:
    // Constructores
    Foto();
    Foto(const Fecha& fechaFoto, int tamanio, const std::string& ruta, unsigned int likes);
    // Operaciones del diagrama
    void darLike();     // suma 1 al conteo
    void quitarLike();  // resta 1, pero nunca baja de 0
    // Getters: devuelven por referencia constante para no copiar objetos grandes
    const Fecha& getFechaFoto() const;
    int getTamanio() const;
    const std::string& getRuta() const;
    unsigned int getLikes() const;

    // Setters
    void setFechaFoto(const Fecha& nuevaFecha);
    void setTamanio(int nuevoTamanio);
    void setRuta(const std::string& nuevaRuta);
    void setLikes(unsigned int nuevosLikes);
};

#endif // FOTO_H
