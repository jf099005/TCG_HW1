#ifndef DFS_SOLVER_H
#define DFS_SOLVER_H

#define max_path_length 100
#define FAIL (-1)

#define MEMORIZE_IN_SEARCH 1

#include "../lib/cdc.h"
#include "../lib/chess.h"
#include "../lib/helper.h"
#include "../lib/movegen.h"
#include "../lib/types.h"
#include<stack>
#include<map>
#define USE_DEBUG false
#define MAX_MOVE_NUM 100


using namespace std;

bool operator == (const Position& a, const Position& b);

inline bool operator < (const PieceType& a, const PieceType& b){
    return b > a;
}

inline int lowbit(int x){
    return x&-x;
}

inline int num_bits(int x){
    int cnt = 0;
    while(x){
        x -= lowbit(x);
        cnt++;
    }
    return cnt;
}

inline bool at_left_boundary(Square sq){
    return sq%8 == 0;
};

inline bool at_upper_boundary(Square sq){
    return sq < 8;
}

inline bool at_lower_boundary(Square sq){
    return sq >= 24;
}

inline bool at_right_boundary(Square sq){
    return sq % 8 == 7;
}

// bool at_boundary(Square sq){
//     return at_left_boundary(sq) || at_right_boundary(sq)||\
//             at_upper_boundary(sq) || at_lower_boundary(sq);
// }


class visit_seq_scheduler{
    public:
        visit_seq_scheduler(Position pos);
        void sort_seq(MoveList<All, Black>& visit_seq, Position pos);
        bool cmp_move(const Move& move1, const Move& move2) const;
        int min_step_estimate(Position pos) const;
        void calculate_shortest_path();
        inline int shortest_path(Square a, Square b) const{
            return _shortest_path[a*SQUARE_NB + b];
        }
    private:
        Position base_position;
        int _shortest_path[SQUARE_NB*SQUARE_NB];
        inline int represent(Square a, Square b){
            return a*SQUARE_NB + b;
        }
        inline int represent(int a, int b){
            return a*SQUARE_NB + b;
        }
        int min_route_estimate(Position pos) const;
};

class solver{
    public:
        solver(Position base_pos);
        ~solver();
        void init(bool clear_visited_states = false);
        
        //approximate the lower bound of moves to finish the pos
        bool dfStack(Position pos, int limit_depth, Move* moves);
    // private:
        visit_seq_scheduler* seq_scheduler;
        map<string, int> visited_seq;
};

#endif