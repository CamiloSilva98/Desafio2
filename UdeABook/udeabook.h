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

    // dibuja las fotos de perfil de amigos, el saludo y el menu principal
    void mostrarPantallaInicio(const Usuario* usuario, const ListaDinamica<Usuario*>& amigosMostrados) const;
    // opcion 3 del menu: pide K, muestra las sugerencias y deja enviar solicitudes
    void menuSugerencias(Usuario* usuario);

    // opcion 1 del menu: enviar solicitudes y aceptar las recibidas
    void menuAmistades(Usuario* usuario);
    void enviarSolicitudPorNombre(Usuario* usuario);
    void menuAceptarSolicitudes(Usuario* usuario);

    // avisa por consola lo que paso al intentar enviar una solicitud
    void mostrarResultadoSolicitud(Usuario::ResultadoSolicitud resultado, const Usuario* destino) const;

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
    // pantalla de inicio y menu principal del usuario; termina cuando escribe S
    void ejecutarSesion(Usuario* usuario);

    // hasta k amigos de amigos (que no son amigos ni el mismo usuario) con mas amigos en comun,
    // de mayor a menor; en enComun queda la cantidad de amigos en comun de cada uno
    ListaDinamica<Usuario*> sugerirAmigos(const Usuario* usuario, int k, ListaDinamica<int>& enComun) const;

    int getNumUsuarios() const;
    Usuario* getUsuario(int indice) const;
    MatrizAmistad& getMatriz();

    const std::string& getSubRuta() const;

    // arma la ruta completa de una ruta relativa: subRuta + "/" + relativa
    std::string rutaCompleta(const std::string& relativa) const;
};

#endif
