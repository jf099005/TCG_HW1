#ifndef DFS_SOLVER_H
#define DFS_SOLVER_H

#define max_path_length 100
#define FAIL (-1)


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
        visit_seq_scheduler(){};
        void sort_seq(MoveList<All, Black>& visit_seq, Position pos);
        bool cmp_move(const Move& move1, const Move& move2);


    // private:
        int min_step_estimate(Position pos);
        Position base_position;
};


class solver{
    public:
        solver();
        ~solver();
        void init(bool clear_visited_states = false);
            
        //approximate the lower bound of moves to finish the pos
        bool dfStack(Position pos, int limit_depth, Move* moves);
    // private:
        visit_seq_scheduler* seq_scheduler;
};
#endif