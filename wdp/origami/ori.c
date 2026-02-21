#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define EPS 1e-6

typedef struct{
    char type; // Typ przechowywanej danej
    uint32_t k; 
    double x1, y1, x2, y2, x, y, r; // Uzywamy tego co wymaga dany typ
}sheet;

typedef struct{
    uint32_t n, q;
    sheet *sheets;
}input;

static void read(input *in){
    scanf("%u %u", &in->n, &in->q);
    in->sheets = (sheet*)malloc(in->n * sizeof(sheet));
    
    for(uint32_t i = 0; i < in->n; i++){
        char type;
        scanf(" %c", &type);
        in->sheets[i].type = type;
        
        if(type == 'P'){
            scanf("%lf %lf %lf %lf", &in->sheets[i].x1, &in->sheets[i].y1, 
                                      &in->sheets[i].x2, &in->sheets[i].y2);
        }else if(type == 'K'){
            scanf("%lf %lf %lf", &in->sheets[i].x, &in->sheets[i].y, &in->sheets[i].r);
        }else{  // type == 'Z'
            scanf("%u %lf %lf %lf %lf", &in->sheets[i].k, 
                  &in->sheets[i].x1, &in->sheets[i].y1, 
                  &in->sheets[i].x2, &in->sheets[i].y2);
        }
    }
}

static uint32_t eval(sheet *s, uint32_t idx, double x, double y){
    sheet sh = s[idx];
    // Sprawdzamy czy punkt jest w prostokącie/okręgu
    if(sh.type == 'P'){
        return (x >= sh.x1 - EPS && x <= sh.x2 + EPS && 
                y >= sh.y1 - EPS && y <= sh.y2 + EPS);
    }
    
    if(sh.type == 'K'){
        double dx = x - sh.x, dy = y - sh.y;
        return (dx * dx + dy * dy <= sh.r * sh.r + EPS);
    }
    
    // Obliczamy wektor kierunkowy linii zgięcia P1 -> P2
    double dx = sh.x2 - sh.x1;
    double dy = sh.y2 - sh.y1;

    // Wektor od punktu P1 do badanego punktu (x,y)
    double ux = x - sh.x1;
    double uy = y - sh.y1;

    // Iloczyn wektorowy mowi po której stronie prostej znajduje się punkt
    double cross = dx * uy - dy * ux;

    // Jeśli punkt jest po prawej stronie linii po zgięciu nie ma tam papieru
    if (cross < -EPS){
        return 0;
    }

    // Jeśli punkt leży na linii zgięcia, nic nie zmienia
    if (fabs(cross) < EPS){
        return eval(s, sh.k - 1, x, y);
    }

    // Obliczamy rzut punktu (x,y) na prostą P1-P2
    // To jest punkt najbliższy na linii zgięcia
    double dot = ux * dx + uy * dy;           // iloczyn skalarny
    double t = dot / (dx * dx + dy * dy);     // parametr określający położenie rzutu na linii

    // Współrzędne rzutu punktu na linię zgięcia
    double px = sh.x1 + t * dx;
    double py = sh.y1 + t * dy;

    // Odbicie punktu względem linii
    double rx = 2 * px - x;
    double ry = 2 * py - y;

    // Wynik to suma warstaw w odbitym i oryginalnym punkcie
    return eval(s, sh.k - 1, x, y) + eval(s, sh.k - 1, rx, ry);

}

int main(){
    input in;
    read(&in);
    
    for(uint32_t i = 0; i < in.q; i++){
        uint32_t k;
        double x, y;
        scanf("%u %lf %lf", &k, &x, &y);
        printf("%d\n", eval(in.sheets, k - 1, x, y));
    }
    
    free(in.sheets);
    return 0;
}
