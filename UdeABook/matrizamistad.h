#ifndef MATRIZAMISTAD_H
#define MATRIZAMISTAD_H

// matriz de adyacencia simetrica guardada en bits (64 casillas por palabra)
class MatrizAmistad
{
private:
    int n;
    int palabrasPorFila;
    unsigned long long* bits;

    bool indiceValido(int i) const;

public:
    MatrizAmistad(int n = 0);
    MatrizAmistad(const MatrizAmistad& otra);
    MatrizAmistad& operator=(const MatrizAmistad& otra);
    ~MatrizAmistad();

    int getN() const;

    // crea la amistad en los dos sentidos
    void establecer(int i, int j);

    bool sonAmigos(int i, int j) const;

    // cuenta los amigos que tienen en comun i y j (AND de filas)
    int contarEnComun(int i, int j) const;

    // bytes que ocupa la matriz (objeto mas el arreglo de bits)
    long memoriaBytes() const;
};

#endif