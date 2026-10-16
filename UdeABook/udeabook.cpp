#include "UdeaBook.h"
#include <fstream>
#include <iostream>

namespace {

// lee una linea y le quita el \r que dejan los archivos hechos en Windows
bool leerLinea(std::ifstream& archivo, std::string& linea)
{
    if (!std::getline(archivo, linea))
    {
        return false;
    }
    if (!linea.empty() && linea[linea.size() - 1] == '\r')
    {
        linea.erase(linea.size() - 1);
    }
    return true;
}

// parte la linea por '|' y devuelve cuantos campos tiene (solo guarda los primeros maxCampos)
int separar(const std::string& linea, std::string* campos, int maxCampos)
{
    int cantidad = 0;
    std::string::size_type inicio = 0;
    while (true)
    {
        std::string::size_type pos = linea.find('|', inicio);
        if (cantidad < maxCampos)
        {
            if (pos == std::string::npos)
            {
                campos[cantidad] = linea.substr(inicio);
            }
            else
            {
                campos[cantidad] = linea.substr(inicio, pos - inicio);
            }
        }
        cantidad++;
        if (pos == std::string::npos)
        {
            break;
        }
        inicio = pos + 1;
    }
    return cantidad;
}

// convierte un texto de solo digitos en numero
bool aEntero(const std::string& texto, long long& valor)
{
    if (texto.empty() || texto.size() > 18)
    {
        return false;
    }
    long long resultado = 0;
    for (std::string::size_type i = 0; i < texto.size(); i++)
    {
        if (texto[i] < '0' || texto[i] > '9')
        {
            return false;
        }
        resultado = resultado * 10 + (texto[i] - '0');
    }
    valor = resultado;
    return true;
}

// convierte AAAAMMDDHH en una Fecha validando mes, dia (con bisiestos) y hora
bool aFecha(const std::string& texto, Fecha& fecha)
{
    long long valor;
    if (texto.size() != 10 || !aEntero(texto, valor))
    {
        return false;
    }
    int anio = (int)(valor / 1000000);
    int mes = (int)((valor / 10000) % 100);
    int dia = (int)((valor / 100) % 100);
    int hora = (int)(valor % 100);

    if (anio < 1 || mes < 1 || mes > 12 || hora > 23)
    {
        return false;
    }
    int diasMes[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool bisiesto = (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
    int maximo = diasMes[mes - 1];
    if (mes == 2 && bisiesto)
    {
        maximo = 29;
    }
    if (dia < 1 || dia > maximo)
    {
        return false;
    }
    fecha = Fecha(dia, mes, anio, hora);
    return true;
}

void error(const std::string& archivo, int linea, const char* motivo)
{
    std::cerr << "Error en " << archivo << ", linea " << linea << ": " << motivo << std::endl;
}

void advertencia(const std::string& archivo, int linea, const char* motivo)
{
    std::cerr << "Aviso en " << archivo << ", linea " << linea << " (se omite): " << motivo << std::endl;
}

}

UdeaBook::UdeaBook() : matriz(new MatrizAmistad(0)) {}

UdeaBook::~UdeaBook() {
    for (int i = 0; i < usuarios.getCantidad(); i++)
    {
        delete usuarios[i];
    }
    delete matriz;
}

void UdeaBook::liberar()
{
    for (int i = 0; i < usuarios.getCantidad(); i++)
    {
        delete usuarios[i];
    }
    usuarios.vaciar();
    delete matriz;
    matriz = new MatrizAmistad(0);
    subRuta = "";
}

bool UdeaBook::cargarDatos(const std::string& carpeta)
{
    liberar();
    std::string base = carpeta.empty() ? "" : carpeta + "/";

    if (!cargarConfig(base + "config.txt") ||
        !cargarUsuarios(base + "usuarios.txt") ||
        !cargarAmistades(base + "amistades.txt") ||
        !cargarSolicitudes(base + "solicitudes.txt"))
    {
        liberar();
        return false;
    }
    return true;
}

bool UdeaBook::cargarConfig(const std::string& ruta)
{
    std::ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        std::cerr << "No se pudo abrir " << ruta << std::endl;
        return false;
    }
    std::string linea;
    if (!leerLinea(archivo, linea) || linea.empty())
    {
        error(ruta, 1, "la primera linea debe ser la sub-ruta fija");
        return false;
    }
    subRuta = linea;
    return true;
}

bool UdeaBook::cargarUsuarios(const std::string& ruta)
{
    std::ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        std::cerr << "No se pudo abrir " << ruta << std::endl;
        return false;
    }

