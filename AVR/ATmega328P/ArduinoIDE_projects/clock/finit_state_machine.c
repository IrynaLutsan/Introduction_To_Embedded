#include "finit_state_machine.h"

static FSM_Event FSM_reservedEvt_[] =
{
        {.sig = 0},
        {.sig = FSM_ENTRY_SIG},
        {.sig = FSM_EXIT_SIG},
        {.sig = FSM_INIT_SIG},
};



void FSM_dispatch(FSM *me, FSM_Event const *e)
{
    FSM_StateHandler s = me->state; /* save the current state */

    FSM_State r = (*s)(me, e); /* call the event handler */

    if (r == FSM_RET_TRAN)  /* transition taken? */
    {
        (void)(*s)(me, &FSM_reservedEvt_[FSM_EXIT_SIG]); /* exit the source */
        (void)(*me->state)(me, &FSM_reservedEvt_[FSM_ENTRY_SIG]); /*enter target*/
    }
}

void FSM_init(FSM *me)
{
    (void)(*me->state)(me, &FSM_reservedEvt_[FSM_ENTRY_SIG]);
}
