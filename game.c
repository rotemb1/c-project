/** @file game.c
 *  @brief game source file.
 *
 *  This module encapsulates the sudoku puzzle game and the Sudoku board.
 *  Includes functions to match all game commands to be entered by the user.
 *  Includes auxiliary functions that are relevant specifically to the game board state or the game mode.
 *
 *  @author Itay Keren (itaykeren)
 *  @author Rotem Bar (rotembar)
 *
 */

/* -- Defines -- */
#define _GNU_SOURCE

/* -- Includes -- */
#include "game.h"
#include "main_aux.h"
#include "solver.h"
#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* -- Global Variables  -- */
extern int blockRows;
extern int blockCols;
extern int markErrors;
extern char mode;
extern bool gameOver;

/* returns the amount of empty (UNASSIGNED) cells in given sudoku board */
int count_empty_cells(cell** board) {
    int i, j;
    int counter = 0;
    int N = blockRows * blockCols;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (board[i][j].number == UNASSIGNED) {
                counter++;
            }
        }
    }
    return counter;
}

/* empty sudoku board by assigning all cells values to UNASSIGNED and by removing fixation or asterisks */
void empty_board(cell** board) {
    int i, j;
    int N = blockRows * blockCols;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            board[i][j].number = UNASSIGNED;
            board[i][j].asterisk = false;
            board[i][j].isFixed = false;
        }
    }
}

/* returns true if all sudoku board cells are empty (UNASSIGNED), else returns false */
bool board_is_empty(cell** board) {
    int N = blockRows * blockCols;
    if (count_empty_cells(board) != N * N){
        return false;
    }
    return true;
}

/* returns a new sudoku board, generated by allocating memory and emptying the board */
cell** generate_empty_board(){
    int i, N;
    cell** board = NULL;
    N =  blockRows * blockCols;
    board = calloc(N, sizeof (*board));
    if (board == NULL) {
        memory_error("Memory allocation failed\n");
        exit(0);
    }
    for (i = 0; i < N; i++) {
        board[i] = NULL;
        board[i] = calloc(N, sizeof (**board));
        if (board[i] == NULL) {
            memory_error("Memory allocation failed\n");
            exit(0);
        }
    }
    empty_board(board);
    return board;
}

/* copies a source sudoku board to a new identical sudoku board */
void copy_board(cell** source_board, cell** new_board){
    int i, j;
    int N = blockRows * blockCols;
    for (i  =0; i < N; i++) {
        for (j = 0; j < N; j++){
            new_board[i][j].number = source_board[i][j].number;
            new_board[i][j].asterisk = source_board[i][j].asterisk;
            new_board[i][j].isFixed = source_board[i][j].isFixed;
        }
    }
}

/* returns true if given value is in the sudoku board's block corresponding to the row and column given,
 * else returns false.
 * if encounters within the block in a cell containing the given value, it marks it with an asterisk */
bool val_in_block(cell** board, int column, int row, int val){
    bool valExist = false;
    int initialCol, initialRow, colIndex, rowIndex;
    initialCol = get_block_col_index(column);
    initialRow = get_block_row_index(row);
    for (colIndex = initialCol; (colIndex < blockCols + initialCol); colIndex++) {
        for (rowIndex = initialRow; (rowIndex < blockRows + initialRow); rowIndex++) {
            if ((colIndex == column - 1) && (rowIndex == row - 1)) { /* skips check for the cell about to change */ }
            else if (board[rowIndex][colIndex].number == val) {
                valExist = true;
                if (board[row - 1][colIndex].isFixed == false) { /* only marking unfixed cells */
                    board[row - 1][colIndex].asterisk = true;
                }
            }
        }
    }
    return valExist;
}

/* returns true if given value is in the sudoku board's given row,
 * else returns false.
 * if encounters within the row in a cell containing the given value, it marks it with an asterisk */
