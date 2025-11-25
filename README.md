# Seat Arrangement Problem with SAT Solver

This project implements a seat arrangement problem solver using the MiniSat SAT solver. The system assigns students to seats in a classroom while satisfying various constraints including preferred seating zones, desired neighbors, and prohibited neighbors.

## Project Structure

```
EDA_project/
├── src/              # Source code files
│   ├── *.cpp         # C++ source files
│   └── *.h           # Header files
├── docs/             # Documentation
│   ├── report.tex    # LaTeX report source
│   ├── *.pdf         # PDF documents
│   ├── *.md          # Markdown documentation
│   └── *.xlsx        # Excel files
├── images/           # Image files for the report
│   └── *.png         # All PNG images
├── build/            # Compiled executables
│   ├── main          # Main program executable
│   ├── generate_random_seat
│   └── *.exe         # Windows executables
├── data/             # Input/output data files
│   ├── *.txt         # Text data files
│   ├── *.cnf         # CNF files for MiniSat
│   └── *.out         # Output files
└── latex_build/      # LaTeX build artifacts
    ├── *.aux
    ├── *.log
    ├── *.toc
    └── *.synctex.gz
```

## Building and Running

### Compile the project:
```bash
cd src
g++ -o ../build/main main.cpp
```

### Generate student choices (optional):
```bash
cd build
./main
# Choose 'y' to generate student choice file
```

### Run the solver:
```bash
cd build
./main
# Choose 'n' if student_choice.txt already exists
```

## Compiling the Report

To compile the LaTeX report:
```bash
cd docs
pdflatex report.tex
pdflatex report.tex  # Run twice for proper cross-references
```

The PDF will be generated in the `docs/` directory.

## File Descriptions

### Source Files (src/)
- `main.cpp` - Main program orchestrating the workflow
- `config.h` - Configuration and global variables
- `generate_cnf.h` - CNF file generation from student preferences
- `generate_student_choice.h` - Test data generation
- `decode_seat.h` - Decodes MiniSat output to seat assignments
- `check.h` - Validates solutions against constraints
- `generate_random_seat.cpp` - Generates random seat configurations

### Data Files (data/)
- `student_choice.txt` - Input file with student preferences
- `cnf.cnf` - Generated CNF file for MiniSat
- `result.out` - MiniSat solver output
- `seat_assignment.txt` - Final seat arrangement result
- `answer.txt` - Reference answer for testing

## Requirements

- C++ compiler (g++ or compatible)
- MiniSat SAT solver
- LaTeX distribution (for report compilation)
- pdflatex (for PDF generation)

## Author

b12901193 Chin-Yen, Pan

