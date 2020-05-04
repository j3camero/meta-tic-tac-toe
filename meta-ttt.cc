#include <stdio.h>
#include <iostream>
#include <vector>

using namespace std;

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

const char human_readable[] = "-XO";

struct BoardState;

struct Move {
  int subboard_constraint;
  BoardState *new_state;

  Move(int subboard_constraint, BoardState *new_state) {
    this->subboard_constraint = subboard_constraint;
    this->new_state = new_state;
  }
};

struct BoardState {
  int index;
  int squares[9];
  bool solved;
  int warpable;
  float px;
  float po;
  vector<Move> x_moves;
  vector<Move> o_moves;
};

const int NUM_STATES = 19683;  // 3^9.
BoardState states[NUM_STATES];  // Every possible configuration of X's and O's.

const float assumed_move_probability = 0.9;
const float epsilon = 0.0000001;

int Index(int *board) {
  int index = 0;
  int base = 1;
  for (int i = 0; i < 9; ++i) {
    index += board[i] * base;
    base *= 3;
  }
  return index;
}

void Deindex(int index, int *board) {
  for (int i = 0; i < 9; ++i) {
    board[i] = index % 3;
    index /= 3;
  }
}

int DetectWin(int *board) {
  for (int i = 0; i < 8; ++i) {
    int a = board[win_patterns[i][0]];
    int b = board[win_patterns[i][1]];
    int c = board[win_patterns[i][2]];
    if (a != 0 && a == b && a == c) {
      return a;
    }
  }
  return 0;
}

void PrintBoard(int *board) {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      cout << human_readable[board[3*i+j]];
    }
    cout << endl;
  }
}

void Swap(Move *a, Move *b) {
  int sc = a->subboard_constraint;
  a->subboard_constraint = b->subboard_constraint;
  b->subboard_constraint = sc;
  BoardState *t = a->new_state;
  a->new_state = b->new_state;
  b->new_state = t;
}

void BubbleSortMoves(vector<Move>& moves, bool asc) {
  int n = moves.size();
  for (int i = 0; i < n - 1; ++i) {
    for (int j = 0; j < n - 1; ++j) {
      BoardState *a = moves[j].new_state;
      BoardState *b = moves[j+1].new_state;
      float as = a->px - a->po;
      float bs = b->px - b->po;
      if ((asc && as > bs) || (!asc && bs > as)) {
	Swap(&moves[j], &moves[j+1]);
      }
    }
  }
}

void InitGameTree() {
  for (int i = 0; i < NUM_STATES; ++i) {
    BoardState& s = states[i];
    Deindex(i, s.squares);
    s.index = i;
    s.solved = false;
    s.warpable = false;
    int winner = DetectWin(s.squares);
    if (winner != 0) {
      s.solved = true;
      s.warpable = true;
      s.px = (winner == 1) ? 1.0 : 0.0;
      s.po = (winner == 2) ? 1.0 : 0.0;
      continue;
    }
    for (int j = 0; j < 9; ++j) {
      if (s.squares[j] != 0) {
	continue;
      }
      s.squares[j] = 1;
      int x_move_index = Index(s.squares);
      BoardState *x_move = &states[x_move_index];
      s.x_moves.push_back(Move(j, x_move));
      s.squares[j] = 2;
      int o_move_index = Index(s.squares);
      BoardState *o_move = &states[o_move_index];
      s.o_moves.push_back(Move(j, o_move));
      s.squares[j] = 0;
    }
    if (s.x_moves.size() == 0 && s.o_moves.size() == 0) {
      s.px = 0;
      s.po = 0;
      s.solved = true;
      s.warpable = true;
    }
  }
  for (int pass = 0; pass < 12; ++pass) {
    int num_positions_solved_this_pass = 0;
    for (int i = 0; i < NUM_STATES; ++i) {
      BoardState& s = states[i];
      if (s.solved) {
	continue;
      }
      bool missing_dependency = false;
      float xx = 0, xo = 0, ox = 0, oo = 0;
      for (int i = 0; i < s.x_moves.size(); ++i) {
	BoardState *m = s.x_moves[i].new_state;
	if (!m->solved) {
	  missing_dependency = true;
	  break;
	}
	xx += m->px;
	xo += m->po;
      }
      if (missing_dependency) {
	continue;
      }
      for (int i = 0; i < s.o_moves.size(); ++i) {
	BoardState *m = s.o_moves[i].new_state;
	if (!m->solved) {
	  missing_dependency = true;
	  break;
	}
	ox += m->px;
	oo += m->po;
      }
      if (missing_dependency) {
	continue;
      }
      float invx = 1.0 / s.x_moves.size();
      float invo = 1.0 / s.o_moves.size();
      s.px = 0.5 * assumed_move_probability * (xx * invx + ox * invo);
      s.po = 0.5 * assumed_move_probability * (xo * invx + oo * invo);
      s.solved = true;
      ++num_positions_solved_this_pass;
      float draw = 1.0 - s.px - s.po;
      //printf("px: %.4f po: %.4f draw: %.4f index: %d\n",
      //     s.px, s.po, draw, s.index);
      //Deindex(s.index, board);
      //PrintBoard(board);
      if (draw < -epsilon || draw > 1 + epsilon ||
	  s.px < -epsilon || s.px > 1 + epsilon ||
	  s.po < -epsilon || s.po > 1 + epsilon) {
	cout << "!!! ERROR" << endl;
	break;
      }
    }
    cout << "Pass " << pass << ": solved " << num_positions_solved_this_pass
	 << " states" << endl;
  }
  cout << "Sorting move lists." << endl;
  for (int i = 0; i < NUM_STATES; ++i) {
    BoardState& s = states[i];
    BubbleSortMoves(s.x_moves, false);
    BubbleSortMoves(s.o_moves, true);
  }
}

