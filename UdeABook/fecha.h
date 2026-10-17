#ifndef FECHA_H
#define FECHA_H

class Fecha{
private:
    unsigned int Anio;
    unsigned int Mes;
    unsigned int Dia;
    unsigned int Hora;
    unsigned int Minuto;
    // Método privado auxiliar para empaquetar la fecha en un solo número
    unsigned long long convertirAEntero() const;
public:
    //constructores
    // Orden de los parametros: dia, mes, anio, hora (igual que en el diagrama).
    // Ejemplo: Fecha(15, 10, 2026, 8) = 15 de octubre de 2026, 8 horas.
    Fecha(unsigned int dia = 1, unsigned int mes = 1, unsigned int anio = 2000,unsigned int hora = 0, unsigned int minuto = 0);
    //metodos de sobrecarga
    bool operator<(const Fecha& otra) const; //Se pasa por referencia constante para evitar copias.
    bool operator==(const Fecha& otra)const;  //usa const al final para evitar modificar el objeto actual
    // Métodos de acceso
    unsigned int getDia() const;
    unsigned int getMes() const;
    unsigned int getAnio() const;
    unsigned int getHora() const;
    unsigned int getMinuto() const;
    void setDia(unsigned int nuevoDia);
    void setMes(unsigned int nuevoMes);
    void setAnio(unsigned int nuevoAnio);
    void setHora(unsigned int nuevaHora);
    void setMinuto(unsigned int nuevoMinuto);
};

#endif // FECHA_H