
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Sudoku {
private:
    int grid[9][9];
    int original[9][9];
    int solutionCount;

    // Check whether a number can be placed in a cell.
    bool isSafe(int row, int col, int num) {
        // Check the row.
        for (int c = 0; c < 9; c++) {
            if (grid[row][c] == num) {
                return false;
            }
        }

        // Check the column.
        for (int r = 0; r < 9; r++) {
            if (grid[r][col] == num) {
                return false;
            }
        }

        // Find the top-left corner of the 3x3 block.
        int startRow = (row / 3) * 3;
        int startCol = (col / 3) * 3;

        // Check the 3x3 block.
        for (int r = startRow; r < startRow + 3; r++) {
            for (int c = startCol; c < startCol + 3; c++) {
                if (grid[r][c] == num) {
                    return false;
                }
            }
        }

        return true;
    }

    // Find the next empty cell.
    bool findEmptyCell(int &row, int &col) {
        for (row = 0; row < 9; row++) {
            for (col = 0; col < 9; col++) {
                if (grid[row][col] == 0) {
                    return true;
                }
            }
        }

        return false;
    }

    // Print the current Sudoku grid.
    void printGrid() {
        for (int row = 0; row < 9; row++) {
            if (row % 3 == 0 && row != 0) {
                cout << "------+-------+------" << endl;
            }

            for (int col = 0; col < 9; col++) {
                if (col % 3 == 0 && col != 0) {
                    cout << "| ";
                }

                if (grid[row][col] == 0) {
                    cout << "_ ";
                } else {
                    cout << grid[row][col] << " ";
                }
            }

            cout << endl;
        }
    }

    // Check whether the original puzzle follows the rules.
    bool isValidPuzzle() {
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                int num = grid[row][col];

                if (num != 0) {
                    // Temporarily remove the number so it
                    // does not conflict with itself.
                    grid[row][col] = 0;

                    bool valid = isSafe(row, col, num);

                    grid[row][col] = num;

                    if (!valid) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    // Recursively find and print every solution.
    void solveAll() {
        int row, col;

        // If there are no empty cells, a solution is found.
        if (!findEmptyCell(row, col)) {
            solutionCount++;

            cout << "\nSolution " << solutionCount << ":" << endl;
            printGrid();
            return;
        }

        // Try every possible number in the empty cell.
        for (int num = 1; num <= 9; num++) {
            if (isSafe(row, col, num)) {
                // Place the number.
                grid[row][col] = num;

                // Recursively solve the remaining puzzle.
                solveAll();

                // Backtrack and try another number.
                grid[row][col] = 0;
            }
        }
    }

public:
    Sudoku() {
        solutionCount = 0;

        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                grid[row][col] = 0;
                original[row][col] = 0;
            }
        }
    }

    // Read a Sudoku puzzle from a file.
    bool loadPuzzle(string filename) {
        ifstream inputFile(filename);

        if (!inputFile) {
            cout << "Error opening " << filename << endl;
            return false;
        }

        int count = 0;
        char ch;

        // Read digits and underscores, ignoring whitespace.
        while (inputFile >> ch) {
            if (ch >= '1' && ch <= '9') {
                if (count >= 81) {
                    cout << "Invalid puzzle file: "
                         << filename << endl;
                    return false;
                }

                grid[count / 9][count % 9] = ch - '0';
                count++;
            } else if (ch == '_') {
                if (count >= 81) {
                    cout << "Invalid puzzle file: "
                         << filename << endl;
                    return false;
                }

                grid[count / 9][count % 9] = 0;
                count++;
            }
        }

        inputFile.close();

        if (count != 81) {
            cout << "Invalid puzzle file: " << filename
                 << " must contain exactly 81 cells." << endl;
            return false;
        }

        // Save the original puzzle for printing.
        for (int row = 0; row < 9; row++) {
            for (int col = 0; col < 9; col++) {
                original[row][col] = grid[row][col];
            }
        }

        return true;
    }

    // Print the original puzzle.
    void printOriginal() {
        for (int row = 0; row < 9; row++) {
            if (row % 3 == 0 && row != 0) {
                cout << "------+-------+------" << endl;
            }

            for (int col = 0; col < 9; col++) {
                if (col % 3 == 0 && col != 0) {
                    cout << "| ";
                }

                if (original[row][col] == 0) {
                    cout << "_ ";
                } else {
                    cout << original[row][col] << " ";
                }
            }

            cout << endl;
        }
    }

    // Solve the puzzle and print every solution.
    void solve() {
        solutionCount = 0;

        if (!isValidPuzzle()) {
            cout << "\nNo solution found." << endl;
            return;
        }

        solveAll();

        if (solutionCount == 0) {
            cout << "\nNo solution found." << endl;
        } else {
            cout << "\nTotal solutions found: "
                 << solutionCount << endl;
        }
    }
};

int main() {
    string filenames[5] = {
        "puzzle1.txt",
        "puzzle2.txt",
        "puzzle3.txt",
        "puzzle4.txt",
        "puzzle5.txt"
    };

    for (int i = 0; i < 5; i++) {
        cout << "\n================================" << endl;
        cout << "File: " << filenames[i] << endl;
        cout << "================================" << endl;

        Sudoku puzzle;

        if (puzzle.loadPuzzle(filenames[i])) {
            cout << "\nOriginal Puzzle:" << endl;
            puzzle.printOriginal();

            cout << "\nSolving..." << endl;
            puzzle.solve();
        }

        cout << endl;
    }

    return 0;
}
