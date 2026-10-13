#include "MatrizAmistad.h"

MatrizAmistad::MatrizAmistad(int n) : n(n), palabrasPorFila((n + 63) / 64) {
    long total = (long)n * palabrasPorFila;
    bits = new unsigned long long[total > 0 ? total : 1];
    for (long k = 0; k < total; k++)
    {
        bits[k] = 0;
    }
}

MatrizAmistad::MatrizAmistad(const MatrizAmistad& otra)
    : n(otra.n), palabrasPorFila(otra.palabrasPorFila)
{
    long total = (long)n * palabrasPorFila;
    bits = new unsigned long long[total > 0 ? total : 1];
    for (long k = 0; k < total; k++)
    {
        bits[k] = otra.bits[k];
    }
}

MatrizAmistad& MatrizAmistad::operator=(const MatrizAmistad& otra) {
    if (this != &otra)
    {
        delete[] bits;
        n = otra.n;
        palabrasPorFila = otra.palabrasPorFila;
        long total = (long)n * palabrasPorFila;
        bits = new unsigned long long[total > 0 ? total : 1];
        for (long k = 0; k < total; k++)
        {
            bits[k] = otra.bits[k];
        }
    }
    return *this;
}

MatrizAmistad::~MatrizAmistad()
{
    delete[] bits;
}

bool MatrizAmistad::indiceValido(int i) const
{
    return i >= 0 && i < n;
}

int MatrizAmistad::getN() const
{
    return n;
}

void MatrizAmistad::establecer(int i, int j)
{
    if (!indiceValido(i) || !indiceValido(j) || i == j)
    {
        return;
    }
    bits[(long)i * palabrasPorFila + j / 64] |= (1ULL << (j % 64));
    bits[(long)j * palabrasPorFila + i / 64] |= (1ULL << (i % 64));
}

bool MatrizAmistad::sonAmigos(int i, int j) const {
    if (!indiceValido(i) || !indiceValido(j))
    {
        return false;
    }
    return (bits[(long)i * palabrasPorFila + j / 64] >> (j % 64)) & 1ULL;
}

int MatrizAmistad::contarEnComun(int i, int j) const
{
    if (!indiceValido(i) || !indiceValido(j))
    {
        return 0;
    }
    int total = 0;
    for (int p = 0; p < palabrasPorFila; p++)
    {
        unsigned long long comun = bits[(long)i * palabrasPorFila + p] &
                                   bits[(long)j * palabrasPorFila + p];
        // se cuentan los bits prendidos quitando el bit mas bajo cada vez
        while (comun != 0) {
            comun &= (comun - 1);
            total++;
        }
    }
    return total;
}

long MatrizAmistad::memoriaBytes() const
{
    return (long)sizeof(*this) + (long)n * palabrasPorFila * (long)sizeof(unsigned long long);
}