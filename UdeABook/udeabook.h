#ifndef UDEABOOK_H
#define UDEABOOK_H

#include <string>
#include "ListaDinamica.h"
#include "MatrizAmistad.h"
#include "Usuario.h"

// clase principal: carga los datos, busca usuarios e inicia sesion
class UdeaBook
{
private:
    // ordenados por nombre; la posicion de cada uno es su indice en la matriz
    ListaDinamica<Usuario*> usuarios;
    MatrizAmistad* matriz;
    std::string subRuta;

    // la plataforma es duena de los usuarios y de la matriz, no se copia
    UdeaBook(const UdeaBook&) = delete;
    UdeaBook& operator=(const UdeaBook&) = delete;

    void liberar();
    bool cargarConfig(const std::string& ruta);
    bool cargarUsuarios(const std::string& ruta);
    bool cargarAmistades(const std::string& ruta);
    bool cargarSolicitudes(const std::string& ruta);

public:
    UdeaBook();
    ~UdeaBook();

    // lee config.txt, usuarios.txt, amistades.txt y solicitudes.txt de la carpeta;
    // devuelve false (y no deja nada cargado) si algun archivo esta mal
    bool cargarDatos(const std::string& carpeta);

    // busqueda binaria por nombre; devuelve el indice o -1
    int buscarIndice(const std::string& nombre) const;
    Usuario* buscarUsuario(const std::string& nombre) const;

    // devuelve el usuario si el nombre y la contrasenia coinciden, si no nullptr
    Usuario* iniciarSesion(const std::string& nombre, const std::string& contrasenia) const;

    // pide las credenciales por consola con maximo 3 intentos
    Usuario* pedirSesion() const;

    int getNumUsuarios() const;
    Usuario* getUsuario(int indice) const;
    MatrizAmistad& getMatriz();

    const std::string& getSubRuta() const;

    // arma la ruta completa de una ruta relativa: subRuta + "/" + relativa
    std::string rutaCompleta(const std::string& relativa) const;
};

#endif
