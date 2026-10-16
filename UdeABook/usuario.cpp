#include "Usuario.h"
#include "Album.h"
//#include "Mensaje.h"
#include "SolicitudAmistad.h"
#include <cstdlib>

Usuario::Usuario(const std::string& nombreUsuario, const std::string& contrasenia,
                 const std::string& correo, const std::string& ciudad,
                 const std::string& pais, const std::string& rutaFotoPerfil,
                 const Fecha& fechaNacimiento, const Fecha& fechaInscripcion, int indice)
    : nombreUsuario(nombreUsuario), contrasenia(contrasenia), correo(correo),
    ciudad(ciudad), pais(pais), rutaFotoPerfil(rutaFotoPerfil),
    fechaNacimiento(fechaNacimiento), fechaInscripcion(fechaInscripcion),
    indice(indice) {}

Usuario::~Usuario()
{
    for (int i = 0; i < albumes.getCantidad(); i++)
    {
        delete albumes[i];
    }
    for (int i = 0; i < enviados.getCantidad(); i++)
    {
        //delete enviados[i];
    }
    for (int i = 0; i < solicitudes.getCantidad(); i++)
    {
        delete solicitudes[i];
    }
}

const std::string& Usuario::getNombreUsuario() const { return nombreUsuario; }
const std::string& Usuario::getContrasenia() const { return contrasenia; }
const std::string& Usuario::getCorreo() const { return correo; }
const std::string& Usuario::getCiudad() const { return ciudad; }
const std::string& Usuario::getPais() const { return pais; }
const std::string& Usuario::getRutaFotoPerfil() const { return rutaFotoPerfil; }
const Fecha& Usuario::getFechaNacimiento() const { return fechaNacimiento; }
const Fecha& Usuario::getFechaInscripcion() const { return fechaInscripcion; }
int Usuario::getIndice() const { return indice; }

void Usuario::setContrasenia(const std::string& c) { contrasenia = c; }
void Usuario::setCorreo(const std::string& c) { correo = c; }
void Usuario::setCiudad(const std::string& c) { ciudad = c; }
void Usuario::setPais(const std::string& p) { pais = p; }
void Usuario::setRutaFotoPerfil(const std::string& r) { rutaFotoPerfil = r; }
void Usuario::setIndice(int i) { indice = i; }

const ListaDinamica<Album*>& Usuario::getAlbumes() const { return albumes; }
const ListaDinamica<Mensaje*>& Usuario::getEnviados() const { return enviados; }
const ListaDinamica<Mensaje*>& Usuario::getRecibidos() const { return recibidos; }
const ListaDinamica<SolicitudAmistad*>& Usuario::getSolicitudes() const { return solicitudes; }
const ListaDinamica<Usuario*>& Usuario::getAmigos() const { return amigos; }

//void Usuario::agregarAlbum(Album* album) { albumes.agregar(album); }
//void Usuario::agregarEnviado(Mensaje* mensaje) { enviados.agregar(mensaje); }
//void Usuario::agregarRecibido(Mensaje* mensaje) { recibidos.agregar(mensaje); }
void Usuario::agregarAmigo(Usuario* amigo) { amigos.agregar(amigo); }

int Usuario::amigosEnComun(const Usuario& otro, const MatrizAmistad& matriz) const
{
    return matriz.contarEnComun(indice, otro.indice);
}

bool Usuario::tieneSolicitudDe(const Usuario* origen) const
{
    for (int i = 0; i < solicitudes.getCantidad(); i++)
    {
        if (solicitudes[i]->getIndiceEmisor() == origen->indice)
        {
            return true;
        }
    }
    return false;
}

