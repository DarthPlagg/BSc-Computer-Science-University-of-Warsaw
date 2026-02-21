#include "zbior_ary.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h> 

int64_t q;

zbior_ary ciag_arytmetyczny(int32_t a, int32_t qq, int32_t b){ 
    q = (int64_t)qq;
    zbior_ary res;
    res.n = 1;
    res.pairs = (pair*)malloc(sizeof(pair));
    res.pairs[0].first = (int64_t)a;
    res.pairs[0].second = (int64_t)b;
    return res;
}

zbior_ary singleton(int32_t a){
    zbior_ary res;
    res.n = 1;
    res.pairs = (pair*)malloc(sizeof(pair));
    res.pairs[0].first = (int64_t)a;
    res.pairs[0].second = (int64_t)a;
    return res;
}

static int64_t modq(int64_t x){
    int64_t r = x % q;
    return (r < 0) ? r + q : r;
}

static bool intersect(pair a, pair b){
    // Sprawdź czy przedziały się stykają lub nakładają (uwzględniając q)
    if (a.second < b.first) {
        return (b.first - a.second == q);
    }
    if (b.second < a.first) {
        return (a.first - b.second == q);
    }
    return true;
}

static uint32_t find_q_end(uint32_t q_start, zbior_ary A){
    if(q_start >= A.n) return A.n;
    uint32_t i = q_start;
    int64_t mod_start = modq(A.pairs[q_start].first);
    while(i < A.n && modq(A.pairs[i].first) == mod_start){
        i++;
    }
    return i;
}

// Funkcja pomocnicza: kopiuje grupę przedziałów do wyniku
static void copy_group(zbior_ary *res, uint32_t *cur, zbior_ary src, 
                       uint32_t start, uint32_t end) {
    for (uint32_t i = start; i < end; i++) {
        res->pairs[(*cur)++] = src.pairs[i];
    }
} 

// Funkcja pomocnicza: scala dwa przedziały jeśli się przecinają/stykają
static void merge_if_intersect(pair *current, pair other) {
    if (current->second < other.second) {
        current->second = other.second;
    }
}

// Funkcja pomocnicza: scala przedziały z tej samej klasy mod q
static void merge_same_modq_group(zbior_ary *res, uint32_t *cur,
                                   zbior_ary A, uint32_t *q_startA, uint32_t q_endA,
                                   zbior_ary B, uint32_t *q_startB, uint32_t q_endB) {
    while (*q_startA < q_endA && *q_startB < q_endB) {
        // Wybierz mniejszy przedział jako punkt startowy
        if (A.pairs[*q_startA].first < B.pairs[*q_startB].first) {
            res->pairs[*cur] = A.pairs[(*q_startA)++];
        } else {
            res->pairs[*cur] = B.pairs[(*q_startB)++];
        }
        
        // Scalaj wszystkie przecinające się/stykające przedziały
        bool has_merged;
        do {
            has_merged = false;
            
            while (*q_startB < q_endB && 
                   intersect(res->pairs[*cur], B.pairs[*q_startB])) {
                merge_if_intersect(&res->pairs[*cur], B.pairs[*q_startB]);
                (*q_startB)++;
                has_merged = true;
            }
            
            while (*q_startA < q_endA && 
                   intersect(res->pairs[*cur], A.pairs[*q_startA])) {
                merge_if_intersect(&res->pairs[*cur], A.pairs[*q_startA]);
                (*q_startA)++;
                has_merged = true;
            }
        } while (has_merged);
        
        (*cur)++;
    }
    
    // Dodaj pozostałe przedziały
    while (*q_startA < q_endA) {
        res->pairs[(*cur)++] = A.pairs[(*q_startA)++];
    }
    while (*q_startB < q_endB) {
        res->pairs[(*cur)++] = B.pairs[(*q_startB)++];
    }
}

// Funkcja pomocnicza: znajduje przecięcie dwóch grup z tej samej klasy mod q
static void intersect_same_modq_group(zbior_ary *res, uint32_t *cur,
                                      zbior_ary A, uint32_t q_startA, uint32_t q_endA,
                                      zbior_ary B, uint32_t q_startB, uint32_t q_endB) {
    uint32_t idxA = q_startA;
    uint32_t idxB = q_startB;
    
    while (idxA < q_endA && idxB < q_endB) {
        pair a = A.pairs[idxA];
        pair b = B.pairs[idxB];
        
        // Oblicz przecięcie przedziałów
        int64_t start = (a.first > b.first) ? a.first : b.first;
        int64_t end = (a.second < b.second) ? a.second : b.second;
        
        // Jeśli przedziały się przecinają, zapisz przecięcie
        if (start <= end) {
            res->pairs[*cur].first = start;
            res->pairs[*cur].second = end;
            (*cur)++;
        }
        
        // Przesuń indeks tego przedziału, który kończy się wcześniej
        if (a.second < b.second) {
            idxA++;
        } else if (a.second > b.second) {
            idxB++;
        } else {
            idxA++;
            idxB++;
        }
    }
}