bool val_in_row(cell** board, int column, int row, int val){
    int N = blockRows * blockCols;
    bool valExist = false;
    int colIndex;
    for (colIndex = 0; colIndex < N; colIndex++) {
        if(colIndex == column - 1) { /* skips check for the cell about to change */ }
        else if (board[row - 1][colIndex].number == val) {
            valExist = true;
            if (board[row - 1][colIndex].isFixed == false) { /* only marking unfixed cells */
                board[row - 1][colIndex].asterisk = true;
            }
        }
    }
    return valExist;
}

/* returns true if given value is in the sudoku board's given column,
 * else returns false.
 * if encounters within the column in a cell containing the given value, it marks it with an asterisk */
bool val_in_column(cell** board, int column, int row, int val){
    int N = blockRows * blockCols;
    bool valExist = false;
    int rowIndex;
    for (rowIndex = 0; rowIndex < N; rowIndex++) {
        if(rowIndex == row - 1){ /* skips check for the cell about to change */ }
        else if (board[rowIndex][column - 1].number == val) {
            valExist = true;
            if (board[rowIndex][column - 1].isFixed == false) { /* only marking unfixed cells */
                board[rowIndex][column - 1].asterisk = true;
            }
        }
    }
    return valExist;
}

/* returns true if given value is valid in the cell corresponding to the row and column given in the sudoku board,
 * else returns false.
 * the valid check is done according to the game rules - checking row, column and block.
 * if value is invalid it marks it with an asterisk */
bool valid_check(cell** board, int column, int row, int val) {
    if(val_in_block(board, column, row, val) | val_in_row(board, column, row, val) | val_in_column(board, column, row, val)){
        board[row - 1][column - 1].asterisk = true;
        return false;
    }
    else{
        board[row - 1][column - 1].asterisk = false;
        return true;
    }
}

/* marks all cells in the sudoku board that contains invalid values according to game rules with an asterisk */
void mark_asterisks(cell** board) {
    int i, j;
    int N = blockCols * blockRows;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (board[i][j].number != UNASSIGNED) {
                valid_check(board, j + 1, i + 1, board[i][j].number);
            }
        }
    }
}

/* updates moves list by adding a copy of given sudoku board(after a game move was done) to the end of the list */
void update_moves_list(cell** board, list* lst) {
    cell** boardAfter;
    boardAfter = generate_empty_board();
    copy_board(board, boardAfter);
    insert_at_tail(boardAfter, lst);
    return;
}

/* helper function to print_board() which prints block separators */
void print_separator(int N, int m) {
    char dash = '-';
    int i;
    int count = 4 * N + m + 1;
    for (i = 0; i < count; i++) {
        putchar(dash);
    }
    putchar('\n');
}

/* prints the given sudoku board according to the format, game mode and markErrors value */
void print_board(cell** board, char mode) {
    int i, j;
    int N = blockRows * blockCols;
    for (i = 0; i < N; i++) {
        if (i % blockRows == 0){
            print_separator(N, blockRows);
        }
        for (j = 0; j < N; j++) {
            if (j % blockCols == 0) {
                printf("|");
            }
            printf(" ");
            if (board[i][j].isFixed) {
                printf("%2d", board[i][j].number);
                printf(".");
            }
            else if ((board[i][j].asterisk) && (markErrors || mode == 'E')) { /* always marking errors in 'E' mode */
                printf("%2d", board[i][j].number);
                printf("*");
            }
            else if (board[i][j].number != UNASSIGNED){
                printf("%2d ", board[i][j].number);
            }
            else{ /* printing blank spaces for UNASSIGNED */
                printf("   ");
            }
        }
        printf("|\n");
    }
    print_separator(N, blockRows);
}

/* returns true if given sudoku board contains erroneous cells (marked with an asterisk),
 * else returns false */
bool check_board_erroneous(cell **board){
    int colIndex, rowIndex = 0;
    int N = blockRows * blockCols;
    for (rowIndex = 0; rowIndex < N; rowIndex++) {
        for (colIndex = 0; colIndex < N; colIndex++) {
            if (board[rowIndex][colIndex].asterisk) {
                return true;
            }
        }
    }
    return false;
}

