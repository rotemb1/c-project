---
id: 9ft45
name: Game
file_version: 1.0.2
app_version: 0.6.7-1
file_blobs:
  game.c: 7bd2264a2d77a8b20b36c925ba1c9e20f10805ab
---

This is the Game class.

<br/>

See below:

<br/>

<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 game.c
```c
⬜ 46     }
⬜ 47     
⬜ 48     /* empty sudoku board by assigning all cells values to UNASSIGNED and by removing fixation or asterisks */
🟩 49     void empty_board(cell** board) {
🟩 50         int i, j;
🟩 51         int N = blockRows * blockCols;
🟩 52         for (i = 0; i < N; i++) {
🟩 53             for (j = 0; j < N; j++) {
🟩 54                 board[i][j].number = UNASSIGNED;
🟩 55                 board[i][j].asterisk = false;
🟩 56                 board[i][j].isFixed = false;
🟩 57             }
🟩 58         }
🟩 59     }
⬜ 60     
⬜ 61     /* returns true if all sudoku board cells are empty (UNASSIGNED), else returns false */
⬜ 62     bool board_is_empty(cell** board) {
```

<br/>

This file was generated by Swimm. [Click here to view it in the app](https://app.swimm.io/repos/Z2l0aHViJTNBJTNBYy1wcm9qZWN0JTNBJTNBcm90ZW1iMQ==/docs/9ft45).