#include <stdio.h>

// Game state. X, O, and empty are represented by 1, 2, and 0.
int turn = 1;
int sub_board_constraint = -1;
int board[9][9];

// Human-readable characters for printing the game state.
char human_readable[] = ".XO";

// For detecting win conditions.
int win_patterns[8][3] = {
  {0, 1, 2},
  {3, 4, 5},
  {6, 7, 8},
  {0, 3, 6},
  {1, 4, 7},
  {2, 5, 8},
  {0, 4, 8},
  {2, 4, 6}
};

// Indexed by the minimum number of moves that it takes each player to win
// a subboard.
const float conditional_win_probabilities[4][4] = {
  {1, 1, 1, 1, 1},
  {0, 0.4, 0.5, 0.6, 1},
  {0, 0.2, 0.3, 0.4, 1},
  {0, 0.1, 0.2, 0.3, 1},
  {0, 0, 0, 0, 0}
};

void DoMove(int big_index, int small_index) {
  board[big_index][small_index] = turn;
  turn = 3 - turn;
}

void UndoMove(int big_index, int small_index) {
  board[big_index][small_index] = 0;
  turn = 3 - turn;
}

void InitGame() {
  turn = 1;
  sub_board_constraint = -1;
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      board[i][j] = 0;
    }
  }
}

void PrintGameState() {
  printf("+---+---+---+\n");
  for (int big_row = 0; big_row < 3; ++big_row) {
    for (int small_row = 0; small_row < 3; ++small_row) {
      printf("|");
      for (int big_col = 0; big_col < 3; ++big_col) {
	for (int small_col = 0; small_col < 3; ++small_col) {
	  int state = board[3 * big_row + big_col][3 * small_row + small_col];
	  printf("%c", human_readable[state]);
	}
	printf("|");
      }
      printf("\n");
    }
    printf("+---+---+---+\n");
  }
}

void MakeHumanMove() {
  int big_index = 0;
  int small_index = 0;
  printf("Enter move for %c: ", human_readable[turn]);
  scanf("%d %d", &big_index, &small_index);
  DoMove(big_index, small_index);
}

void MakeRandomMove() {
  int big_index = sub_board_constraint;
  if (big_index < 0) {
    big_index = rand() % 
  }
}

struct DualProbability {
  float me;
  float opp;
};

void EvalSubboard(int big_index, DualProbability* p) {
  int me_min_win_moves = 999;
  int opp_min_win_moves = 999;
  for (int i = 0; i < 8; ++i) {
    int me_count = 0;
    int opp_count = 0;
    int empty_count = 0;
    for (int j = 0; j < 3; ++j) {
      int value = board[big_index][win_patterns[i][j]];
      if (value == 0) ++empty_count;
      else if (value == turn) ++me_count;
      else ++opp_count;
    }
    if (me_count == 3) {
      p->me = 1;
      p->opp = 0;
      return;
    }
    if (opp_count == 3) {
      p->me = 0;
      p->opp = 1;
      return;
    }
    if (me_count == 0) {
      if (empty_count < opp_min_win_moves) {
	opp_min_win_moves = empty_count;
      }
    }
    if (opp_count == 0) {
      if (empty_count < me_min_win_moves) {
	me_min_win_moves = empty_count;
      }
    }
  }
  p->me = win_probabilities[me_min_win_moves][opp_min_win_moves];
  p->opp = win_probabilities[opp_min_win_moves][me_min_win_moves];
}

double Eval() {
  double me_win_subboard[9];
  double opp_win_subboard[9];
  for (int i = 0; i < 9; ++i) {
    me_win_subboard[i] = ;
    opp_win_subboard[i] = ;
  }
  double me_win_game = 1.0;
  double opp_win_game = 1.0;
  for (int i = 0; i < 8; ++i) {
    double me_win_pattern = 1.0;
    double opp_win_pattern = 1.0;
    for (int j = 0; j < 3; ++j) {
      me_win_pattern *= me_win_subboard[win_patterns[i][j]];
      opp_win_pattern *= opp_win_subboard[win_patterns[i][j]];
    }
    me_win_game *= 1.0 - me_win_pattern;
    opp_win_game *= 1.0 - opp_win_pattern;
  }
  me_win_game = 1.0 - me_win_game;
  opp_win_game = 1.0 - opp_win_game;
  return me_win_game - opp_win_game;
}

int main() {
  InitGame();
  while (true) {
    PrintGameState();
    GetHumanMove();
  }
  return 0;
}
