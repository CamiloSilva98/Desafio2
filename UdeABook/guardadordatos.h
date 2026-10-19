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
    // Los cuatro devuelven false si no se pudo escribir el archivo.
    // "iteraciones" se SUMA a lo que traiga (vueltas de los ciclos) y "bytesTemporales" queda en el
    // mayor valor visto de memoria temporal usada para armar el texto (para medidorrecursos).
    static bool guardarAmistades(const std::string& archivo, const UdeaBook& plataforma, long& iteraciones, long& bytesTemporales);
    static bool guardarSolicitudes(const std::string& archivo, const UdeaBook& plataforma, long& iteraciones, long& bytesTemporales);
    static bool guardarMensajes(const std::string& archivo, const UdeaBook& plataforma, long& iteraciones, long& bytesTemporales);
    static bool guardarFotos(const std::string& archivo, const UdeaBook& plataforma, long& iteraciones, long& bytesTemporales);

private:
    // Escribe el contenido completo en el archivo; false si no se pudo escribir
    static bool escribirArchivo(const std::string& archivo, const std::string& contenido);
};

#endif // GUARDADORDATOS_H