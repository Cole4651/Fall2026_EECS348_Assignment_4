/*
 * Program Name : EECS 348 Assignment 4 - Sudoku Brute-Force Solver
 * Description  : Reads five Sudoku puzzle files and uses recursive depth-first search
 *                with backtracking to find and print EVERY solution of each puzzle.
 * Inputs       : puzzle1.txt ... puzzle5.txt (digits 1-9, '_' marks a blank cell)
 * Output       : For each file: the file name, the original puzzle, and each solution
 *                (or "No solution found.") plus the total solution count.
 * Collaborators: None
 * Other sources: ChatGPT (base design: validity check, loader, output format),
 *                Gemini (idea of walking cells by index), Claude (rewrite with bitmasks,
 *                shared print routine, comments). All code was reviewed/edited by the author.
 * Author       : Cole Nola
 * Creation date: 10/15/2026
 */


#include <iostream>   // cout for printing results
#include <fstream>    // ifstream for reading puzzle files
#include <string>     // std::string for file names
 
using namespace std;
 
// ---------------------------------------------------------------------------
// Sudoku class. Source: structure from ChatGPT's class; internals rewritten by author/Claude.
// Rows, columns and blocks each keep a 9-bit mask: bit d is 1 when digit d is already used.
// ---------------------------------------------------------------------------
class Sudoku {
private:
    static const int SIZE = 9;                 // width/height of the grid
    static const int CELLS = SIZE * SIZE;      // total number of cells (81)
    static const int ALL_DIGITS = 0x3FE;       // bits 1..9 set (binary 1111111110)
 
    int cells[CELLS];          // current board, 0 means blank (flat array, row-major)
    int original[CELLS];       // untouched copy of the puzzle for printing
    int rowUsed[SIZE];         // digits used in each row (bitmask)
    int colUsed[SIZE];         // digits used in each column (bitmask)
    int blockUsed[SIZE];       // digits used in each 3x3 block (bitmask)
    bool givensValid;          // false if the starting digits already break a rule
    long long solutionCount;   // number of solutions found so far
 
    // Converts a cell position (0-80) to its 3x3 block number (0-8). Author-written.
    static int blockOf(int pos) {
        return ((pos / SIZE) / 3) * 3 + (pos % SIZE) / 3;  // block row * 3 + block column
    }
 
    // Marks digit d as used (or free) in the row, column and block of pos. Author-written.
    void setUsed(int pos, int d, bool used) {
        int bit = 1 << d;                                  // bit that represents digit d
        int r = pos / SIZE, c = pos % SIZE, b = blockOf(pos); // row, column, block of the cell
        if (used) {                                        // placing the digit
            rowUsed[r] |= bit; colUsed[c] |= bit; blockUsed[b] |= bit;
        } else {                                           // removing the digit (backtrack)
            rowUsed[r] &= ~bit; colUsed[c] &= ~bit; blockUsed[b] &= ~bit;
        }
    }
 
    // Prints any 81-cell board with separators between blocks. Replaces two duplicate
    // print functions in the ChatGPT code. Author/Claude.
    void printBoard(const int board[CELLS]) const {
        for (int r = 0; r < SIZE; r++) {                   // go through each row
            if (r % 3 == 0 && r != 0) {                    // line between block rows
                cout << "------+-------+------" << endl;
            }
            for (int c = 0; c < SIZE; c++) {               // go through each column
                if (c % 3 == 0 && c != 0) cout << "| ";    // bar between block columns
                int v = board[r * SIZE + c];               // value in this cell
                if (v == 0) cout << "_ "; else cout << v << " "; // blank or digit
            }
            cout << endl;                                  // end of row
        }
    }
 