// Funkcja pomocnicza: wycina przedziały z B z przedziałów A (dla tej samej klasy mod q)
static void subtract_same_modq_group(zbior_ary *res, uint32_t *cur,
                                     zbior_ary A, uint32_t q_startA, uint32_t q_endA,
                                     zbior_ary B, uint32_t q_startB, uint32_t q_endB) {
    uint32_t idxB = q_startB;
    
    for (uint32_t idxA = q_startA; idxA < q_endA; idxA++) {
        int64_t start = A.pairs[idxA].first;
        int64_t end = A.pairs[idxA].second;
        
        // Pomiń przedziały z B, które są przed aktualnym przedziałem z A
        while (idxB < q_endB && B.pairs[idxB].second < start) {
            idxB++;
        }
        
        // Wytnij wszystkie przedziały z B, które się przecinają
        while (idxB < q_endB && B.pairs[idxB].first <= end) {
            int64_t b_start = B.pairs[idxB].first;
            int64_t b_end = B.pairs[idxB].second;
            
            if (b_start <= start && b_end >= end) {
                // B całkowicie przykrywa A - nic nie zostaje
                start = end + 1;
                break;
                
            } else if (b_start <= start) {
                // B przykrywa lewą część A
                start = b_end + q;
                
            } else if (b_end >= end) {
                // B przykrywa prawą część A
                end = b_start - q;
                break;
                
            } else {
                // B jest w środku A - zapisz lewą część, kontynuuj z prawą
                if (start <= b_start - q) {
                    res->pairs[*cur].first = start;
                    res->pairs[*cur].second = b_start - q;
                    (*cur)++;
                }
                start = b_end + q;
            }
            idxB++;
        }
        
        // Zapisz to, co zostało z przedziału
        if (start <= end) {
            res->pairs[*cur].first = start;
            res->pairs[*cur].second = end;
            (*cur)++;
        }
    }
}

// Funkcja pomocnicza: finalizuje wynik (przycina tablicę do rzeczywistego rozmiaru)
static void finalize_result(zbior_ary *res, uint32_t cur) {
    res->n = cur;
    if (cur > 0) {
        pair *tmp = (pair*)realloc(res->pairs, res->n * sizeof(pair));
        if (tmp != NULL) {
            res->pairs = tmp;
        }
    } else {
        free(res->pairs);
        res->pairs = NULL;
    }
}

zbior_ary suma(zbior_ary A, zbior_ary B) {
    // Scala dwa zbiory A i B.
    // Zbiory są posortowane według (mod q, wartość).
    // Przetwarzamy grupy o tej samej wartości mod q.

    zbior_ary res;
    res.n = (A.n + B.n) * 2 + 10;
    res.pairs = (pair*)malloc(res.n * sizeof(pair));
    
    uint32_t q_startA = 0, q_startB = 0;
    uint32_t q_endA, q_endB;
    uint32_t cur = 0;
    
    q_endA = find_q_end(q_startA, A);
    q_endB = find_q_end(q_startB, B);
    
    while (q_startA < A.n && q_startB < B.n) {
        int64_t modA = modq(A.pairs[q_startA].first);
        int64_t modB = modq(B.pairs[q_startB].first);
        
        if (modA < modB) {
            copy_group(&res, &cur, A, q_startA, q_endA);
            q_startA = q_endA;
            q_endA = find_q_end(q_startA, A);
        } else if (modA > modB) {
            copy_group(&res, &cur, B, q_startB, q_endB);
            q_startB = q_endB;
            q_endB = find_q_end(q_startB, B);
        } else {
            merge_same_modq_group(&res, &cur, A, &q_startA, q_endA,
                                  B, &q_startB, q_endB);
            q_endA = find_q_end(q_startA, A);
            q_endB = find_q_end(q_startB, B);
        }
    }
    
    // Dodaj pozostałe grupy
    while (q_startA < A.n) {
        copy_group(&res, &cur, A, q_startA, q_endA);
        q_startA = q_endA;
        q_endA = find_q_end(q_startA, A);
    }
    while (q_startB < B.n) {
        copy_group(&res, &cur, B, q_startB, q_endB);
        q_startB = q_endB;
        q_endB = find_q_end(q_startB, B);
    }
    
    finalize_result(&res, cur);
    return res;
}

