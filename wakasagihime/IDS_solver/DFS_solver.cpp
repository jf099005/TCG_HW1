#include "DFS_solver.h"
#include<cassert>


typedef pair<int, Move> weighted_move;

bool visit_seq_scheduler::cmp_move(const Move& move1, const Move& move2){
    Position pos1(base_position), pos2(base_position);
    pos1.do_move(move1);
    pos2.do_move(move2);
    return min_step_estimate(pos1) < min_step_estimate(pos2);
}

void visit_seq_scheduler::sort_seq(MoveList<All, Black>& visit_seq,  Position pos){
    base_position = pos;

    // weighted_move sorted_seq[ visit_seq.size() ];
    // for(int i=0; i<visit_seq.size(); i++){
    //     Position nx_pos(pos);
    //     nx_pos.do_move(visit_seq[i]);
    //     sorted_seq[i] = weighted_move( min_step_estimate( nx_pos ), visit_seq[i] );
    // }

    // sort(sorted_seq, sorted_seq + visit_seq.size());

    // for(int i=0; i<visit_seq.size(); i++){
    //     visit_seq[i] = sorted_seq[i].second;
    // }

    sort(visit_seq.begin(), visit_seq.end(), 
        [this](const Move& m1, const Move& m2) {
            return cmp_move(m1, m2);
        }
    );
}

int rough_estimate(Position pos){
    int pieces = pos.pieces(Red);
    return num_bits(pieces);
}


void visit_seq_scheduler::calculate_shortest_path(){
    //dis[i,j] = shortest_path[i*SQUARE_NB + j]
    static const int inf = 100;
    // static const Direction main_directions[] = {EAST, WEST, NORTH, SOUTH};

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

    for(int a=0; a<SQUARE_NB; a++){
        for(int b=0; b<SQUARE_NB; b++){
            for(int c=0; c<SQUARE_NB; c++){
                int path_len = _shortest_path[represent(a, c)] + _shortest_path[represent(c, b)];
                _shortest_path[represent(a, b)] = min( _shortest_path[represent(a, b)], path_len);
            }
        }
    }

}

int min_route_estimate(Position pos){
    static int total_dis[1000];
    int dis_cnt = 0;
    int num_red = num_bits(pos.pieces(Red));
    if(num_red == 0){
        return 0;
    }
    assert(num_red < 32);
    auto all_pieces = BoardView(pos.pieces());

    // int shortest_path[SQUARE_NB* SQUARE_NB];
    bool black_Chariot_exist = pos.count(Black, Chariot);

    for(Square sq_a: all_pieces){
            
        for(Square sq_b: all_pieces){
            if(sq_a <= sq_b)
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

            if(!(a.type > b.type)){
                continue;
            }

            int piece_distance = distance<Square> (sq_a, sq_b);

            if(a.type == Chariot){
                piece_distance = 2 - (sq_a%8 == sq_b%8) - (sq_a/8 == sq_b/8);
            }

            if(black_Chariot_exist and a.side != Black and\
                             Chariot > a.type and Chariot > b.type){
                piece_distance = 2 - (sq_a%8 == sq_b%8) - (sq_a/8 == sq_b/8);
            }
            assert(dis_cnt < 100);
            total_dis[dis_cnt++] = piece_distance;
        }
    }
    if(dis_cnt <= 0){
        debug << "ERROR: call min_route_cnt at dis_cnt == 0";
        debug << pos <<endl;
        abort;
    };
    sort(total_dis, total_dis + dis_cnt);
    int move_estimate = 0;
    for(int i=0; i<num_red; i++){
        move_estimate += total_dis[i];
    }

    return move_estimate;
}

int visit_seq_scheduler::min_step_estimate(Position pos){
    return min_route_estimate(pos);
}


solver::solver()
{
    seq_scheduler = new visit_seq_scheduler();
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

bool operator != (const Position& A, const Position& B){
    return !( A==B );
}


bool solver::dfStack(Position start_pos, int limit_depth, Move* moves){
    if(start_pos.winner() == Black){
        return true;
    }
    if(limit_depth == 0){
        return false;
    }
    stack<Move> search_space;
    //top is the number of members of current stack top of the current top of search_space
    static int layer_cnt[MAX_MOVE_NUM];
    static Position prv_positions[MAX_MOVE_NUM];
    
    //top is the root(previous state) of the current stack top of search_space
    

    layer_cnt[0] = 0;
    prv_positions[0] = (start_pos);
    MoveList<All, Black> first_moves(start_pos);


    for(int i=0; i<first_moves.size(); i++){
        search_space.push( first_moves[i] );
        layer_cnt[0]++;
    }
    
    int depth = 1;
    //layer[d-1]: number of node at Depth = d
    //prv_positions[d]: state at Depth = d
    while(!search_space.empty()){

        assert(depth > 0);
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
            for(int i=0;i<depth;i++){
                debug << moves[i]<<" / ";
            }
            debug << endl;
        }

        if( current_pos.winner() == Black and depth <= limit_depth){
            return true;
        }
        // record(current_pos, limit_depth - depth);

        if( depth < limit_depth){
            MoveList<All, Black> nx_moves(current_pos);
            
            seq_scheduler->sort_seq(nx_moves, current_pos);

            prv_positions[depth] = (current_pos);
            layer_cnt[depth] = 0; // initialize cnt for layer depth+1
            for(int move_idx = 0; move_idx < nx_moves.size(); move_idx++){
                Move nx_move = nx_moves[move_idx];
                Position nx_state(current_pos);
                nx_state.do_move(nx_move);
                if(nx_state.winner() == Black){
                    moves[depth] = nx_move;
                    return true;
                }

                bool is_new_state = true;
                
                for(int i=0; i<=depth; i++){
                    is_new_state &= (prv_positions[i] != nx_state);
                }
                
                if( seq_scheduler->min_step_estimate(nx_state) + depth + 1 > limit_depth ){
                    continue;
                }

                if(!is_new_state)
                    continue;

                search_space.push(nx_move);
                layer_cnt[depth]++;
            }
            if(layer_cnt[depth] > 0){
                depth++;
                layer_cnt[depth] = 0;
            }
        }
    }
    return false;
}