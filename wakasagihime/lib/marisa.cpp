// Chinese Dark Chess: Magic
// ----------------------------------

#include "marisa.h"

unsigned pext(unsigned x, unsigned m)
{
#if __BMI2__
    return _pext_u32(x, m);
#else
    // From Hacker's Delight Ch. 7
    unsigned mk, mp, mv, t;
    int i;
    x  = x & m;              // Clear irrelevant bits.
    mk = ~m << 1;            // We will count 0's to right.
    for (i = 0; i < 5; i++) {
        mp = mk ^ (mk << 1); // Parallel suffix.
        mp = mp ^ (mp << 2);
        mp = mp ^ (mp << 4);
        mp = mp ^ (mp << 8);
        mp = mp ^ (mp << 16);
        mv = mp & m;                    // Bits to move.
        m  = m ^ mv | (mv >> (1 << i)); // Compress m.
        t  = x & mv;
        x  = x ^ t | (t >> (1 << i));   // Compress x.
        mk = mk & ~mp;
    }
    return x;
#endif
}

alignas(32) Magic chariotMagics[SQUARE_NB];
alignas(32) Magic cannonMagics[SQUARE_NB];

Board chariotTable[3840];
Board cannonTable[32768];

Board safe_destination(Square s, int step)
{
    Square to = Square(s + step);
    return is_okay(to) && distance(s, to) <= 2 ? square_bb(to) : Board(0);
}

// Generate moves for cannons and chariots the normal way
template<PieceType pt>
Board sliding_attack(Square sq, Board occupied)
{
    static_assert(
        pt == PieceType::Chariot || pt == PieceType::Cannon,
        "SA: Only chariots and cannons have magic!"
    );

    Board attacks     = 0;
    Direction dirs[4] = { NORTH, SOUTH, EAST, WEST };

    if (pt == PieceType::Chariot) {
        // Chariots work like rooks
        for (Direction d : dirs) {
            Square s = sq;
            while (safe_destination(s, d)) {
                attacks |= (s += d);
                if (occupied & s) {
                    break;
                }
            }
        }
        return attacks;
    } else {
        // Cannons do not
        for (Direction d : dirs) {
            Square s    = sq;
            bool last   = false;
            bool screen = false;
            // ------------------------------
            // O__o__oo <- pieces arrangement
            // xxxx     <- chariot range
            // xxx   x  <- cannon range
            // ------------------------------
            while (!last) {
                if (!safe_destination(s, d)) {
                    last = true;
                }

                if (screen) {
                    if (occupied & s) {
                        attacks |= s;
                        break;
                    }
                } else {
                    if (occupied & s) {
                        screen = true;
                    } else {
                        attacks |= s;
                    }
                }
                s += d;
            }
        }
        return attacks ^ sq; // remove origin square
    }
}

// Please pass the correct tables
template<PieceType pt>
void init_magic(Board table[], Magic magics[])
{
    static_assert(
        pt == PieceType::Chariot || pt == PieceType::Cannon,
        "IM: Only chariots and cannons have magic!"
    );

    int size = 0;

    for (Square sq = SQ_A1; sq < SQUARE_NB; sq += 1) {
        Board edges = (pt == PieceType::Chariot)
                          // For chariots we don't have to consider edges
                          ? edges = ((Rank1BB | Rank4BB) & ~rank_bb(sq)) |
                                    ((FileABB | FileHBB) & ~file_bb(sq))
                          // For cannons we do
                          : edges = 0;

        // The mask is the range of the piece on an empty board
        Magic &m = magics[sq];
        m.mask   = sliding_attack<pt>(sq, 0) & ~edges;

        // We have a different sized table for each square
        m.attacks = (sq == SQ_A1) ? table : magics[sq - 1].attacks + size;
        size      = 0;

        // Iterate through all subsets of the mask, calculate and store
        // the resulting attack
        Board b = 0;
        do {
            m.attacks[pext(b, m.mask)] = sliding_attack<pt>(sq, b);
            size += 1;
            b = (b - m.mask) & m.mask; // See: carry-rippler
        } while (b);
    }
}

template void init_magic<Chariot>(Board[], Magic[]);
template void init_magic<Cannon>(Board[], Magic[]);