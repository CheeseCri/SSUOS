#include <mem/palloc.h>
#include <bitmap.h>
#include <type.h>
#include <round.h>
#include <mem/mm.h>
#include <synch.h>
#include <device/console.h>
#include <mem/paging.h>
#include <proc/proc.h>

/* Page allocator.  Hands out memory in page-size (or
   page-multiple) chunks.  
   */

/* page struct */
struct kpage{
	uint32_t type;
	uint32_t *vaddr;
	uint32_t nalloc;
	pid_t pid;
};


static struct kpage *kpage_list;
static uint32_t page_alloc_index;

/* Initializes the page allocator. */
void
init_palloc (void) 
{
	/* Calculate the space needed for the kpage list */
	size_t pool_size = sizeof(struct kpage) * PAGE_POOL_SIZE;

	/* kpage list alloc */
	kpage_list = (struct kpage *)(KERNEL_ADDR);

	/* initialize */
	memset((void*)kpage_list, 0, pool_size);
	page_alloc_index = 0;
}

/* Obtains and returns a group of PAGE_CNT contiguous free pages.
   */
uint32_t *
palloc_get_multiple (uint32_t page_type, size_t page_cnt)
{
	extern struct process *cur_process;
	void *pages = NULL;
	struct kpage *kpage = kpage_list;
	size_t page_idx;
	int i,j;

	if (page_cnt == 0)
		return NULL;

	switch(page_type){
		case HEAP__: //(1)
		for(i = 0; i<= page_alloc_index; ++i)
		{
			if(i != 0)
				kpage += sizeof(struct kpage);

			if(kpage->type == FREE__ && kpage->nalloc == page_cnt)
			{
				kpage->type = HEAP__;
				pages = kpage->vaddr;
				kpage->pid = cur_process->pid;
				break;
			}
		}

		if(pages == NULL)
		{
			page_idx = page_alloc_index + page_cnt;
			page_alloc_index += page_cnt;
			kpage->type = HEAP__;
			kpage->nalloc = page_cnt;
			kpage->pid = cur_process->pid;
			pages = (void *)(VKERNEL_HEAP_START - page_idx * PAGE_SIZE);
			kpage->vaddr = (uint32_t *)pages;
		}
		break;
		case STACK__: 
			//(2)

		break;
		default:
		return NULL;
	}

	memset(pages, 0, PAGE_SIZE * page_cnt);
	return (uint32_t*)pages; 
}

/* Obtains a single free page and returns its address.
   */
uint32_t *
palloc_get_page (uint32_t page_type) 
{
	return palloc_get_multiple (page_type, 1);
}

/* Frees the PAGE_CNT pages starting at PAGES. */
void
palloc_free_multiple (void *pages, size_t page_cnt) 
{

	struct kpage *kpage = kpage_list;

	for(int i = 0; i <= page_alloc_index; i++){
		if(i != 0)
			kpage += sizeof(struct kpage);
		if(kpage->vaddr == (uint32_t*)pages && kpage->nalloc == page_cnt) 
			kpage->type = FREE__;
	}

}

/* Frees the page at PAGE. */
void
palloc_free_page (void *page) 
{
	palloc_free_multiple (page, 1);
}


uint32_t *
va_to_ra (uint32_t *va){
	if(va < RKERNEL_HEAP_START)
		return va;
	else
		return VH_TO_RH(va);
}

uint32_t *
ra_to_va (uint32_t *ra){
	if(ra < RKERNEL_HEAP_START)
		return ra;
	else
		return RH_TO_VH(ra);

}

void palloc_pf_test(void)
{
	uint32_t *one_page1 = palloc_get_page(HEAP__);
	uint32_t *one_page2 = palloc_get_page(HEAP__);
	uint32_t *two_page1 = palloc_get_multiple(HEAP__,2);
	uint32_t *three_page;
	printk("one_page1 = %x\n", one_page1); 
	printk("one_page2 = %x\n", one_page2); 
	printk("two_page1 = %x\n", two_page1);

	printk("=----------------------------------=\n");
	palloc_free_page(one_page1);
	palloc_free_page(one_page2);
	palloc_free_multiple(two_page1,2);

	one_page1 = palloc_get_page(HEAP__);
	two_page1 = palloc_get_multiple(HEAP__,2);
	one_page2 = palloc_get_page(HEAP__);

	printk("one_page1 = %x\n", one_page1);
	printk("one_page2 = %x\n", one_page2);
	printk("two_page1 = %x\n", two_page1);

	printk("=----------------------------------=\n");
	three_page = palloc_get_multiple(HEAP__,3);

	printk("three_page = %x\n", three_page);
	palloc_free_page(one_page1);
	palloc_free_page(one_page2);
	palloc_free_multiple(two_page1,2);
	palloc_free_multiple(three_page, 3);
}