    // Recursive depth-first search with backtracking. Idea from Gemini (walk by index),
    // candidate bitmask technique by author/Claude.
    void solveFrom(int pos) {
        while (pos < CELLS && cells[pos] != 0) pos++;      // skip cells that are already filled
        if (pos == CELLS) {                                // no blanks left: a full solution
            solutionCount++;                               // count it
            cout << "\nSolution " << solutionCount << ":" << endl;
            printBoard(cells);                             // print it
            return;                                        // go back to look for more
        }
        int r = pos / SIZE, c = pos % SIZE;                // row and column of the blank
        // candidates = digits not used in this row, column or block
        int candidates = ALL_DIGITS & ~(rowUsed[r] | colUsed[c] | blockUsed[blockOf(pos)]);
        for (int d = 1; d <= 9; d++) {                     // try digits in increasing order
            if (candidates & (1 << d)) {                   // only if d is a legal candidate
                cells[pos] = d;                            // place the digit
                setUsed(pos, d, true);                     // record it in the masks
                solveFrom(pos + 1);                        // recurse on the next cell
                setUsed(pos, d, false);                    // backtrack: free the digit
                cells[pos] = 0;                            // backtrack: clear the cell
            }
        }
    }
 
public:
    // Constructor: start with an empty board. Author-written.
    Sudoku() { clear(); }
 
    // Resets every field so one object can be reused for several puzzles.
    void clear() {
        for (int i = 0; i < CELLS; i++) { cells[i] = 0; original[i] = 0; } // blank boards
        for (int i = 0; i < SIZE; i++) { rowUsed[i] = colUsed[i] = blockUsed[i] = 0; } // no digits used
        givensValid = true;                                // assume valid until proven otherwise
        solutionCount = 0;                                 // no solutions yet
    }
 
    // Reads a puzzle file. Based on ChatGPT's loadPuzzle, extended with stricter checking.
    bool loadPuzzle(const string& filename) {
        clear();                                           // forget any previous puzzle
        ifstream in(filename.c_str());                     // open the file
        if (!in) {                                         // could not open
            cout << "Error opening " << filename << endl;
            return false;
        }
        int count = 0;                                     // how many cells were read
        char ch;                                           // current character
        while (in >> ch) {                                 // operator>> skips whitespace
            if (!((ch >= '1' && ch <= '9') || ch == '_')) { // anything else is invalid
                cout << "Invalid character '" << ch << "' in " << filename << endl;
                return false;
            }
            if (count >= CELLS) {                          // more than 81 cells
                cout << "Invalid puzzle file: " << filename << " has too many cells." << endl;
                return false;
            }
            int d = (ch == '_') ? 0 : ch - '0';            // blank -> 0, digit -> its value
            if (d != 0) {                                  // a given digit
                if ((rowUsed[count / SIZE] | colUsed[count % SIZE] | blockUsed[blockOf(count)]) & (1 << d)) {
                    givensValid = false;                   // digit repeats in row, column or block
                }
                setUsed(count, d, true);                   // record the given digit
            }
            cells[count] = d;                              // store the value
            original[count] = d;                           // keep a copy for printing
            count++;                                       // move to the next cell
        }
        if (count != CELLS) {                              // fewer than 81 cells
            cout << "Invalid puzzle file: " << filename << " must contain exactly 81 cells." << endl;
            return false;
        }
        return true;                                       // file loaded successfully
    }
 
    // Prints the puzzle as it appeared in the file.
    void printOriginal() const { printBoard(original); }
 
    // Finds and prints all solutions. Validity check idea from ChatGPT's isValidPuzzle.
    void solve() {
        solutionCount = 0;                                 // reset counter
        if (!givensValid) {                                // starting digits already conflict
            cout << "\nNo solution found." << endl;
            return;
        }
        solveFrom(0);                                      // start the search at cell 0
        if (solutionCount == 0) cout << "\nNo solution found." << endl;
        else cout << "\nTotal solutions found: " << solutionCount << endl;
    }
};
 
// Main driver: solve puzzle1.txt through puzzle5.txt. Author-written, loop style from ChatGPT.
int main() {
    Sudoku puzzle;                                         // one reusable solver object
    for (int i = 1; i <= 5; i++) {                         // five puzzle files
        string filename = "puzzle" + to_string(i) + ".txt"; // build the file name
        cout << "\n================================" << endl;
        cout << "File: " << filename << endl;
        cout << "================================" << endl;
        if (puzzle.loadPuzzle(filename)) {                 // only continue if loading worked
            cout << "\nOriginal Puzzle:" << endl;
            puzzle.printOriginal();                        // show the puzzle
            puzzle.solve();                                // search for all solutions
        }
    }
    return 0;                                              // normal exit
}