/* validates the given sudoku board using ILP and prints an appropriate message */
void validate(cell** board) {
    cell** copyBoard;
    int solvable;
    if (check_board_erroneous(board)) {
        printf(ERRONEOUS_ERROR );
        return;
    }
    copyBoard = duplicate_board(board);
    solvable = ILP(board, copyBoard);
    if (solvable == true) {
        printf("Validation passed: board is solvable\n");
    } else {
        printf("Validation failed: board is unsolvable\n");
    }
    free_board(copyBoard);
}

/* helper function to num_solutions that returns the number of solutions of the given sudoku board,
 * using exhaustive deterministic backtrack  */
int count_solutions(cell** board) {
    int numOfSolutions;
    int* unassignedsArray;
    cell** boardCopy = generate_empty_board();
    copy_board(board, boardCopy);
    unassignedsArray = get_next_play(boardCopy);
    if (unassignedsArray[0] == -1) {
        free(unassignedsArray);
        return 0;
    }
    numOfSolutions = deterministic_backtrack(boardCopy, unassignedsArray[0], unassignedsArray[1]);
    free_board(boardCopy);
    free(unassignedsArray);
    return numOfSolutions;
}

/* counts the number of solutions of the given sudoku board and prints it with an appropriate message */
void num_solutions(cell** board) {
    int solutionsCounter;
    if (check_board_erroneous(board)) {
        printf(ERRONEOUS_ERROR);
    }
    solutionsCounter = count_solutions(board);
    printf("Number of solutions: %d\n", solutionsCounter);
    if (solutionsCounter == 1) {
        printf("This is a good board!\n");
    } else if (solutionsCounter > 1) {
        printf("The puzzle has more than 1 solution, try to edit it further\n");
    }
}

/* saves the given sudoku board to the given file path */
void save_command(cell** board, char* filePath) {
    FILE* fp;
    int i, j, N;
    cell** copyOfBoard;
    bool solvable;
    if (mode == 'E') {
        if (check_board_erroneous(board)) { /* board to be saved in 'E' mode must'nt contain errors */
            printf(ERRONEOUS_ERROR);
            return;
        }
        copyOfBoard = generate_empty_board();
        solvable = ILP(board,copyOfBoard);
        free_board(copyOfBoard);
        if (!solvable) { /* board to be saved in 'E' mode must be solvable */
            printf("Error: board validation failed\n");
            return;
        }
    }
    if ((fp = fopen(filePath, "wb")) == NULL) { /* file does not exist or can't be open */
        printf("Error: File cannot be created or modified\n");
    }
    if (fprintf(fp, "%d %d\n", blockRows, blockCols) < 1){ /* failed to write */
        printf("Writing to file failed. Game not saved\n");
        return;
    }
    N = blockRows * blockCols;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            fprintf(fp, "%d", board[i][j].number);
            if ((board[i][j].isFixed || mode == 'E') && (board[i][j].number != UNASSIGNED)) {
                fprintf(fp, ".");
            }
            fprintf(fp, " ");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("Saved to: %s\n", filePath);
}

/* sets global variable markErrors with legal given value  */
void mark_errors_command(int value) {
    if (value == 0) {
        markErrors = 0;
    } else if (value == 1) {
        markErrors = 1;
    }
    else {
        printf(MARK_ERROR_ERROR);
    }
}

/* loads a sudoku board from given file path */
cell** load_board(FILE* fp, char mode){
    int i, j, N;
    cell** board = NULL;
    char line[257];
    char* token;
    char* delimiter = " \t\r\n";
    if (fgets(line, 257, fp) != NULL) {
        token = strtok(line, delimiter);
        blockRows = atoi(token);
        token = strtok(NULL, delimiter);
        blockCols = atoi(token);
    } else {
        printf("Error: File read failed\n");
        return NULL;
    }
    board = generate_empty_board();
    N = blockRows * blockCols;
    for (i = 0; i < N; i++) {
        fgets(line, 256, fp);
        token = strtok(line, delimiter);
        for (j = 0; j < N; j++) {
            board[i][j].number = token[0] - '0';
            if ((token[1] == '.') && (mode == 'S')) {  /* cells are marked as fixes only in 'S' mode */
                board[i][j].isFixed = true;
            }
            else if (token[1] == '*') {
                board[i][j].asterisk = true;
            }
            token = strtok(NULL, delimiter);
        }
    }
    mark_asterisks(board);
    return board;
}

