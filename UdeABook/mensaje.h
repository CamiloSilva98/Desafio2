#ifndef MENSAJE_H
#define MENSAJE_H

#include "fecha.h"
#include <string>
#include <usuario.h>

class Mensaje
{
public:
    static const int MAX_CARACTERES = 512;
private:
    std::string texto;
    Usuario* emisor;
    Usuario* receptor;
    Fecha fechaEnvio;
    bool leido; //F = no le ido, T = Leido
public:
    Mensaje(Usuario* emisor, Usuario* receptor, const Fecha& fechanvio, bool leido, const std::string& texto);
    void marcarLeido();
    //getters (pasamos por referencia constante cuando son ojetos para no copiarlos)
    const std::string& getTexto() const;
    Usuario* getEmisor() const;
    Usuario* getReceptor() const;
    const Fecha& getFechaEnvio() const;
    bool estaLeido() const;
    // Se ordena por fecha de envio: asi ordenarCronologicamente() funciona con mensajes.
    bool operator<(const Mensaje& otro) const;

    // Bytes que ocupa el mensaje (objeto + texto en memoria dinamica), para MedidorRecursos.
    long memoriaBytes() const;
};

#endif // MENSAJE_H
