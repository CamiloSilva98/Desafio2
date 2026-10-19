#include "medidorrecursos.h"
#include "udeabook.h"
#include "usuario.h"
#include "album.h"
#include "mensaje.h"
#include "solicitudamistad.h"
#include <iostream>
#include <cstdint>

long bytesDinamicosTexto(const std::string& texto)
{
    std::uintptr_t inicio = reinterpret_cast<std::uintptr_t>(&texto);
    std::uintptr_t datos = reinterpret_cast<std::uintptr_t>(texto.data());
    // si los caracteres estan dentro del propio objeto string (SSO), no hay memoria dinamica aparte
    if (datos >= inicio && datos < inicio + sizeof(texto))
    {
        return 0;
    }
    return (long)texto.capacity();
}

namespace {
// Memoria de un usuario: el objeto, el contenido de sus textos y sus listas.
// Los mensajes se cuentan solo en "enviados" porque cada mensaje existe una sola vez:
// el receptor lo guarda como puntero en "recibidos" y ese puntero ya esta en la capacidad de la lista.
long memoriaUsuario(const Usuario* usuario){
    long total = (long)sizeof(Usuario);
    total+= bytesDinamicosTexto(usuario->getNombreUsuario());
    total += bytesDinamicosTexto(usuario->getContrasenia());
    total += bytesDinamicosTexto(usuario->getCorreo());
    total += bytesDinamicosTexto(usuario->getCiudad());
    total += bytesDinamicosTexto(usuario->getPais());
    total += bytesDinamicosTexto(usuario->getRutaFotoPerfil());
    // arreglos de las listas: ocupan TODA la capacidad reservada
    total += (long)usuario->getAlbumes().getCapacidad() * (long)sizeof(Album*);
    total += (long)usuario->getEnviados().getCapacidad() * (long)sizeof(Mensaje*);
    total += (long)usuario->getRecibidos().getCapacidad() * (long)sizeof(Mensaje*);
    total += (long)usuario->getSolicitudes().getCapacidad() * (long)sizeof(SolicitudAmistad*);
    total += (long)usuario->getAmigos().getCapacidad() * (long)sizeof(Usuario*);
    // objetos que pertenecen al usuario
    for (int i = 0; i < usuario->getAlbumes().getCantidad(); i++)
    {
        total += usuario->getAlbumes()[i]->memoriaBytes();
    }
    for (int i = 0; i < usuario->getEnviados().getCantidad(); i++)
    {
        total += usuario->getEnviados()[i]->memoriaBytes();
    }
    total += (long)usuario->getSolicitudes().getCantidad() * (long)sizeof(SolicitudAmistad);
    return total;
}
}
medidorrecursos::medidorrecursos(): iteraciones(0), memoriaBytes(0),
    externosRand(0), externosTiempo(0), externosChrono(0) {}
// Suma vueltas de ciclo al contador. Cada funcionalidad cuenta sus ciclos en una variable local
// y al final la suma aqui de una sola vez (sumarIteraciones(total)); sin argumento suma 1.
void medidorrecursos::sumarIteraciones(long cantidad)
{
    iteraciones += cantidad;
}
void medidorrecursos::sumarExternos(long llamadasRand, long llamadasTiempo, long llamadasChrono)
{
    externosRand += llamadasRand;
    externosTiempo += llamadasTiempo;
    externosChrono += llamadasChrono;
}
void medidorrecursos::calcularMemoria(UdeaBook& plataforma, long bytesLocales){
    int cantidadUsuarios = plataforma.getNumUsuarios();
    long total = (long)sizeof(UdeaBook);
    total+= bytesDinamicosTexto(plataforma.getSubRuta());
    total+= bytesDinamicosTexto(plataforma.getcarpeta());
    total+=(long)cantidadUsuarios* (long)sizeof(Usuario*);
    total+=plataforma.getMatriz().memoriaBytes();

    for (int i = 0; i < cantidadUsuarios; ++i) {
        total+= memoriaUsuario(plataforma.getUsuario(i));
    }
    memoriaBytes = total+ bytesLocales;
}
void medidorrecursos::mostrarMetricas()const{
    std::cout<< std::endl <<"-----consumo de Recursos-----"<<std::endl
              << "Iteraciones     :"<<iteraciones<<std::endl
              << "Memoria en uso: " << memoriaBytes << " bytes ("
              << memoriaBytes / 1024 << " KB)" << std::endl;
    // componentes externos usados (libreria estandar); si no se uso ninguno se dice
    if (externosRand == 0 && externosTiempo == 0 && externosChrono == 0)
    {
        std::cout << "Externos: ninguno" << std::endl;
        return;
    }
    std::cout << "Externos:";
    const char* separador = " ";
    if (externosRand > 0)
    {
        std::cout << separador << "rand x" << externosRand;
        separador = ", ";
    }
    if (externosTiempo > 0)
    {
        std::cout << separador << "time/localtime x" << externosTiempo;
        separador = ", ";
    }
    if (externosChrono > 0)
    {
        std::cout << separador << "chrono x" << externosChrono;
    }
    std::cout << std::endl;
}
long medidorrecursos::getIteraciones() const { return iteraciones; }
long medidorrecursos::getMemoriaBytes() const { return memoriaBytes; }
long medidorrecursos::getExternosRand() const { return externosRand; }
long medidorrecursos::getExternosTiempo() const { return externosTiempo; }
long medidorrecursos::getExternosChrono() const { return externosChrono; }
void medidorrecursos::setIteraciones(long nuevasIteraciones){iteraciones = nuevasIteraciones;}
void medidorrecursos::setMemoriaBytes(long nuevaMemoria){memoriaBytes = nuevaMemoria;}