/* returns a sudoku board to be edited:
 * loading an existing sudoku board from given file path or generating a new 9X9 board.
 * if succeeded prints the board, else prints an appropriate error message */
cell** edit_command(char* parsedCommand[4], char mode){
    cell** board = NULL;
    FILE* fp = NULL;
    if (parsedCommand[1] != '\0'){
        fp = fopen(parsedCommand[1], "r");
        if (fp != NULL) {
            board = load_board(fp, mode);
            fclose(fp);
        }
        else {
            printf("Error: File cannot be opened\n");
            return board;
        }
    } else {
        blockRows = 3;
        blockCols = 3;
        board = generate_empty_board();
    }
    print_board(board, mode);
    return board;
}

/* returns a sudoku board to be solved:
 * loading an existing sudoku board from given file path.
 * if succeeded prints the board, else prints an appropriate error message */
cell** solve_command(char* parsedCommand[4], char mode){
    cell** board = NULL;
    FILE* fp = NULL;
    if (parsedCommand[1] == '\0'){
        printf(INVALID_ERROR);
        return board;
    }
    fp = fopen(parsedCommand[1], "r");
    if (fp != NULL) {
        board = load_board(fp, mode);
        fclose(fp);
        print_board(board, mode);
    } else {
        printf("Error: File doesn't exist or cannot be opened\n");
    }
    return board;
}


/* checks if given sudoku board is completely full and solved successfully according to the game rules,
 * using ILP for validation.
 * if solved - prints appropriate message, updating game mode to 'I' and gameOver flag to true.
 * if full and solved incorrectly prints appropriate message and not changing game mode or gameOver flag. */
void game_over(cell** board){
    int col, row;
    bool solvable;
    bool full = true;
    cell** copyBoard;
    int N = blockCols * blockRows;
    for (col = 0; col < N; col++) {
        for (row = 0; row < N; row++) {
            if (board[row][col].number == UNASSIGNED) {
                full = false;
            }
        }
    }
    if (full) {
        copyBoard = generate_empty_board();
        solvable = ILP(board, copyBoard);
        free_board(copyBoard);
        if (solvable) {
            printf(GAME_OVER);
            mode = 'I';
            gameOver = true;
        } else {
            printf("Puzzle solution erroneous\n");
        }
    }
}

/* helper function which validates asterisks after a new set of value in the given indexes was made.
 * returns true if at least one cell was found to be invalid, else return false.
 * if previously asterisk cell is now valid it updates asterisk to false
 * if new risks are found, it marks them with an asterisk */
bool validate_risks(cell** board, int column, int row) {
    int initialCol, initialRow, colIndex, rowIndex;
    int N = blockRows * blockCols;
    bool risksFound = true;
    initialCol = get_block_col_index(column);
    initialRow = get_block_row_index(row);
    for (colIndex = initialCol; (colIndex < blockCols + initialCol); colIndex++) { /* block check */
        for (rowIndex = initialRow; (rowIndex < blockRows + initialRow); rowIndex++) {
            if (colIndex == (column-1) && rowIndex == (row-1)) { /* not checking cell changed */
            }
            else if (board[rowIndex][colIndex].asterisk){
                if(valid_check(board, colIndex + 1, rowIndex + 1, board[rowIndex][colIndex].number)){
                    board[rowIndex][colIndex].asterisk = false;
                }
                else{
                    risksFound = false;
                }
            }
        }
    }
    for (colIndex = 0; (colIndex < N); colIndex++) { /* row check */
        if (colIndex == column - 1) {
        }
        else if (board[row - 1][colIndex].asterisk){
            if (valid_check(board, colIndex + 1, row, board[row - 1][colIndex].number)){
                board[row - 1][colIndex].asterisk = false;
            }
            else{
                risksFound = false;
            }
        }
    }

    for (rowIndex = 0; (rowIndex < N); rowIndex++) {  /* col check */
        if (rowIndex == row - 1){
        }
        else if (board[rowIndex][column - 1].asterisk){
            if( valid_check(board, column , rowIndex + 1, board[rowIndex][column - 1].number)){
                board[rowIndex][column - 1].asterisk = false;
            }
            else{
                risksFound = false;
            }
        }
    }
    return risksFound;
}

