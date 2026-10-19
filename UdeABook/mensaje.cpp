#include "mensaje.h"

// Lista de inicializacion: cada atributo se construye directamente con su valor
Mensaje::Mensaje(Usuario* emisor, Usuario* receptor, const Fecha& fechaEnvio,
                 bool leido, const std::string& texto)
    : texto(texto), emisor(emisor), receptor(receptor),
    fechaEnvio(fechaEnvio), leido(leido) {}

void Mensaje::marcarLeido()
{
    leido = true;
}

const std::string& Mensaje::getTexto() const { return texto; }
Usuario* Mensaje::getEmisor() const { return emisor; }
Usuario* Mensaje::getReceptor() const { return receptor; }
const Fecha& Mensaje::getFechaEnvio() const { return fechaEnvio; }
bool Mensaje::estaLeido() const { return leido; }

bool Mensaje::operator<(const Mensaje& otro) const
{
    // Reutiliza el operator< de Fecha
    return fechaEnvio < otro.fechaEnvio;
}

long Mensaje::memoriaBytes() const
{
    // sizeof(*this) incluye los atributos; el contenido del texto vive aparte, en memoria dinamica
    return (long)sizeof(*this) + (long)texto.capacity();
}
void Mensaje::setTexto(const std::string& nuevoTexto){texto = nuevoTexto;}
void Mensaje::setFechaEnvio(const Fecha& nuevaFecha){fechaEnvio = nuevaFecha;}
void Mensaje::setLeido(bool nuevoLeido){leido = nuevoLeido;}
