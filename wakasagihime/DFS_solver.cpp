#include "solver.h"

typedef pair<int, Move> weighted_move;

bool visit_seq_scheduler::cmp_move(const Move& move1, const Move& move2){
    Position pos1(base_position), pos2(base_position);
    pos1.do_move(move1);
    pos2.do_move(move2);
    return min_step_estimate(pos1) < min_step_estimate(pos2);
}

void visit_seq_scheduler::sort_seq(MoveList<All, Black> visit_seq,  Position pos){
    base_position = pos;
    sort(visit_seq.begin(), visit_seq.end(), 
        [this](const Move& m1, const Move& m2) {
            return cmp_move(m1, m2);
        }
    );
}

int visit_seq_scheduler::min_step_estimate(Position pos){
    int pieces = pos.pieces(Red);
    return num_bits(pieces);
}


solver::solver()
{
    seq_scheduler = new visit_seq_scheduler();
};


solver::~solver(){
    delete seq_scheduler;
}

bool solver::dfStack(Position start_pos, int limit_depth, Move* moves){
    if(start_pos.winner() == Black){
        return true;
    }
    stack<Move> search_space;
    //top is the number of members of current stack top of the current top of search_space
    stack<int> layer_cnt;
    //top is the root(previous state) of the current stack top of search_space
    stack<Position> prv_position;
    

    
    prv_position.push(start_pos);
    MoveList<All, Black> first_moves(start_pos);
    layer_cnt.push(first_moves.size());

    while(!search_space.empty()){
        if(layer_cnt.top() == 0){
            layer_cnt.pop();
            prv_position.pop();
            continue;
        }

        Move action_cur = search_space.top();
        search_space.pop();
        layer_cnt.top()--;

        Position current_pos = prv_position.top();
        current_pos.do_move( action_cur );

        int depth = prv_position.size();

        moves[depth-1] = action_cur;


        if(USE_DEBUG){
            for(int i=0;i<depth;i++){
                debug << moves[i]<<" / ";
            }
            debug << endl;
        }

        if( current_pos.winner() == Black ){
            return true;
        }
        // record(current_pos, limit_depth - depth);

        if( depth < limit_depth){
            MoveList<All, Black> nx_moves(current_pos);
            
            seq_scheduler->sort_seq(nx_moves, current_pos);

            // search_space.push(END);
            prv_position.push(current_pos);
            layer_cnt.push(0);
            for(int move_idx = 0; move_idx < nx_moves.size(); move_idx++){
                Move nx_move = nx_moves[move_idx];
                Position nx_state(current_pos);
                nx_state.do_move(nx_move);


                search_space.push(nx_move);
                layer_cnt.top()++;
            }
            if(layer_cnt.top() == 0){
                layer_cnt.pop();
                prv_position.pop();
            }
        }
    }
    return false;
}