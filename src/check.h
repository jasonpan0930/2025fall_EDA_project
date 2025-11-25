// validate_seating_with_empty_cells.cpp
// C++17 - robust parser that handles empty cells in ASCII seat table
#include <bits/stdc++.h>
using namespace std;

struct Rect { int r1,c1,r2,c2; };

static string trim(const string &s){
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a==string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b-a+1);
}

static vector<int> parse_int_list(const string &s){
    vector<int> out;
    string t = trim(s);
    if (t.empty() || t == "-") return out;
    istringstream iss(t);
    int x;
    while (iss >> x) out.push_back(x);
    return out;
}

// parse available-seat line into vector<Rect>
// supports: "-" (empty -> no restriction)
// or "r1 c1 r2 c2 ; r3 c3 r4 c4"  OR "r1 c1 r2 c2 r3 c3 r4 c4"
static vector<Rect> parse_rects(const string &line){
    string s = trim(line);
    vector<Rect> rects;
    if (s.empty() || s == "-") return rects;
    // try ';' separated
    {
        vector<string> parts;
        string cur;
        istringstream iss(s);
        while (getline(iss, cur, ';')) parts.push_back(cur);
        if (parts.size() > 1) {
            for (auto &p: parts) {
                string t = trim(p);
                if (t.empty()) continue;
                istringstream iss2(t);
                int a,b,c,d;
                if (!(iss2 >> a >> b >> c >> d)) throw runtime_error("bad rectangle spec: '" + t + "'");
                if (a>c) swap(a,c); if (b>d) swap(b,d);
                rects.push_back({a,b,c,d});
            }
            return rects;
        }
    }
    // try space multiples
    {
        istringstream iss(s);
        vector<int> nums; int x;
        while (iss >> x) nums.push_back(x);
        if (nums.size() % 4 == 0 && nums.size() >= 4) {
            for (size_t i=0;i<nums.size(); i+=4) {
                int a=nums[i], b=nums[i+1], c=nums[i+2], d=nums[i+3];
                if (a>c) swap(a,c); if (b>d) swap(b,d);
                rects.push_back({a,b,c,d});
            }
            return rects;
        }
    }
    throw runtime_error("bad rectangle spec: '" + s + "'");
}

// === 修正部分：以 '|' 的位置切片，保留空 cell ===
// Parse seat assignment table, preserving empty cells as 0.
// expected_R/expected_C used to validate/pad rows. If expected_R/C == -1, infer from file.
static vector<vector<int>> parse_seat_assignment(const string &path, int expected_R = -1, int expected_C = -1){
    ifstream ifs(path);
    if (!ifs) throw runtime_error("cannot open assignment file: " + path);
    string line;
    vector<vector<int>> grid;
    regex num_re(R"(-?\d+)");
    while (getline(ifs, line)){
        // we still skip simple separator lines that don't look like data rows (heuristic)
        if (line.find('|') == string::npos) continue;

        // find all '|' positions
        vector<size_t> pipe_pos;
        for (size_t i=0;i<line.size();++i) if (line[i] == '|') pipe_pos.push_back(i);

        // if less than 2 pipes, skip line
        if (pipe_pos.size() < 2) continue;

        // collect substrings between adjacent pipes (these correspond to cells)
        vector<string> cells;
        for (size_t k = 0; k + 1 < pipe_pos.size(); ++k){
            size_t start = pipe_pos[k] + 1;
            size_t len = pipe_pos[k+1] - start;
            string cell;
            if (len > 0) cell = line.substr(start, len);
            else cell = string();
            cells.push_back(trim(cell));
        }

        // Sometimes border rows include +---+ etc and will produce cells like "----"; we still attempt to extract numbers
        vector<int> row;
        for (auto &cell : cells){
            if (cell.empty()){
                row.push_back(0); // empty cell -> 0
                continue;
            }
            smatch m;
            if (regex_search(cell, m, num_re)){
                try { row.push_back(stoi(m.str())); }
                catch (...) { row.push_back(0); }
            } else {
                // cell contains no digits -> treat as empty
                row.push_back(0);
            }
        }

        // accept row if it has any column (could be border rows with only non-numeric content but we already map to zeros)
        if (!row.empty()) grid.push_back(row);
    }

    if (grid.empty()) throw runtime_error("no seat rows parsed from assignment file");

    // If expected_C provided, enforce/pad/truncate columns per row
    if (expected_C != -1){
        for (auto &r : grid){
            if ((int)r.size() < expected_C){
                // pad with zeros
                r.resize(expected_C, 0);
            } else if ((int)r.size() > expected_C){
                // truncate extra columns
                r.resize(expected_C);
            }
        }
    }

    // If expected_R provided, optionally check / pad rows
    if (expected_R != -1 && (int)grid.size() != expected_R){
        cerr << "Warning: parsed assignment has " << grid.size() << " rows but expected R=" << expected_R << "\n";
        if ((int)grid.size() < expected_R){
            int padC = expected_C != -1 ? expected_C : (int)grid[0].size();
            for (int i = (int)grid.size(); i < expected_R; ++i) grid.push_back(vector<int>(padC, 0));
        }
    }

    // final consistency: ensure all rows have same column count
    size_t C = grid[0].size();
    for (auto &r : grid){
        if (r.size() != C) throw runtime_error("inconsistent columns after padding/truncation");
    }
    return grid;
}

// helper
static bool in_any_rect(int r, int c, const vector<Rect> &rects){
    if (rects.empty()) return true; // empty => no restriction
    for (auto &rc : rects) if (rc.r1 <= r && r <= rc.r2 && rc.c1 <= c && c <= rc.c2) return true;
    return false;
}