BoardState *big_board[9] = {
  &states[0], &states[0], &states[0],
  &states[0], &states[0], &states[0],
  &states[0], &states[0], &states[0]
};
int global_constraint = -1;

float Eval(int turn, bool *game_over) {
  // abc
  // def
  // ghi
  float xa = big_board[0]->px;
  float xb = big_board[1]->px;
  float xc = big_board[2]->px;
  float xd = big_board[3]->px;
  float xe = big_board[4]->px;
  float xf = big_board[5]->px;
  float xg = big_board[6]->px;
  float xh = big_board[7]->px;
  float xi = big_board[8]->px;
  float oa = big_board[0]->po;
  float ob = big_board[1]->po;
  float oc = big_board[2]->po;
  float od = big_board[3]->po;
  float oe = big_board[4]->po;
  float of = big_board[5]->po;
  float og = big_board[6]->po;
  float oh = big_board[7]->po;
  float oi = big_board[8]->po;
  float px = 1.0 -
    (1.0 - xa * xb * xc) *
    (1.0 - xd * xe * xf) *
    (1.0 - xg * xh * xi) *
    (1.0 - xa * xd * xg) *
    (1.0 - xb * xe * xh) *
    (1.0 - xc * xf * xi) *
    (1.0 - xa * xe * xi) *
    (1.0 - xc * xe * xg);
  float po = 1.0 -
    (1.0 - oa * ob * oc) *
    (1.0 - od * oe * of) *
    (1.0 - og * oh * oi) *
    (1.0 - oa * od * og) *
    (1.0 - ob * oe * oh) *
    (1.0 - oc * of * oi) *
    (1.0 - oa * oe * oi) *
    (1.0 - oc * oe * og);
  *game_over = ((px < epsilon || px > 1.0 - epsilon) &&
		(po < epsilon || po > 1.0 - epsilon));
  //cout << px << " " << po << endl;
  if (turn == 1) {
    return px - po;
  } else {
    return po - px;
  }
}

float Minimax(int depth, int turn, int subboard_constraint,
	      int *node_counter) {
  bool game_over;
  *node_counter += 1;
  float score = Eval(turn, &game_over);
  if (depth <= 0 || game_over) {
    return score;
  }
  float alpha = -999;
  BoardState *b = (subboard_constraint < 0) ?
    NULL : big_board[subboard_constraint];
  if (subboard_constraint < 0 || b->warpable) {
    for (int i = 0; i < 9; ++i) {
      if (!big_board[i]->warpable) {
	float score = Minimax(depth, turn, i, node_counter);
	if (score > alpha) {
	  alpha = score;
	}
      }
    }
    return alpha;
  }
  const vector<Move>& moves = (turn == 1) ? b->x_moves : b->o_moves;
  for (int i = 0; i < moves.size(); ++i) {
    const Move& m = moves[i];
    big_board[subboard_constraint] = m.new_state;  // Do move.
    float score = -Minimax(depth - 1, 3 - turn, m.subboard_constraint,
			   node_counter);
    big_board[subboard_constraint] = b;  // Undo move.
    if (score > alpha) {
      alpha = score;
    }
  }
  return alpha;
}

