# Project Reorganization Summary

## New Folder Structure

The project has been reorganized into the following structure:

```
EDA_project/
├── src/                    # Source code files
│   ├── main.cpp
│   ├── config.h
│   ├── generate_cnf.h
│   ├── generate_student_choice.h
│   ├── decode_seat.h
│   ├── check.h
│   └── generate_random_seat.cpp
│
├── build/                  # Compiled executables
│   ├── main
│   ├── generate_random_seat
│   ├── generate_random_seat.exe
│   └── check
│
├── data/                   # Input/output data files
│   ├── student_choice.txt
│   ├── cnf.cnf
│   ├── result.out
│   ├── seat_assignment.txt
│   ├── answer.txt
│   └── exp1.txt
│
├── docs/                   # Documentation
│   ├── report.tex          # Main LaTeX report (updated with new image paths)
│   ├── report.pdf
│   ├── README_choice_format.md
│   ├── seats arrangement problem.pdf
│   └── 報告資料1015.xlsx
│
├── images/                 # All image files for the report
│   ├── eda_data_flow.png
│   ├── experiment_data.png
│   ├── number_of_clauses.png
│   ├── cnf_gen_time__n_clauses.png
│   ├── cpu_time.png
│   ├── random_seat.png
│   └── sol_seat.png
│
├── latex_build/            # LaTeX build artifacts
│   ├── report.aux
│   ├── report.log
│   ├── report.toc
│   └── report.synctex.gz
│
├── README.md               # Main project README
└── REORGANIZATION_SUMMARY.md  # This file
```

## Changes Made

1. **Source Code**: All `.cpp` and `.h` files moved to `src/`
2. **Executables**: All compiled binaries moved to `build/`
3. **Images**: All PNG files moved to `images/`
4. **Data Files**: All input/output text files moved to `data/`
5. **Documentation**: All documentation files moved to `docs/`
6. **LaTeX Build Files**: All LaTeX auxiliary files moved to `latex_build/`

## Updated Files

- **docs/report.tex**: All image paths updated to use `../images/` prefix since report.tex is now in `docs/` folder

## Important Notes

### Running the Program

The source code currently references data files with relative paths (e.g., `student_choice.txt`). You have two options:

**Option 1: Run from project root** (Recommended for now)
```bash
cd build
../src/g++ -o main ../src/main.cpp
./main  # This will look for data files in current directory
# Copy or symlink data files to build/ if needed
```

**Option 2: Update paths in config.h** (For permanent reorganization)
Update `src/config.h` to use `../data/` prefix:
```cpp
inline string cnf_file_name = "../data/cnf.cnf";
inline string student_choice_file = "../data/student_choice.txt";
inline string minisat_result_file = "../data/result.out";
inline string seat_assignment_file = "../data/seat_assignment.txt";
```

### Compiling the Report

The report should be compiled from the `docs/` directory:
```bash
cd docs
pdflatex report.tex
pdflatex report.tex  # Run twice for cross-references
```

The images are correctly referenced with `../images/` paths.

## Benefits of This Organization

1. **Cleaner root directory**: Only essential files in root
2. **Better separation**: Source, build, data, and docs are clearly separated
3. **Easier maintenance**: Build artifacts don't clutter the project
4. **Professional structure**: Follows common project organization patterns

