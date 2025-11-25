#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <set>
#include <charconv>
#include <cstdio>
#include <chrono>
#include "config.h"
using namespace std;

int n_vars;
int n_clauses;

// 座位變數編碼
inline int student_seat_to_var(int student_id, int row, int col){
    return (student_id - 1) * R * C + (row - 1) * C + col;
}

void generate_cnf() {
    using namespace std::chrono;
    auto t0 = steady_clock::now();

    ifstream in("student_choice.txt");
    if (!in) {
        cerr << "Error opening student_choice.txt\n";
        return;
    }

    in >> n_students >> R >> C;
    n_vars = n_students * R * C;

    vector<set<pair<int,int>>> student_choices(n_students + 1);
    vector<vector<int>> sit_with(n_students + 1);
    vector<vector<int>> not_sit_with(n_students + 1);
    int id;

    while(in >> id){
        stringstream ss;
        string temp_s;
        getline(in, temp_s);
        getline(in, temp_s);
        ss << temp_s;
        int r1, c1, r2, c2;
        while(ss >> r1){
            ss >> c1 >> r2 >> c2;
            for(int i = r1; i <= r2; i++)
                for(int j = c1; j <= c2; j++)
                    student_choices[id].insert({i, j});
            char temp;
            ss >> temp;
        }

        getline(in, temp_s);
        ss.clear(); ss.str(temp_s);
        int temp_id;
        while(ss >> temp_id) sit_with[id].push_back(temp_id);

        getline(in, temp_s);
        ss.clear(); ss.str(temp_s);
        while(ss >> temp_id) not_sit_with[id].push_back(temp_id);
    }
    in.close();
    cout << "student choices have been read." << endl;

    // ---- 一次性輸出到最終 CNF ----
    FILE* fout = fopen(cnf_file_name.c_str(), "wb+");
    if (!fout) {
        cerr << "Error opening " << cnf_file_name << endl;
        return;
    }

    // 預留 header 空間，稍後回填子句數
    const int CLAUSE_FIELD_WIDTH = 20;
    string header_prefix = "p cnf ";
    {
        char tmp[64];
        auto res = std::to_chars(tmp, tmp + 64, n_vars);
        header_prefix.append(tmp, res.ptr - tmp);
        header_prefix.push_back(' ');
    }
    string header = header_prefix;
    header.append(CLAUSE_FIELD_WIDTH, ' ');
    header.push_back('\n');
    fwrite(header.c_str(), 1, header.size(), fout);
    long header_clause_pos = header_prefix.size();

    vector<char> buf;
    buf.reserve(1 << 20); // 1MB buffer

    auto flush_buf = [&]() {
        if (!buf.empty()) {
            fwrite(buf.data(), 1, buf.size(), fout);
            buf.clear();
        }
    };
    auto write_int = [&](int v) {
        char tmp[32];
        if (v < 0) {
            buf.push_back('-');
            v = -v;
        }
        auto res = to_chars(tmp, tmp + 32, v);
        buf.insert(buf.end(), tmp, res.ptr);
    };

    n_clauses = 0;

    // -------------------------
    // 1️⃣ 每個座位只能有一個學生
    // -------------------------
    for (int i = 1; i <= R; i++) {
        for (int j = 1; j <= C; j++) {
            for (int k1 = 1; k1 <= n_students; k1++) {
                for (int k2 = k1 + 1; k2 <= n_students; k2++) {
                    buf.push_back('-'); write_int(student_seat_to_var(k1, i, j));
                    buf.push_back(' '); buf.push_back('-'); write_int(student_seat_to_var(k2, i, j));
                    buf.push_back(' '); buf.push_back('0'); buf.push_back('\n');
                    n_clauses++;
                    if (buf.size() > (1 << 19)) flush_buf();
                }
            }
        }
    }

    int first_part_clauses = n_clauses;

    // -------------------------
    // 2️⃣ 學生偏好與限制
    // -------------------------
    for (int n = 1; n <= n_students; n++) {
        for (auto &p : student_choices[n]) {
            write_int(student_seat_to_var(n, p.first, p.second));
            buf.push_back(' ');
        }
        buf.push_back('0'); buf.push_back('\n');
        n_clauses++;

        for (auto &p1 : student_choices[n]) {
            for (auto &p2 : student_choices[n]) {
                if (p1 != p2) {
                    buf.push_back('-'); write_int(student_seat_to_var(n, p1.first, p1.second));
                    buf.push_back(' '); buf.push_back('-'); write_int(student_seat_to_var(n, p2.first, p2.second));
                    buf.push_back(' '); buf.push_back('0'); buf.push_back('\n');
                    n_clauses++;
                    if (buf.size() > (1 << 19)) flush_buf();
                }
            }
        }

        for (int r = 1; r <= R; r++)
            for (int c = 1; c <= C; c++)
                if (student_choices[n].find({r, c}) == student_choices[n].end()) {
                    buf.push_back('-'); write_int(student_seat_to_var(n, r, c));
                    buf.push_back(' '); buf.push_back('0'); buf.push_back('\n');
                    n_clauses++;
                    if (buf.size() > (1 << 19)) flush_buf();
                }

        for (int m : sit_with[n]) {
            for (auto &p1 : student_choices[n]) {
                buf.push_back('-'); write_int(student_seat_to_var(n, p1.first, p1.second));
                buf.push_back(' ');
                if (p1.first > 1) write_int(student_seat_to_var(m, p1.first - 1, p1.second)), buf.push_back(' ');
                if (p1.first < R) write_int(student_seat_to_var(m, p1.first + 1, p1.second)), buf.push_back(' ');
                if (p1.second > 1) write_int(student_seat_to_var(m, p1.first, p1.second - 1)), buf.push_back(' ');
                if (p1.second < C) write_int(student_seat_to_var(m, p1.first, p1.second + 1)), buf.push_back(' ');
                buf.push_back('0'); buf.push_back('\n');
                n_clauses++;
                if (buf.size() > (1 << 19)) flush_buf();
            }
        }

        for (int m : not_sit_with[n]) {
            for (auto &p1 : student_choices[n]) {
                auto add_pair = [&](int r, int c){
                    buf.push_back('-'); write_int(student_seat_to_var(n, p1.first, p1.second));
                    buf.push_back(' '); buf.push_back('-'); write_int(student_seat_to_var(m, r, c));
                    buf.push_back(' '); buf.push_back('0'); buf.push_back('\n');
                    n_clauses++;
                    if (buf.size() > (1 << 19)) flush_buf();
                };
                if (p1.first > 1) add_pair(p1.first - 1, p1.second);
                if (p1.first < R) add_pair(p1.first + 1, p1.second);
                if (p1.second > 1) add_pair(p1.first, p1.second - 1);
                if (p1.second < C) add_pair(p1.first, p1.second + 1);
            }
        }
    }
    int second_part_clauses = n_clauses - first_part_clauses;
    flush_buf();

    // -------------------------
    // 回填 clause 數到 header
    // -------------------------
    if (fseek(fout, header_clause_pos, SEEK_SET) == 0) {
        char tmp[64];
        int len = snprintf(tmp, sizeof(tmp), "%d", n_clauses);
        string padded(CLAUSE_FIELD_WIDTH - len, ' ');
        padded.append(tmp, len);
        fwrite(padded.c_str(), 1, padded.size(), fout);
    }

    fclose(fout);

    auto t1 = steady_clock::now();
    cout << "first part clauses: " << first_part_clauses << "\n";
    cout << "second part clauses: " << second_part_clauses << "\n";
    cout << "clauses have been generated: " << n_clauses << "\n";
    cout << "time elapsed: " << duration<double>(t1 - t0).count() << " s" << "\n";
}



// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <sstream>
// #include <set>
// #include "config.h"
// using namespace std;


// int n_vars; // number of variables in the cnf file
// int n_clauses; // number of clauses in the cnf file

// // transfer student-seat pair to  number to use in cnf file
// int student_seat_to_var(int student_id, int row, int col){
//     return (student_id - 1) * R * C + (row - 1) * C + col;
// }

// void generate_cnf(){
//     ifstream in("student_choice.txt");
//     // string student_choice;
//     // student_choice = argv[1];
//     // ifstream infile(student_choice);
//     // if (!infile) {
//     //     cerr << "Error opening file: " << student_choice << endl;
//     //     return 1;
//     // }
//     in >> n_students >> R >> C;
//     n_vars = n_students * R * C; // each student can sit in any seat
//     // store student choices
//     vector<set<pair<int, int>>> student_choices(n_students + 1);
//     vector<vector<int>> sit_with(n_students + 1);
//     vector<vector<int>> not_sit_with(n_students + 1);
//     int id;
//     while(in >> id){
//         stringstream ss;
//         string temp_s;
//         // read prefered seats
//         getline(in, temp_s);
//         getline(in, temp_s);
//         ss << temp_s;
//         int r1, c1, r2, c2;
//         while(ss >> r1){
//             ss >> c1 >> r2 >> c2;
//             for(int i = r1; i <= r2; i++){
//                 for(int j = c1; j <= c2; j++){
//                     student_choices[id].insert(make_pair(i, j));
//                 }
//             }
//             char temp;
//             ss >> temp;
//         }

