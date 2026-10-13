#ifndef FOTO_H
#define FOTO_H
#include "fecha.h"
#include <string>
using namespace std;

class Foto
{
private:
    Fecha date;
    int size;
    string ruta;
    unsigned int likes;
public:
    // Constructores
    Foto();
    Foto(const Fecha& date, int size, const std::string& ruta, unsigned int likes);

    //metodos
    void darlike();
    void quitarlike();
    //setters y gettes
    int getsize() const;
    unsigned int getlikes() const;
    const std::string& getruta() const;
    const Fecha& getDate() const;

    // Setters corregidos con los tipos correspondientes
    void setsize(int nuevoSize);
    void setlikes(unsigned int nuevolike); // Corregido a unsigned int para coincidir con el atributo
    void setruta(const std::string& nuevaRuta); // Corregido de int a const std::string&
    void setDate(const Fecha& nuevaFecha); // Reemplaza la fecha completa respetando el encapsulamiento de Fecha
    //destrucctores
};

#endif // FOTO_H
