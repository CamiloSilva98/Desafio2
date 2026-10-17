#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "UdeaBook.h"

using namespace std;

int main()
{

    cout << R"(
 _   _______ _____  ___  ______  _____  _____ _   __
| | | |  _  \  ___|/ _ \ | ___ \|  _  ||  _  | | / /
| | | | | | | |__ / /_\ \| |_/ /| | | || | | | |/ /
| | | | | | |  __||  _  || ___ \| | | || | | |    \
| |_| | |/ /| |___| | | || |_/ /\ \_/ /\ \_/ / |\  \
 \___/|___/ \____/\_| |_/\____/  \___/  \___/\_| \_/)" << endl;

    // la semilla se pone una sola vez para toda la ejecucion
    std::srand((unsigned int)std::time(0));

    std::string carpeta = "datos";
    UdeaBook app;
    if (!app.cargarDatos(carpeta))
    {
        std::cerr << "No se pudieron cargar los datos de \"" << carpeta << "\"" << std::endl;
        return 1;
    }

    while (true)
    {
        std::cout << std::endl << "Ude@Book" << std::endl
                  << "1.- Iniciar sesion" << std::endl
                  << "0.- Salir" << std::endl
                  << "Su opcion: ";
        std::string opcion;
        if (!std::getline(std::cin, opcion))
        {
            std::cout << std::endl;
            break;
        }
        if (!opcion.empty() && opcion[opcion.size() - 1] == '\r')
        {
            opcion.erase(opcion.size() - 1);
        }

        if (opcion == "1")
        {
            Usuario* usuario = app.pedirSesion();
            if (usuario != nullptr)
            {
                app.ejecutarSesion(usuario);
            }
            else
            {
                std::cout << "No se pudo iniciar sesion." << std::endl;
            }
        } else if (opcion == "0")
        {
            break;
        }
        else
        {
            std::cout << "Opcion no valida." << std::endl;
        }
    }
    return 0;
}
