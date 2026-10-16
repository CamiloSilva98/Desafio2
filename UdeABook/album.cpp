#include "album.h"
#include <iostream>

using namespace std;

Album::Album(): nombre(""), ruta(""), privacidad(PRIVADO) {}

Album::Album(const string& nombre, const string& ruta, Privacidad privacidad)
    :nombre(nombre), ruta(ruta), privacidad(privacidad){}
void Album::agregarFoto(const Foto& foto){
    fotos.agregar(foto);
}
long Album::verCarrusel(){
    long iteraciones = 0;
    int total= fotos.getCantidad();

    //si el album esta vacio, no hay nada q recorrer
    if(total == 0){
        cout<<"el album "<<nombre<< " no tiene fotos"<<endl;
        return iteraciones;
    }
    //para dar o quitar like:
    // meGusta[i] recuerda, solo durante esta visita, si ya se dio like a la foto i.
    // Asi "quitar" solo deshace el like propio y nunca borra likes de otras personas.
    // (El enunciado no pide validar un like por usuario, por eso no se guarda mas.)
    bool* meGusta = new bool[total];
    for (int i =0; i < total ; i++){
        meGusta[i] = false;
        iteraciones++;
    }
    int posicion = 0; //indice de la foto que se esta viendo
    bool salir = false;
    while(!salir){
        iteraciones++; //cada vuelta del carrusel  = accion de usuario
        Foto& actual = fotos[posicion];
        cout<<"\n--- Album \""<<nombre << "\" - Foto " <<(posicion+1) << " de "<<total<<" ---"<<endl;
        cout<<"Ruta:  "<<actual.getRuta()<<endl;
        cout << "Fecha:  " << actual.getFechaFoto().getDia() << "/"
             << actual.getFechaFoto().getMes() << "/"
             << actual.getFechaFoto().getAnio() << "\n";
        cout << "Tamano: " << actual.getTamanio() << "\n";
        cout << "Likes:  " << actual.getLikes();
        if (meGusta[posicion]){
            cout<<"(incluye tu like)";
        }
        cout<<endl;
        cout << "1. Siguiente  2. Previa  3. Dar/quitar like  0. Salir del album\n";
        cout << "Opcion: ";
        int opcion=-1;
        if(!(cin>>opcion)){
            if(cin.eof()){
                opcion = 0;
            }
            else{
                cin.clear();
                opcion = -1;
            }
        }
        cin.ignore(1000, '\n');
        if(opcion == 0){
            salir = true;
        } else if(opcion == 1){
            posicion = (posicion +1)%total;
        }
        else if(opcion == 2){
            posicion = (posicion -1 + total)% total;
        }
        else if (opcion == 3){
            if (meGusta[posicion]){
                actual.quitarLike();
                meGusta[posicion] =false;
            }else{
                actual.darLike();
                meGusta[posicion] = true;
            }
        }
        else{
            cout<<"opcion no valida"<<endl;
        }
    }

    delete[]meGusta;
    return iteraciones;
}
bool Album::esVsisibleParaAmigo() const{
    return privacidad != PRIVADO;
}
const string& Album::getNombre() const{return nombre;}
const string& Album::getRuta() const{return ruta;}
Album::Privacidad Album::getPrivacidad() const{return privacidad;}
int Album::getCantidadFotos() const{return fotos.getCantidad();}
const Foto& Album::getFoto(int indice) const{return fotos[indice];}

long Album::memoriaBytes() const{
    // sizeof(*this) ya incluye los atributos del objeto (strings, enum y el objeto lista)
    long total = (long)sizeof(*this);
    total+= (long)nombre.capacity() + (long)ruta.capacity();
    // arreglo dinamico de fotos: ocupa TODA la capacidad reservada, no solo lo usado
    total += (long)fotos.getCapacidad() * (long)sizeof(Foto);
    // la ruta de cada foto tambien es un texto dinamico
    for (int i = 0; i < fotos.getCantidad(); i++)
    {
        total += (long)fotos[i].getRuta().capacity();
    }
    return total;
}