#include <filesys/procfs.h>
#include <filesys/vnode.h>
#include <filesys/ssufs.h>
#include <proc/proc.h>
#include <list.h>
#include <string.h>
#include <ssulib.h>

extern struct list p_list;
extern struct process *cur_process;

struct vnode *init_procfs(struct vnode *mnt_vnode)
{
	mnt_vnode->v_op.ls = proc_process_ls;
	mnt_vnode->v_op.mkdir = NULL;
	mnt_vnode->v_op.cd = proc_process_cd;

	return mnt_vnode;
}

int proc_process_ls()
{
	int result = 0;
	struct list_elem *e;

	printk(". .. ");
	for(e = list_begin (&p_list); e != list_end (&p_list); e = list_next (e))
	{
		struct process* p = list_entry(e, struct process, elem_all);
		printk("%d ", p->pid);

	}
	printk("\n");

	return result;
}

int proc_process_cd(char *dirname)
{
	struct list_elem *e;
	struct vnode *cwd = cur_process->cwd;
	struct process * p;
	struct vnode * vret;
	char input[64];

	for(e = list_begin(&p_list); e != list_end(&p_list);e = list_next(e))
	{
		p = list_entry(e, struct process, elem_all);
		snprintf(input,sizeof(p->pid), "%d", p->pid);
		if(strcmp(input, dirname) == 0){
			vret = vnode_alloc();
			vret->v_parent = cwd;
			memset(vret->v_name, 0, LEN_VNODE_NAME);
			memcpy(vret->v_name, input, strlen(input));
			vret->v_op.ls = proc_process_info_ls;
			vret->v_op.cd = proc_process_info_cd;
			vret->v_op.cat = proc_process_info_cat;
			cur_process->cwd = vret;
			return 0;
		}
	}
	return 0;

}

int proc_process_info_ls()
{
	int result = 0;

	printk(". .. ");
	printk("cwd root time stack");

	printk("\n");

	return result;
}

int proc_process_info_cd(char *dirname)
{
	struct list_elem *e;
	struct vnode *cwd = cur_process->cwd;
	struct process *p;
	struct vnode * vret;
	int input[64];


	for(e = list_begin(&p_list); e != list_end(&p_list); e = list_next(e))
	{
		p = list_entry(e, struct process, elem_all);
		snprintf(input, sizeof(p->pid), "%d", p->pid);
		if(strcmp(cwd->v_name, input) == 0){
			if(strcmp(dirname, "cwd") == 0){
				vret = vnode_alloc();
				copy_vnode(p->cwd, vret);
				memset(vret->v_name, 0, LEN_VNODE_NAME);
				memcpy(vret->v_name, "cwd", strlen("cwd"));
				vret->v_parent = cwd;
				vret->v_op.ls = proc_link_ls;
				cur_process->cwd = vret;
				return 0;
			}
			if(strcmp(dirname, "root") == 0){
				vret = vnode_alloc();
				copy_vnode(p->rootdir, vret);
				memset(vret->v_name, 0, LEN_VNODE_NAME);
				memcpy(vret->v_name, "root", strlen("root"));
				vret->v_parent = cwd;
				vret->v_op.ls = proc_link_ls;
				cur_process->cwd = vret;
				return 0;
			}
		}
	}
	return 0;
}


int proc_process_info_cat(char *filename)
{

	struct list_elem *e;
	struct vnode *cwd = cur_process->cwd;
	struct process *p;
	int input[64];


	for(e = list_begin(&p_list); e != list_end(&p_list); e = list_next(e))
	{
		p = list_entry(e, struct process, elem_all);
		snprintf(input, sizeof(p->pid), "%d", p->pid);
		if(strcmp(cwd->v_name, input) == 0){
			if(strcmp(filename, "stack") == 0){
				printk("stack : %x\n", p->stack);
			}
			if(strcmp(filename, "time") == 0){
				printk("time : %ld\n", p->time_used);
			}
		}
	}
	return 0;
}

int proc_link_ls()
{
	struct list_elem *e;
	struct vnode *cwd =cur_process->cwd;
	struct vnode *child;
	int num_dir = num_direntry((struct ssufs_inode *) cwd ->info);
	printk(". .. ");
	e = list_begin(&cwd->childlist);
	for(int i = 2; i < num_dir; i++)
	{
		child = list_entry(e, struct vnode, elem);
		printk("%s ", child->v_name);
		e = list_next(e);
	}
	printk("\n");
}

