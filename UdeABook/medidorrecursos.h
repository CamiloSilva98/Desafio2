#ifndef MEDIDORRECURSOS_H
#define MEDIDORRECURSOS_H
class UdeaBook;
// Se crea un MedidorRecursos nuevo al empezar cada funcionalidad (asi arranca en 0),
// la funcionalidad le va sumando las iteraciones y al terminar se calcula la memoria
// y se muestran las dos metricas.
class medidorrecursos
{
private:
    long iteraciones;
    long memoriaBytes;
public:
    medidorrecursos();

    void sumarIteraciones(long cantidad = 1); // suma vueltas de ciclo; sin argumento suma 1

    // Memoria que ocupa TODO lo que existe ahora: la plataforma, la matriz de amistad,
    // los usuarios con sus listas, albumes, fotos, solicitudes y mensajes.
    // bytesLocales = variables locales y parametros por valor de la funcionalidad
    // que se esta midiendo (los suma quien la llama con sizeof).
    void calcularMemoria(UdeaBook& plataforma, long bytesLocales);

    void mostrarMetricas() const;
    long getIteraciones() const;
    long getMemoriaBytes() const;
    void setIteraciones(long nuevasIteraciones);
    void setMemoriaBytes(long nuevaMemoria);

};

#endif // MEDIDORRECURSOS_H
