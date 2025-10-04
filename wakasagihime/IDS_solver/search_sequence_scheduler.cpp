#include"search_sequence_scheduler.h"

#include<queue>
#include<cassert>

visit_seq_scheduler::visit_seq_scheduler(Position pos):
    base_position(pos)
{
    calculate_shortest_path();
};

static const int inf = 40;

void visit_seq_scheduler::calculate_shortest_path(){
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
    static short min_dis[SQUARE_NB + 5];
    int num_red = pos.count(Red);//num_bits(pos.pieces(Red));

    std::fill_n(min_dis, num_red, inf);
    
    Board Red_pieces = (pos.pieces(Red));
    Board Black_pieces = (pos.pieces(Black));
    // Board all_pieces = (pos.pieces());

    // int shortest_path[SQUARE_NB* SQUARE_NB];
    bool black_Chariot_exist = (pos.count(Black, Chariot) > 0);

    for(Square sq_a: BoardView(Red_pieces)){
        for(Square sq_b: BoardView(Red_pieces)){
            if(int(sq_a) <= int(sq_b))
                continue;

            Piece a = pos.peek_piece_at(sq_a), b = pos.peek_piece_at(sq_b);
            int piece_distance = shortest_path(sq_a, sq_b);//distance<Square> (sq_a, sq_b);
            if(black_Chariot_exist and\
                             Chariot > a.type and Chariot > b.type){
                piece_distance = 2 - (sq_a%8 == sq_b%8) - (sq_a/8 == sq_b/8);
            }

            if(min_dis[num_red - 1] > piece_distance){
                min_dis[num_red-1] = piece_distance;
                for(int j=num_red-1; j > 0; j--){
                    if(min_dis[j] < min_dis[j-1]){
                        swap(min_dis[j], min_dis[j-1]);
                    }
                }
            }            
        }
    }


    for(Square sq_a: BoardView(Black_pieces)){
            
        for(Square sq_b: BoardView(Red_pieces)){
            Piece a = pos.peek_piece_at(sq_a), b = pos.peek_piece_at(sq_b);

            if( !(a.type > b.type) ){
                continue;
            }

            int piece_distance = shortest_path(sq_a, sq_b);//distance<Square> (sq_a, sq_b);

            if(a.type == Chariot){
                piece_distance = 2 - (sq_a%8 == sq_b%8) - (sq_a/8 == sq_b/8);
            }

            if(min_dis[num_red - 1] > piece_distance){
                min_dis[num_red-1] = piece_distance;
                for(int j=num_red-1; j > 0; j--){
                    if(min_dis[j] < min_dis[j-1]){
                        swap(min_dis[j], min_dis[j-1]);
                    }
                }
            }
            
        }
    }
    int move_estimate = std::accumulate(min_dis, min_dis + num_red, 0);
    return move_estimate;
}

int visit_seq_scheduler::min_step_estimate(Position pos){
    int estimate = min_route_estimate(pos);
    assert(estimate >= 0);
    return estimate;
}
