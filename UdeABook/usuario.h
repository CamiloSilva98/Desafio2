#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include "fecha.h"
#include "ListaDinamica.h"
#include "MatrizAmistad.h"

class Album;
class Mensaje;
class SolicitudAmistad;

class Usuario {
public:
    // resultados posibles al intentar enviar una solicitud
    enum ResultadoSolicitud
    {
        SOLICITUD_ENVIADA,
        SOLICITUD_A_SI_MISMO,
        SOLICITUD_YA_AMIGOS,
        SOLICITUD_YA_RECIBIDA,
        SOLICITUD_YA_ENVIADA
    };

private:
    std::string nombreUsuario;
    std::string contrasenia;
    std::string correo;
    std::string ciudad;
    std::string pais;
    std::string rutaFotoPerfil;
    Fecha fechaNacimiento;
    Fecha fechaInscripcion;

    // posicion del usuario en el arreglo ordenado y en la matriz de amistad
    int indice;

    // el usuario es dueno de estos objetos y los borra en el destructor
    ListaDinamica<Album*> albumes;
    ListaDinamica<Mensaje*> enviados;
    ListaDinamica<SolicitudAmistad*> solicitudes;

    // solo referencias, no se borran aqui
    ListaDinamica<Mensaje*> recibidos;
    ListaDinamica<Usuario*> amigos;

    // un usuario no se copia porque duplicaria los objetos que le pertenecen
    Usuario(const Usuario&) = delete;
    Usuario& operator=(const Usuario&) = delete;

    static int pesoPrioridad(int enComun);
    static int elegirPonderado(const int* pesos, int n);
    void sortearGrupo(int* pesos, int n, ListaDinamica<Usuario*>& elegidos) const;

public:
    Usuario(const std::string& nombreUsuario, const std::string& contrasenia,
            const std::string& correo, const std::string& ciudad,
            const std::string& pais, const std::string& rutaFotoPerfil,
            const Fecha& fechaNacimiento, const Fecha& fechaInscripcion, int indice);
    ~Usuario();

    const std::string& getNombreUsuario() const;
    const std::string& getContrasenia() const;
    const std::string& getCorreo() const;
    const std::string& getCiudad() const;
    const std::string& getPais() const;
    const std::string& getRutaFotoPerfil() const;
    const Fecha& getFechaNacimiento() const;
    const Fecha& getFechaInscripcion() const;
    int getIndice() const;

    void setContrasenia(const std::string& contrasenia);
    void setCorreo(const std::string& correo);
    void setCiudad(const std::string& ciudad);
    void setPais(const std::string& pais);
    void setRutaFotoPerfil(const std::string& ruta);
    void setIndice(int indice);

    const ListaDinamica<Album*>& getAlbumes() const;
    const ListaDinamica<Mensaje*>& getEnviados() const;
    const ListaDinamica<Mensaje*>& getRecibidos() const;
    const ListaDinamica<SolicitudAmistad*>& getSolicitudes() const;
    const ListaDinamica<Usuario*>& getAmigos() const;

    void agregarAlbum(Album* album);
    void agregarEnviado(Mensaje* mensaje);
    void agregarRecibido(Mensaje* mensaje);
    void agregarAmigo(Usuario* amigo);

    // amigos que tiene en comun con otro usuario
    int amigosEnComun(const Usuario& otro, const MatrizAmistad& matriz) const;

    // revisa si ya hay una solicitud recibida de ese usuario
    bool tieneSolicitudDe(const Usuario* origen) const;

    // valida las reglas y, si todo esta bien, deja la solicitud en el receptor
    ResultadoSolicitud enviarSolicitud(Usuario* destino, const MatrizAmistad& matriz);

    // acepta la solicitud de la posicion i, que debe ser de ese emisor: prende la matriz,
    // deja a cada uno en la lista de amigos del otro y borra la solicitud
    bool aceptarSolicitud(int i, Usuario* emisor, MatrizAmistad& matriz);

    // borra la solicitud de la posicion i (despues de aceptarla o rechazarla)
    bool eliminarSolicitud(int i);

    // hasta 3 amigos al azar con prioridad segun amigos en comun;
    // los de la lista excluidos solo salen si no alcanzan los demas
    ListaDinamica<Usuario*> elegirAmigosAleatorio(const MatrizAmistad& matriz,
                                                   const ListaDinamica<Usuario*>& excluidos) const;
};

#endif