#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define NAMESIZE 80
class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, NAMESIZE);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        size_t size = 2 * sizeof(int16_t) + NAMESIZE*sizeof(char); //Espacio para x e y y el array de char de tamaño 80
        alloc_data(size);
        char *tmp=_data;
        memcpy(tmp,name ,sizeof(char)*NAMESIZE );
        tmp+=sizeof(char)*NAMESIZE;
        memcpy(tmp,&x ,sizeof(int16_t)); 
        tmp+=sizeof(int16_t);
        memcpy(tmp,&y ,sizeof(int16_t)); 
    }

    int from_bin(char * data)
    {
        memcpy(name,data ,sizeof(char)*NAMESIZE);
        data+=sizeof(char)*NAMESIZE;
        memcpy(&x,data ,sizeof(int16_t));//x es el destino
        data+=sizeof(int16_t);
        memcpy(&y,data ,sizeof(int16_t));

        return 0;
    }


public:
    char name[NAMESIZE];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    int fd = open("jugador.bin", O_RDWR | O_CREAT, 0666);
    // 1. Serializar el objeto one_w
    one_w.to_bin();
    char * data;
    data=one_w.data();
    // 2. Escribir la serialización en un fichero
    write(fd,data,one_w.size());
    lseek(fd, 0, SEEK_SET);
    // 3. Leer el fichero
    read(fd, data, one_r.size());
    // 4. "Deserializar" en one_r
    one_r.from_bin(data);
    // 5. Mostrar el contenido de one_r
    std::cout<< one_r.name<<"\n";
    std::cout<< one_r.x<<"\n";
    std::cout<< one_r.y<<"\n";

    close(fd);
   

    return 0;
}

