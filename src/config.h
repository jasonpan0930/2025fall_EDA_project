#include <string>
using namespace std;
#ifndef CONFIG_H
#define CONFIG_H
    inline string cnf_file_name = "../data/cnf.cnf";
    inline string student_choice_file = "../data/student_choice.txt";
    inline string minisat_result_file = "../data/result.out";
    inline string seat_assignment_file = "../data/seat_assignment.txt";
    inline int n_students, R, C; // number of students, rows and columns in the classroom
#endif