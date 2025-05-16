#ifndef PREEMPT_H
#define PREEMPT_H

#ifdef __cplusplus
extern "C" {
#endif

void preempt_start(void);
void preempt_stop(void);
void preempt_enable(void);
void preempt_disable(void);

#ifdef __cplusplus
}
#endif

#endif // PREEMPT_H
