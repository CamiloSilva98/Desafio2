#include "cargadordatos.h"
#include "udeabook.h"
#include "usuario.h"
#include "album.h"
#include "foto.h"
#include "mensaje.h"
#include "fecha.h"
#include <fstream>
#include <sstream>

using namespace std;
namespace //funciones auxiliares para usar solo en este archivo
{
// Lee una linea del archivo.
bool leerLinea(ifstream& entrada, string& linea)
{
    if (!getline(entrada, linea))
    {
        return false;
    }
    if (!linea.empty() && linea[linea.size() - 1] == '\r')
    {
        linea.erase(linea.size() - 1);
    }
    return true;
}

// Convierte un texto de solo digitos ("1234") en numero.
// Devuelve false si esta vacio, si trae otra cosa que digitos o si es muy largo.
bool textoANumero(const string& texto, unsigned long long& resultado)
{
    if (texto.empty() || texto.size() > 18)
    {
        return false;
    }
    resultado = 0;
    for (size_t i = 0; i < texto.size(); i++)
    {
        if (texto[i] < '0' || texto[i] > '9')
        {
            return false;
        }
        // corre el numero una cifra a la izquierda y le suma la nueva: "123" -> 1, 12, 123
        resultado = resultado * 10 + (unsigned long long)(texto[i] - '0');
    }
    return true;
}

// Convierte "AAAAMMDDHHMM" (12 digitos) en una Fecha.
// Devuelve false si el texto no tiene 12 digitos o la fecha no es valida.
bool fechaDesdeTexto(const string& texto, Fecha& resultado)
{
    unsigned long long numero = 0;
    if (texto.size() != 12 || !textoANumero(texto, numero))
    {
        return false;
    }
    // Con % (resto) y / (division) se separa cada parte del numero:
    unsigned int minuto = (unsigned int)(numero % 100);
    unsigned int hora = (unsigned int)((numero / 100) % 100);
    unsigned int dia = (unsigned int)((numero / 10000) % 100);
    unsigned int mes = (unsigned int)((numero / 1000000) % 100);
    unsigned int anio = (unsigned int)(numero / 100000000);

    // El constructor de Fecha no valida, asi que se revisan los rangos aqui
    if (anio == 0 || mes < 1 || mes > 12 || dia < 1 || dia > 31 || hora > 23 || minuto > 59)
    {
        return false;
    }
    resultado = Fecha(dia, mes, anio, hora, minuto);
    return true;
}

// Busca un usuario por su nombre con BUSQUEDA BINARIA. Devuelve su puntero, o 0 si no existe.
Usuario* buscarUsuario(const UdeaBook& plataforma, const string& nombre, long& iteraciones)
{
    int bajo = 0;
    int alto = plataforma.getNumUsuarios() - 1;
    while (bajo <= alto)
    {
        iteraciones++;
        int medio = (bajo + alto) / 2;
        Usuario* candidato = plataforma.getUsuario(medio);
        const string& nombreCandidato = candidato->getNombreUsuario();   // referencia para no copiar el texto
        if (nombreCandidato == nombre)
        {
            return candidato;
        }
        if (nombreCandidato < nombre)
        {
            bajo = medio + 1;       // el buscado esta en la mitad de arriba
        }
        else
        {
            alto = medio - 1;       // el buscado esta en la mitad de abajo
        }
    }
    return 0;
}

// Busca un album de un usuario por su nombre. Devuelve el puntero al album ORIGINAL o 0 si no existe
// Es busqueda lineal porque un usuario tiene pocos albumes
Album* buscarAlbum(const Usuario* usuario, const string& nombreAlbum, long& iteraciones)
{
    const ListaDinamica<Album*>& albumes = usuario->getAlbumes();   // referencia: no copia la lista
    for (int i = 0; i < albumes.getCantidad(); i++)
    {
        iteraciones++;
        if (albumes[i]->getNombre() == nombreAlbum)
        {
            return albumes[i];
        }
    }
    return 0;
}
}

CargadorDatos::CargadorDatos(const string& subRuta) : subRuta(subRuta) {}

int CargadorDatos::cargarAlbumes(const string& archivo, const UdeaBook& plataforma, long& iteraciones)
{
    ifstream entrada(archivo.c_str());
    if (!entrada.is_open())
    {
        return -1;
    }

    int cargados = 0;
    string linea;
    while (getline(entrada, linea)) //lee una linea completa del archivo y la guarda en "linea".
    {
        iteraciones++;                       // una vuelta por linea leida
        if (linea.empty())
        {
            continue;
        }

        // usuario|album|privacidad
        istringstream campos(linea);  //convierte ese texto en algo que se puede "leer por pedazos"
        string nombreUsuario, nombreAlbum, textoPrivacidad;
        unsigned long long codigo = 0;
        if (!getline(campos, nombreUsuario, '|') || //lee hasta el proximo '|' y lo guarda
            !getline(campos, nombreAlbum, '|') ||
            !getline(campos, textoPrivacidad) ||
            nombreAlbum.empty() ||
            !textoANumero(textoPrivacidad, codigo) || codigo > 2)
        {
            continue;                        // linea dañada: se ignora
        }

        Usuario* dueno = buscarUsuario(plataforma, nombreUsuario, iteraciones);
        if (dueno == 0)
        {
            continue;                        // el dueño del album no existe
        }

        // Ruta del album: /users/<subRuta>/<usuario>/albums/<album>  (como en el enunciado)
        string rutaAlbum = "/users/" + subRuta + "/" + nombreUsuario + "/albums/" + nombreAlbum;

        // El album se crea con new porque Usuario guarda PUNTEROS a sus albumes y es quien los borra en su destructor
        // El numero del archivo (0, 1, 2) coincide con los valores del enum Album::Privacidad.
        dueno->agregarAlbum(new Album(nombreAlbum, rutaAlbum, static_cast<Album::Privacidad>(codigo)));
        cargados++;
    }
    return cargados;
}

