#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int N = 100000 + 10;
struct Ask {
    int l, r, id;
    friend bool operator<(Ask &a, Ask &b) { return a.r < b.r; };
} ask[N];
ll ans[N];
char s[N];
// SAM
int Tr(char c) { return c - 'a'; }
int cnt, lst, a[N << 1][26], l[N << 1], f[N << 1], mrk[N << 1];
void extend(int c) {
    int p = lst;
    int np = lst = ++cnt;
    l[np] = l[p] + 1;
    mrk[np] = 1;
    while (!a[p][c] && p) a[p][c] = np, p = f[p];
    if (!p)
        f[np] = 1;
    else {
        int q = a[p][c];
        if (l[p] + 1 == l[q])
            f[np] = q;
        else {
            int nq = ++cnt;
            l[nq] = l[p] + 1;
            mrk[nq] = 0;
            memcpy(a[nq], a[q], sizeof(a[q]));
            f[nq] = f[q];
            f[np] = f[q] = nq;
            while (a[p][c] == q) a[p][c] = nq, p = f[p];
        }
    }
}
// Segment Tree
ll tag[N << 1], sum[N << 1];
int idx(int l, int r) { return l + r | l != r; }
void build(int l, int r) {
    int x = idx(l, r), mid = l + r >> 1;
    tag[x] = sum[x] = 0;
    if (l == r) return;
    build(l, mid);
    build(mid + 1, r);
}
void add_tag(int l, int r, ll v) {
    int x = idx(l, r);
    tag[x] += v;
    sum[x] += v * (r - l + 1);
}
void pd(int l, int r) {
    int x = idx(l, r), mid = l + r >> 1;
    if (tag[x]) {
        add_tag(l, mid, tag[x]);
        add_tag(mid + 1, r, tag[x]);
        tag[x] = 0;
    }
}
void up(int l, int r) {
    int mid = l + r >> 1;
    sum[idx(l, r)] = sum[idx(l, mid)] + sum[idx(mid + 1, r)];
}
void upd(int l, int r, int ul, int ur, ll v) {
    int x = idx(l, r), mid = l + r >> 1;
    if (ul <= l && r <= ur) {
        add_tag(l, r, v);
        return;
    }
    pd(l, r);
    if (ul <= mid) upd(l, mid, ul, ur, v);
    if (mid < ur) upd(mid + 1, r, ul, ur, v);
    up(l, r);
}
ll qry(int l, int r, int ql, int qr) {
    int x = idx(l, r), mid = l + r >> 1;
    if (ql <= l && r <= qr) return sum[x];
    pd(l, r);
    ll res = 0;
    if (ql <= mid) res += qry(l, mid, ql, qr);
    if (mid < qr) res += qry(mid + 1, r, ql, qr);
    return res;
}
// LCT
pair<int, int> mp[N];
int fa[N << 1], son[N << 1][2], tmp[N << 1];
int cov[N << 1], ps[N << 1], len[N << 1];
void cov1(int x, int v) {
    cov[x] = v;
    ps[x] = v;
}
void pb(int x) {
    if (cov[x]) {
        cov1(son[x][0], cov[x]);
        cov1(son[x][1], cov[x]);
        cov[x] = 0;
    }
}
void lct_up(int x) {
    if (son[x][0]) ps[x] = max(ps[x], ps[son[x][0]]);
    if (son[x][1]) ps[x] = max(ps[x], ps[son[x][1]]);
}
void rotate(int x) {
    int y = fa[x], w = son[y][1] == x;
    son[y][w] = son[x][w ^ 1];
    if (son[x][w ^ 1]) fa[son[x][w ^ 1]] = y;
    if (fa[y]) {
        int z = fa[y];
        if (son[z][0] == y)
            son[z][0] = x;
        else if (son[z][1] == y)
            son[z][1] = x;
    }
    fa[x] = fa[y];
    fa[y] = x;
    son[x][w ^ 1] = y;
    lct_up(y);
}
bool isroot(int x) { return !fa[x] || son[fa[x]][0] != x && son[fa[x]][1] != x; }
void splay(int x) {
    int s = 1, i = x, y;
    tmp[1] = i;
    while (!isroot(i)) tmp[++s] = i = fa[i];
    while (s) pb(tmp[s--]);
    while (!isroot(x)) {
        y = fa[x];
        if (!isroot(y)) {
            if ((son[fa[y]][0] == y) ^ (son[y][0] == x))
                rotate(x);
            else
                rotate(y);
        }
        rotate(x);
    }
    lct_up(x);
}
int mcnt; 
void access(int x, int v) {
    mcnt = 0;
    cov1(x, v);
    for (int y = 0; x; y = x, x = fa[x]) {
        splay(x);
        son[x][1] = y;
        mp[++mcnt] = make_pair(len[x], ps[x]);
        lct_up(x);
    }
}
int n, q;
void lct_init() {
    for (int i = 0; i <= 2 * n; ++i) {
        son[i][0] = son[i][1] = fa[i] = 0;
        ps[i] = cov[i] = 0;
    }
}
int b[N << 1], x[N << 1];
int ver[N << 1], id[N << 1];
void init() {
    scanf("%d%d", &n, &q);
    scanf("%s", s + 1);
    lct_init();
    build(1, n);
    cnt = 0, lst = ++cnt;
    for (int i = 1; i <= n; i++) extend(Tr(s[i]));
    for (int i = 1; i <= cnt; i++) b[l[i]]++;
    for (int i = 1; i <= n; i++) b[i] += b[i - 1];
    for (int i = 1; i <= cnt; i++) x[b[l[i]]--] = i;
    for (int i = 1; i <= cnt; ++i) {
        id[x[i]] = i;
        if (mrk[x[i]]) ver[l[x[i]]] = i;
    }
    for (int i = 1; i <= cnt; i++) {
        len[i] = l[x[i]];
        if (f[x[i]]) fa[i] = id[f[x[i]]];
    }
    int l, r;
    for (int i = 1; i <= q; i++) {
        scanf("%d%d", &l, &r);
        ask[i] = {l + 1, r + 1, i};
    }
    sort(ask + 1, ask + q + 1);
}
void solve() {
    int p = 1;
    for (int i = 1; i <= n; i++) {
        upd(1, n, 1, i, 1);
        access(ver[i], i);
        int pre = 0;
        for (int j = mcnt; j > 1; j--) {
            if (mp[j].first == 0) continue;
            if (mp[j].second != 0)
                upd(1, n, mp[j].second - mp[j].first + 1, mp[j].second - pre,
                    -1);
            pre = mp[j].first;
        }
        while (p <= q && ask[p].r == i) {
            ans[ask[p].id] = qry(1, n, ask[p].l, ask[p].r);
            p++;
        }
    }
    for (int i = 1; i <= q; i++) printf("%lld\n", ans[i]);
}
int main() {
    init();
    solve();
    return 0;
}