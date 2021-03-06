#include <bits/stdc++.h>
#define N 10001
using namespace std;


//  ISAP
struct edge { int p, v; ll c, f; };
vector<edge> g[N];
int cur[N], dis[N], gap[N], nc;
const ll inf = 1ll<<20;

void fclr() { nc = 0; }

int addn(int cnt = 1) {
    while(cnt--) {
        g[++nc].clear(); dis[nc] = -1;
        cur[nc] = gap[nc] = 0;
    }
    return nc;
}

void adde(int u, int v, ll c) {
    g[u].push_back({ g[v].size(), v, c, 0 });
    g[v].push_back({ g[u].size() - 1, u, 0, 0 });
    //cout << u << ' ' << v << ' ' << c << endl;
}

ll dfs_isap(int s, int t, int u, ll f) {
    if (u == t) return f;
    ll sf = 0;
    for (int& i = cur[u]; i != g[u].size(); ++i) {
        edge& e = g[u][i]; int v = e.v;
        if (e.c <= e.f || dis[u] != dis[v] + 1)
            continue;
        ll df = dfs_isap(s, t, v, min(f, e.c - e.f));
        e.f += df; g[v][e.p].f -= df;
        sf += df; f -= df; if (!f) return sf;
    }
    if (!--gap[dis[u]]) dis[s] = nc;
    gap[++dis[u]]++; cur[u] = 0;
    return sf;
}

ll isap(int s, int t, ll f = inf) {
    ll sf = 0; queue<int> q;
    gap[0] = 1; dis[t] = 0; q.push(t);
    while(!q.empty()) {
        int u = q.front(); q.pop();
        for (int i = 0; i != g[u].size(); ++i) {
            edge e = g[u][i]; int v = e.v;
            if (g[v][e.p].c <= g[v][e.p].f || dis[v] != -1)
                continue;
            dis[v] = dis[u] + 1;
            gap[dis[v]]++; q.push(v);
        }
    }
    while(dis[s] < nc) sf += dfs_isap(s, t, s, f);
    return sf;
}

//  你的板子很不错，但打完这场之后就变成我的了
template< typename flow_t, typename cost_t >
struct PrimalDual {
    const cost_t inf;

    struct edge {
        int v;
        flow_t f;
        cost_t c;
        int p;
        bool isrev;
    };
    vector< vector< edge > > g;
    vector< cost_t > pot, d;
    vector< int > pv, pe;

    PrimalDual(int V) : g(V), inf(numeric_limits< cost_t >::max()) {}

    void add_edge(int u, int v, flow_t f, cost_t c) {
        g[u].push_back({v, f, c, (int) g[v].size(), false});
        g[v].push_back({u, 0, -c, (int) g[u].size() - 1, true});
    }

    pair<flow_t, cost_t> min_cost_flow(int s, int t, flow_t f) {
        int V = (int) g.size();
        cost_t ret = 0; flow_t f0 = 0;
        using Pi = pair< cost_t, int >;
        priority_queue< Pi, vector< Pi >, greater< Pi > > q;
        pot.assign(V, 0); pe.assign(V, -1); pv.assign(V, -1);
        while(f > 0) {
            d.assign(V, inf);
            q.emplace(0, s); d[s] = 0;
            while(!q.empty()) {
                Pi p = q.top(); q.pop();
                cost_t du = p.first; int u = p.second;
                if(d[u] < du) continue;
                for(int i = 0; i < g[u].size(); i++) {
                    edge &e = g[u][i];
                    cost_t dv = du + e.c + pot[u] - pot[e.v];
                    if(e.f > 0 && d[e.v] > dv) {
                        d[e.v] = dv;
                        pv[e.v] = u, pe[e.v] = i;
                        q.emplace(d[e.v], e.v);
                    }
                }
            }
            if(d[t] == inf) return { f0, ret };
            for(int v = 0; v < V; v++) pot[v] += d[v];
            flow_t df = f;
            for(int v = t; v != s; v = pv[v])
                df = min(df, g[pv[v]][pe[v]].f);
            f -= df; f0 += df;
            ret += df * pot[t];
            for(int v = t; v != s; v = pv[v]) {
                edge &e = g[pv[v]][pe[v]];
                e.f -= df;
                g[v][e.p].f += df;
            }
        }
        return { f0, ret };
    }
};
//  Hopcroft-Karp Algorithm
vector<int> g[N]; int n1, n2;
int lnk[N], dis[N], dm; bool vis[N];
void clr_hk(int n, int m) {
    n1 = n; n2 = m;
    for (int i = 1; i <= n1 + n2; ++i)
        g[i].clear();
}

bool bfs_hk() {
    queue<int> q; dm = INT_MAX;
    fill(dis + 1, dis + n1 + n2 + 1, -1);
    for (int i = 1; i <= n1; ++i)
        if (!lnk[i]) q.push(i), dis[i] = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (dis[u] > dm) break;
        for (int v : g[u]) {
            if (dis[v] != -1) continue;
            dis[v] = dis[u] + 1;
            if (!lnk[v]) dm = dis[v];
            else dis[lnk[v]] = dis[v] + 1, q.push(lnk[v]);
        }
    }
    return dm != INT_MAX;
}

int dfs_hk(int u) {
    for (int v : g[u]) {
        if (vis[v] || dis[v] != dis[u] + 1) continue;
        vis[v] = 1;
        if (lnk[v] && dis[v] == dm) continue;
        if (lnk[v] && !dfs_hk(lnk[v])) continue;
        lnk[v] = u; lnk[u] = v; return 1;
    }
    return 0;
}

int hk() {
    fill (lnk + 1, lnk + n1 + n2 + 1, 0);
    int res = 0;
    while (bfs_hk()) {
        fill (vis + 1, vis + n1 + n2 + 1, 0);
        for (int i = 1; i <= n1; ++i)
            if (!lnk[i] && dfs_hk(i)) res++;
    }
    return res;
}