int CargadorDatos::cargarFotos(const string& archivo, const UdeaBook& plataforma, long& iteraciones)
{
    ifstream entrada(archivo.c_str());
    if (!entrada.is_open())
    {
        return -1;
    }

    int cargadas = 0;
    string linea;
    while (getline(entrada, linea))
    {
        iteraciones++;                       // una vuelta por linea leida
        if (linea.empty())
        {
            continue;
        }

        // usuario|album|nombreFoto|fecha|tamano|likes
        istringstream campos(linea);
        string nombreUsuario, nombreAlbum, nombreFoto, textoFecha, textoTamano, textoLikes;
        unsigned long long tamano = 0;
        unsigned long long likes = 0;
        Fecha fecha;
        if (!getline(campos, nombreUsuario, '|') ||
            !getline(campos, nombreAlbum, '|') ||
            !getline(campos, nombreFoto, '|') ||
            !getline(campos, textoFecha, '|') ||
            !getline(campos, textoTamano, '|') ||
            !getline(campos, textoLikes) ||
            nombreFoto.empty() ||
            !fechaDesdeTexto(textoFecha, fecha) ||
            !textoANumero(textoTamano, tamano) || tamano > 2000000000ULL ||   // debe caber en un int
            !textoANumero(textoLikes, likes) || likes > 4000000000ULL)        // debe caber en un unsigned int
        {
            continue;                        // linea danada: se ignora
        }

        // Se busca el usuario  y luego su album
        Usuario* dueno = buscarUsuario(plataforma, nombreUsuario, iteraciones);
        if (dueno == 0)
        {
            continue;                        // usuario inexistente
        }
        Album* album = buscarAlbum(dueno, nombreAlbum, iteraciones);
        if (album == 0)
        {
            continue;                        // album inexistente
        }

        // Ruta de la foto = ruta del album + "/" + nombre del archivo (que ya trae ".png")
        string rutaFoto = album->getRuta() + "/" + nombreFoto;
        album->agregarFoto(Foto(fecha, (int)tamano, rutaFoto, (unsigned int)likes));
        cargadas++;
    }
    return cargadas;
}

int CargadorDatos::cargarMensajes(const string& archivo, const UdeaBook& plataforma, long& iteraciones)
{
    ifstream entrada(archivo.c_str());
    if (!entrada.is_open())
    {
        return -1;
    }

    int cargados = 0;
    string linea;
    while (getline(entrada, linea))
    {
        iteraciones++;                       // una vuelta por linea leida
        if (linea.empty())
        {
            continue;
        }

        // emisor|receptor|fecha|leido|texto
        // El texto es el ULTIMO campo y se lee sin separador '|'.
        //getline  toma todo lo que queda de la linea, asi un mensaje que tenga un '|' adentro no se parte.
        istringstream campos(linea);
        string nombreEmisor, nombreReceptor, textoFecha, textoLeido, texto;
        unsigned long long leido = 0;
        Fecha fecha;
        if (!getline(campos, nombreEmisor, '|') ||
            !getline(campos, nombreReceptor, '|') ||
            !getline(campos, textoFecha, '|') ||
            !getline(campos, textoLeido, '|') ||
            !getline(campos, texto) ||
            !fechaDesdeTexto(textoFecha, fecha) ||
            !textoANumero(textoLeido, leido) || leido > 1)
        {
            continue;                        // linea danada: se ignora
        }
        // No se aceptan mensajes vacios ni de mas de 512 caracteres
        if (texto.empty() || texto.size() > (size_t)Mensaje::MAX_CARACTERES)
        {
            continue;
        }

        Usuario* emisor = buscarUsuario(plataforma, nombreEmisor, iteraciones);
        Usuario* receptor = buscarUsuario(plataforma, nombreReceptor, iteraciones);
        if (emisor == 0 || receptor == 0)
        {
            continue;                        // alguno de los dos usuarios no existe
        }

        // Un SOLO objeto Mensaje, compartido: el emisor lo guarda en "enviados"
        //y el receptor guarda el MISMO puntero en "recibidos". No se duplica el texto y el estado "leido" es uno solo.
        Mensaje* mensaje = new Mensaje(emisor, receptor, fecha, leido == 1, texto);
        emisor->agregarEnviado(mensaje);
        receptor->agregarRecibido(mensaje);
        cargados++;
    }
    //Se ordena UNA vez al final, usuario por usuario (cada lista es corta, y
    // Insertion Sort es muy rapido cuando los datos ya vienen casi ordenados).
    for (int i = 0; i < plataforma.getNumUsuarios(); i++)
    {
        iteraciones++;
        iteraciones += plataforma.getUsuario(i)->ordenarMensajes();
    }
    return cargados;
}