    std::string linea;
    long long n;
    if (!leerLinea(archivo, linea) || !aEntero(linea, n))
    {
        error(ruta, 1, "la primera linea debe ser la cantidad de usuarios");
        return false;
    }
    if (n > 10000000)
    {
        error(ruta, 1, "cantidad de usuarios demasiado grande");
        return false;
    }

    // el arreglo se crea una sola vez con el tamano exacto
    usuarios = ListaDinamica<Usuario*>(n > 0 ? (int)n : 1);

    std::string campos[8];
    for (int i = 0; i < n; i++)
    {
        int numLinea = i + 2;
        if (!leerLinea(archivo, linea))
        {
            error(ruta, numLinea, "faltan usuarios, hay menos de los indicados");
            return false;
        }
        if (separar(linea, campos, 8) != 8)
        {
            error(ruta, numLinea, "debe tener 8 campos separados por |");
            return false;
        }
        if (campos[0].empty())
        {
            error(ruta, numLinea, "nombre de usuario vacio");
            return false;
        }
        // la busqueda binaria necesita el orden estricto por nombre
        if (i > 0 && !(usuarios[i - 1]->getNombreUsuario() < campos[0]))
        {
            error(ruta, numLinea, "los usuarios deben ir ordenados por nombre y sin repetir");
            return false;
        }
        Fecha nacimiento;
        Fecha inscripcion;
        if (!aFecha(campos[6], nacimiento) || !aFecha(campos[7], inscripcion))
        {
            error(ruta, numLinea, "fecha invalida, el formato es AAAAMMDDHH");
            return false;
        }
        usuarios.agregar(new Usuario(campos[0], campos[1], campos[2], campos[3],
                                     campos[4], campos[5], nacimiento, inscripcion, i));
    }

    delete matriz;
    matriz = new MatrizAmistad((int)n);
    return true;
}

bool UdeaBook::cargarAmistades(const std::string& ruta)
{
    std::ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
    {
        std::cerr << "No se pudo abrir " << ruta << std::endl;
        return false;
    }

    std::string linea;
    long long m;
    if (!leerLinea(archivo, linea) || !aEntero(linea, m))
    {
        error(ruta, 1, "la primera linea debe ser la cantidad de amistades");
        return false;
    }

    std::string campos[2];
    for (long long k = 0; k < m; k++)
    {
        int numLinea = (int)k + 2;
        if (!leerLinea(archivo, linea))
        {
            error(ruta, numLinea, "faltan amistades, hay menos de las indicadas");
            return false;
        }
        if (separar(linea, campos, 2) != 2)
        {
            error(ruta, numLinea, "debe tener 2 campos separados por |");
            return false;
        }
        int a = buscarIndice(campos[0]);
        int b = buscarIndice(campos[1]);
        if (a < 0 || b < 0)
        {
            advertencia(ruta, numLinea, "algun usuario no existe");
            continue;
        }
        if (a == b)
        {
            advertencia(ruta, numLinea, "un usuario no puede ser amigo de si mismo");
            continue;
        }
        if (matriz->sonAmigos(a, b))
        {
            advertencia(ruta, numLinea, "amistad repetida");
            continue;
        }
        matriz->establecer(a, b);
        usuarios[a]->agregarAmigo(usuarios[b]);
        usuarios[b]->agregarAmigo(usuarios[a]);
    }
    return true;
}

