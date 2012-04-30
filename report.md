

## CS-323, Assignment 3

### Authors

 * Amos Wenger
 * Ismail Amrani
 * Sebastien Zurfluh

### Requirements

The requirements for our scheduler were the following:

 * Support 5 level of priorities
 * Implement pre-emption
 * To prevent starvation, implement aging
 * Implement round-robin for tasks of same priority

### How our scheduler works

#### Basics

Our scheduler is really simple, and quite dumb.

It keeps a linked list (standard Linux kernel lists, really) of `struct rq*`.
Every time we need to pick a new task, we go through that list to find the
task with the highest priority.

#### Priorities & aging

To make our lives easier, we created a `dummy_calculate_prio` helper, to compute
the priority of the task according to our scheduler. It is a calculation based
on the `static_prio`, along with the number of ticks we recorded.

Aging is implemented this way: instead of heightening the priority of processes
which have been waiting for the CPU a long time, we lower the priority of processes
which have been running for a long time.

In order to do so, we increase a value called `ticks` in the `dummy_rq` struct every
time `task_tick_dummy` is called. That ticks value is reset to zero whenever the
task is pre-empted and some other task is scheduled in its place.

The 'reactivity' of aging can be changed by adjusting `DUMMY_TICKS_ATOM`. A value
of 1 (the default setting) will make aging very reactive: it almost seems as if all
tasks were run with the same priority: however they're still prioritized, with tasks
of prio 10 having more CPU time than tasks of prio 15, for example.

#### Pre-emption

We call `resched_task` when a task heightened its priority (ie. lowered its static
prio value), in case it has become of higher priority than the current running task.

However, in our experience, and testing around with printk, we have never seen
`prio_changed_dummy` being called. Hence, pre-emption happens naturally whenever
a scheduler quantum is elapsed, and `pick_next_task_dummy` is called again, with
actual priorities being recomputed again.

We tested with the following experiment: we changed the code of the ab.c example
program in order to change the priority of the second process from 12 to 15 whenever
it was being run. As a result, the output looked like:

    AAAAB<priority change>CCCCDDDDEBEBEBE

#### Round-robin

Simply finding the task with highest priority in the list would not solve the problem
of sharing time fairly between tasks of same priority. In order to solve this, we do
two things:

  * We iterate through the list backward (from tail to head): this
    incurs no performance overhead because lists are doubly linked.

  * Every time a task is picked, we remove it from the list and add
    it back to the head of the list.

Together, these measures ensure that there is a rotation even between tasks of the
same priority, in round-robin fashion.

#### Performance

Since we have to go through the whole list every time we pick a new task,
that makes the scheduler O(n). A simple way to improve the performance of
our scheduler would be to keep our list sorted. That would require a sort
algorithm that:

  * Is fast on nearly-sorted lists (as it's going to be the case most of the
    time, if we have stable tasks.)
  * Works well with doubly-linked lists (ie. doesn't require random access)

However, since this is an exercise, the performance of this scheduler seems
acceptable, as it fills all the requirements!


