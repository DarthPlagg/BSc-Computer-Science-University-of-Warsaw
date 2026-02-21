#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

static uint32_t min(uint32_t a, uint32_t b){return a > b ? b : a;}
static uint32_t max(uint32_t a, uint32_t b){return a > b ? a : b;}

//przechowujemy dane zgodnie z treścią
typedef struct{
    uint32_t n; 
    uint32_t *net, *dist;
}input;

//zczytaj wejście
static void read(input *in){
    scanf("%u", &in->n);

    in->net = (uint32_t*)malloc(in->n*sizeof(uint32_t));
    in->dist = (uint32_t*)malloc(in->n*sizeof(uint32_t));

    for(uint32_t i = 0; i < in->n; i++){
        scanf("%u%u", &in->net[i], &in->dist[i]);
    }
}

//sprawdź czy istnieją 3 różne sieci
static uint32_t count_distinct_networks(input in){
    if(in.n == 0) return 0;
    
    uint32_t max_net = 0;
    for(uint32_t i = 0; i < in.n; i++){
        if(in.net[i] > max_net) max_net = in.net[i];
    }
    
    uint32_t *seen = (uint32_t*)calloc(max_net + 1, sizeof(uint32_t));
    uint32_t count = 0;
    
    for(uint32_t i = 0; i < in.n; i++){
        if(seen[in.net[i]] == 0){
            seen[in.net[i]] = 1;
            count++;
        }
    }
    
    free(seen);
    return count;
}
//obsługa przypadków gdy nie ma rozwiązania
static int32_t handle_edge_cases(input in){
    //za mało moteli
    if(in.n < 3){
        return -1;
    }
    
    //mniej niż 3 różne sieci
    uint32_t distinct = count_distinct_networks(in);
    if(distinct < 3){
        return -1;
    }
    
    return 0; 
}

//najmniejsza odległość pomiędzy trzema motelami z 3 roznych sieci
static uint32_t find_min(input in){
    /*
    przesuwam prawy koniec dopoki w rozwazanym przedziale nie beda sie znajdyawc motele 3 roznych sieci(zliczam ile razy wystapil motel danej sieci).
    Przesuwam koniec dopoki mam 3 motele w srodku. Koncze z przedzialem typu "a b...b c", liniowo rozwazam ktory srodek bedzie najlepszy.
    Potem przesuwam koniec i powtarzam. Zlozonosc czasowa o(n) zamostyzowana. 
    */

    uint32_t res = UINT32_MAX;
    uint32_t *cnt = (uint32_t*)calloc(in.n + 1, sizeof(uint32_t));
    uint32_t diff = 0;

    for(uint32_t l = 0, r = 0; r < in.n; r++){
        diff += (cnt[in.net[r]] == 0);
        cnt[in.net[r]]++;
        while(diff >= 3){
            if(cnt[in.net[l]] == 1){
                for(uint32_t mid = l+1; mid < r; mid++){
                    uint32_t dist_left = in.dist[mid] - in.dist[l];
                    uint32_t dist_right = in.dist[r] - in.dist[mid];

                    res = min(res, max(dist_left, dist_right));
                }
            }
            cnt[in.net[l]]--;
            diff -= (cnt[in.net[l]] == 0);
            l++;
        }
    }
    free(cnt);
    return res;
}

//największa odległość pomiędzy trzema motelami z 3 roznych sieci
static uint32_t find_max(input in){
    
    //znajduję 3 pierwsze różne sieci z lewej i prawej  
    //dla każdego motelu jako "środek" testuję wszystkie kombinacje
    //warunki: A < B < C, różne sieci

    uint32_t *L = (uint32_t*)calloc(3, sizeof(uint32_t));
    uint32_t *Lnet = (uint32_t*)calloc(3, sizeof(uint32_t));
    uint32_t *Rnet = (uint32_t*)calloc(3, sizeof(uint32_t));
    uint32_t *R = (uint32_t*)calloc(3, sizeof(uint32_t));
    
    uint32_t ln = 0, rn = 0, res = 0;

    for(uint32_t i = 0; i < in.n && ln < 3; i++){
        uint32_t f = 1;
        for(uint32_t j = 0; j < ln; j++){
            if(in.net[i] == Lnet[j]){ 
                f = 0; break; 
            }
        }
        if(f) {
            L[ln] = i;
            Lnet[ln++] = in.net[i];
        }   
    }

    for(int32_t i = (int32_t)in.n - 1; i >= 0 && rn < 3; i--){
        uint32_t f = 1;
        for(uint32_t j = 0; j < rn; j++){
            if(in.net[i] == Rnet[j]){
                f = 0; break; 
            }
        }
        if(f){ 
            R[rn] = (uint32_t)i; 
            Rnet[rn++] = in.net[i];
        }
    }

    if(ln < 3 || rn < 3) return 0;

    //testuj każdy motel jako środek
    for(uint32_t B = 0; B < in.n; B++){
        for(uint32_t i = 0; i < ln; i++){
            if(Lnet[i] == in.net[B]) continue;
            
            for(uint32_t j = 0; j < rn; j++){
                if(Rnet[j] == in.net[B] || Rnet[j] == Lnet[i]) continue;
                
                uint32_t A = L[i], C = R[j];
                if(A >= B || B >= C) continue;

                uint32_t d = min(in.dist[B] - in.dist[A], in.dist[C] - in.dist[B]);
                if(d > res) res = d;
            }
        }
    }
    return res;
}


int main(){
    input in; read(&in);

    //sprawdź przypadki brzegowe
    if(handle_edge_cases(in) == -1){
        printf("0 0\n");
        free(in.net);
        free(in.dist);
        return 0;
    }

    uint32_t ans_min = find_min(in);
    uint32_t ans_max = find_max(in);

    printf("%u %u\n", ans_min, ans_max);
}