bool UdeaBook::cargarSolicitudes(const std::string& ruta)
{
    std::ifstream archivo(ruta.c_str());
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir " << ruta << std::endl;
        return false;
    }

    std::string linea;
    long long s;
    if (!leerLinea(archivo, linea) || !aEntero(linea, s))
    {
        error(ruta, 1, "la primera linea debe ser la cantidad de solicitudes");
        return false;
    }

    std::string campos[2];
    for (long long k = 0; k < s; k++)
    {
        int numLinea = (int)k + 2;
        if (!leerLinea(archivo, linea))
        {
            error(ruta, numLinea, "faltan solicitudes, hay menos de las indicadas");
            return false;
        }
        if (separar(linea, campos, 2) != 2)
        {
            error(ruta, numLinea, "debe tener 2 campos separados por |");
            return false;
        }
        int emisor = buscarIndice(campos[0]);
        int receptor = buscarIndice(campos[1]);
        if (emisor < 0 || receptor < 0) {
            advertencia(ruta, numLinea, "algun usuario no existe");
            continue;
        }
        // enviarSolicitud ya aplica las reglas (amigos, repetidas, a si mismo)
        if (usuarios[emisor]->enviarSolicitud(usuarios[receptor], *matriz) != Usuario::SOLICITUD_ENVIADA)
        {
            advertencia(ruta, numLinea, "solicitud que no cumple las reglas");
        }
    }
    return true;
}

int UdeaBook::buscarIndice(const std::string& nombre) const
{
    int bajo = 0;
    int alto = usuarios.getCantidad() - 1;
    while (bajo <= alto)
    {
        int medio = bajo + (alto - bajo) / 2;
        const std::string& actual = usuarios[medio]->getNombreUsuario();
        if (actual == nombre)
        {
            return medio;
        }
        if (actual < nombre)
        {
            bajo = medio + 1;
        }
        else
        {
            alto = medio - 1;
        }
    }
    return -1;
}

Usuario* UdeaBook::buscarUsuario(const std::string& nombre) const
{
    int indice = buscarIndice(nombre);
    if (indice < 0)
    {
        return nullptr;
    }
    return usuarios[indice];
}

Usuario* UdeaBook::iniciarSesion(const std::string& nombre, const std::string& contrasenia) const
{
    Usuario* usuario = buscarUsuario(nombre);
    if (usuario == nullptr || usuario->getContrasenia() != contrasenia)
    {
        return nullptr;
    }
    return usuario;
}

Usuario* UdeaBook::pedirSesion() const
{
    for (int intento = 1; intento <= 3; intento++)
    {
        std::string nombre;
        std::string contrasenia;
        std::cout << "Usuario: ";
        if (!std::getline(std::cin, nombre))
        {
            return nullptr;
        }
        std::cout << "Contrasena: ";
        if (!std::getline(std::cin, contrasenia))
        {
            return nullptr;
        }
        Usuario* usuario = iniciarSesion(nombre, contrasenia);
        if (usuario != nullptr)
        {
            return usuario;
        }
        // no se dice cual de los dos datos esta mal
        std::cout << "Usuario o contrasena incorrectos (intento " << intento << " de 3)" << std::endl;
    }
    return nullptr;
}

int UdeaBook::getNumUsuarios() const
{
    return usuarios.getCantidad();
}

Usuario* UdeaBook::getUsuario(int indice) const
{
    if (indice < 0 || indice >= usuarios.getCantidad())
    {
        return nullptr;
    }
    return usuarios[indice];
}

MatrizAmistad& UdeaBook::getMatriz()
{
    return *matriz;
}

const std::string& UdeaBook::getSubRuta() const
{
    return subRuta;
}

std::string UdeaBook::rutaCompleta(const std::string& relativa) const
{
    return subRuta + "/" + relativa;
}