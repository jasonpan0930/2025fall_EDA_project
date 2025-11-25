#include <iostream>
#include <vector>
#include <map>
#include <algorithm>   // for shuffle
#include <random>      // for random_device, mt19937
#include <iomanip>    // for setw
#include <fstream>
#include <cstdlib>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, R, C;
    cin >> N >> R >> C;

    int totalSeats = R * C;
    if (N > totalSeats) {
        cerr << "Error: 學生數 N 不能大於座位數 R*C\n";
        return 1;
    }

    // 先建立座位表，全設為 -1
    vector<vector<int>> grid(R, vector<int>(C, -1));

    // 所有座位索引
    vector<pair<int,int>> seats;
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            seats.push_back({i, j});
        }
    }

    // 打亂座位順序
    random_device rd;
    mt19937 g(rd());
    shuffle(seats.begin(), seats.end(), g);

    // 前 N 個位置放學生
    for (int id = 1; id <= N; id++) {
        auto [r, c] = seats[id - 1];
        grid[r][c] = id;
    }

    ofstream fout("answer.txt");
    // 輸出結果
    fout << "y" << endl;
    fout << N << " " << R << " " << C << "\n";
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            fout << setw(3) << grid[i][j] << " ";
        }
        fout << "\n";
    }

    cout << "result: ";
    string command = "cat answer.txt";
    system(command.c_str());
    cout << "the result is also in answer.txt\n";

    return 0;
}
