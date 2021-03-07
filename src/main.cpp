#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cassert>
#include <ctime>
#include <random>

using namespace std;

#define reps(i,j,k) for(int i = j; i < k; ++i)
#define rep(i,j) reps(i,0,j)
#define W 10001
#define H 10001
#define INF 100000000
#define SQUARE_MARGIN -0.2
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
clock_t start_time;


random_device rnd;
mt19937 mt(rnd());
uniform_int_distribution<int> rand_first_step(0,4); 

struct result {
    // x-axis [a, c)
    // y-axis [b, d)

    int a, b, c, d;
    int idnex_points;
    int s;
    int r;
    result() {
        this->a = -1;
        this->b = -1;
        this->c = -1;
        this->d = -1;
        this->idnex_points = -1;
        this->s = -1;
        this->r = -1;
    }
    
    result(int a, int b, int c, int d) {
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        bool ret = update_area();
        m_assert(ret, "negative area is invalid");
    }

    result(pair<int,int> x, pair<int,int> y) {
        this->a = x.second;
        this->b = x.first;
        this->c = y.second;
        this->d = y.first;
        bool ret = update_area();
        m_assert(ret, "negative area is invalid");
    }

    bool update_area() {
        
        m_assert((this->c)-(this->a) >= 0, "c-a must be positive value");
        m_assert((this->d)-(this->b) >= 0, "d-b must be positive value");
        this->s = ((this->c)-(this->a)) * ((this->d)-(this->b));
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

    bool is_contains(const result &r) {
        return is_contains(make_pair(r.b, r.a), make_pair(r.d, r.c));
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
        result rst = result(p, q);
        rst.idnex_points = points[i].first.second;
        m_assert(rst.update_area(), "initial setup was wrong");
        rst.r = points[i].first.first;
        ans.emplace_back(rst);
    }
}


double eval_ad(int s, int r) {
    m_assert(s>=0, "negative area s is not enough");
    m_assert(r>=0, "negative area r is not enough");
    double q = 1.0 - ((double)min(r,s)/(double)max(r,s));
    double p = 1.0 - q*q;
    return p;
}

void tune_detail(result &rst) {
    // current
    double s_eval = eval_ad(rst.s, rst.r);

    // y - 1
    double t_eval = eval_ad(rst.s - (rst.c - rst.a), rst.r);

    // x - 1
    double u_eval = eval_ad(rst.s - (rst.d - rst.b), rst.r);

    // y - 1 x - 1
    double v_eval = eval_ad(rst.s - (rst.d - rst.b) - (rst.c - rst.a) - 1, rst.r);

    if (rst.s <= 2) {
        return ;
    }

    if (s_eval > t_eval && s_eval > u_eval && v_eval) {
        ;
    } else if (t_eval > s_eval && t_eval > u_eval && t_eval > v_eval) {
        rst.a++;
    } else if (u_eval > s_eval && u_eval > t_eval && u_eval > v_eval){
        rst.b++;
    } else {
        rst.a++;
        rst.b++;
    }
}

void output() {
    m_assert(ans.size() == N, "Insert all ad?");
    sort(ans.begin(), ans.end());
    rep(i, N) {
        tune_detail(ans[i]);
    }
    rep(i, N) {
        printf("%d %d %d %d\n", ans[i].a, ans[i].b, ans[i].c, ans[i].d);
    }
}

bool is_time_limit_over() {
    double time = static_cast<double> (clock()-start_time) / CLOCKS_PER_SEC * 1.0;
    return (time > 4.0);
}

bool move_to_left_up() {
    int cnt = 0;
    rep(i, N) {
        ans[i].a--;
        ans[i].b--;
        if (ans[i].a < 0 || ans[i].b < 0) {
            ans[i].a++;
            ans[i].b++;
            continue;
        }
        // 要望のサイズを超えたら更新を止める
        m_assert(ans[i].c-ans[i].a >= 0, "c-a must be positive value");
        m_assert(ans[i].d-ans[i].b >= 0, "d-b must be positive value");
        m_assert(ans[i].s >= 0, "s must be positive value");
        if (eval_ad((ans[i].c-ans[i].a)*(ans[i].d-ans[i].b), ans[i].r)+SQUARE_MARGIN < eval_ad(ans[i].s, ans[i].r)) {
            ans[i].a++;
            ans[i].b++;
            continue;
        }
        bool flg = false;
        rep(j, N) {
            if (i == j) continue;
            if (ans[j].is_contains(ans[i]) || ans[i].is_contains(ans[j])) {
                flg = true;
                break;
            }
        }

        if (flg) {
            ans[i].a++;
            ans[i].b++;
        } else {
            ans[i].update_area();
            cnt++;
        }
    }
    return cnt > 0;
}

bool move_to_right_down() {
    int cnt = 0;
    rep(i, N) {
        ans[i].c++;
        ans[i].d++;
        if (ans[i].c >= H || ans[i].d >= W) {
            ans[i].c--;
            ans[i].d--;
            continue;
        }
        m_assert(ans[i].c-ans[i].a >= 0, "c-a must be positive value");
        m_assert(ans[i].d-ans[i].b >= 0, "d-b must be positive value");
        m_assert(ans[i].s >= 0, "s must be positive value");
        // 要望のサイズを超えたら更新を止める
        if (eval_ad((ans[i].c-ans[i].a)*(ans[i].d-ans[i].b), ans[i].r)+SQUARE_MARGIN < eval_ad(ans[i].s, ans[i].r)) {
            ans[i].c--;
            ans[i].d--;
            continue;
        }
        bool flg = false;
        rep(j, N) {
            if (i == j) continue;
            if (ans[j].is_contains(ans[i]) || ans[i].is_contains(ans[j])) {
                flg = true;
                break;
            }
        }

        if (flg) {
            ans[i].c--;
            ans[i].d--;
        } else {
            ans[i].update_area();
            cnt++;
        }
    }
    return cnt > 0;
}

bool move_to_left_down() {
    int cnt = 0;
    rep(i, N) {
        ans[i].a--;
        ans[i].d++;
        if (ans[i].a < 0 || ans[i].d >= W) {
            ans[i].a++;
            ans[i].d--;
            continue;
        }
        m_assert(ans[i].c-ans[i].a >= 0, "c-a must be positive value");
        m_assert(ans[i].d-ans[i].b >= 0, "d-b must be positive value");
        m_assert(ans[i].s >= 0, "s must be positive value");
        // 要望のサイズを超えたら更新を止める
        if (eval_ad((ans[i].c-ans[i].a)*(ans[i].d-ans[i].b), ans[i].r)+SQUARE_MARGIN < eval_ad(ans[i].s, ans[i].r))  {
            ans[i].a++;
            ans[i].d--;
            continue;
        }
        bool flg = false;
        rep(j, N) {
            if (i == j) continue;
            if (ans[j].is_contains(ans[i]) || ans[i].is_contains(ans[j])) {
                flg = true;
                break;
            }
        }

        if (flg) {
            ans[i].a++;
            ans[i].d--;
        } else {
            ans[i].update_area();
            cnt++;
        }
    }
    return cnt > 0;
}

bool move_to_right_up() {
    int cnt = 0;
    rep(i, N) {
        ans[i].b--;
        ans[i].c++;
        if (ans[i].c >= H || ans[i].b < 0) {
            ans[i].c--;
            ans[i].b++;
            continue;
        }
        m_assert(ans[i].c-ans[i].a >= 0, "c-a must be positive value");
        m_assert(ans[i].d-ans[i].b >= 0, "d-b must be positive value");
        m_assert(ans[i].s >= 0, "s must be positive value");
        // 要望のサイズを超えたら更新を止める
        if (eval_ad((ans[i].c-ans[i].a)*(ans[i].d-ans[i].b), ans[i].r)+SQUARE_MARGIN < eval_ad(ans[i].s, ans[i].r))  {
            ans[i].c--;
            ans[i].b++;
            continue;
        }
        bool flg = false;
        rep(j, N) {
            if (i == j) continue;
            if (ans[j].is_contains(ans[i]) || ans[i].is_contains(ans[j])) {
                flg = true;
                break;
            }
        }

        if (flg) {
            ans[i].c--;
            ans[i].b++;
        } else {
            ans[i].update_area();
            cnt++;
        }
    }
    return cnt > 0;
}

bool move_to_left() {
    int cnt = 0;
    rep(i, N) {
        ans[i].a--;
        if (ans[i].a < 0) {
            ans[i].a++;
            continue;
        }
        // 要望のサイズを超えたら更新を止める
        if (eval_ad((ans[i].c-ans[i].a)*(ans[i].d-ans[i].b), ans[i].r) < eval_ad(ans[i].s, ans[i].r)) {
            ans[i].a++;
            continue;
        }
        bool flg = false;
        rep(j, N) {
            if (i == j) continue;
            if (ans[j].is_contains(ans[i]) || ans[i].is_contains(ans[j])) {
                flg = true;
                break;
            }
        }

        if (flg) {
            ans[i].a++;
        } else {
            ans[i].update_area();
            cnt++;
        }
    }
    return cnt > 0;
}

bool move_to_right() {
    int cnt = 0;
    rep(i, N) {
        ans[i].c++;
        if (ans[i].c >= H) {
            ans[i].c--;
            continue;
        }
        // 要望のサイズを超えたら更新を止める
        if (eval_ad((ans[i].c-ans[i].a)*(ans[i].d-ans[i].b), ans[i].r) < eval_ad(ans[i].s, ans[i].r)) {
            ans[i].c--;
            continue;
        }
        bool flg = false;
        rep(j, N) {
            if (i == j) continue;
            if (ans[j].is_contains(ans[i]) || ans[i].is_contains(ans[j])) {
                flg = true;
                break;
            }
        }

        if (flg) {
            ans[i].c--;
        } else {
            ans[i].update_area();
            cnt++;
        }
    }
    return cnt > 0;
}

bool move_to_up() {
    int cnt = 0;
    rep(i, N) {
        ans[i].b--;
        if (ans[i].b < 0) {
            ans[i].b++;
            continue;
        }
        // 要望のサイズを超えたら更新を止める
        if (eval_ad((ans[i].c-ans[i].a)*(ans[i].d-ans[i].b), ans[i].r) < eval_ad(ans[i].s, ans[i].r)) {
            ans[i].b++;
            continue;
        }
        bool flg = false;
        rep(j, N) {
            if (i == j) continue;
            if (ans[j].is_contains(ans[i]) || ans[i].is_contains(ans[j])) {
                flg = true;
                break;
            }
        }

        if (flg) {
            ans[i].b++;
        } else {
            ans[i].update_area();
            cnt++;
        }
    }
    return cnt > 0;
}

bool move_to_down() {
    int cnt = 0;
    rep(i, N) {
        ans[i].d++;
        if (ans[i].d >= W) {
            ans[i].d--;
            continue;
        }
        // 要望のサイズを超えたら更新を止める
        if (eval_ad((ans[i].c-ans[i].a)*(ans[i].d-ans[i].b), ans[i].r) < eval_ad(ans[i].s, ans[i].r)) {
            ans[i].d--;
            continue;
        }
        bool flg = false;
        rep(j, N) {
            if (i == j) continue;
            if (ans[j].is_contains(ans[i]) || ans[i].is_contains(ans[j])) {
                flg = true;
                break;
            }
        }

        if (flg) {
            ans[i].d--;
        } else {
            ans[i].update_area();
            cnt++;
        }
    }
    return cnt > 0;
}

bool solve_second_step() {
    bool result = move_to_right();
    result |= move_to_down();
    result |= move_to_left();
    result |= move_to_up();
    return !result;
}

bool solve_first_step() {

    // 左上に動かす
    bool result = move_to_left_up();
    // 右下に動かす
    result |= move_to_right_down();
    // 右上に動かす
    result |= move_to_right_up();
    // 左下に動かす
    result |= move_to_left_down();

    if (rand_first_step(mt) == 0) {
        rep(i,50){
            solve_second_step();
        }
        
    }


    return !result;
}


int main() {
    
    start_time = clock();

    handle_inputs();

    map_to_base_points();

    //solver
    bool first_step_finished = false;
    while(!is_time_limit_over()){
        if (!first_step_finished) {
            first_step_finished = solve_first_step();
        } else {
            bool ret = solve_second_step();
            if (ret) {
                break;
            }
        }
    }

    output();
    return 0;
}