//         // read sit with
//         getline(in, temp_s);
//         ss.clear();
//         ss.str("");
//         ss << temp_s;
//         int temp_id;
//         while(ss >> temp_id) sit_with[id].push_back(temp_id);

//         //read not sit with
//         getline(in, temp_s);
//         ss.clear();
//         ss.str("");
//         ss << temp_s;
//         while(ss >> temp_id) not_sit_with[id].push_back(temp_id);
//     }
//     in.close();
//     cout << "student choices have been read." << endl;

//     // // try to check the choices
//     // for(int i = 1; i <= n_students; i++){
//     //     cout << "Student " << i << " prefers seat :" <<endl;
//     //     for(pair<int, int> p : student_choices[i]) cout << "(" << p.first << ", " << p.second << ")";
//     //     cout << "\n";
//     //     cout << "Wants to sit with: ";
//     //     for(int j : sit_with[i]) cout << j << " ";
//     //     cout << "\nDoes not want to sit with: ";
//     //     for(int j : not_sit_with[i]) cout << j << " ";
//     //     cout << "\n\n";
//     // }

//     // start to generate cnf file
//     ofstream out("temp.cnf");
//     for(int i = 1 ; i <= R; i++){
//         for(int j = 1; j <= C; j++){
//             for(int k1 = 1; k1 <= n_students; k1++){
//                 for(int k2 = k1 + 1; k2 <= n_students; k2++){
//                     out << -student_seat_to_var(k1, i, j) << " " << -student_seat_to_var(k2, i, j) << " 0\n";
//                     n_clauses++;
//                 }
//             }
//         }
//     }
//     for(int n = 1; n <= n_students; n++){
//         for(pair<int, int> p : student_choices[n]){
//             out << student_seat_to_var(n, p.first, p.second) << " ";
//         }
//         out << "0\n";
//         n_clauses++;
//         for(pair<int, int> p1 : student_choices[n]){
//             for(pair<int, int> p2 : student_choices[n]){
//                 if(p1 != p2){
//                     out << -student_seat_to_var(n, p1.first, p1.second) << " " << -student_seat_to_var(n, p2.first, p2.second) << " 0\n";
//                     n_clauses++;
//                 }
//             }
//         }
//         for(int r = 1; r <= R; r++){
//             for(int c = 1; c <= C; c++){
//                 if(student_choices[n].find(make_pair(r, c)) == student_choices[n].end()){
//                     out << -student_seat_to_var(n, r, c) << " 0\n";
//                     n_clauses++;
//                 }
//             }
//         }
//         for(int m : sit_with[n]){
//             for(pair<int, int> p1 : student_choices[n]){
//                 out << -student_seat_to_var(n, p1.first, p1.second) << " ";
//                 if(p1.first > 1) out << student_seat_to_var(m, p1.first - 1, p1.second) << " ";
//                 if(p1.first < R) out << student_seat_to_var(m, p1.first + 1, p1.second) << " ";
//                 if(p1.second > 1) out << student_seat_to_var(m, p1.first, p1.second - 1) << " ";
//                 if(p1.second < C) out << student_seat_to_var(m, p1.first, p1.second + 1) << " ";
//                 out << "0\n";
//                 n_clauses++;
//             }
//         }
//         for(int m : not_sit_with[n]){
//             for(pair<int, int> p1 : student_choices[n]){
//                 if(p1.first > 1){
//                     out << -student_seat_to_var(n, p1.first, p1.second) << " " << -student_seat_to_var(m, p1.first - 1, p1.second) << " 0\n";
//                     n_clauses++;
//                 }
//                 if(p1.first < R){
//                     out << -student_seat_to_var(n, p1.first, p1.second) << " " << -student_seat_to_var(m, p1.first + 1, p1.second) << " 0\n";
//                     n_clauses++;
//                 }
//                 if(p1.second > 1){
//                     out << -student_seat_to_var(n, p1.first, p1.second) << " " << -student_seat_to_var(m, p1.first, p1.second - 1) << " 0\n";
//                     n_clauses++;
//                 }
//                 if(p1.second < C){
//                     out << -student_seat_to_var(n, p1.first, p1.second) << " " << -student_seat_to_var(m, p1.first, p1.second + 1) << " 0\n";
//                     n_clauses++;
//                 }
                
//             }
//         }
//     }
//     out.close();
//     cout << "clauses have been generated." << endl;

//     // write the first line
//     ifstream in_cnf("temp.cnf");
//     ofstream out_cnf(cnf_file_name);
//     out_cnf << "p cnf " << n_vars << " " << n_clauses << "\n";
//     string line;
//     while(getline(in_cnf, line)){
//         out_cnf << line << "\n";
//     }
//     in_cnf.close();
//     out_cnf.close();
//     remove("temp.cnf");
// }

