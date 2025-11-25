#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>   // for shuffle
#include <random>      // for random_device, mt19937
using namespace std;

struct Seat {
    int r, c;
};

void generate_student_choice() {
    int N, R, C;
    cout << "Please input N R C:\n";
    cin >> N >> R >> C;

    vector<vector<int>> ans(R, vector<int>(C));
    map<int, Seat> pos; // 學生編號 → 座標
    cout << "Please input the seating arrangement (-1 for empty seat):\n";
    // 讀取座位配置
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            int id;
            cin >> id;
            ans[i][j] = id;
            if (id != -1) {
                pos[id] = {i + 1, j + 1}; // 1-based
            }
        }
    }

    ofstream fout("student_choice.txt");
    fout << N << " " << R << " " << C << "\n";

    // 建立隨機數引擎（替代 random_shuffle）
    random_device rd;
    mt19937 g(rd());

    srand(time(0));

    for (int id = 1; id <= N; id++) {
        fout << id << "\n";

        // Preferred zone: 至少包含自己的座位
        auto [r, c] = pos[id];
        fout << r << " " << c << " " << r << " " << c;

        // 隨機多加 1/3 ~ 2/3 * grid 個小方框
        int grid = R * C;
        int extra = grid / 3 + rand() % (grid / 3);
        for (int k = 0; k < extra; k++) {
            int r1 = rand() % R + 1, c1 = rand() % C + 1;
            // int r2 = min(R, r1 + rand() % 2);
            // int c2 = min(C, c1 + rand() % 2);
            fout << " ; " << r1 << " " << c1 << " " << r1 << " " << c1;
        }
        fout << "\n";

        // Want to sit with: 鄰居 (必須有學生)
        vector<int> beside;
        vector<int> wants;
        int dr[4] = {1, -1, 0, 0};
        int dc[4] = {0, 0, 1, -1};
        for (int k = 0; k < 4; k++) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr >= 1 && nr <= R && nc >= 1 && nc <= C) {
                int neighbor = ans[nr - 1][nc - 1];
                if (neighbor != -1) beside.push_back(neighbor);
            }
        }
        for( int j : beside ) if(rand() % 4 == 0) wants.push_back(j);
        //清空wants
        // wants.clear();
        if (!wants.empty()) {
            for (int i = 0; i < (int)wants.size(); i++) {
                if (i) fout << " ";
                fout << wants[i];
            }
            fout << "\n";
        } else {
            fout << "-\n";
        }

        // Don’t want to sit with: 隨機 2 個非鄰居
        vector<int> others;
        for (int j = 1; j <= N; j++) {
            if (j == id) continue;
            if (find(beside.begin(), beside.end(), j) == beside.end())
                others.push_back(j);
        }
        shuffle(others.begin(), others.end(), g); // ✅ 改成 std::shuffle
        if (!others.empty()) {
            int cnt = (rand() % ((int)others.size()/10)) + 1;
            for (int i = 0; i < cnt; i++) {
                if (i) fout << " ";
                fout << others[i];
            }
            fout << "\n";
        } else {
            fout << "-\n";
        }
    }

    fout.close();
}