float Alphabeta(int depth, int turn, int subboard_constraint,
		float alpha, float beta, int *node_counter) {
  bool game_over;
  *node_counter += 1;
  float score = Eval(turn, &game_over);
  if (depth <= 0 || game_over) {
    return score;
  }
  BoardState *b = (subboard_constraint < 0) ?
    NULL : big_board[subboard_constraint];
  if (subboard_constraint < 0 || b->warpable) {
    for (int i = 0; i < 9; ++i) {
      if (!big_board[i]->warpable) {
	float score = Alphabeta(depth, turn, i, alpha, beta, node_counter);
	if (score > alpha) {
	  alpha = score;
	}
      }
    }
    return alpha;
  }
  const vector<Move>& moves = (turn == 1) ? b->x_moves : b->o_moves;
  for (int i = 0; i < moves.size(); ++i) {
    const Move& m = moves[i];
    big_board[subboard_constraint] = m.new_state;  // Do move.
    float score = -Alphabeta(depth - 1, 3 - turn, m.subboard_constraint,
			     -beta, -alpha, node_counter);
    big_board[subboard_constraint] = b;  // Undo move.
    if (score >= beta) {
      return beta;
    }
    if (score > alpha) {
      alpha = score;
    }
  }
  return alpha;
}

float MakeAiMove(int turn, int depth) {
  float best_score = -999;
  int best_board = -1;
  BoardState *best_board_state = NULL;
  int best_move_index = -1;
  BoardState *a = big_board[global_constraint];
  bool multisearch = global_constraint < 0 || a->warpable;
  int node_counter = 0;
  for (int i = 0; i < 9; ++i) {
    if (!multisearch && i != global_constraint) {
      continue;
    }
    BoardState *b = big_board[i];
    if (b->warpable) {
      continue;
    }
    const vector<Move>& moves = (turn == 1) ? b->x_moves : b->o_moves;
    for (int j = 0; j < moves.size(); ++j) {
      const Move& m = moves[j];
      big_board[i] = m.new_state;  // Do move.
      float score = -Alphabeta(depth - 1, 3 - turn, m.subboard_constraint,
			       -999, -best_score, &node_counter);
      big_board[i] = b;  // Undo move.
      if (score > best_score) {
	best_score = score;
	best_board = i;
	best_board_state = m.new_state;
	best_move_index = m.subboard_constraint;
      }
    }
  }
  if (best_board < 0) {
    cout << "ERROR: no moves available!" << endl;
    return -1.0;
  }
  big_board[best_board] = best_board_state;
  global_constraint = best_move_index;
  cout << "Searched " << (node_counter/1000000) << "M nodes. score: "
       << best_score << endl;
  return best_score;
}

void PrintGameState() {
  printf("+===+===+===+\n");
  for (int big_row = 0; big_row < 3; ++big_row) {
    for (int small_row = 0; small_row < 3; ++small_row) {
      printf("|");
      for (int big_col = 0; big_col < 3; ++big_col) {
	for (int small_col = 0; small_col < 3; ++small_col) {
	  int state = big_board[3 * big_row + big_col]
	    ->squares[3 * small_row + small_col];
	  printf("%c", human_readable[state]);
	}
	printf("|");
      }
      printf("\n");
    }
    printf("+===+===+===+\n");
  }
}

void MakeHumanMove(int turn) {
  int big_index = 0;
  int small_index = 0;
  printf("Enter move for %c: ", human_readable[turn]);
  scanf("%d %d", &big_index, &small_index);
  BoardState *b = big_board[big_index];
  const vector<Move>& moves = (turn == 1) ? b->x_moves : b->o_moves;
  for (int i = 0; i < moves.size(); ++i) {
    const Move& m = moves[i];
    if (m.subboard_constraint == small_index) {
      big_board[big_index] = m.new_state;
      global_constraint = m.subboard_constraint;
      break;
    }
  }
}

int main() {
  InitGameTree();
  int depth = 10;
  bool game_over;
  while (true) {
    PrintGameState();
    MakeAiMove(1, depth);
    //MakeHumanMove(1);
    Eval(1, &game_over);
    if (game_over) break;
    PrintGameState();
    //MakeAiMove(2, depth);
    MakeHumanMove(2);
    Eval(2, &game_over);
    if (game_over) break;
  }
  return 0;
}
