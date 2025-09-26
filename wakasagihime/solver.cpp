#include "solver.h"
#include "lib/helper.h"
#define FAIL (-1)
using namespace std::chrono;
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

int solver::dfStack(Position pos, int limit_depth, Move* moves){
    const string END = "end";
    init();
    int depth = 0;
    stack<Move> search_space;
    //top is the number of members of current stack top of the current top of search_space
    stack<int> layer_cnt;
    //top is the root(previous state) of the current stack top of search_space
    stack<Position> prv_position;
    
    
    prv_position.push(pos);
    MoveList<All, Black> first_moves(pos);
    layer_cnt.push(first_moves.size());

    for(int i=0; i<first_moves.size(); i++){
        search_space.push( first_moves[i] );
    }
    
    while(!search_space.empty()){
        if(layer_cnt.top() == 0){
            layer_cnt.pop();
            prv_position.pop();
            depth--;
        }

        Move action_cur = search_space.top();
        search_space.pop();
        layer_cnt.top()--;

        Position current_pos = prv_position.top();
        current_pos.do_move( action_cur );

        if( current_pos.winner() == Black ){
            return depth;
        }

        record(current_pos, limit_depth - depth);
        MoveList<All, Black> nx_moves(current_pos);
        
        // search_space.push(END);
        prv_position.push(pos);
        layer_cnt.push(0);
        for(int move_idx = 0; move_idx < nx_moves.size(); move_idx++){
            Move nx_move = nx_moves[move_idx];
            Position nx_state(pos);
            nx_state.do_move(nx_move);
            if(is_visited(nx_state, limit_depth)){
                continue;
            }
            search_space.push(nx_move);
            layer_cnt.top()++;
        }
        if(layer_cnt.top() == 0){
            layer_cnt.pop();
            prv_position.pop();
        }
        else{
            depth++;
        }
    }
    return FAIL;
}

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

    auto start = high_resolution_clock::now();
    Move opt_path[100];
    solver s;
    
    if(USE_DEBUG){
        debug << "step estimation:" << s.min_step_estimate(pos) <<endl;
        debug<<"start DFS\n";

    }
    int opt_path_len = 0;
    // for(int d=1;d<=30;d++){
    //     s.init();
    //     opt_path_len = 0;
    //     bool status = s.dfs(pos, d, opt_path, opt_path_len);
    //     if(!status){
    //         if(USE_DEBUG)
    //             debug<<"fail at depth "<<d<<endl;
    //         continue;
    //     }
    //     if(USE_DEBUG){
    //         debug<<"visit cnt:" << s.visit_cnt<<endl;
    //         debug<<  "movelen:" << opt_path_len <<endl;
    //         debug << "DFS:\n";
    //     }
    //     break;
    // }
    for(int d=1;d<=30;d++){
        s.init();
        // opt_path_len = 0;
        opt_path_len = s.dfStack(pos, d, opt_path);
        if(opt_path_len == FAIL){
            if(USE_DEBUG)
                debug<<"fail at depth "<<d<<endl;
            continue;
        }
        if(USE_DEBUG){
            debug<<"visit cnt:" << s.visit_cnt<<endl;
            debug<<  "movelen:" << opt_path_len <<endl;
            debug << "DFS:\n";
        }
        break;
    }


    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    info << double(duration.count())*microseconds::period::num/microseconds::period::den << endl;

    info << opt_path_len<<endl;
    for(int i=0;i<opt_path_len; i++){
        info << opt_path[i];
    }
}