static vector<pair<int,int>> neighbors(int r, int c, int R, int C){
    vector<pair<int,int>> out;
    if (r>1) out.push_back({r-1,c});
    if (r<R) out.push_back({r+1,c});
    if (c>1) out.push_back({r,c-1});
    if (c<C) out.push_back({r,c+1});
    return out;
}

// Main function requested
bool validate_seating(const string& student_choice_file, const string& assignment_file){
    try {
        // --- read header from student_choice to get N,R,C first ---
        ifstream sifs(student_choice_file);
        if (!sifs) throw runtime_error("cannot open student_choice file: " + student_choice_file);
        string header;
        if (!getline(sifs, header)) throw runtime_error("student_choice file empty");
        header = trim(header);
        istringstream hss(header);
        int N, R_spec, C_spec;
        if (!(hss >> N >> R_spec >> C_spec)) throw runtime_error("first line must be: N R C");
        // parse assignment using expected C (and R if desired)
        auto grid = parse_seat_assignment(assignment_file, R_spec, C_spec);
        int R = (int)grid.size();
        int C = (int)grid[0].size();
        if (R != R_spec || C != C_spec){
            cerr << "Note: parsed assignment grid is " << R << "x" << C << " but header says " << R_spec << "x" << C_spec << "\n";
        }

        // build position map for student IDs 1..N; ignore zeros (empty seats)
        unordered_map<int, pair<int,int>> pos_of;
        for (int i=0;i<R;i++){
            for (int j=0;j<C;j++){
                int id = grid[i][j];
                if (id == 0) continue; // empty seat
                if (pos_of.count(id)){
                    cerr << "ERROR: student id " << id << " appears more than once in assignment\n";
                    return false;
                }
                if (id < 1 || id > N){
                    cerr << "ERROR: student id " << id << " in assignment out of expected range 1.."<<N<<"\n";
                    return false;
                }
                pos_of[id] = {i+1, j+1}; // 1-based
            }
        }

        // ensure all students 1..N are present exactly once
        vector<int> missing;
        for (int id=1; id<=N; ++id){
            if (!pos_of.count(id)) missing.push_back(id);
        }
        if (!missing.empty()){
            cerr << "ERROR: missing student IDs in assignment (not placed): ";
            for (int m : missing) cerr << m << ' ';
            cerr << "\n";
            return false;
        }

        // --- parse remaining lines from student_choice (we already consumed header line) ---
        for (int i=1; i<=N; ++i){
            string id_line, avail_line, want_line, dont_line;
            if (!getline(sifs, id_line)) { cerr << "unexpected EOF while reading id for student " << i << "\n"; return false; }
            if (!getline(sifs, avail_line)) { cerr << "unexpected EOF while reading available_seat for student " << i << "\n"; return false; }
            if (!getline(sifs, want_line)) { cerr << "unexpected EOF while reading want for student " << i << "\n"; return false; }
            if (!getline(sifs, dont_line)) { cerr << "unexpected EOF while reading dont_want for student " << i << "\n"; return false; }

            id_line = trim(id_line);
            avail_line = trim(avail_line);
            want_line = trim(want_line);
            dont_line = trim(dont_line);

            int sid;
            try { sid = stoi(id_line); } catch (...) { cerr << "bad student id line at student index " << i << ": '"<< id_line << "'\n"; return false; }

            // parse rects
            vector<Rect> rects;
            try { rects = parse_rects(avail_line); } catch (const exception &e){
                cerr << "bad rectangle spec for student " << sid << ": " << e.what() << "\n"; return false;
            }

            auto wants = parse_int_list(want_line);
            auto donts = parse_int_list(dont_line);

            // check sid assigned
            if (!pos_of.count(sid)){ cerr << "student " << sid << " not assigned any seat\n"; return false; }
            auto p = pos_of[sid];
            int pr = p.first, pc = p.second;

            // check available seats
            if (!in_any_rect(pr, pc, rects)){
                cerr << "student " << sid << " assigned seat ("<<pr<<","<<pc<<") NOT in available rects\n";
                return false;
            }

            // neighbors ids
            auto neigh = neighbors(pr, pc, R, C);
            unordered_set<int> neigh_ids;
            for (auto &pp : neigh) {
                int nid = grid[pp.first-1][pp.second-1];
                if (nid != 0) neigh_ids.insert(nid);
            }

            // wants: ALL must be adjacent
            for (int w : wants){
                if (!pos_of.count(w)){ cerr << "student " << sid << " wants " << w << " but that id not present\n"; return false; }
                if (!neigh_ids.count(w)){ cerr << "student " << sid << " wants " << w << " but they are NOT adjacent\n"; return false; }
            }
            // donts: NONE may be adjacent
            for (int d : donts){
                if (neigh_ids.count(d)){ cerr << "student " << sid << " has prohibited neighbor " << d << " adjacent\n"; return false; }
            }
            // ok for this student -> continue
        }

        // all checked
        return true;
    } catch (const exception &ex){
        cerr << "Exception: " << ex.what() << "\n";
        return false;
    }
}

// example main (for quick run)
// int main(){
//     string student_choice_file = "student_choice.txt";
//     string assignment_file = "seat_assignment.txt";
//     bool ok = validate_seating(student_choice_file, assignment_file);
//     cerr << "validate_seating returned: " << (ok ? "true" : "false") << "\n";
//     return ok ? 0 : 1;
// }
