*This project has been created as part of the 42 curriculum by luzolo.*

# Codexion

## Description
**Codexion** is a high-performance concurrency simulation challenge inspired by the classic Dining Philosophers problem, adapted to represent coders in a shared co-working space competing for a limited set of USB dongles.

Coders alternate between compiling quantum code (which requires holding two adjacent USB dongles), debugging, and refactoring. If a coder is unable to start compiling before a certain amount of time passes (`time_to_burnout`), they burn out, and the simulation terminates. A custom priority queue (binary min-heap) is used to manage dongle requests under two scheduler algorithms:
1. **FIFO (First In, First Out)**: Access is granted to whichever coder requested the dongle first.
2. **EDF (Earliest Deadline First)**: Access is granted to the coder with the closest burnout deadline.

---

## Instructions

### Compilation
Build the application using the provided `Makefile`:
```bash
make
```
This generates the `codexion` binary.

### Execution
Run the binary with the following mandatory parameters:
```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

#### Example:
```bash
./codexion 3 1000 200 100 100 3 10 edf
```

- `number_of_coders`: Number of coders (and adjacent dongles) participating in the simulation.
- `time_to_burnout`: Time limit (in ms) a coder has to start compiling before burning out.
- `time_to_compile`: Compilation duration (in ms).
- `time_to_debug`: Debugging duration (in ms).
- `time_to_refactor`: Refactoring duration (in ms).
- `number_of_compiles_required`: Number of compilations each coder must perform for the simulation to stop successfully.
- `dongle_cooldown`: Lockout duration (in ms) before a released dongle can be acquired again.
- `scheduler`: `fifo` or `edf` scheduling policy.

---

## Blocking Cases Handled

Our implementation addresses several classic concurrency issues:

1. **Deadlock Prevention & Coffman's Conditions**:
   - To prevent deadlocks (where coders hold one dongle while waiting forever for the other), coders must acquire both of their adjacent dongles atomically from the perspective of other threads.
   - A coder thread registers its request in the priority queues of both adjacent dongles. The thread only transitions to compiling when it holds the top priority in both queues, and both dongles are free and out of cooldown.
   - This breaks the "Hold and Wait" condition, eliminating circular wait dependencies.

2. **Starvation Prevention**:
   - Priority queues (binary min-heaps) guarantee that requests are serviced deterministically based on the chosen scheduler policy.
   - In EDF mode, the coder closest to burnout is prioritized, ensuring that coders at risk of starving are serviced first.
   - Deterministic tie-breaking (by coder ID) prevents live-lock and ensures fair arbitration.

3. **Dongle Cooldown Handling**:
   - After a dongle is released, its `cooldown_until` timestamp is updated.
   - Waiting coders recalculate their wait timeouts using `pthread_cond_timedwait`, waking up precisely when the cooldown expires or when they are at risk of burning out.

4. **Precise Burnout Detection**:
   - A dedicated monitor thread checks all coders' deadlines every 1 ms.
   - If a coder's deadline has passed, the monitor logs the burnout and terminates the simulation.
   - This ensures the burnout log is printed within 10 ms of the actual event.

5. **Log Serialization**:
   - All console output is protected by a dedicated `log_mutex`.
   - Logging checks the simulation state to prevent late or interleaved messages from printing after termination.

---

## Thread Synchronization Mechanisms

The simulation uses standard POSIX threading primitives to coordinate access to shared state:

1. **`pthread_mutex_t state_mutex`**:
   - Protects all shared variables, coder states, dongle availability, and request heaps.
   - Prevents race conditions during state transitions (e.g. going from refactoring to waiting).

2. **`pthread_mutex_t log_mutex`**:
   - Serializes console outputs, ensuring log messages from different coder threads are printed sequentially without corruption or overlap.

3. **`pthread_cond_t state_cond`**:
   - Used to block coder threads when they cannot acquire their dongles.
   - Threads use `pthread_cond_timedwait` to wait for a state change, a cooldown expiration, or a burnout deadline.
   - Broadcast signals (`pthread_cond_broadcast`) are emitted when a coder releases dongles, waking up all waiting coders to evaluate if they can now acquire their resources.

---

## Resources
- POSIX Threads Programming: [LLNL Pthreads Tutorial](https://computing.llnl.gov/tutorials/pthreads/)
- Priority Queue and Binary Heaps: [Introduction to Algorithms (CLRS)](https://mitpress.mit.edu/9780262046305/introduction-to-algorithms/)
- Dining Philosophers Problem and Deadlocks: [Operating Systems: Three Easy Pieces (OSTEP)](https://pages.cs.wisc.edu/~remzi/OSTEP/)

### AI Tool Usage Description
During the development of this project:
- **Architecture design**: AI assisted in designing the priority queue min-heap representation and mapping adjacent dongles in a circular topology.
- **Deadlock prevention**: AI helped formulate the atomic acquisition check to satisfy Coffman's conditions without using busy waiting.
- **Documentation**: AI assisted in generating this README file and formatting the implementation plan.
