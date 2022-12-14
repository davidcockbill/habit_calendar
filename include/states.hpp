#ifndef _STATES_HPP_
#define _STATES_HPP_


#define STATES C(IDLE)C(TOGGLE)C(SELECT)C(RESET)C(DATE)C(RAM)
#define C(x) x,
enum State { STATES };
#undef C

#endif
