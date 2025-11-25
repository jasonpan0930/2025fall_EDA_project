#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
#include "config.h"
using namespace std;


pair<int, pair<int, int>> decode_var(int var){
    int col = var % C;
    if(col == 0) col = C;
    int row = ((var - col) / C) % R + 1;
    int student_id = (var - col - (row - 1) * C) / (R * C) + 1;
    return {student_id, {row, col}};
}

void decode_seat(){
    ifstream in(student_choice_file);
    in >> n_students >> R >> C;
    in.close();

    ifstream sol(minisat_result_file);
    vector<vector<int>> seat_of_student(R, vector<int>(C, -1));
    string temp;
    getline(sol, temp); // skip the first line
    int var;
    while(sol >> var){
        if(var > 0){
            auto [student_id, seat] = decode_var(var);
            auto [r, c] = seat;
            seat_of_student[r - 1][c - 1] = student_id;
        }
    }
    sol.close();

    ofstream seat_assign(seat_assignment_file);

    // 設定欄寬，方便對齊
    int width = 4;

    // 畫表格
    auto print_sep = [&](){
        seat_assign << "+";
        for(int j = 0; j < C; j++){
            for(int k = 0; k < width; k++) seat_assign << "-";
            seat_assign << "+";
        }
        seat_assign << "\n";
    };

    for(int i = 0; i < R; i++){
        print_sep();
        seat_assign << "|";
        for(int j = 0; j < C; j++){
            int val = seat_of_student[i][j];
            if(val == -1) seat_assign << setw(width) << " " << "|";
            else seat_assign << setw(width) << val << "|";
        }
        seat_assign << "\n";
    }
    print_sep();

    seat_assign.close();
}
