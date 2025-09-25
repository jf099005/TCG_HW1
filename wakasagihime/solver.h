// Chinese Dark Chess: your turn
// ----------------------------------
// You should add your own headers if needed, instead of modifying this file!

#ifndef SOLVER_H
#define SOLVER_H

#include "lib/cdc.h"
#include "lib/chess.h"
#include "lib/helper.h"
#include "lib/movegen.h"
#include "lib/types.h"
#include<stack>
#include<map>
#define USE_DEBUG false
#define max_path_length 100
using namespace std;

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

class solver{
    public:
        map<string, int> visited_states;
        int visit_cnt;
        void record(Position pos, int depth_limit);
        bool is_visited(Position pos, int depth_limit);
        
        solver();
        void init(bool clear_visited_states);
            
        //approximate the lower bound of moves to finish the pos
        int min_step_estimate(Position pos);
        bool dfs(Position pos, int limit_depth, Move* moves, int &depth);

};

void resolve(Position &pos);

#endif