zbior_ary iloczyn(zbior_ary A, zbior_ary B) {
    // Oblicza część wspólną zbiorów A i B.
    // Przedziały mogą się przecinać tylko w tej samej klasie mod q.
    zbior_ary res;
    res.n = (A.n + B.n)*2 + 10;
    res.pairs = (pair*)malloc(res.n * sizeof(pair));
    
    uint32_t q_startA = 0, q_startB = 0;
    uint32_t q_endA = 0, q_endB = 0;
    uint32_t cur = 0;
    
    if (A.n > 0) q_endA = find_q_end(q_startA, A);
    if (B.n > 0) q_endB = find_q_end(q_startB, B);
    
    while (q_startA < A.n && q_startB < B.n) {
        int64_t modA = modq(A.pairs[q_startA].first);
        int64_t modB = modq(B.pairs[q_startB].first);
        
        if (modA < modB) {
            q_startA = q_endA;
            q_endA = find_q_end(q_startA, A);
        } else if (modA > modB) {
            q_startB = q_endB;
            q_endB = find_q_end(q_startB, B);
        } else {
            intersect_same_modq_group(&res, &cur, A, q_startA, q_endA,
                                      B, q_startB, q_endB);
            q_startA = q_endA;
            q_endA = find_q_end(q_startA, A);
            q_startB = q_endB;
            q_endB = find_q_end(q_startB, B);
        }
    }
    
    finalize_result(&res, cur);
    return res;
}

zbior_ary roznica(zbior_ary A, zbior_ary B) {
    // Oblicza różnicę A \ B (elementy z A, które nie należą do B).
    // Wycinamy przedziały z B z przedziałów A 
    zbior_ary res;
    res.n = (A.n + B.n) * 2 + 10;
    res.pairs = (pair*)malloc(res.n * sizeof(pair));
    
    uint32_t q_startA = 0, q_startB = 0;
    uint32_t q_endA = 0, q_endB = 0;
    uint32_t cur = 0;
    
    if (A.n > 0) q_endA = find_q_end(q_startA, A);
    if (B.n > 0) q_endB = find_q_end(q_startB, B);
    
    while (q_startA < A.n) {
        if (q_startB >= B.n) {
            // B się skończył - kopiuj resztę A
            copy_group(&res, &cur, A, q_startA, q_endA);
            q_startA = q_endA;
            q_endA = find_q_end(q_startA, A);
            continue;
        }
        
        int64_t modA = modq(A.pairs[q_startA].first);
        int64_t modB = modq(B.pairs[q_startB].first);
        
        if (modA < modB) {
            copy_group(&res, &cur, A, q_startA, q_endA);
            q_startA = q_endA;
            q_endA = find_q_end(q_startA, A);
        } else if (modA > modB) {
            q_startB = q_endB;
            q_endB = find_q_end(q_startB, B);
        } else {
            subtract_same_modq_group(&res, &cur, A, q_startA, q_endA,
                                     B, q_startB, q_endB);
            q_startA = q_endA;
            q_endA = find_q_end(q_startA, A);
            q_startB = q_endB;
            q_endB = find_q_end(q_startB, B);
        }
    }
    
    finalize_result(&res, cur);
    return res;
}

static int32_t compare_interval_with_value(pair interval, int64_t x) {
    int64_t mod_interval = modq(interval.first);
    int64_t mod_x = modq(x);
    
    // Najpierw porównaj mod q
    if (mod_interval < mod_x) return -1;
    if (mod_interval > mod_x) return 1;
    
    // Ta sama klasa mod q - porównaj pozycje
    if (interval.second < x) return -1;  
    if (interval.first > x) return 1;     
    return 0;                              
}

bool nalezy(zbior_ary A, int32_t x) {

    if (A.n == 0 || A.pairs == NULL) {
        return false;
    }
    uint32_t left = 0, right = A.n;
    
    while (left < right) {
        uint32_t mid = left + (right - left) / 2;
        int32_t cmp = compare_interval_with_value(A.pairs[mid], (int64_t)x);
        
        if (cmp < 0) {
            left = mid + 1;
        } else if (cmp > 0) {
            right = mid;
        } else {
            return true;
        }
    }
    
    return false;
}

unsigned moc(zbior_ary A){
    // Zwraca liczbę elementów w zbiorze A.
    // Dla każdego przedziału [a, b] liczba elementów to (b - a) / q + 1.

    uint32_t suma = 0;
    for (uint32_t i = 0; i < A.n; i++) {
        int64_t diff = A.pairs[i].second - A.pairs[i].first;
        suma += (uint32_t)(diff / q) + 1;
    }
    return suma;
}

unsigned ary(zbior_ary A){
    // Zwraca Ary_q(A) - liczbę przedziałów w reprezentacji zbioru A.
    return A.n;
}
