// guardadordatos.h
#ifndef GUARDADORDATOS_H
#define GUARDADORDATOS_H

#include <string>

class UdeaBook;

// Escribe amistades.txt, solicitudes.txt, mensajes.txt y fotos.txt con el mismo formato que leen
// UdeaBook::cargarDatos y CargadorDatos. usuarios.txt y albums.txt no cambian en esta version.
class GuardadorDatos
{
public:
    // Los cuatro devuelven false si no se pudo escribir el archivo
    static bool guardarAmistades(const std::string& archivo, const UdeaBook& plataforma);
    static bool guardarSolicitudes(const std::string& archivo, const UdeaBook& plataforma);
    static bool guardarMensajes(const std::string& archivo, const UdeaBook& plataforma);
    static bool guardarFotos(const std::string& archivo, const UdeaBook& plataforma);

private:
    // Escribe el contenido completo en el archivo; false si no se pudo escribir
    static bool escribirArchivo(const std::string& archivo, const std::string& contenido);
};

#endif // GUARDADORDATOS_H