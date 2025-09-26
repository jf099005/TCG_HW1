#include "solver.h"
#include "lib/helper.h"
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
        if(USE_DEBUG){
            debug << "try depth "<<d<<endl;
        }
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