#include "worki.h"

struct storage{
    przedmiot *head = nullptr; 
    worek *owner = nullptr;
    int items_count = 0;
};

static storage *desk = nullptr;
static int id = 0;

static void build_desk(){
    if(!desk){
        desk = new storage();
        desk->owner = nullptr;
        desk->items_count = 0;
        desk->head = nullptr;
        id = 0;
    }
}

static void update_counts(storage *s, int val){
    s->items_count += val;
    
    if(s->owner && s->owner->container){
        s->owner->container->items_count += val;
    }
}

static void detach(przedmiot *p){
    if(!p->container) return;
    storage *s = p->container;

    if(p->prev){
        p->prev->next = p->next;
    }
    if(p->next){
        p->next->prev = p->prev;
    }
    if(s->head == p){
        s->head = p->next;
    }

    int weight = 1;
    if(p->is_bag){
        worek *w = (worek*)p;
        weight = w->inner_storage->items_count;
    }
    update_counts(s, -weight);

    p->prev = nullptr;
    p->next = nullptr;
    p->container = nullptr;
}

static void attach(przedmiot *p, storage *target){
    detach(p); 
    
    p->next = target->head;
    if(target->head){
        target->head->prev = p;
    }
    target->head = p;
    p->container = target;

    int weight = 1;
    if(p->is_bag){
        worek *w = (worek*)p;
        weight = w->inner_storage->items_count;
    }
    update_counts(target, weight);
}

static void clean_storage(storage *s){
    if(!s) return;
    przedmiot *cur = s->head;
    while(cur){
        przedmiot *next = cur->next;
        if(cur->is_bag){
            worek *w = (worek*)cur;
            clean_storage(w->inner_storage);
            delete w->inner_storage;
            delete w; 
        }else{
            delete cur;
        }
        cur = next;
    }
    s->head = nullptr;
    s->items_count = 0;
}

void gotowe(){
    if(desk){
        clean_storage(desk);
        delete desk;
        desk = nullptr;
    }
    id = 0;
}

przedmiot *nowy_przedmiot(){
    build_desk();
    przedmiot *p = new przedmiot();
    attach(p, desk);
    return p;
}

worek *nowy_worek(){
    build_desk();
    worek *w = new worek();
    w->id = id++;
    
    w->inner_storage = new storage();
    w->inner_storage->owner = w;
    w->inner_storage->items_count = 0;
    
    attach(w, desk);
    return w;
}

void wloz(przedmiot *co, worek *gdzie){
    attach(co, gdzie->inner_storage);
}

void wloz(worek *co, worek *gdzie){
    attach(co, gdzie->inner_storage);
}

void wyjmij(przedmiot *p){
    attach(p, desk);
}

void wyjmij(worek *w){
    attach(w, desk);
}

int w_ktorym_worku(przedmiot *p){
    if(!p || !p->container){
        return -1;
    }
    worek *owner = p->container->owner;
    return owner ? owner->id : -1;
}

int w_ktorym_worku(worek *w){
    if(!w || !w->container){
        return -1;
    }
    worek *owner = w->container->owner;
    return owner ? owner->id : -1;
}

int ile_przedmiotow(worek *w){
    if(!w){
        return 0;
    }
    return w->inner_storage->items_count;
}

void na_odwrot(worek *w){
    if(!w) return;
    build_desk();

    storage *old_desk = desk;
    storage *old_inner = w->inner_storage;

    bool on_desk = (w->container == desk);
    if(on_desk){
        detach(w);
    }

    desk = old_inner;
    w->inner_storage = old_desk;

    desk->owner = nullptr;
    w->inner_storage->owner = w;

    if(on_desk){
        attach(w, desk);
    }
}