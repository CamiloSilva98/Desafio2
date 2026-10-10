#ifndef LISTADINAMICA_H
#define LISTADINAMICA_H

// arreglo dinamico que duplica su capacidad cuando se llena
template <typename T>
class ListaDinamica
{
private:
    T* datos;
    int cantidad;
    int capacidad;

    // duplica la capacidad y pasa los elementos al nuevo arreglo
    void crecer()
    {
        int nuevaCapacidad = capacidad * 2;
        T* nuevo = new T[nuevaCapacidad];
        for (int i = 0; i < cantidad; i++)
        {
            nuevo[i] = datos[i];
        }
        delete[] datos;
        datos = nuevo;
        capacidad = nuevaCapacidad;
    }

public:
    ListaDinamica(int capacidadInicial = 4)
        : cantidad(0), capacidad(capacidadInicial > 0 ? capacidadInicial : 4)
    {
        datos = new T[capacidad];
    }

    // constructor de copia
    ListaDinamica(const ListaDinamica& otra)
        : cantidad(otra.cantidad), capacidad(otra.capacidad)
    {
        datos = new T[capacidad];
        for (int i = 0; i < cantidad; i++)
        {
            datos[i] = otra.datos[i];
        }
    }

    // operador de asignacion
    ListaDinamica& operator=(const ListaDinamica& otra)
    {
        if (this != &otra)
        {
            delete[] datos;
            cantidad = otra.cantidad;
            capacidad = otra.capacidad;
            datos = new T[capacidad];
            for (int i = 0; i < cantidad; i++)
            {
                datos[i] = otra.datos[i];
            }
        }
        return *this;
    }

    ~ListaDinamica()
    {
        delete[] datos;
    }

    void agregar(const T& elemento)
    {
        if (cantidad == capacidad)
        {
            crecer();
        }
        datos[cantidad] = elemento;
        cantidad++;
    }

    // inserta en una posicion corriendo los demas a la derecha
    bool insertar(int indice, const T& elemento)
    {
        if (indice < 0 || indice > cantidad)
        {
            return false;
        }
        if (cantidad == capacidad)
        {
            crecer();
        }
        for (int i = cantidad; i > indice; i--)
        {
            datos[i] = datos[i - 1];
        }
        datos[indice] = elemento;
        cantidad++;
        return true;
    }

    // elimina una posicion corriendo los demas a la izquierda
    bool eliminar(int indice)
    {
        if (indice < 0 || indice >= cantidad)
        {
            return false;
        }
        for (int i = indice; i < cantidad - 1; i++)
        {
            datos[i] = datos[i + 1];
        }
        cantidad--;
        return true;
    }

    void vaciar()
    {
        cantidad = 0;
    }

    T& operator[](int indice)
    {
        return datos[indice];
    }

    const T& operator[](int indice) const
    {
        return datos[indice];
    }

    int getCantidad() const { return cantidad; }
    int getCapacidad() const { return capacidad; }
    bool estaVacia() const { return cantidad == 0; }

    // bytes que ocupa la lista (objeto mas el arreglo reservado)
    long memoriaBytes() const
    {
        return (long)sizeof(*this) + (long)capacidad * (long)sizeof(T);
    }
};

#endif