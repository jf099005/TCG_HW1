#include "solver.h"
#include "lib/helper.h"
#include "IDS_solver/DFS_solver.h"
#include <chrono>
#define OUTPUT_BOARD false
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
    solver s(pos);
    
    int opt_path_len = -1;
    int L=0, R=30;
    for(int d=L;d<=R;d++){
        bool result = s.IDS(pos, d, opt_path);
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