/* fills cells in given sudoku board which can contain a single legal value.
 * returns true if at least one cell was filled, else returns false */
bool auto_fill(cell** board)    {
    int i, j, k, candidate;
    int fillFlag = false;
    int numOfCandidates = 0;
    int maxValue = blockRows * blockCols + 1;
    cell** copyOfBoard;
    int N = blockRows * blockCols;
    if (check_board_erroneous(board)){
        printf("Error: board contains erroneous values\n");
        return false;
    }
    copyOfBoard = generate_empty_board();
    copy_board(board, copyOfBoard);
    for (i = 0; i < N; i++){
        for (j = 0; j < N; j++){
            if (copyOfBoard[i][j].number == UNASSIGNED){
                for (k = 1; k < maxValue; k++){
                    if (valid_check(copyOfBoard, j + 1, i + 1, k)){
                        candidate = k;
                        numOfCandidates++;
                    }
                }
                if (numOfCandidates == 1){
                    board[i][j].number = candidate;
                    validate_risks(board, j + 1, i + 1);
                    fillFlag = true;
                    printf("Cell <%d,%d> set to %d\n", (j+1), (i+1), candidate);
                }
                numOfCandidates = 0;
            }
        }
    }
    free_board(copyOfBoard);
    print_board(board, mode);
    game_over(board);
    return fillFlag;
}

/* sets given value to corresponding cell(according to indexes given) in given sudoku board.
 * if either value or indexes are invalid or cell is fixed prints appropriate error message.
 * validating asterisks to update marking of asterisks cells if needed.
 * checks if game is over after the set was made.
 * returns true if s set which changed the board status was made, else return false.
 * prints the board if set was successful */
bool set(cell** board, int column, int row, int val, char mode) {
    int N = blockRows * blockCols;
    if((!valid_board_index(column, N)) || (!valid_board_index(row, N)) || (!valid_set_value(val, N))){
        printf(VALUE_RANGE_ERROR, blockCols * blockRows);
        return false;
    }
    if (board[row - 1][column - 1].isFixed) {
        printf(FIXED_ERROR);
        return false;
    }
    else if (board[row - 1][column - 1].number == val) {
        print_board(board, mode);
        return  false;
    }
    else if (val == 0) {
        board[row - 1][column - 1].number = UNASSIGNED;
        board[row - 1][column - 1].asterisk = false;
        validate_risks(board, column, row);
        print_board(board, mode);
        return true;
    }
    else {
        valid_check(board, column, row, val);
        board[row - 1][column - 1].number = val;
        validate_risks(board, column, row);
        print_board(board, mode);
        if (mode == 'S'){
            game_over(board);
        }
        return true;
    }
}

/* prints a valid hint value of the corresponding cell(according to indexes given) in the given sudoku board.
 * hint value is taken from solving the board given using ILP.
 * if either indexes are invalid, board contains errors, cell is fixed or full prints appropriate error message.
 * if board is unsolvable prints appropriate error message.
 */
void hint(cell** board, int column, int row){
    int hint;
    bool solvable;
    cell** solvedBoard = NULL;
    int N = blockCols * blockRows;
    if((!valid_board_index(column, N)) || (!valid_board_index(row, N))) {
        printf("Error: value not in range 1-%d\n",N);
        return;
    }
    if (check_board_erroneous(board)) {
        printf("Error: board contains erroneous values\n");
        return;
    }
    if (board[row - 1][column - 1].isFixed) {
        printf(FIXED_ERROR);
        return;
    }
    if (board[row - 1][column - 1].number != 0) {
        printf("Error: cell already contains a value\n");
        return;
    }
    solvedBoard = generate_empty_board();
    solvable = ILP(board, solvedBoard);
    if (solvable == false){
        printf("Error: board is unsolvable\n");
        free_board(solvedBoard);
        return;
    }
    else{
        hint = solvedBoard[row - 1][column - 1].number;
        printf("Hint: set cell to %d\n", hint);
        free_board(solvedBoard);
    }
}

