/**
 * Dummy scheduler
 */
#include <linux/sched.h>

static void _enqueue(struct rq *rq, struct task_struct *p)
{
	struct dummy_rq *n = kmalloc(sizeof(struct dummy_rq), GFP_KERNEL);
	n->task = p;
        n->ticks = 0;

	list_add_tail(&(n->list), &(rq->dummy_rq.list));

	printk(KERN_INFO "process %d (prio %d, static_prio = %d, ticks = %lu) added\n", task_pid_nr(p), p->prio, p->static_prio, n->ticks);
}

static void _remove(struct rq *rq, struct task_struct *p)
{
	struct list_head *pos, *q;
	struct dummy_rq *tmp;

	list_for_each_safe(pos, q, &rq->dummy_rq.list) {
		tmp = list_entry(pos, struct dummy_rq, list);
		if (tmp->task == p) {
                        printk(KERN_INFO "process %d (prio %d, static_prio = %d, ticks = %lu) removed\n", task_pid_nr(p), p->prio, p->static_prio, tmp->ticks);

			list_del(pos);
			kfree(tmp);
		}
	}
}

/**
 * Schedule classes to implement:
 * - enqueue_task_dummy
 *   task starts being handled by the dummy scheduler
 *
 * - dequeue_task_dummy
 *   task stops being handled by the dummy scheduler
 *
 * - pick_next_task_dummy
 *   select the next task to switch to
 *
 * - put_prev_task_dummy
 *   handles tasks that have been preempted
 *
 * - task_tick_dummy
 *   should a task be preempted?
 *
 * - prio_changed_dummy
 *   some task changed its priority
 */

// very inefficient way to find the drq from the task
static struct dummy_rq * dummy_rq_find_from_task(struct rq *rq, struct task_struct *task)
{
        struct list_head *pos;
        struct dummy_rq *tmp;
       
        list_for_each(pos, &rq->dummy_rq.list) {
            tmp = list_entry(pos, struct dummy_rq, list);
            if (tmp->task == task) {
                return tmp;
            }
        }

        return NULL;
}

static void enqueue_task_dummy(struct rq *rq, struct task_struct *p, int wakeup)
{
	_enqueue(rq, p);
}
static void dequeue_task_dummy(struct rq *rq, struct task_struct *p, int sleep)
{
	_remove(rq, p);
}

static int dummy_calculate_prio(struct dummy_rq * rq)
{
        int ticks, TICKS_ATOM;

        TICKS_ATOM = 1;
        ticks = rq->ticks;

        rq->task->prio = rq->task->static_prio;

        while (ticks > TICKS_ATOM) {
            ticks -= TICKS_ATOM;
            rq->task->prio++;
        }

        return rq->task->prio;
}

static struct task_struct *really_pick_next_task_dummy(struct rq * rq)
{
        // printk(KERN_INFO "picking next task! current = %d\n", task_pid_nr(rq->curr));

        struct dummy_rq *best, *candidate;
        struct list_head *pos;

        // so that's really stupid but let's see if it works.

        best = list_first_entry(&rq->dummy_rq.list, struct dummy_rq, list);
        list_for_each_prev(pos, &rq->dummy_rq.list) {
                candidate = list_entry(pos, struct dummy_rq, list);

                if (dummy_calculate_prio(candidate) < dummy_calculate_prio(best)) {
                        best = candidate;
                }
        }

        // move best to the tail of the list
        list_del(&(best->list));
	list_add_tail(&(best->list), &(rq->dummy_rq.list));

        // reset ticks
        if (best->task != rq->curr) {
            best->ticks = 0;
        }

        return best->task;
}

static struct task_struct *pick_next_task_dummy(struct rq *rq)
{
	if (!list_empty(&rq->dummy_rq.list)) {
                return really_pick_next_task_dummy(rq);
        } else {
                return NULL;
        }
}

static void put_prev_task_dummy(struct rq *rq, struct task_struct *prev)
{
        // nothing to do here?
}

static void task_tick_dummy(struct rq *rq, struct task_struct *curr, int queued)
{
        // called whenever a task ticks? increment ticks count.
        struct dummy_rq *tmp;

        if ((tmp = dummy_rq_find_from_task(rq, curr))) {
            tmp->ticks++;
        }
}

static void prio_changed_dummy(struct rq *rq, struct task_struct *p,
    int oldprio, int running)
{
	printk(KERN_INFO "process %d (with prio %d) changed prio to %d (running = %d)\n", task_pid_nr(p), oldprio, p->prio, running);

	// if our priority increased, reschedule current task
	if (running) {
		if (p->prio < oldprio) {
			resched_task(rq->curr);
		}
	} else {
		// preempt?
	}
}

/**
 * You can ignore the following functions...
 */

static void yield_task_dummy(struct rq *rq) { }

static void check_preempt_wakeup_dummy(struct rq *rq, struct task_struct *p, int wake_flags) { }

static void set_curr_task_dummy(struct rq *rq) { }

static void task_fork_dummy(struct task_struct *p) { }

static void switched_to_dummy(struct rq *rq, struct task_struct *p,
    int running)
{
	if (!running) {
		resched_task(p);
	}
}

static unsigned int get_rr_interval_dummy(struct rq *rq, struct task_struct *task)
{
	return 0;
}

static const struct sched_class dummy_sched_class = {
	.next		    = &idle_sched_class,
	.enqueue_task       = enqueue_task_dummy,
	.dequeue_task       = dequeue_task_dummy,
	.yield_task         = yield_task_dummy,

	.check_preempt_curr = check_preempt_wakeup_dummy,

	.pick_next_task     = pick_next_task_dummy,
	.put_prev_task      = put_prev_task_dummy,

	.set_curr_task      = set_curr_task_dummy,
	.task_tick          = task_tick_dummy,
	.task_fork          = task_fork_dummy,

	.prio_changed       = prio_changed_dummy,
	.switched_to        = switched_to_dummy,

	.get_rr_interval    = get_rr_interval_dummy,
};
