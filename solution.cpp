#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Edge {
    int to;
    int cap;
    int flow;
    int rev;
};

const int MAXN = 3005;
vector<Edge> adj[MAXN];
int visited[MAXN];
int visit_id = 0;

void add_edge(int u, int v, int cap) {
    adj[u].push_back({v, cap, 0, (int)adj[v].size()});
    adj[v].push_back({u, cap, 0, (int)adj[u].size() - 1});
}

int dfs(int u, int t, int f) {
    if (u == t) return f;
    visited[u] = visit_id;
    for (auto& edge : adj[u]) {
        if (visited[edge.to] != visit_id && edge.cap - edge.flow > 0) {
            int pushed = dfs(edge.to, t, min(f, edge.cap - edge.flow));
            if (pushed > 0) {
                edge.flow += pushed;
                adj[edge.to][edge.rev].flow -= pushed;
                return pushed;
            }
        }
    }
    return 0;
}

int max_flow(int s, int t, int n) {
    for (int i = 1; i <= n; ++i) {
        for (auto& edge : adj[i]) {
            edge.flow = 0;
        }
    }
    int flow = 0;
    while (true) {
        visit_id++;
        int pushed = dfs(s, t, 1e9);
        if (pushed == 0) break;
        flow += pushed;
    }
    return flow;
}

int parent_node[MAXN];
int dsu_size[MAXN];

int find_set(int v) {
    if (v == parent_node[v])
        return v;
    return parent_node[v] = find_set(parent_node[v]);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        add_edge(u, v, 1);
    }

    vector<int> p(n + 1, 1);
    vector<int> weight(n + 1, 0);

    for (int i = 2; i <= n; ++i) {
        int s = i, t = p[i];
        int f = max_flow(s, t, n);
        weight[i] = f;
        for (int j = i + 1; j <= n; ++j) {
            if (p[j] == t && visited[j] == visit_id) {
                p[j] = i;
            }
        }
    }

    vector<pair<int, pair<int, int>>> tree_edges;
    for (int i = 2; i <= n; ++i) {
        tree_edges.push_back({weight[i], {i, p[i]}});
    }

    sort(tree_edges.rbegin(), tree_edges.rend());

    for (int i = 1; i <= n; ++i) {
        parent_node[i] = i;
        dsu_size[i] = 1;
    }

    long long total_sum = 0;
    for (auto& edge : tree_edges) {
        int w = edge.first;
        int u = edge.second.first;
        int v = edge.second.second;
        
        int root_u = find_set(u);
        int root_v = find_set(v);
        
        if (root_u != root_v) {
            total_sum += 1LL * dsu_size[root_u] * dsu_size[root_v] * w;
            parent_node[root_u] = root_v;
            dsu_size[root_v] += dsu_size[root_u];
        }
    }

    cout << total_sum << "\n";

    return 0;
}