/* frees allocated memory of given sudoku board */
void free_board(cell** board){
    int i;
    int N = blockRows * blockCols;
    for (i = 0; i < N; i++) {
        free(board[i]);
    }
    free(board);
    board = NULL;

}
/* frees allocated memory for given command, exists the game after printing appropriate message */
void exit_game(char* command){
    printf("Exiting...\n");
    free(command);
    exit(0);
}

/* fills the corresponding cell(according to indexes given) in the given sudoku board with a random legal value,
 * if no value found returns false, else returns true */
bool fill_cell(cell** board, int column, int row){
    cell** copyOfBoard;
    int numbersLeft, randomIndex, nextNum;
    int N = blockRows * blockCols;
    int* availableNumbers;
    availableNumbers = generate_int_array(N); /* int pointer containing available numbers to be filled from 1-N */
    copyOfBoard = generate_empty_board();
    copy_board(board, copyOfBoard);
    numbersLeft = N;
    while (numbersLeft > 0){
        randomIndex = rand() % numbersLeft;
        nextNum = availableNumbers[randomIndex];
        if(valid_check(copyOfBoard, column + 1 ,row + 1, nextNum)){
            copyOfBoard[row][column].number = nextNum;
            copy_board(copyOfBoard, board); /* updating original board */
            free_board(copyOfBoard);
            free(availableNumbers);
            return true;
        }
        copyOfBoard[row][column].number = UNASSIGNED;
        copyOfBoard[row][column].asterisk = false;
        numbersLeft--;
        if (numbersLeft > 0) {
            delFromArr(randomIndex, numbersLeft, availableNumbers); /* updating available numbers by deleting illegals*/
        }
    }
    free_board(copyOfBoard); /* no legal value was found */
    free(availableNumbers);
    return false;
}

/* generates a random sudoku board by filling the empty board given:
 * first step: filling board with amount of initialFullCells of random legal values
 * if first step is successful - second step: validating board to using ILP to see if solvable
 * if second step is successful - third step :  copying valid amont of random copyCells to the given board.
 * when either step one or two fails board is emptying and process starts again from first step
 * after 1000 failed attempts an appropriate error message is printed and function returns false
 * if generation is successful the given board is updated and function return true and prints the board */
bool generate_randomized_solved_board (cell** board, int initialFullCells, int copyCells) {
    cell **newBoard;
    bool solvable;
    int randCol, randRow, emptyCellsCounter;
    int cellsFilled = 0;
    int triesLeft = 1000;
    int N = blockCols * blockRows;
    emptyCellsCounter = count_empty_cells(board);
    if ((!valid_set_value(initialFullCells, emptyCellsCounter)) || (!valid_set_value(copyCells, emptyCellsCounter))) {
        printf(VALUE_RANGE_ERROR, emptyCellsCounter);
        return false;
    }
    if (!board_is_empty(board)) {
        printf("Error: board is not empty\n");
        return false;
    }
    while (triesLeft > 0) {
        if (cellsFilled < initialFullCells) {
            randCol = rand() % N;
            randRow = rand() % N;
            if (board[randRow][randCol].number == UNASSIGNED) {
                if (!fill_cell(board, randCol, randRow)) { /* failed to find legal value to cell chosen */
                    triesLeft--;
                    empty_board(board);
                    cellsFilled = 0;
                } else {
                    cellsFilled++;
                }
            }
        }
        else if (cellsFilled == initialFullCells) {
            newBoard = generate_empty_board();
            solvable = ILP(board, newBoard);
            empty_board(board);
            if (solvable) {
                copy_random_cells(newBoard, copyCells, board);
                free_board(newBoard);
                print_board(board, mode);
                if (board_is_empty(board)){
                    return false;
                }
                return true;
            } else { /* failed to solve board */
                triesLeft--;
                empty_board(board);
                free_board(newBoard);
                cellsFilled = 0;
            }
        }
    }
    printf("Error: puzzle generator failed\n");
    return false;
}

