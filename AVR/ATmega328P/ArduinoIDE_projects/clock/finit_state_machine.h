#ifndef _FINIT_STATE_MACHINE_H_
#define _FINIT_STATE_MACHINE_H_

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t FSM_Signal;

/* the event structure */
typedef struct FSM_Event_Tag
{
  FSM_Signal sig;    /* signal of the event */
} FSM_Event;

typedef uint8_t FSM_State; /* status returned from a state-handler function */

/* state-handler function */
typedef FSM_State (*FSM_StateHandler)(void *me, FSM_Event const *e);

/* Finite State Machine */
typedef struct FSM_Tag
{
    FSM_StateHandler state; /* current active state */
}FSM;


#define FSM_ctor(me_, initial_) ((me_)->state = (initial_))
void FSM_init(FSM *me);
void FSM_dispatch(FSM *me, FSM_Event const *e);

#define FSM_RET_HANDLED ((FSM_State)0)
#define FSM_RET_IGNORED ((FSM_State)1)
#define FSM_RET_TRAN    ((FSM_State)2)
#define FSM_HANDLED()   (FSM_RET_HANDLED)
#define FSM_IGNORED()   (FSM_RET_IGNORED)
#define FSM_TRAN(target_) \
(((FSM *)me)->state = (FSM_StateHandler)(target_), FSM_RET_TRAN)


enum FSM_ReservedSignals
{
    FSM_ENTRY_SIG = 1, /* signal for coding entry actions */
    FSM_EXIT_SIG,      /* signal for coding exit actions */
    FSM_INIT_SIG,      /* signal for coding initial transitions */
    FSM_USER_SIG       /* first signal that can be used in user applications */
};

#ifdef __cplusplus
}
#endif

#endif // _FINIT_STATE_MACHINE_H_
