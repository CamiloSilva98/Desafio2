#ifndef MEDIDORRECURSOS_H
#define MEDIDORRECURSOS_H
#include <string>
class UdeaBook;

// Bytes que un texto ocupa en memoria dinamica. Un texto corto (SSO) vive dentro del propio objeto
// y ya esta contado en sizeof, asi que devuelve 0; si no, devuelve capacity().
long bytesDinamicosTexto(const std::string& texto);

// Se crea un MedidorRecursos nuevo al empezar cada funcionalidad (asi arranca en 0),
// la funcionalidad le va sumando las iteraciones y al terminar se calcula la memoria
// y se muestran las dos metricas.
class medidorrecursos
{
private:
    long iteraciones;
    long memoriaBytes;
    // llamadas a componentes externos (libreria estandar) que uso la funcionalidad
    long externosRand;     // std::rand
    long externosTiempo;   // std::time y std::localtime
    long externosChrono;   // std::chrono::steady_clock::now
public:
    medidorrecursos();

    void sumarIteraciones(long cantidad = 1); // suma vueltas de ciclo; sin argumento suma 1

    // suma llamadas a componentes externos (el enunciado pide reportar su uso)
    void sumarExternos(long llamadasRand, long llamadasTiempo, long llamadasChrono);

    // Memoria que ocupa TODO lo que existe ahora: la plataforma, la matriz de amistad,
    // los usuarios con sus listas, albumes, fotos, solicitudes y mensajes.
    // bytesLocales = variables locales y parametros por valor de la funcionalidad
    // que se esta midiendo (los suma quien la llama con sizeof).
    void calcularMemoria(UdeaBook& plataforma, long bytesLocales);

    void mostrarMetricas() const;
    long getIteraciones() const;
    long getMemoriaBytes() const;
    long getExternosRand() const;
    long getExternosTiempo() const;
    long getExternosChrono() const;
    void setIteraciones(long nuevasIteraciones);
    void setMemoriaBytes(long nuevaMemoria);

};

#endif // MEDIDORRECURSOS_H