Usuario::ResultadoSolicitud Usuario::enviarSolicitud(Usuario* destino, const MatrizAmistad& matriz)
{
    if (destino == this)
    {
        return SOLICITUD_A_SI_MISMO;
    }
    if (matriz.sonAmigos(indice, destino->indice))
    {
        return SOLICITUD_YA_AMIGOS;
    }
    // el otro ya me habia enviado una solicitud
    if (tieneSolicitudDe(destino))
    {
        return SOLICITUD_YA_RECIBIDA;
    }
    // yo ya le habia enviado una solicitud
    if (destino->tieneSolicitudDe(this))
    {
        return SOLICITUD_YA_ENVIADA;
    }
    destino->solicitudes.agregar(new SolicitudAmistad(indice, destino->indice));
    return SOLICITUD_ENVIADA;
}

bool Usuario::aceptarSolicitud(int i, Usuario* emisor, MatrizAmistad& matriz)
{
    if (emisor == 0 || i < 0 || i >= solicitudes.getCantidad())
    {
        return false;
    }
    // la solicitud tiene que ser de ese emisor
    if (solicitudes[i]->getIndiceEmisor() != emisor->indice)
    {
        return false;
    }
    if (!solicitudes[i]->aceptar(matriz))
    {
        return false;
    }
    // cada uno queda en la lista de amigos del otro
    agregarAmigo(emisor);
    emisor->agregarAmigo(this);
    eliminarSolicitud(i);
    return true;
}

bool Usuario::eliminarSolicitud(int i)
{
    if (i < 0 || i >= solicitudes.getCantidad())
    {
        return false;
    }
    delete solicitudes[i];
    return solicitudes.eliminar(i);
}

// pesos 2, 5 y 7 mantienen la proporcion 1 : 2.5 : 3.5 sin usar decimales
int Usuario::pesoPrioridad(int enComun)
{
    if (enComun <= 10)
    {
        return 2;
    }
    if (enComun <= 20)
    {
        return 5;
    }
    return 7;
}

// ruleta: devuelve la posicion elegida o -1 si todos los pesos son 0
int Usuario::elegirPonderado(const int* pesos, int n)
{
    int total = 0;
    for (int i = 0; i < n; i++)
    {
        total += pesos[i];
    }
    if (total == 0)
    {
        return -1;
    }
    int r = std::rand() % total;
    int acumulado = 0;
    for (int i = 0; i < n; i++)
    {
        acumulado += pesos[i];
        if (r < acumulado)
        {
            return i;
        }
    }
    return -1;
}

// saca amigos por ruleta hasta tener 3; el elegido queda con peso 0 para no repetirse
void Usuario::sortearGrupo(int* pesos, int n, ListaDinamica<Usuario*>& elegidos) const
{
    while (elegidos.getCantidad() < 3)
    {
        int k = elegirPonderado(pesos, n);
        if (k == -1)
        {
            break;
        }
        elegidos.agregar(amigos[k]);
        pesos[k] = 0;
    }
}

ListaDinamica<Usuario*> Usuario::elegirAmigosAleatorio(const MatrizAmistad& matriz,
                                                        const ListaDinamica<Usuario*>& excluidos) const
{
    ListaDinamica<Usuario*> elegidos;
    int n = amigos.getCantidad();
    if (n == 0)
    {
        return elegidos;
    }

    int* pesos = new int[n];
    bool* vetado = new bool[n];
    int* actuales = new int[n];

    for (int i = 0; i < n; i++)
    {
        pesos[i] = pesoPrioridad(amigosEnComun(*amigos[i], matriz));
        vetado[i] = false;
        for (int j = 0; j < excluidos.getCantidad(); j++)
        {
            if (amigos[i] == excluidos[j])
            {
                vetado[i] = true;
            }
        }
        actuales[i] = vetado[i] ? 0 : pesos[i];
    }

    // primero entre los que no se han mostrado
    sortearGrupo(actuales, n, elegidos);

    // si faltan, se completa con los que ya se habian mostrado
    if (elegidos.getCantidad() < 3)
    {
        for (int i = 0; i < n; i++)
        {
            actuales[i] = vetado[i] ? pesos[i] : 0;
        }
        sortearGrupo(actuales, n, elegidos);
    }

    delete[] pesos;
    delete[] vetado;
    delete[] actuales;
    return elegidos;
}