#include "guardadordatos.h"
#include "udeabook.h"
#include "usuario.h"
#include "album.h"
#include "foto.h"
#include "mensaje.h"
#include "solicitudamistad.h"
#include "fecha.h"
#include <fstream>

using namespace std;

namespace
{
// AAAAMMDDHHMM con ceros a la izquierda (12 digitos), el formato que lee CargadorDatos
string fechaATexto(const Fecha& fecha)
{
    unsigned long long numero = fecha.getAnio() * 100000000ULL +
                                fecha.getMes() * 1000000ULL +
                                fecha.getDia() * 10000ULL +
                                fecha.getHora() * 100ULL +
                                fecha.getMinuto();
    string texto = to_string(numero);
    while (texto.size() < 12)
    {
        texto = "0" + texto;
    }
    return texto;
}

// deja en "mayor" el valor mas grande visto de memoria temporal
void registrarTemporal(long& mayor, long bytes)
{
    if (bytes > mayor)
    {
        mayor = bytes;
    }
}
}

bool GuardadorDatos::guardarAmistades(const string& archivo, const UdeaBook& plataforma, long& iteraciones, long& bytesTemporales)
{
    long total = 0;
    string lineas;
    for (int i = 0; i < plataforma.getNumUsuarios(); i++)
    {
        iteraciones++;
        const Usuario* usuario = plataforma.getUsuario(i);
        const ListaDinamica<Usuario*>& amigos = usuario->getAmigos();
        for (int k = 0; k < amigos.getCantidad(); k++)
        {
            iteraciones++;
            // la amistad es de los dos lados: se escribe una sola vez, desde el de menor indice
            if (amigos[k]->getIndice() > usuario->getIndice())
            {
                lineas += usuario->getNombreUsuario() + "|" + amigos[k]->getNombreUsuario() + "\n";
                total++;
            }
        }
    }
    // la primera linea es la cantidad de amistades; el texto final es una segunda copia temporal
    string contenido = to_string(total) + "\n" + lineas;
    registrarTemporal(bytesTemporales, (long)lineas.capacity() + (long)contenido.capacity());
    return escribirArchivo(archivo, contenido);
}

bool GuardadorDatos::guardarSolicitudes(const string& archivo, const UdeaBook& plataforma, long& iteraciones, long& bytesTemporales)
{
    long total = 0;
    string lineas;
    for (int i = 0; i < plataforma.getNumUsuarios(); i++)
    {
        // cada solicitud vive en la lista de quien la recibe
        iteraciones++;
        const Usuario* receptor = plataforma.getUsuario(i);
        const ListaDinamica<SolicitudAmistad*>& pendientes = receptor->getSolicitudes();
        for (int k = 0; k < pendientes.getCantidad(); k++)
        {
            iteraciones++;
            const Usuario* emisor = plataforma.getUsuario(pendientes[k]->getIndiceEmisor());
            lineas += emisor->getNombreUsuario() + "|" + receptor->getNombreUsuario() + "\n";
            total++;
        }
    }
    string contenido = to_string(total) + "\n" + lineas;
    registrarTemporal(bytesTemporales, (long)lineas.capacity() + (long)contenido.capacity());
    return escribirArchivo(archivo, contenido);
}

bool GuardadorDatos::guardarMensajes(const string& archivo, const UdeaBook& plataforma, long& iteraciones, long& bytesTemporales)
{
    // formato: emisor|receptor|fecha|leido|texto  (sin linea de cantidad)
    string lineas;
    for (int i = 0; i < plataforma.getNumUsuarios(); i++)
    {
        // cada mensaje existe una sola vez y es del emisor, asi no se escribe repetido
        iteraciones++;
        const Usuario* emisor = plataforma.getUsuario(i);
        const ListaDinamica<Mensaje*>& enviados = emisor->getEnviados();
        for (int k = 0; k < enviados.getCantidad(); k++)
        {
            iteraciones++;
            const Mensaje* mensaje = enviados[k];
            lineas += emisor->getNombreUsuario() + "|" +
                      mensaje->getReceptor()->getNombreUsuario() + "|" +
                      fechaATexto(mensaje->getFechaEnvio()) + "|" +
                      (mensaje->estaLeido() ? "1" : "0") + "|" +
                      mensaje->getTexto() + "\n";
        }
    }
    registrarTemporal(bytesTemporales, (long)lineas.capacity());
    return escribirArchivo(archivo, lineas);
}

bool GuardadorDatos::guardarFotos(const string& archivo, const UdeaBook& plataforma, long& iteraciones, long& bytesTemporales)
{
    // formato: usuario|album|nombreFoto|fecha|tamano|likes  (sin linea de cantidad)
    string lineas;
    for (int i = 0; i < plataforma.getNumUsuarios(); i++)
    {
        iteraciones++;
        const Usuario* usuario = plataforma.getUsuario(i);
        const ListaDinamica<Album*>& albumes = usuario->getAlbumes();
        for (int a = 0; a < albumes.getCantidad(); a++)
        {
            iteraciones++;
            const Album* album = albumes[a];
            for (int f = 0; f < album->getCantidadFotos(); f++)
            {
                iteraciones++;
                const Foto& foto = album->getFoto(f);
                // la ruta de la foto es ruta del album + "/" + nombre: se deja solo el nombre
                const string& ruta = foto.getRuta();
                string::size_type barra = ruta.rfind('/');
                string nombreFoto = (barra == string::npos) ? ruta : ruta.substr(barra + 1);
                lineas += usuario->getNombreUsuario() + "|" + album->getNombre() + "|" +
                          nombreFoto + "|" + fechaATexto(foto.getFechaFoto()) + "|" +
                          to_string(foto.getTamanio()) + "|" + to_string(foto.getLikes()) + "\n";
            }
        }
    }
    registrarTemporal(bytesTemporales, (long)lineas.capacity());
    return escribirArchivo(archivo, lineas);
}

bool GuardadorDatos::escribirArchivo(const string& archivo, const string& contenido)
{
    // el contenido ya esta completo en memoria: el archivo se abre y se escribe de una sola vez
    ofstream salida(archivo.c_str());
    salida << contenido;
    salida.flush();
    return !salida.fail();
}