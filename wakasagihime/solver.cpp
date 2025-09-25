#include "solver.h"
#include "lib/helper.h"
/*
 * Wakasagi will call this and only this function.
 * Below you will find examples of functions you might use
 * and an example for outputting random moves.
 *
 * The header files contain detailed comments.
 *
 * Guide & documentation can be found here:
 * https://docs.google.com/document/d/1AcbayzQvl0wyp6yl5XzhtDRPyOxINTpjR-1ghtxHQso/edit?usp=sharing
 *
 * Good luck!
 */


// bool is_cycle_move(){
    
// }

typedef pair<int, Move> weighted_move;


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

solver::solver()
{};

void solver::init(bool clear_visited_states = false){
    this->visit_cnt = 0;
    if(clear_visited_states){
            this->visited_states.clear();
    }
}

        //dfs of non-revursion version
        //return the path length
        // int dfStack(Position pos, int limit_depth, Move* moves){
        //     const string END = "end";
        //     init();
        //     int depth = 0;
        //     stack<string> search_space;
        //     stack<Move> search_actions;
        //     search_space.push( pos.toFEN() );
        //     MoveList<All, Black> first_moves(pos);
        //     for(int i=0; i<first_moves.size(); i++){
        //         search_actions.push( first_moves[i] );
        //     }
        //     while(!search_space.empty()){
        //         string s_cur = search_space.top();
        //         search_space.pop();
                
        //         if(s_cur == END){
        //             depth--;
        //             continue;
        //         }

        //         Position current_pos(s_cur);
        //         if( current_pos.winner() == Black ){
        //             return depth;
        //         }

        //         record(current_pos, depth);
        //         MoveList<All, Black> nx_moves(current_pos);
                
        //         search_space.push(END);
        //         for(int move_idx = 0; move_idx < nx_moves.size(); move_idx++){
        //             Move nx_move = nx_moves[move_idx];
        //             Position nx_state( pos.toFEN() );
        //             nx_state.do_move(nx_move);
        //             if(is_visited(nx_state, limit_depth)){
        //                 continue;
        //             }
        //             search_space.push(nx_state.toFEN());
        //         }
        //         depth++;
        //     }
        // }

int solver::min_step_estimate(Position pos){
    int pieces = pos.pieces(Red);
    return num_bits(pieces);
}

bool solver::dfs(Position pos, int limit_depth, Move* moves, int &depth){
    if(limit_depth < 0){
        return false;
    }
    if( pos.winner() == Black ){
        return true;
    }
    if(USE_DEBUG){
        debug << "visit state "<< pos << "of depth " << depth<<endl;
        string x;
        getline(cin, x);
    }
    if( is_visited(pos, limit_depth) ){
        return false;
    }
    record( pos, limit_depth );
    visit_cnt++;

    MoveList<All, Black> nx_moves(pos);

    weighted_move nx_moves_with_weight[ nx_moves.size() ];
    for(int i=0; i<nx_moves.size(); i++){
        Move nx_move = nx_moves[i];
        Position nx_state( pos.toFEN() );
        nx_state.do_move(nx_move);

        nx_moves_with_weight[i] =  weighted_move(min_step_estimate(nx_state), nx_move);
    }
    
    sort(nx_moves_with_weight, nx_moves_with_weight + nx_moves.size());

    depth++;
    for(int move_idx = 0; move_idx < nx_moves.size(); move_idx++){
        // Move nx_move = nx_moves[move_idx];
        Move nx_move = nx_moves_with_weight[move_idx].second;
        moves[depth-1] = nx_move;
        Position nx_state( pos.toFEN() );
        nx_state.do_move(nx_move);

        // if( visited_states.count( encode_board(nx_state, ) )  )
        //     continue;
        if(is_visited(nx_state, limit_depth)){
            continue;
        }

        bool solution = dfs(nx_state, limit_depth-1, moves, depth);
        if(solution){
            return true;
        }
    }
    depth -= 1;
    return false;
}

void resolve(Position &pos)
{
    /* You can use these aliases */
    // info << "Output (only) your answers here!\n"; // ====> stdout <====
    // debug << "Print debug info here!\n";          // stderr
    // error << "Print errors here?\n";              // also stderr

    // info << pos;

    /* A random number generator is available globally */
    int random_num_below_42 = rng(42);

    /* Generate moves like this */
    MoveList<All, Black> moves(pos); // `<All, Black>` is optional

    /* Iterate all moves */
    // for (Move mv : moves) {
    //     info << mv;                       // print the move
    //     Position pos_copy(pos);           // copy the position
    //     bool done = pos_copy.do_move(mv); // do the move
    // }

    /* Get some pieces */
    Board all_the_pieces = pos.pieces();
    Board black_pieces   = pos.pieces(Black);
    Board black_soldiers = pos.pieces(Black, Soldier);

    /* Iterate those pieces */
    // for (Square sq : BoardView(black_pieces)) {
    //     info << "There is a black piece at square " << sq << ".";

    //     // To get _complete_ information about the piece at a square,
    //     // use peek_piece_at()
    //     Piece p = pos.peek_piece_at(sq);
    //     if (p.type == Advisor) {
    //         info << " It is an advisor.";
    //     }

    //     info << "\n";
    // }

    /* Distance between two squares */
    int dist      = distance(SQ_B2, SQ_C4);       // 3
    int dist_rank = distance<Rank>(SQ_B2, SQ_C4); // 1

    Move opt_path[100];
    solver s;
    
    if(USE_DEBUG){
        debug << "step estimation:" << s.min_step_estimate(pos) <<endl;
        debug<<"start DFS\n";

    }
    
    for(int d=1;d<=30;d++){
        int opt_path_len = 0;
        s.init();
        bool status = s.dfs(pos, d, opt_path, opt_path_len);
        if(!status){
            if(USE_DEBUG)
                debug<<"fail at depth "<<d<<endl;
            continue;
        }
        if(USE_DEBUG){
            debug<<"visit cnt:" << s.visit_cnt<<endl;
            debug<<  "movelen:" << opt_path_len <<endl;
            debug << "DFS:\n";
        }
        for(int i=0;i<opt_path_len; i++){
            info << opt_path[i];
        }
        break;
    }
    // info<<"End DFS\n";
    /* Example: output a random legal move */
    // info << "Random walking:" << std::endl;
    // for (int counter = 21; counter -- /* counter slides to 0 */
    //                                  \
    //                                   \
    //                                    \
    //                                     > 0;) {
    //     MoveList mvs(pos);
    //     if (mvs.size() == 0) {
    //         return;
    //     }
    //     Move chosen = mvs[rng(mvs.size())];
    //     info << (21 - counter) << ". " << chosen;
    //     pos.do_move(chosen);
    // }
}