/* evaluates the user-entered game command and calls the relevant function to execute it.
 * performs preliminary input checks before calling the relevant function,
 * prints an appropriate error message when necessary.
 * updates the moves list in each step that changed the status of the given sudoku board */
void execute_command(char* parsedCommand[4], cell** board, char* command, int counter, char mode, list* lst){
    bool cmdExecuted = false;
    int N = blockRows *blockCols;
    if (strcmp(parsedCommand[0], "set") == 0 && counter == 4 && (mode == 'E' || mode == 'S')) {
        if ((!is_integer(parsedCommand[1])) || (!is_integer(parsedCommand[2])) || (!is_integer(parsedCommand[3]))){
            printf(VALUE_RANGE_ERROR, N);
            return;
        }
        cmdExecuted = set(board, atoi(parsedCommand[1]), atoi(parsedCommand[2]), atoi(parsedCommand[3]), mode);
        if (cmdExecuted) {
            update_moves_list(board, lst);
        }
    } else if (strcmp(parsedCommand[0], "hint") == 0 && counter >= 3 && mode == 'S') {
        if ((!is_integer(parsedCommand[1])) || (!is_integer(parsedCommand[2]))){
            printf(VALUE_RANGE_ERROR, N);
            return;
        }
        hint(board, atoi(parsedCommand[1]), atoi(parsedCommand[2]));
    } else if (strcmp(parsedCommand[0], "validate") == 0  && (mode == 'E' || mode == 'S')) {
        validate(board);
    } else if ((strcmp(parsedCommand[0], "print_board") == 0) && (mode == 'E' || mode == 'S')) {
        print_board(board, mode);
    } else if (strcmp(parsedCommand[0], "mark_errors") == 0 && counter >= 2 && (mode == 'S')) {
        if (!is_integer(parsedCommand[1])) {
            printf(MARK_ERROR_ERROR);
            return;
        }
        mark_errors_command(atoi(parsedCommand[1]));
    } else if (strcmp(parsedCommand[0], "autofill") == 0 && mode == 'S') {
        cmdExecuted = auto_fill(board);
        if (cmdExecuted) {
            update_moves_list(board, lst);
        }
    } else if (strcmp(parsedCommand[0], "save") == 0 && counter >= 2 && (mode == 'E' || mode == 'S')) {
        save_command(board, parsedCommand[1]);
    } else if (strcmp(parsedCommand[0], "num_solutions") == 0 && (mode == 'E' || mode == 'S')) {
        num_solutions(board);
    } else if (strcmp(parsedCommand[0], "generate") == 0 && counter >= 3 && mode == 'E' ) {
        if ((!is_integer(parsedCommand[1])) || (!is_integer(parsedCommand[2]))){
            printf(VALUE_RANGE_ERROR, N);
            return;
        }
        cmdExecuted = generate_randomized_solved_board(board, atoi(parsedCommand[1]), atoi(parsedCommand[2]));
        if (cmdExecuted) {
            update_moves_list(board, lst);
        }
    } else if (((strcmp(parsedCommand[0], "undo") == 0)) && (mode == 'E' || mode == 'S')) {
        undo(lst, board, mode);
    } else if (((strcmp(parsedCommand[0], "redo") == 0)) && (mode == 'E' || mode == 'S')) {
        redo(lst, board, mode);
    } else if (((strcmp(parsedCommand[0], "reset") == 0)) && (mode == 'E' || mode == 'S')) {
        reset(lst, board, mode);
    } else if (strcmp(parsedCommand[0], "exit") == 0) {
        if (mode != 'I' || gameOver) {
            free_board(board);
            free_list(lst);
        }
        exit_game(command);
    } else {
        printf(INVALID_ERROR);
    }
}