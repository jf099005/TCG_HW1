#ifndef DFS_SOLVER_H
#define DFS_SOLVER_H

#define MEMORIZE_IN_SEARCH 1

#include "../lib/cdc.h"
#include "../lib/chess.h"
#include "../lib/helper.h"
#include "../lib/movegen.h"
#include "../lib/types.h"

#include"search_sequence_scheduler.h"
#include<stack>
#include<map>
#define USE_DEBUG false
#define MAX_MOVE_NUM 40

class solver{
    public:
        solver(Position base_pos);
        ~solver();
        void init(bool clear_visited_states = false);
        
        //approximate the lower bound of moves to finish the pos
        bool IDS(Position pos, int limit_depth, Move* moves);
    // private:

        map<string, short> visited_states;

        int visit_cnt;
        void record(Position pos, int depth_limit);
        bool is_visited(Position pos, int depth_limit);
        visit_seq_scheduler* seq_scheduler;
};

#endif