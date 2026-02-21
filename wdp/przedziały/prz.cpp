#include <bits/stdc++.h>
using namespace std;

struct point {
    int64_t x, y;           // współrzędne punktu
};

struct interval {
    int32_t l, r;           // lewy i prawy koniec przedziału (indeksy od 0)
    __int128_t val;         // (x_r - x_l)^2 kwadrat różnicy x, używane do porównywania jakości
    
    void set_quality(int64_t xl, int64_t xr) {
        this->val = (__int128_t)(xr - xl) * (xr - xl);
    }
    
    // porównuje dwa przedziały według jakości, a w remisie według lewego końca
    bool is_better_than(const interval& other) const {
        __int128_t my_len = this->r - this->l + 1;          
        __int128_t other_len = other.r - other.l + 1;       
        __int128_t my_score = this->val * other_len;        // porównanie bez dzielenia
        __int128_t other_score = other.val * my_len;
        if (my_score > other_score) return true;           
        if (my_score < other_score) return false;
        return this->l < other.l;                           // remis wybierz mniejsze l
    }
};

// Dla każdego indeksu l znajduje największe r takie, że przedział [l, r] jest U-ścisły
void find_mxU_strict(vector<point>& el, vector<int32_t>& mxU_strict, int32_t n, int64_t U) {
    deque<int32_t> mnDq, mxDq;  // kolejki monotoniczne dla minimum i maksimum y na przedziale
    int32_t r = 0;               // prawy koniec aktualnego przedziału
    
    for (int32_t l = 0; l < n; l++) {
        // poszerzaj przedział w prawo, dopóki jest U-ścisły
        while (r < n) {
            // utrzymuj kolejki monotoniczne dla minimum
            while (!mnDq.empty() && el[mnDq.back()].y >= el[r].y) {
                mnDq.pop_back();
            }
            mnDq.push_back(r);
            
            // utrzymuj kolejki monotoniczne dla maksimum
            while (!mxDq.empty() && el[mxDq.back()].y <= el[r].y) {
                mxDq.pop_back();
            }
            mxDq.push_back(r);
            
            int64_t curMin = el[mnDq.front()].y;
            int64_t curMax = el[mxDq.front()].y;
            
            if (curMax - curMin <= U) {   // przedział jest U-ścisły
                r++;
            } else {                       // przekroczenie U 
                if (!mnDq.empty() && mnDq.back() == r) mnDq.pop_back();
                if (!mxDq.empty() && mxDq.back() == r) mxDq.pop_back();
                break;
            }
        }
        
        mxU_strict[l] = r - 1;            // zapisz maksymalne r dla tego l
        
        // usuń l z kolejek przed przesunięciem lewego końca
        if (!mnDq.empty() && mnDq.front() == l) mnDq.pop_front();
        if (!mxDq.empty() && mxDq.front() == l) mxDq.pop_front();
        if (r < l + 1) r = l + 1;         // zachowaj r >= l+1
    }
}

// Wybiera tylko maksymalne przedziały U-ścisłe i oblicza ich jakość
void set_max_intervals(vector<point>& el, vector<interval>& mx_inv, vector<int32_t>& mxU_strict, int32_t n) {
    for (int32_t l = 0; l < n; l++) {
        int32_t r = mxU_strict[l];
        // przedział jest maksymalny, gdy nie można go rozszerzyć w lewo
        if (l == 0 || mxU_strict[l - 1] < r) { 
            interval inv;
            inv.l = l;
            inv.r = r;
            inv.set_quality(el[l].x, el[r].x);
            mx_inv.push_back(inv);
        }
    }
}

// Dla każdego indeksu i wybiera najlepszy przedział zawierający i
void find_best_intervals(vector<interval>& mx_inv, vector<pair<int32_t, int32_t>>& ans, int32_t n) { 
    int32_t m = (int32_t)mx_inv.size();   // liczba maksymalnych przedziałów
    int32_t idx = 0;                      // wskaźnik na kolejny przedział do dodania
    deque<int32_t> dq;                    // przechowuje indeksy przedziałów w kolejności malejącej jakości
    
    for (int32_t i = 0; i < n; i++) {
        // dodaj wszystkie przedziały zaczynające się w i
        while (idx < m && mx_inv[idx].l == i) {
            // usuń z końca deque gorsze przedziały
            while (!dq.empty() && mx_inv[idx].is_better_than(mx_inv[dq.back()])) {
                dq.pop_back();
            }
            dq.push_back(idx);
            idx++;
        }
        
        // usuń przedziały, które już się skończyły (nie zawierają i)
        while (!dq.empty() && mx_inv[dq.front()].r < i) {
            dq.pop_front();
        }
        
        // najlepszy przedział jest na początku deque
        if (!dq.empty()) {
            int32_t best_idx = dq.front();
            ans[i] = {mx_inv[best_idx].l + 1, mx_inv[best_idx].r + 1};   // +1 bo indeksy od 1
        } else {
            ans[i] = {i + 1, i + 1};   // przedział jednopunktowy
        }
    }
}

int main() {

    int32_t n; int64_t U;
    cin >> n >> U;
    
    vector<point> el(n);
    for (auto &[x, y] : el) {
        cin >> x >> y;
    }
    
    vector<int32_t> mxU_strict(n, -1);
    find_mxU_strict(el, mxU_strict, n, U);
    
    vector<interval> mx_inv;
    set_max_intervals(el, mx_inv, mxU_strict, n);
    
    vector<pair<int32_t, int32_t>> ans(n);
    find_best_intervals(mx_inv, ans, n);
    
    for (int32_t i = 0; i < n; i++) {
        cout << ans[i].first << " " << ans[i].second << "\n";
    }
}