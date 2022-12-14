#ifndef _STATE_HPP_
#define _STATE_HPP_

//*******************************************************************
//* To add a new state:
//*   1) Add an entry into the state template below
//*   2) Implement appropriately named functions in state_machine.cpp
//*
//* Note: There will be compile errors if functions are not added
//*******************************************************************

#define STATE_TEMPLATE \
STATE(IDLE) \
STATE(TOGGLE) \
STATE(SELECT) \
STATE(RESET) \
STATE(DATE) \
STATE(RAM)

#define STATE(x) x,
enum State { STATE_TEMPLATE };
#undef STATE

#endif
