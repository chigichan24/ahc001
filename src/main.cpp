#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>

using namespace std;

#define reps(i,j,k) for(int i = j; i < k; ++i)
#define rep(i,j) reps(i,0,j)
#define W 10001
#define H 10001
#define INF 100000000
#define m_assert(expr, msg) assert(( (void)(msg), (expr) ))

template<class T>
ostream& operator<<(ostream &out, const vector<T> &v){
	out << "{";
	rep(i,v.size()){
		out << v[i] <<", ";
	}
	return out << "}" << endl;
}

int N;
vector< vector < int > > memo(10000, vector<int>(10000, 0));
vector< pair< pair<int,int>, pair<int,int> > > points; // s, base_index, y, x


struct result {
    // x-axis [a, c)
    // y-axis [b, d)

    int a, b, c, d;
    int idnex_points;
    int s;
    result() {
        this->a = -1;
        this->b = -1;
        this->c = -1;
        this->d = -1;
        this->idnex_points = -1;
        this->s = -1;
    }
    
    result(int a, int b, int c, int d) {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->s = (c-a) * (d-b);
    }

    result(pair<int,int> x, pair<int,int> y) {
        this->a = x.first;
        this->b = x.second;
        this->c = y.first;
        this->d = y.second;
    }

    bool update_area() {
        this->s = ((this->c)-(this->a)) * ((this->d)-(this->b));

        if ((this->c)-(this->a) < 0) return false;
        if ((this->d)-(this->b) < 0) return false;
        return true;
    }

    bool update_target(int idx) {
        if (idx < 0 || idx > N-1) return false;
        this->idnex_points = idx;
        return true;
    }

    bool is_contains(int y, int x) {
        return is_contains(make_pair(y, x));
    }

    bool is_contains(pair<int, int> point) {
        m_assert((0 <= point.first && point.first < H), "Illegal coordinate. negative value or over 10000");
        m_assert((0 <= point.second && point.second < W), "Illegal coordinate. negative value or over 10000");

        if (a > point.second || point.second >= c) return false;
        if (b > point.first || point.first >= d) return false;
        return true;
    }

    bool is_contains(pair<int, int> point_a, pair<int, int> point_b) {
        pair<int, int> p = point_a;
        pair<int, int> q = point_b;
        pair<int, int> r = make_pair(point_a.first, point_b.second);
        pair<int, int> s = make_pair(point_b.first, point_a.second);
        return (is_contains(p) || is_contains(q) || is_contains(r) || is_contains(s));
    }

    bool operator<(const result &a)const {
        return this->idnex_points < a.idnex_points;
    }
};

// general inputs
vector<result> ans;

void handle_inputs() {
    scanf("%d", &N);
    rep(i,N) {
        int x, y, r;
        scanf("%d%d%d", &x, &y, &r);
        points.emplace_back(make_pair(make_pair(r, i), make_pair(y, x)));
    }

    // test handle inputs
    assert(points.size() == N);

    // sort with requested area size
    sort(points.begin(), points.end(), greater< pair< pair<int,int> ,pair<int,int> > >());
}

void map_to_base_points() {

    rep(i, N) {
        bool flg = false;
        pair<int, int> p = points[i].second;
        pair<int, int> q = make_pair(points[i].second.first+1, points[i].second.second+1);
        /*
        // これは必ず通る。
        rep(j, i) {
            if (ans[j].is_contains(p, q)) {
                flg = true;
                break;
            }
        }
        assert(!flg);
        */
        result rst = result(p, q);
        rst.idnex_points = points[i].first.second;
        ans.emplace_back(rst);
    }
}

void output() {
    m_assert(ans.size() == N, "Insert all ad?");
    sort(ans.begin(), ans.end());
    rep(i, N) {
        printf("%d %d %d %d\n", ans[i].b, ans[i].a, ans[i].d, ans[i].c);
    }
}

int main() {
    handle_inputs();

    map_to_base_points();

    output();
}