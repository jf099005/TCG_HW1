#include "DFS_solver.h"
#include<cassert>
solver::solver(Position base_pos)
{
    seq_scheduler = new visit_seq_scheduler(base_pos);
};


solver::~solver(){
    delete seq_scheduler;
}

bool operator == (const Position& A, const Position& B){
    for (Square sq = SQ_A1; sq < SQUARE_NB; sq += 1) {
        Piece a = A.peek_piece_at(sq);
        Piece b = B.peek_piece_at(sq);
        if(char(a) != char(b)){
            return false;
        }
    }
    return A.due_up() == B.due_up();
}


bool operator < (const Position& A, const Position& B){
    for (Square sq = SQ_A1; sq < SQUARE_NB; sq += 1) {
        Piece a = A.peek_piece_at(sq);
        Piece b = B.peek_piece_at(sq);
        if(char(a) != char(b)){
            return char(a) < char(b);
        }
    }
    return A.due_up() <  B.due_up();
}



bool operator != (const Position& A, const Position& B){
    return !( A==B );
}


void solver::init(bool clear_visited_states){
    visit_cnt = 0;
    if(clear_visited_states){
            visited_states.clear();
    }
}

void solver::record(Position pos, int depth_limit){
    visited_states[ pos.toFEN() ] = depth_limit;
}

bool solver::is_visited(Position pos, int depth_limit){
    if( visited_states.find(pos.toFEN()) == visited_states.end() ){
        return false;
    }
    else{
        return visited_states[pos.toFEN()] >= depth_limit; 
    }
}


bool solver::IDS(Position start_pos, int limit_depth, Move* moves){
    if(start_pos.winner() == Black){
        return true;
    }
    if(limit_depth == 0)
        return false;
    static stack<Move> search_space;
    //top is the number of members of current stack top of the current top of search_space
    static int layer_cnt[MAX_MOVE_NUM];
    static Position prv_positions[MAX_MOVE_NUM];
    
    //top is the root(previous state) of the current stack top of search_space
    

    layer_cnt[0] = 0;
    prv_positions[0] = (start_pos);
    MoveList<All, Black> first_moves(start_pos);

    int first_move_size = seq_scheduler->sort_arr(first_moves, start_pos, limit_depth - 1);

    for(int i=0; i<first_move_size; i++){
        search_space.push( first_moves[i] );
        layer_cnt[0]++;
    }
    
    int depth = 1;
    int mx_dep = MAX_MOVES;
    while(!search_space.empty()){
        assert(0< depth);
        
        assert(depth <= limit_depth);
        if(layer_cnt[depth-1] == 0){
            depth--;
            continue;
        }

        Move action_cur = search_space.top();
        search_space.pop();
        layer_cnt[depth-1]--;

        Position current_pos = prv_positions[depth - 1];
        current_pos.do_move( action_cur );

        moves[depth-1] = action_cur;

        if( current_pos.winner() == Black and depth <= limit_depth){
            return true;
        }

        // record(current_pos, limit_depth - depth);

        if( depth < limit_depth){
            MoveList<All, Black> nx_moves(current_pos);
            
            int nx_moves_size = seq_scheduler->sort_arr(nx_moves, current_pos, limit_depth - depth -1);

            // search_space.push(END);
            prv_positions[depth] = (current_pos);
            layer_cnt[depth] = 0; // initialize cnt for layer depth+1

            for(int move_idx = 0; move_idx < nx_moves_size; move_idx++){
                Move nx_move = nx_moves[move_idx];
                Position nx_state(current_pos);
                nx_state.do_move(nx_move);

                // if(is_visited(nx_state, limit_depth - depth - 1)){
                //     continue;
                // }

                if(nx_state.winner() == Black){
                    moves[depth] = nx_move;
                    return true;
                }

                bool is_new_state = true;
                
                for(int i=0; i<=depth; i++){
                    is_new_state &= (prv_positions[i] != nx_state);
                }
                
                if(!is_new_state)
                    continue;

                search_space.push(nx_move);
                layer_cnt[depth]++;
            }
            if(layer_cnt[depth] > 0){
                depth++;
            }
        }
    }
    return false;
}