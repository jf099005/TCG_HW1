#include "solver.h"
#include "lib/helper.h"
#include "IDS_solver/DFS_solver.h"
#include <chrono>
#define OUTPUT_BOARD true
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
    Move opt_path[MAX_MOVE_NUM];
    solver s;
    
    if(OUTPUT_BOARD){
        debug << pos <<endl;
        visit_seq_scheduler s;
        debug <<"num of C:" <<pos.count(Black, Chariot) <<endl;
        debug << "min step estimate:" << s.min_step_estimate(pos) <<endl;
    }
    int opt_path_len = -1;
    int L=0, R=30;
    if(USE_DEBUG){
        cout<<"input the depth:";
        cin>>L;
        R = L;
        cout<<"search with depth "<<L<<endl;
    }
    for(int d=L;d<=R;d++){
        bool result = s.dfStack(pos, d, opt_path);
        if(result){
            opt_path_len = d;
            break;
        }
    }


    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    info << double(duration.count())*microseconds::period::num/microseconds::period::den << endl;

    info << opt_path_len<<endl;
    for(int i=0;i<opt_path_len; i++){
        info << opt_path[i];
    }
}