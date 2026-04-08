#pragma GCC optimize("O3,unroll-loops")
#include <iostream>
#include <vector>
#include <map>
#include <random>

using namespace std;

const int MAXN = 3005;
vector<pair<int, int>> adj[MAXN]; // to, edge_index
int u_edge[4505], v_edge[4505];
uint64_t weight[4505];
bool is_tree_edge[4505];
bool visited[MAXN];
int depth[MAXN];
uint64_t val[MAXN];

void dfs_tree(int u, int p, int d) {
    visited[u] = true;
    depth[u] = d;
    for (auto& edge : adj[u]) {
        int v = edge.first;
        int idx = edge.second;
        if (v == p) continue;
        if (!visited[v]) {
            is_tree_edge[idx] = true;
            dfs_tree(v, u, d + 1);
        } else if (depth[v] < depth[u]) {
            // Back-edge
            is_tree_edge[idx] = false;
        }
    }
}

uint64_t dfs_weight(int u, int p) {
    uint64_t current_val = val[u];
    for (auto& edge : adj[u]) {
        int v = edge.first;
        int idx = edge.second;
        if (v == p) continue;
        if (is_tree_edge[idx] && depth[v] > depth[u]) {
            uint64_t subtree_val = dfs_weight(v, u);
            weight[idx] = subtree_val;
            current_val ^= subtree_val;
        }
    }
    return current_val;
}

int comp2[MAXN];
void dfs_comp2(int u, int c) {
    comp2[u] = c;
    for (auto& edge : adj[u]) {
        int v = edge.first;
        int idx = edge.second;
        if (comp2[v] == 0 && weight[idx] != 0) {
            dfs_comp2(v, c);
        }
    }
}

uint64_t vertex_hash[MAXN];
map<uint64_t, uint64_t> H;

void dfs_hash(int u, int p, uint64_t current_hash) {
    vertex_hash[u] = current_hash;
    for (auto& edge : adj[u]) {
        int v = edge.first;
        int idx = edge.second;
        if (v == p) continue;
        if (is_tree_edge[idx] && depth[v] > depth[u]) {
            uint64_t next_hash = current_hash;
            if (weight[idx] != 0 && H.count(weight[idx])) {
                next_hash ^= H[weight[idx]];
            }
            dfs_hash(v, u, next_hash);
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    for (int i = 1; i <= m; ++i) {
        cin >> u_edge[i] >> v_edge[i];
        adj[u_edge[i]].push_back({v_edge[i], i});
        adj[v_edge[i]].push_back({u_edge[i], i});
    }

    mt19937_64 rng(1337);

    long long total_sum = 0;

    // 1. Connected components
    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfs_tree(i, 0, 1);
        }
    }

    vector<int> parent(n + 1);
    vector<int> sz(n + 1, 1);
    for (int i = 1; i <= n; ++i) parent[i] = i;
    auto find_set = [&](auto& self, int v) -> int {
        if (v == parent[v]) return v;
        return parent[v] = self(self, parent[v]);
    };
    auto union_sets = [&](int a, int b) {
        a = find_set(find_set, a);
        b = find_set(find_set, b);
        if (a != b) {
            parent[b] = a;
            sz[a] += sz[b];
        }
    };

    for (int i = 1; i <= m; ++i) {
        union_sets(u_edge[i], v_edge[i]);
    }

    for (int i = 1; i <= n; ++i) {
        if (parent[i] == i) {
            total_sum += 1LL * sz[i] * (sz[i] - 1) / 2;
        }
    }

    // Assign random weights to back-edges
    for (int i = 1; i <= m; ++i) {
        if (!is_tree_edge[i]) {
            weight[i] = rng();
            val[u_edge[i]] ^= weight[i];
            val[v_edge[i]] ^= weight[i];
        }
    }

    // Compute tree edge weights
    for (int i = 1; i <= n; ++i) {
        if (depth[i] == 1) {
            dfs_weight(i, 0);
        }
    }

    // 2. 2-Edge-Connected Components
    int c2 = 0;
    for (int i = 1; i <= n; ++i) {
        if (comp2[i] == 0) {
            c2++;
            dfs_comp2(i, c2);
        }
    }

    vector<int> sz2(c2 + 1, 0);
    for (int i = 1; i <= n; ++i) {
        sz2[comp2[i]]++;
    }
    for (int i = 1; i <= c2; ++i) {
        total_sum += 1LL * sz2[i] * (sz2[i] - 1) / 2;
    }

    // 3. 3-Edge-Connected Components
    map<uint64_t, int> freq;
    for (int i = 1; i <= m; ++i) {
        if (weight[i] != 0) {
            freq[weight[i]]++;
        }
    }

    for (auto& p : freq) {
        if (p.second >= 2) {
            H[p.first] = rng();
        }
    }

    for (int i = 1; i <= n; ++i) {
        if (depth[i] == 1) {
            dfs_hash(i, 0, 0);
        }
    }

    map<pair<int, uint64_t>, int> comp3_sz;
    for (int i = 1; i <= n; ++i) {
        comp3_sz[{comp2[i], vertex_hash[i]}]++;
    }

    for (auto& p : comp3_sz) {
        long long S = p.second;
        total_sum += S * (S - 1) / 2;
    }

    cout << total_sum << "\n";

    return 0;
}
