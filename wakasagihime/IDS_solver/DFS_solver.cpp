#include "DFS_solver.h"

#include<cassert>

visit_seq_scheduler::visit_seq_scheduler(Position pos):
    base_position(pos)
{
    calculate_shortest_path();
};

void visit_seq_scheduler::calculate_shortest_path(){
    static const int inf = 40;

    for(int i=0; i< SQUARE_NB*SQUARE_NB; i++){
        _shortest_path[i] = inf;
    }
    for(int i=0; i<SQUARE_NB; i++){
        Square sq_i = Square(i);
        if(base_position.peek_piece_at(sq_i).type == Duck){
            continue;
        }
        _shortest_path[represent(i,i)] = 0;
        if(!at_left_boundary(sq_i)){
            Square sq_iL = sq_i + WEST;
            if( base_position.peek_piece_at(sq_iL).type != Duck ){
                _shortest_path[represent(sq_i, sq_iL)] = 1;
            }
        }

        if(!at_right_boundary(sq_i)){
            Square sq_iL = sq_i + EAST;
            if( base_position.peek_piece_at(sq_iL).type != Duck ){
                _shortest_path[represent(sq_i, sq_iL)] = 1;
            }
        }

        if(!at_upper_boundary(sq_i)){
            Square sq_iL = sq_i + SOUTH;
            if( base_position.peek_piece_at(sq_iL).type != Duck ){
                _shortest_path[represent(sq_i, sq_iL)] = 1;
            }
        }

        if(!at_lower_boundary(sq_i)){
            Square sq_iL = sq_i + NORTH;
            if( base_position.peek_piece_at(sq_iL).type != Duck ){
                _shortest_path[represent(sq_i, sq_iL)] = 1;
            }
        }
    }

    for(int c=0; c<SQUARE_NB; c++){
        for(int b=0; b<SQUARE_NB; b++){
            for(int a=0; a<SQUARE_NB; a++){
                short path_len = _shortest_path[represent(a, c)] + _shortest_path[represent(c, b)];
                _shortest_path[represent(a, b)] = min( _shortest_path[represent(a, b)], path_len);
            }
        }
    }

}

bool visit_seq_scheduler::cmp_move(const Move& move1, const Move& move2){
    Position pos1(base_position), pos2(base_position);
    pos1.do_move(move1);
    pos2.do_move(move2);
    int estimate_1 = min_step_estimate(pos1);
    int estimate_2 = min_step_estimate(pos2);
    return estimate_1 < estimate_2;
}

inline void swap_move( Move* a, Move* b ){
    Move c = *a;
    *a = *b;
    *b = c;
}

int visit_seq_scheduler::sort_arr(MoveList<All, Black>& visit_seq, Position &pos, int depth_limit){
    int valid_moves = 0;
    base_position = pos;

    static int scores[SQUARE_NB];
    
    for(int i=0; i<visit_seq.size(); i++){
        Position pos_cur(pos);
        pos_cur.do_move( visit_seq[i] );
        scores[i] = min_step_estimate(pos_cur);
    }

    for(int j=0; j<visit_seq.size(); j++){
        
        if(scores[j] <= depth_limit)
            valid_moves++;
        else{
            continue;
        }

        swap_move( visit_seq.begin() + j,\
                 visit_seq.begin() + valid_moves-1);
        swap(scores[j], scores[valid_moves-1]);

        for(int i=valid_moves-1; i>0; i--){
            if( scores[i] < scores[i-1] ){
                swap_move(visit_seq.begin() + i, visit_seq.begin() + i-1);
                swap(scores[i], scores[i-1]);
            }
            else{
                break;
            }
        }
    }
    return valid_moves;
}

int rough_estimate(Position pos){
    int pieces = pos.pieces(Red);
    return num_bits(pieces);
}

