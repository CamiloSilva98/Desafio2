#ifndef CARGADORDATOS_H
#define CARGADORDATOS_H

#include <string>

// Declaracion adelantada: aqui solo se usa "const UdeaBook&" (una referencia), asi que
// no hace falta incluir udeabook.h en este .h. El #include completo va en el .cpp.
class UdeaBook;

// Lee los archivos albums.txt, fotos.txt y mensajes.txt y llena los objetos en memoria.
// UdeaBook::cargarDatos() debe llamar a estos tres metodos, DESPUES de cargar los usuarios.
// FORMATO: un registro por linea y los campos separados por '|'
//   albums.txt    usuario|album|privacidad            (privacidad: 0 publico, 1 amigos, 2 privado)
//   fotos.txt     usuario|album|nombreFoto|fecha|tamano|likes
//   mensajes.txt  emisor|receptor|fecha|leido|texto   (leido: 0 no leido, 1 leido)
//   fecha = AAAAMMDDHHMM, por ejemplo 202610101530 = 10/10/2026 a las 15:30
// ORDEN: primero cargarAlbumes y DESPUES cargarFotos (cada foto busca su album ya creado).
class CargadorDatos
{
private:
    std::string subRuta;   // parte fija de las rutas de las fotos (la lee UdeaBook de config.txt)

public:
    // Recibe la subRuta que ya leyo UdeaBook, para no leer config.txt dos veces.
    CargadorDatos(const std::string& subRuta);

    // Los tres devuelven cuantos registros se cargaron bien, o -1 si el archivo no se pudo abrir.
    // Una linea danada (campos que faltan, numero invalido, usuario inexistente...) se ignora:
    // asi un solo error no impide cargar todo lo demas.
    // "iteraciones" se SUMA a lo que traiga: cuenta 1 por cada linea leida y 1 por cada vuelta
    // de las busquedas. Sirve para MedidorRecursos
    int cargarAlbumes(const std::string& archivo, const UdeaBook& plataforma, long& iteraciones);
    int cargarFotos(const std::string& archivo, const UdeaBook& plataforma, long& iteraciones);
    int cargarMensajes(const std::string& archivo, const UdeaBook& plataforma, long& iteraciones);
};

#endif // CARGADORDATOS_H