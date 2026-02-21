#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <cstdint>
#include <algorithm>
#include <numeric>

using namespace std;

struct State {
    uint64_t key;
    uint32_t dist;
};

uint64_t compress(const vector<int32_t>& state, const vector<int32_t>& caps) {
    uint64_t result = 0;
    uint64_t multiplier = 1;
    for (uint32_t i = 0; i < state.size(); i++) {
        result += static_cast<uint64_t>(state[i]) * multiplier;
        multiplier *= static_cast<uint64_t>(caps[i] + 1);
    }
    return result;
}

int32_t bfs(const vector<int32_t>& target, const vector<int32_t>& caps) {
    uint32_t n = static_cast<uint32_t>(caps.size());
    vector<int32_t> start(n, 0);
    
    uint64_t start_key = compress(start, caps);
    uint64_t target_key = compress(target, caps);
    
    if (start_key == target_key) return 0;
    
    unordered_set<uint64_t> visited;
    queue<State> q;
    
    visited.insert(start_key);
    q.push({start_key, 0});
    
    vector<int32_t> state(n);
    vector<uint64_t> multipliers(n);
    
    multipliers[0] = 1;
    for (uint32_t i = 1; i < n; i++) {
        multipliers[i] = multipliers[i-1] * static_cast<uint64_t>(caps[i-1] + 1);
    }
    
    while (!q.empty()) {
        State cur = q.front();
        q.pop();
        
        // Dekompresja stanu
        uint64_t temp = cur.key;
        for (uint32_t i = 0; i < n; i++) {
            state[i] = static_cast<int32_t>(temp % static_cast<uint64_t>(caps[i] + 1));
            temp /= static_cast<uint64_t>(caps[i] + 1);
        }
        
        // Napełnienie naczynia do pełna
        for (uint32_t i = 0; i < n; i++) {
            if (state[i] < caps[i]) {
                uint64_t new_key = cur.key + static_cast<uint64_t>(caps[i] - state[i]) * multipliers[i];
                if (new_key == target_key) return static_cast<int32_t>(cur.dist + 1);
                if (visited.find(new_key) == visited.end()) {
                    visited.insert(new_key);
                    q.push({new_key, cur.dist + 1});
                }
            }
        }
        
        // Opróżnienie naczynia
        for (uint32_t i = 0; i < n; i++) {
            if (state[i] > 0) {
                uint64_t new_key = cur.key - static_cast<uint64_t>(state[i]) * multipliers[i];
                if (new_key == target_key) return static_cast<int32_t>(cur.dist + 1);
                if (visited.find(new_key) == visited.end()) {
                    visited.insert(new_key);
                    q.push({new_key, cur.dist + 1});
                }
            }
        }
        
        // Przelanie z naczynia i do naczynia j
        for (uint32_t i = 0; i < n; i++) {
            if (state[i] == 0) continue;
            for (uint32_t j = 0; j < n; j++) {
                if (i == j || state[j] >= caps[j]) continue;
                
                int32_t transfer = min(state[i], caps[j] - state[j]);
                uint64_t new_key = cur.key - static_cast<uint64_t>(transfer) * multipliers[i] 
                                           + static_cast<uint64_t>(transfer) * multipliers[j];
                
                if (new_key == target_key) return static_cast<int32_t>(cur.dist + 1);
                if (visited.find(new_key) == visited.end()) {
                    visited.insert(new_key);
                    q.push({new_key, cur.dist + 1});
                }
            }
        }
    }
    
    return -1;
}

int main() {    
    uint32_t n;
    cin >> n;
    
    if (n == 0) {
        cout << "0\n";
        return 0;
    }
    
    vector<int32_t> caps(n), target(n);
    for (uint32_t i = 0; i < n; i++) {
        cin >> caps[i] >> target[i];
    }
    
    // Sprawdzenie poprawności danych
    int32_t total_cap = 0, target_sum = 0;
    for (uint32_t i = 0; i < n; i++) {
        total_cap += caps[i];
        target_sum += target[i];
    }

    if (target_sum > total_cap) {
        cout << "-1\n"; 
        return 0;
    }
    
    if(total_cap == 0){
        cout << "0\n";
        return 0;
    }

    // Sprawdzenie warunku GCD (wszystkie wartości muszą być podzielne przez GCD pojemności)
    int32_t gcd_val = caps[0];
    for (int32_t x : caps) {
        if(x == 0) continue;
        gcd_val = __gcd(gcd_val, x);
    }

    for (int32_t y : target){
        if(y == 0) continue;
        if(y % gcd_val){
            cout << "-1\n";
            return 0;
        }
    }

    uint32_t tg0 = 0;
    uint32_t l_op = 0;
    for (uint32_t i = 0; i < n; i++){
        l_op += (caps[i] == target[i] && target[i] != 0);
        tg0 += (caps[i] == 0);
    }
    if(l_op + tg0 >= n){
        cout << l_op << "\n";
        return 0;
    }
    
    int32_t result = bfs(target, caps);
    cout << result << "\n";
    
    return 0;
}