int visit_seq_scheduler::min_route_estimate(const Position& pos) const{
    static short total_dis[1000];
    int dis_cnt = 0;
    int num_red = pos.count(Red);//num_bits(pos.pieces(Red));

    Board all_pieces = (pos.pieces());

    // int shortest_path[SQUARE_NB* SQUARE_NB];
    bool black_Chariot_exist = (pos.count(Black, Chariot) > 0);

    for(Square sq_a: BoardView(all_pieces)){
            
        for(Square sq_b: BoardView(all_pieces)){
            if(int(sq_a) <= int(sq_b))
                continue;

            Piece a = pos.peek_piece_at(sq_a), b = pos.peek_piece_at(sq_b);
            
            if(a.side==Black and b.side==Black){
                continue;
            }
            
            if( a.type==Duck || b.type==Duck){
                continue;
            }

            if(b.side == Black){
                swap(a,b);
            }

            if( !(a.type > b.type) and a.side == Black){
                continue;
            }

            int piece_distance = shortest_path(sq_a, sq_b);//distance<Square> (sq_a, sq_b);

            if(a.type == Chariot and a.side == Black){
                piece_distance = 2 - (sq_a%8 == sq_b%8) - (sq_a/8 == sq_b/8);
                // (distance<Rank>(sq_a, sq_b) > 0) + ( distance<File>(sq_a, sq_b) > 0 );//
            }

            if(black_Chariot_exist and a.side != Black and\
                             Chariot > a.type and Chariot > b.type){
                // piece_distance = (distance<Rank>(sq_a, sq_b) > 0) + ( distance<File>(sq_a, sq_b) > 0 );//2 - (sq_a%8 == sq_b%8) - (sq_a/8 == sq_b/8);
                piece_distance = 2 - (sq_a%8 == sq_b%8) - (sq_a/8 == sq_b/8);
            }

            total_dis[dis_cnt++] = piece_distance;
        }
    }

    // sort(total_dis, total_dis + dis_cnt);

    for(int i=0; i<dis_cnt; i++){
        for(int j=i; j>0; j--){
            if(total_dis[j] < total_dis[j-1]){
                swap(total_dis[j], total_dis[j-1]);
            }
            else{
                break;
            }
        }
    }

    int move_estimate = 0;
    for(int i=0; i<num_red; i++){
        move_estimate += total_dis[i];
    }

    return move_estimate;
}

int visit_seq_scheduler::min_step_estimate(Position pos){
    int estimate = min_route_estimate(pos);
    assert(estimate >= 0);
    return estimate;
}


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


bool solver::dfStack(Position start_pos, int limit_depth, Move* moves){
    if(start_pos.winner() == Black){
        return true;
    }
    if(limit_depth == 0)
        return false;
    stack<Move> search_space;
    //top is the number of members of current stack top of the current top of search_space
    int layer_cnt[MAX_MOVE_NUM];
    Position prv_positions[MAX_MOVE_NUM];
    
    //top is the root(previous state) of the current stack top of search_space
    

    layer_cnt[0] = 0;
    prv_positions[0] = (start_pos);
    MoveList<All, Black> first_moves(start_pos);

    int first_move_size = seq_scheduler->sort_arr(first_moves, start_pos, limit_depth);


    if(USE_DEBUG){
        debug <<"depth "<< 0 <<"/" <<limit_depth <<endl;
        debug <<"generated moves: " << first_move_size <<endl;
        for(int i=0; i<first_move_size; i++){
            debug<<"\t\t" <<first_moves[i];
        }
        debug <<endl;
    }

    for(int i=0; i<first_move_size; i++){
        search_space.push( first_moves[i] );
        layer_cnt[0]++;
    }
    
    int depth = 1;
    //layer[d-1]: number of node at Depth = d
    //prv_positions[d]: state at Depth = d
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


        if(USE_DEBUG){
            debug <<"depth "<<depth <<"/" <<limit_depth <<endl;
            for(int i=0;i<depth;i++){
                debug <<"\t" << i <<": " << moves[i];
            }
            debug << endl;
        }

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

            if(USE_DEBUG){
                debug <<"generated next:" << nx_moves_size <<endl;

                for(int i=0; i<nx_moves_size ; i++){
                    debug <<"\t\t" << nx_moves[i];
                }
                cout<<endl;

            }
            
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
                
                // if( seq_scheduler->min_step_estimate(nx_state) + depth + 1 > limit_depth ){
                //     // break;
                //     debug <<"ERROR at depth " <<depth <<" and limit "<< limit_depth <<endl;
                //     debug <<"board: " << nx_state <<endl;
                //     assert(1>2);
                // }
                
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