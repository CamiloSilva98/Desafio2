#include "udeaBook.h"
#include "SolicitudAmistad.h"
#include "cargadordatos.h"
#include "guardadordatos.h"
#include <fstream>
#include <iostream>


namespace
{

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

// convierte AAAAMMDDHHMM en una Fecha validando mes, dia (con bisiestos) y hora
bool aFecha(const std::string& texto, Fecha& fecha)
{
    long long valor;
    if (texto.size() != 12 || !aEntero(texto, valor))
    {
        return false;
    }
    int anio = (int)(valor / 100000000);
    int mes = (int)((valor / 1000000) % 100);
    int dia = (int)((valor / 10000) % 100);
    int hora = (int)((valor / 100) % 100);
    int minutos =(int)(valor % 100);

    if (anio < 1 || mes < 1 || mes > 12 || hora > 23 || minutos > 59)
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
    fecha = Fecha(dia, mes, anio, hora, minutos);
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

bool leerOpcion(std::string& opcion)
{
    if (!std::getline(std::cin, opcion))
    {
        return false;
    }
    std::string::size_type inicio = 0;
    std::string::size_type fin = opcion.size();
    while (inicio < fin && (opcion[inicio] == ' ' || opcion[inicio] == '\t'))
    {
        inicio++;
    }
    while (fin > inicio && (opcion[fin - 1] == ' ' || opcion[fin - 1] == '\t' || opcion[fin - 1] == '\r'))
    {
        fin--;
    }
    opcion = opcion.substr(inicio, fin - inicio);
    return true;
}


}

UdeaBook::UdeaBook() : matriz(new MatrizAmistad(0)), cambiosMenores(0)
{
    for (int i = 0; i < NUM_ARCHIVOS; i++)
    {
        archivoSucio[i] = false;
    }
}

UdeaBook::~UdeaBook()
{
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
    cambiosMenores = 0;
    for (int i = 0; i < NUM_ARCHIVOS; i++) {
        archivoSucio[i] = false;
    }
}

bool UdeaBook::cargarDatos(const std::string& carpeta)
{
    liberar();
    std::string base = carpeta.empty() ? "" : carpeta + "/";
    carpetaDatos = base;

    if (!cargarConfig(base + "config.txt") ||
        !cargarUsuarios(base + "usuarios.txt") ||
        !cargarAmistades(base + "amistades.txt") ||
        !cargarSolicitudes(base + "solicitudes.txt")) {
        liberar();
        return false;
    }

    // albumes, fotos y mensajes los lee CargadorDatos; los albumes van antes que las fotos
    CargadorDatos cargador(subRuta);
    long iteraciones = 0;   // por ahora no se usa, despues lo recibe el medidor
    if (cargador.cargarAlbumes(base + "albums.txt", *this, iteraciones) < 0 ||
        cargador.cargarFotos(base + "fotos.txt", *this, iteraciones) < 0 ||
        cargador.cargarMensajes(base + "mensajes.txt", *this, iteraciones) < 0) {
        std::cerr << "No se pudo abrir albums.txt, fotos.txt o mensajes.txt en \"" << carpeta << "\"" << std::endl;
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
        if (emisor < 0 || receptor < 0)
        {
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

void UdeaBook::mostrarPantallaInicio(const Usuario* usuario, const ListaDinamica<Usuario*>& amigosMostrados) const
{
    std::cout << std::endl;
    if (amigosMostrados.estaVacia())
    {
        std::cout << "Aun no tienes amigos para mostrar." << std::endl;
    }
    for (int i = 0; i < amigosMostrados.getCantidad(); i++)
    {
        std::cout << "Foto perfil de amigo " << (i + 1) << ": "
                  << rutaCompleta(amigosMostrados[i]->getRutaFotoPerfil()) << std::endl;
    }
    std::cout << std::endl << "Bienvenido a UdeaBook " << usuario->getNombreUsuario() << "\n\n";
    std::cout << "Menu de opciones:" << std::endl << std::endl
              << "1.- Administrar amistades" << std::endl
              << "2.- Visitar perfil de un amigo" << std::endl
              << "3.- Ver sugerencias de amistad" << std::endl
              << "4.- Administrar mensajes" << std::endl << std::endl
              << ".- Ingrese 'N' o 'n' para alternar el grupo de fotos de perfil de amigos." << std::endl
              << ".- Ingrese 'S' o 's' para cerrar su sesion." << std::endl << std::endl;
}

void UdeaBook::ejecutarSesion(Usuario* usuario)
{
    // el grupo de fotos se elige una vez y se conserva mientras dure la sesion
    ListaDinamica<Usuario*> sinExcluir;
    ListaDinamica<Usuario*> fotos = usuario->elegirAmigosAleatorio(*matriz, sinExcluir);
    bool yaCambio = false;
    std::string aviso;

    while (true)
    {
        revisarGuardado();
        mostrarPantallaInicio(usuario, fotos);
        if (!aviso.empty())
        {
            std::cout << aviso << std::endl;
            aviso = "";
        }
        std::cout << "Su opcion: ";

        std::string opcion;
        if (!leerOpcion(opcion))
        {
            std::cout << std::endl;
            guardarPendientes();
            return;
        }
        if (opcion == "S" || opcion == "s")
        {
            guardarPendientes();
            std::cout << "Sesion cerrada." << std::endl;
            return;
        }
        else if (opcion == "N" || opcion == "n")
        {
            if (yaCambio)
            {
                aviso = "Las fotos de perfil de amigos solo se pueden actualizar una vez.";
            }
            else if (usuario->getAmigos().estaVacia()) {
                aviso = "Aun no tienes amigos, no hay fotos para actualizar.";
            }
            else
            {
                // el nuevo grupo no repite las fotos del anterior (si hay amigos suficientes)
                fotos = usuario->elegirAmigosAleatorio(*matriz, fotos);
                yaCambio = true;
            }
        }
        else if (opcion == "1")
        {
            menuAmistades(usuario);
        }
        else if (opcion == "2")
        {
            aviso = "Visitar perfil de un amigo: todavia no esta disponible.";
        }
        else if (opcion == "3")
        {
            menuSugerencias(usuario);
        }
        else if (opcion == "4")
        {
            aviso = "Administrar mensajes: todavia no esta disponible.";
        }
        else
        {
            aviso = "Opcion no valida.";
        }
    }
}

ListaDinamica<Usuario*> UdeaBook::sugerirAmigos(const Usuario* usuario, int k, ListaDinamica<int>& enComun) const
{
    ListaDinamica<Usuario*> sugeridos;
    enComun.vaciar();
    int n = usuarios.getCantidad();
    if (k < 1 || n == 0)
    {
        return sugeridos;
    }

    // un contador por usuario: al final cuenta cuantos amigos en comun tiene con este usuario
    int* contadores = new int[n];
    for (int i = 0; i < n; i++)
    {
        contadores[i] = 0;
    }

    int yo = usuario->getIndice();
    const ListaDinamica<Usuario*>& misAmigos = usuario->getAmigos();
    for (int a = 0; a < misAmigos.getCantidad(); a++)
    {
        const ListaDinamica<Usuario*>& susAmigos = misAmigos[a]->getAmigos();
        for (int b = 0; b < susAmigos.getCantidad(); b++)
        {
            int candidato = susAmigos[b]->getIndice();
            // no cuentan el mismo usuario ni los que ya son sus amigos
            if (candidato != yo && !matriz->sonAmigos(yo, candidato))
            {
                contadores[candidato]++;
            }
        }
    }

    // arreglo pequeno de tamano k ordenado de mayor a menor: cada candidato se inserta
    // en su lugar y el menor sale, asi no se ordena toda la lista
    int* mejores = new int[k];
    int* valores = new int[k];
    int llenos = 0;
    for (int c = 0; c < n; c++)
    {
        if (contadores[c] == 0)
        {
            continue;
        }
        if (llenos == k && contadores[c] <= valores[k - 1])
        {
            continue;
        }
        int pos = (llenos < k) ? llenos : k - 1;
        // se corren los que tienen menos amigos en comun; con empate queda primero el que ya estaba
        while (pos > 0 && valores[pos - 1] < contadores[c])
        {
            mejores[pos] = mejores[pos - 1];
            valores[pos] = valores[pos - 1];
            pos--;
        }
        mejores[pos] = c;
        valores[pos] = contadores[c];
        if (llenos < k)
        {
            llenos++;
        }
    }

    for (int i = 0; i < llenos; i++)
    {
        sugeridos.agregar(usuarios[mejores[i]]);
        enComun.agregar(valores[i]);
    }

    delete[] contadores;
    delete[] mejores;
    delete[] valores;
    return sugeridos;
}

void UdeaBook::mostrarResultadoSolicitud(Usuario::ResultadoSolicitud resultado, const Usuario* destino) const
{
    const std::string& nombre = destino->getNombreUsuario();
    switch (resultado)
    {
    case Usuario::SOLICITUD_ENVIADA:
        std::cout << "Solicitud enviada a " << nombre << "." << std::endl;
        break;
    case Usuario::SOLICITUD_A_SI_MISMO:
        std::cout << "No puedes enviarte una solicitud a ti mismo." << std::endl;
        break;
    case Usuario::SOLICITUD_YA_AMIGOS:
        std::cout << "Ya eres amigo de " << nombre << "." << std::endl;
        break;
    case Usuario::SOLICITUD_YA_RECIBIDA:
        std::cout << nombre << " ya te envio una solicitud, puedes aceptarla en Administrar amistades." << std::endl;
        break;
    case Usuario::SOLICITUD_YA_ENVIADA:
        std::cout << "Ya le enviaste una solicitud a " << nombre << "." << std::endl;
        break;
    }
}

void UdeaBook::menuAmistades(Usuario* usuario)
{
    while (true) {
        std::cout << std::endl << "Administrar amistades" << std::endl
                  << "1.- Enviar solicitud de amistad" << std::endl
                  << "2.- Aceptar solicitudes de amistad recibidas" << std::endl
                  << "0.- Volver" << std::endl << std::endl
                  << "Su opcion: ";

        std::string opcion;
        if (!leerOpcion(opcion))
        {
            std::cout << std::endl;
            return;
        }
        if (opcion == "1")
        {
            enviarSolicitudPorNombre(usuario);
        }
        else if (opcion == "2")
        {
            menuAceptarSolicitudes(usuario);
        }
        else if (opcion == "0")
        {
            return;
        }
        else
        {
            std::cout << "Opcion no valida." << std::endl;
        }
    }
}

void UdeaBook::enviarSolicitudPorNombre(Usuario* usuario)
{
    std::cout << "Nombre de usuario al que desea enviar la solicitud (Enter para volver): ";
    std::string nombre;
    if (!leerOpcion(nombre))
    {
        std::cout << std::endl;
        return;
    }
    if (nombre.empty())
    {
        return;
    }

    Usuario* destino = buscarUsuario(nombre);
    if (destino == nullptr)
    {
        std::cout << "El usuario " << nombre << " no existe." << std::endl;
        return;
    }
    // enviarSolicitud revisa las reglas: a si mismo, amigos y solicitudes pendientes
    Usuario::ResultadoSolicitud resultado = usuario->enviarSolicitud(destino, *matriz);
    mostrarResultadoSolicitud(resultado, destino);
    if (resultado == Usuario::SOLICITUD_ENVIADA)
    {
        registrarCambio(ARCHIVO_SOLICITUDES, true);
    }
}

void UdeaBook::menuAceptarSolicitudes(Usuario* usuario)
{
    while (true)
    {
        // la lista se vuelve a leer en cada vuelta porque al aceptar una se borra
        const ListaDinamica<SolicitudAmistad*>& pendientes = usuario->getSolicitudes();
        if (pendientes.estaVacia())
        {
            std::cout << "No tienes solicitudes de amistad pendientes." << std::endl;
            return;
        }

        std::cout << std::endl << "Solicitudes de amistad recibidas:" << std::endl;
        for (int i = 0; i < pendientes.getCantidad(); i++)
        {
            std::cout << (i + 1) << ".- "
                      << usuarios[pendientes[i]->getIndiceEmisor()]->getNombreUsuario() << std::endl;
        }
        std::cout << std::endl << "Numero de la solicitud que desea aceptar (0 para volver): ";

        std::string texto;
        long long valor;
        if (!leerOpcion(texto))
        {
            std::cout << std::endl;
            return;
        }
        if (aEntero(texto, valor) && valor <= pendientes.getCantidad())
        {
            if (valor == 0)
            {
                return;
            }
            int posicion = (int)valor - 1;
            Usuario* emisor = usuarios[pendientes[posicion]->getIndiceEmisor()];
            if (usuario->aceptarSolicitud(posicion, emisor, *matriz))
            {
                std::cout << "Ahora eres amigo de " << emisor->getNombreUsuario() << "." << std::endl;
                // primero amistades y luego solicitudes: si se corta entre los dos, queda una solicitud
                // de mas (que al cargar se descarta) y no una amistad perdida
                registrarCambio(ARCHIVO_AMISTADES, true);
                registrarCambio(ARCHIVO_SOLICITUDES, true);
            }
            else
            {
                std::cout << "No se pudo aceptar la solicitud." << std::endl;
            }
        }
        else
        {
            std::cout << "Opcion no valida." << std::endl;
        }
    }
}

void UdeaBook::menuSugerencias(Usuario* usuario)
{
    std::string texto;
    long long valor;

    // se pide K entre 1 y 20
    int k = 0;
    while (true)
    {
        std::cout << "Cuantas sugerencias desea ver (1 a 20, 0 para volver): ";
        if (!leerOpcion(texto))
        {
            std::cout << std::endl;
            return;
        }
        if (aEntero(texto, valor) && valor <= 20)
        {
            if (valor == 0)
            {
                return;
            }
            k = (int)valor;
            break;
        }
        std::cout << "Valor no valido, debe estar entre 1 y 20." << std::endl;
    }

    ListaDinamica<int> enComun;
    ListaDinamica<Usuario*> sugeridos = sugerirAmigos(usuario, k, enComun);
    if (sugeridos.estaVacia())
    {
        std::cout << "Por ahora no hay sugerencias de amistad." << std::endl;
        return;
    }

    std::cout << std::endl << "Sugerencias de amistad:" << std::endl;
    for (int i = 0; i < sugeridos.getCantidad(); i++)
    {
        std::cout << (i + 1) << ".- " << sugeridos[i]->getNombreUsuario() << " (" << enComun[i]
                  << (enComun[i] == 1 ? " amigo en comun)" : " amigos en comun)") << std::endl;
    }
    std::cout << std::endl;

    // se puede enviar solicitud a las que quiera, hasta que escriba 0
    while (true)
    {
        std::cout << "Numero de la sugerencia a la que desea enviar solicitud (0 para volver): ";
        if (!leerOpcion(texto))
        {
            std::cout << std::endl;
            return;
        }
        if (aEntero(texto, valor) && valor <= sugeridos.getCantidad())
        {
            if (valor == 0)
            {
                return;
            }
            Usuario* destino = sugeridos[(int)valor - 1];
            Usuario::ResultadoSolicitud resultado = usuario->enviarSolicitud(destino, *matriz);
            mostrarResultadoSolicitud(resultado, destino);
            if (resultado == Usuario::SOLICITUD_ENVIADA)
            {
                registrarCambio(ARCHIVO_SOLICITUDES, true);
            }
        }
        else
        {
            std::cout << "Opcion no valida." << std::endl;
        }
    }
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

void UdeaBook::registrarCambio(ArchivoDatos archivo, bool critico)
{
    archivoSucio[archivo] = true;
    if (critico) {
        // se guarda ya (junto con lo menor que estuviera pendiente)
        guardarPendientes();
        return;
    }
    // el tiempo se cuenta desde el primer cambio menor que no se ha guardado
    if (cambiosMenores == 0)
    {
        primerCambioMenor = std::chrono::steady_clock::now();
    }
    cambiosMenores++;
    revisarGuardado();
}

void UdeaBook::revisarGuardado()
{
    // sin cambios menores pendientes ni se mira la hora
    if (cambiosMenores == 0)
    {
        return;
    }
    long long segundos = std::chrono::duration_cast<std::chrono::seconds>(
                             std::chrono::steady_clock::now() - primerCambioMenor).count();
    if (cambiosMenores >= MAX_CAMBIOS_MENORES || segundos >= MAX_SEGUNDOS_SIN_GUARDAR)
    {
        guardarPendientes();
    }
}

bool UdeaBook::guardarPendientes()
{
    bool bien = true;

    // solo se reescriben los archivos marcados; si uno falla queda marcado para el proximo intento
    if (archivoSucio[ARCHIVO_AMISTADES])
    {
        if (GuardadorDatos::guardarAmistades(carpetaDatos + "amistades.txt", *this))
        {
            archivoSucio[ARCHIVO_AMISTADES] = false;
        }
        else
        {
            bien = false;
        }
    }
    if (archivoSucio[ARCHIVO_SOLICITUDES])
    {
        if (GuardadorDatos::guardarSolicitudes(carpetaDatos + "solicitudes.txt", *this))
        {
            archivoSucio[ARCHIVO_SOLICITUDES] = false;
        }
        else
        {
            bien = false;
        }
    }
    if (archivoSucio[ARCHIVO_MENSAJES])
    {
        if (GuardadorDatos::guardarMensajes(carpetaDatos + "mensajes.txt", *this))
        {
            archivoSucio[ARCHIVO_MENSAJES] = false;
        }
        else
        {
            bien = false;
        }
    }
    if (archivoSucio[ARCHIVO_FOTOS])
    {
        if (GuardadorDatos::guardarFotos(carpetaDatos + "fotos.txt", *this))
        {
            archivoSucio[ARCHIVO_FOTOS] = false;
        }
        else
        {
            bien = false;
        }
    }

    // el conteo de cambios menores empieza de nuevo aunque algo haya fallado,
    // asi no se reintenta (ni se avisa del error) despues de cada accion
    cambiosMenores = 0;
    if (!bien)
    {
        std::cerr << "No se pudieron guardar todos los archivos en \"" << carpetaDatos << "\"" << std::endl;
    }
    return bien;
}

const std::string& UdeaBook::getSubRuta() const
{
    return subRuta;
}

const std::string& UdeaBook::getcarpeta() const
{
    return carpetaDatos;
}

std::string UdeaBook::rutaCompleta(const std::string& relativa) const
{
    return subRuta + "/" + relativa;
}