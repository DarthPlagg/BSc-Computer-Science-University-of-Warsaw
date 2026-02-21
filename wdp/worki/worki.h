#ifndef WORKI_H
#define WORKI_H

struct storage; 

struct przedmiot{
    przedmiot *prev = nullptr;
    przedmiot *next = nullptr;
    storage *container = nullptr; 
    bool is_bag = false;          
};

struct worek : public przedmiot{
    int id;
    storage *inner_storage; 
    worek(){ 
        is_bag = true; 
    }
};

przedmiot *nowy_przedmiot();
worek *nowy_worek();
void wloz(przedmiot *co, worek *gdzie);
void wloz(worek *co, worek *gdzie);
void wyjmij(przedmiot *p);
void wyjmij(worek *w);
int w_ktorym_worku(przedmiot *p);
int w_ktorym_worku(worek *w);
int ile_przedmiotow(worek *w);
void na_odwrot(worek *w);
void gotowe();

#endif