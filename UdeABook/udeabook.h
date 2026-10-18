#ifndef UDEABOOK_H
#define UDEABOOK_H

#include <string>
#include <chrono>
#include "ListaDinamica.h"
#include "MatrizAmistad.h"
#include "Usuario.h"

// clase principal: carga los datos, busca usuarios e inicia sesion
class UdeaBook
{
public:
    // archivos que pueden cambiar mientras el programa corre
    enum ArchivoDatos
    {
        ARCHIVO_AMISTADES,
        ARCHIVO_SOLICITUDES,
        ARCHIVO_MENSAJES,
        ARCHIVO_FOTOS,
        NUM_ARCHIVOS
    };
private:
    // ordenados por nombre; la posicion de cada uno es su indice en la matriz
    ListaDinamica<Usuario*> usuarios;
    MatrizAmistad* matriz;

    std::string subRuta;
    std::string carpetaDatos;

    // politica de guardado: los cambios importantes se guardan de inmediato y los menores al
    // llegar a MAX_CAMBIOS_MENORES o a MAX_SEGUNDOS_SIN_GUARDAR desde el primero sin guardar
    static const int MAX_CAMBIOS_MENORES = 10;
    static const int MAX_SEGUNDOS_SIN_GUARDAR = 120;
    bool archivoSucio[NUM_ARCHIVOS];     // true si el archivo cambio y falta guardarlo
    int cambiosMenores;                  // cambios menores sin guardar
    std::chrono::steady_clock::time_point primerCambioMenor;   // hora del primero de ellos

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

    // opcion 2 del menu: visitar el perfil de un amigo (sus albumes, carrusel de fotos y likes)
    void menuVisitarPerfil(Usuario* usuario);

    // opcion 4 del menu: enviar mensaje, leer nuevos y buscar por rango de fechas
    void menuMensajes(Usuario* usuario);
    void enviarMensajePorNombre(Usuario* usuario);
    void leerMensajesNuevos(Usuario* usuario);
    void buscarMensajesPorFechas(Usuario* usuario);

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
    // igual que la anterior, pero SUMA a "iteraciones" las vueltas de la busqueda (para medidorrecursos)
    int buscarIndice(const std::string& nombre, long& iteraciones) const;
    Usuario* buscarUsuario(const std::string& nombre) const;

    // devuelve el usuario si el nombre y la contrasenia coinciden, si no nullptr
    Usuario* iniciarSesion(const std::string& nombre, const std::string& contrasenia) const;

    // pide las credenciales por consola con maximo 3 intentos
    Usuario* pedirSesion() const;
    // pantalla de inicio y menu principal del usuario; termina cuando escribe S
    void ejecutarSesion(Usuario* usuario);

    // hasta k amigos de amigos (que no son amigos ni el mismo usuario) con mas amigos en comun,
    // de mayor a menor; en enComun queda la cantidad de amigos en comun de cada uno
    ListaDinamica<Usuario*> sugerirAmigos(const Usuario* usuario, int k, ListaDinamica<int>& enComun, long& iteraciones) const;

    int getNumUsuarios() const;
    Usuario* getUsuario(int indice) const;
    MatrizAmistad& getMatriz();

    const std::string& getSubRuta() const;
    const std::string& getcarpeta() const;


    // arma la ruta completa de una ruta relativa: subRuta + "/" + relativa
    std::string rutaCompleta(const std::string& relativa) const;

    // avisa que un archivo cambio; si el cambio es importante (critico) se guarda de inmediato
    void registrarCambio(ArchivoDatos archivo, bool critico);

    // guarda si ya hubo MAX_CAMBIOS_MENORES cambios menores o pasaron MAX_SEGUNDOS_SIN_GUARDAR
    // desde el primero; se llama despues de cada accion del usuario
    void revisarGuardado();

    // guarda solo los archivos marcados como modificados (ver GuardadorDatos)
    bool guardarPendientes();
};

#endif
