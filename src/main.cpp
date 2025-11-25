#include "generate_cnf.h"
#include "decode_seat.h"
#include "generate_student_choice.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "config.h"
#include "check.h"
using namespace std;


int main(){
    cout << "Generate choice file with given input ?(y/n)\n";
    char c;
    cin >> c;
    if(c == 'y'){
        generate_student_choice();
        cout << "student choice has been generated." << endl;
    }
    generate_cnf();
    cout << "cnf file has been generated." << endl;
    string command = "minisat " + cnf_file_name + " " + minisat_result_file;
    cout << "start to run minisat" << endl;
    system(command.c_str());
    ifstream in(minisat_result_file);
    string result;
    in >> result;
    if(result == "UNSAT"){
        cout << "No solution\n";
        return 0;
    }
    else if(result == "SAT"){
        cout << "Found a solution: \n";
        decode_seat();
        command = "cat " + seat_assignment_file;
        system(command.c_str());
        cout << "the result is also in " << seat_assignment_file << "\n";

        cout << "begin to check the result\n";
        bool ok = validate_seating(student_choice_file, seat_assignment_file);
        cout << "check result: " << (ok ? "true" : "false") << "\n";
    }
    else{
        cout << "Something's wrong with minisat output\n";
        return 1;
    }
    return 0;
}