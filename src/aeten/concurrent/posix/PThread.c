#define _GNU_SOURCE 
#include <sched.h>
#include <sys/sysinfo.h>
#include <pthread.h>

#define impl
#include "aeten/concurrent/posix/PThread.h"
 
#define import
#include "Callable.h"
#include "aeten/concurrent/Lock.h"
#include "aeten/concurrent/Condition.h"
#include "aeten/concurrent/posix/ReentrantLock.h"
#include "aeten/concurrent/posix/ReentrantLockCondition.h"

/*!
@startuml
!include Callable.c
!include aeten/concurrent/Thread.c
!include aeten/concurrent/posix/ReentrantLock.c!ReentrantLock
!include aeten/concurrent/posix/ReentrantLock.c!ReentrantLockCondition
namespace aeten.concurrent.posix {
	class PThread<T> implements aeten.concurrent.Thread {
        + {static} PThread(char *name, Callable task, bool loop) <<constructor>>
        + {static} withPriorityAndAffinity(char *name, Callable task, bool loop, int priority, int scheduler, int cpu_affinity, bool detached) <<constructor>>
        + {static} withAttr(char *name, Callable task, bool loop, pthread_attr_t *attr) <<constructor>>
		- name: char*
		- thread: pthread_t
		- attr: pthread_attr_t
        - lock: ReentrantLock
        - started: ReentrantLockCondition
        - stopped: ReentrantLockCondition
        - task: Callable
		- run: bool <<volatile>>
		- running: bool
	}
}
@enduml
*/

static void* thread_task(void* arg);
static inline void set_cpu_affinity(PThread *self, char *name, pthread_attr_t *attr, int cpu_affinity);

void PThread_new(PThread *self, char *name, Callable task, bool loop) {
	pthread_attr_t attr;
	check(pthread_attr_init(&attr) == 0, PThreadException, "Unable to init attributes for thread %s", name);
	PThread_new_withAttr(self, name, task, loop, &attr);
}

void PThread_new_withPriorityAndAffinity(PThread *self, char *name, Callable task, bool loop, int priority, int scheduler, int cpu_affinity, bool detached) {
	pthread_attr_t attr;
	struct sched_param sched;
	check(pthread_attr_init(&attr) == 0, PThreadException, "Unable to init attributes for thread %s", name);
	set_cpu_affinity(self, name, &attr, cpu_affinity);
	sched.sched_priority = priority;
	check(sched_setscheduler(self->_thread, scheduler, &sched) == 0, PThreadException, "Unable to init sheduling for thread %s", name);
	pthread_attr_setdetachstate(&attr, detached? PTHREAD_CREATE_DETACHED: PTHREAD_CREATE_JOINABLE);
	PThread_new_withAttr(self, name, task, loop, &attr);
}

void PThread_new_withAttr(PThread *self, char *name, Callable task, bool loop, pthread_attr_t *attr) {
	self->_attr = *attr;
	self->_name = strdup(name);
	self->_task = task;
	self->_run = loop;
    init_ReentrantLock(&self->_lock);
    init_ReentrantLockCondition(&self->_started, &self->_lock._mutex);
    init_ReentrantLockCondition(&self->_stopped, &self->_lock._mutex);
}

void PThread_start(PThread *self) {
    ReentrantLock_lock(&self->_lock);
	check(pthread_create(&self->_thread, &self->_attr, &thread_task, self) != 0, PThreadException, "Unable to create thread %s", self->_name);
    ReentrantLock_unlock(&self->_lock);
}

void PThread_stop(PThread *self) {
	self->_run = false;
}

void *PThread_join(PThread *self) {
	void *retval;
	pthread_join(self->_thread, &retval);
	return retval;
}

void *thread_task(void* arg) {
	PThread *self = (PThread*)arg;
	void *retval;
	check(pthread_setname_np(self->_thread, self->_name) != 0, PThreadException, "Unable to rename thread to %s", self->_name);
	do {
        ReentrantLock_lock(&self->_lock);
		if (self->_running == false) {
			self->_running = true;
            ReentrantLockCondition_signalAll(&self->_started);
		}
        ReentrantLock_unlock(&self->_lock);
        retval = Callable_call(self->_task);
	} while(self->_run);
    ReentrantLock_lock(&self->_lock);
	self->_running = false;
    ReentrantLockCondition_signalAll(&self->_stopped);
    ReentrantLock_unlock(&self->_lock);
	return retval;
}

void set_cpu_affinity(PThread *self, char *name, pthread_attr_t *attr, int cpu_affinity) {
	cpu_set_t *cpusetp;
	size_t size;
	int num_cpus = get_nprocs_conf();
	check((cpusetp = CPU_ALLOC(num_cpus)) != NULL, PThreadException, "Unable to allocate thread %s CPU affinity", name);
	size = CPU_ALLOC_SIZE(num_cpus);

	CPU_ZERO_S(size, cpusetp);
	for (int i=0; i<num_cpus; ++i) {
		if (((cpu_affinity >> i) & 0x1) == 0x1) {
			CPU_SET_S(i, size, cpusetp);
		}
	}
	check(pthread_attr_setaffinity_np(attr, size, cpusetp) == 0, PThreadException, "Unable to set thread %s CPU affinity 0x%x", name, cpu_affinity);
	CPU_FREE(cpusetp);
}

