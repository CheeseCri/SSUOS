#include <list.h>
#include <proc/sched.h>
#include <mem/malloc.h>
#include <proc/proc.h>
#include <proc/switch.h>
#include <interrupt.h>
#include <device/console.h>

extern struct list level_que[QUE_LV_MAX];
extern struct list plist;
extern struct list slist;
extern struct process procs[PROC_NUM_MAX];
extern struct process *idle_process;

struct process *latest;

struct process* get_next_proc(struct list *rlist_target);
void proc_que_levelup(struct process *cur);
void proc_que_leveldown(struct process *cur);
struct process* sched_find_que(void);

int scheduling;

/*
   linux multilevelfeedback queue scheduler
   level 1 que policy is FCFS(First Come, First Served)
   level 2 que policy is RR(Round Robin).
   */

//sched_find_que find the next process from the highest queue that has proccesses.
struct process* sched_find_que(void) {
	int i,j;
	struct process * result = NULL;

	proc_wake();

	/*TODO :check the queue whether it is empty or not  
	  and find each queue for the next process.
	  */
	//큐레벨1의 리스트가  비지 않았을 경우
	if(!list_empty(&level_que[1]))
		//큐레벨1 리스트에서 첫번째 프로세스 가져옴
		result = get_next_proc(&level_que[1]);
	//큐레벨2의 리스트가 비지 않았을 경우
	else if(!list_empty(&level_que[2]))
		//큐레벨2 리스트에서 첫번째 프로세스 가져옴
		result = get_next_proc(&level_que[2]);

	return result;
}

struct process* get_next_proc(struct list *rlist_target) {
	struct list_elem *e;

	for(e = list_begin (rlist_target); e != list_end (rlist_target);
			e = list_next (e))
	{
		struct process* p = list_entry(e, struct process, elem_stat);

		if(p->state == PROC_RUN)
			return p;
	}
	return NULL;
}

void schedule(void)
{
	struct process *cur;
	struct process *next;
	struct process *tmp;
	struct list_elem *ele;
	int i = 0, printed = 0;

	scheduling = 1;	
	cur = cur_process;
	/*TODO : if current process is idle_process(pid 0), schedule() choose the next process (not pid 0).
	  when context switching, you can use switch_process().  
	  if current process is not idle_process, schedule() choose the idle process(pid 0).
	  complete the schedule() code below.
	  */
	if ((cur -> pid) != 0) {//현재 PID가 0이 아닐 경우
		latest = cur;//latest를 cur로 변경
		latest->time_slice = 0;//time_slice 값 초기화
		scheduling = 0;//tick 증가로 명시
		cur_process = idle_process;//현재 프로세스를 idle로 지졍
		switch_process(cur, idle_process);//현재 프로세스를 idle로 변경
		return;
	}
	switch (latest -> que_level){
		//현재 큐레벨이 1일 경우
		case 1:
			//latest의 상태가 PROC_RUN일 때
			if(latest->state == PROC_RUN){
				//큐레벨1 리스트 첫번째 프로세스를 큐레벨2 리스트 마지막으로 보냄
				list_push_back(&level_que[2], list_pop_front(&level_que[1]));
				//보낸 프로세스의 que_level값 2로 변경
				latest->que_level = 2;
				intr_disable();
				//큐레벨 변경 메세지 출력
				printk("proc%d change the queue(1->2)\n", latest->pid);
				intr_enable();
			}
			break;
			//현재 큐레벨이 2일 경우
		case 2:
			//latest의 상태가 PROC_RUN일 때
			if(latest->state == PROC_RUN)
				//큐레벨2 리스트 첫번째 프로세스를 큐레벨2 리스트 마지막으로 보냄
				list_push_back(&level_que[2], list_pop_front(&level_que[2]));
			break;
	}

	proc_wake(); //wake up the processes 

	//print the info of all 10 proc.
	for (ele = list_begin(&plist); ele != list_end(&plist); ele = list_next(ele)) {
		tmp = list_entry (ele, struct process, elem_all);
		if ((tmp -> state == PROC_ZOMBIE) || 
				//(tmp -> state == PROC_BLOCK) || 
				//	(tmp -> state == PROC_STOP) ||
				(tmp -> pid == 0)) 	continue;
		if (!printed) {	
			printk("#=%2d t=%3d u=%3d ", tmp -> pid, tmp -> time_slice, tmp -> time_used);
			printk("q=%3d\n", tmp->que_level);
			printed = 1;			
		}
		else {
			printk(", #=%2d t=%3d u=%3d ", tmp -> pid, tmp -> time_slice, tmp->time_used);
			printk("q=%3d\n", tmp->que_level);
		}

	}

	if (printed)
		printk("\n");

	//큐에 프로세스가 존재 할 경우 next에 프로세스 지정
	if ((next = sched_find_que()) != NULL) {
		intr_disable();
		//현재 선택된 프로세스 아이디 출력
		printk("Selected process : %d\n", next -> pid);
		intr_enable();

		cur_process = next;//현재 프로세스를 next로 변경
		scheduling = 0;//tick 증가 가능 명시
		switch_process(cur, next);//현재 프로세스를 next로 변경
		return;
	}
	return;
}

void proc_que_levelup(struct process *cur)
{
	/*TODO : change the queue lv2 to queue lv1.*/
	cur->que_level = 1;
}

void proc_que_leveldown(struct process *cur)
{
	/*TODO : change the queue lv1 to queue lv2.*/
	cur->que_level = 2;
}
