#ifndef FECHA_H
#define FECHA_H

class Fecha{
private:
    unsigned int Anio;
    unsigned int Mes;
    unsigned int Dia;
    unsigned int Hora;

    // Método privado auxiliar para empaquetar la fecha en un solo número
    unsigned long long convertirAEntero() const;
public:
    //constructores
    Fecha(unsigned int Dia = 1, unsigned int Mes = 1, unsigned int Anio = 2000, unsigned int Hora = 0);
    //metodos de sobrecarga
    bool operator<(const Fecha& otra) const; //Se pasa por referencia constante para evitar copias.
    bool operator==(const Fecha& otra)const;  //usa const al final para evitar modificar el objeto actual
    // Métodos de acceso
    unsigned int getDia() const;
    unsigned int getMes() const;
    unsigned int getAnio() const;
    unsigned int getHora() const;
    void setDia(unsigned int nuevoDia);
    void setMes(unsigned int nuevoMes);
    void setAnio(unsigned int nuevoAnio);
    void setHora(unsigned int nuevaHora);
};

#endif // FECHA_H
