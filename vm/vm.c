/* vm.c: Generic interface for virtual memory objects. */

#include "threads/malloc.h"
#include "vm/vm.h"
#include "vm/inspect.h"
#include "threads/vaddr.h"
// NOTE: 임시 선언
#include "threads/palloc.h"
#include "threads/mmu.h"
#include "userprog/process.h"
#include <string.h>

// NOTE: 임시 선언
static bool install_page (void *upage, void *kpage, bool writable);
void spt_dealloc_page (struct hash_elem *e, void *aux);

/* Initializes the virtual memory subsystem by invoking each subsystem's
 * intialize codes. */
void
vm_init (void) {
	vm_anon_init ();
	vm_file_init ();
#ifdef EFILESYS  /* For project 4 */
	pagecache_init ();
#endif
	register_inspect_intr ();
	/* DO NOT MODIFY UPPER LINES. */
	/* TODO: Your code goes here. */
}

static uint64_t vm_hash_func(const struct hash_elem *e, void *aux){
	struct page *p = hash_entry(e, struct page, elem);
	return hash_int(p->va);
}

static bool vm_less_func(const struct hash_elem *a, const struct hash_elem *b){
	void *a_va = hash_entry(a, struct page, elem)->va;
	void *b_va = hash_entry(b, struct page, elem)->va;

	return b_va > a_va;
}

/* Get the type of the page. This function is useful if you want to know the
 * type of the page after it will be initialized.
 * This function is fully implemented now. */
enum vm_type
page_get_type (struct page *page) {
	int ty = VM_TYPE (page->operations->type);
	switch (ty) {
		case VM_UNINIT:
			return VM_TYPE (page->uninit.type);
		default:
			return ty;
	}
}

/* Helpers */
static struct frame *vm_get_victim (void);
static bool vm_do_claim_page (struct page *page);
static struct frame *vm_evict_frame (void);


// 우리에게 3가지 타입의 페이지가 있기 때문에, 각 페이지 타입별로 초기화 루틴이 다릅니다. 아래의 섹션에서 다시 설명할것이니, 여기서는 high-level 관점에서 페이지 초기화 과정이 어떻게 되는지 설명하고자 합니다.
// 우선 커널이 새로운 페이지를 달라는 요청을 받으면, vm_alloc_page_with_initializer 가 호출됩니다. 
// 이 함수는 페이지 구조체를 할당하고 페이지 타입에 맞는 적절한 초기화 함수를 세팅함으로써 새로운 페이지를 초기화 합니다. 
// 그리고 유저 프로그램으로 제어권을 넘깁니다. 
// 유저 프로그램이 실행될 때, 지연 로딩으로 인해 콘텐츠가 아직 로드되지 않은 페이지에 접근하게 되면 페이지 폴트가 일어나게 됩니다. 
// 이 페이지 폴트를 처리하는 과정에서 uninit_initialize 을 호출하고 이전에 당신이 세팅해 놓은 초기화 함수를 호출합니다. 
// 익명 페이지를 위한 초기화 함수는 anon_initializer 이고, 파일 기반 페이지를 위한 초기화 함수는 file_backed_initializer 입니다.
//
/* Create the pending page object with initializer. If you want to create a
 * page, do not create it directly and make it through this function or
 * `vm_alloc_page`. */
bool
vm_alloc_page_with_initializer (enum vm_type type, void *upage, bool writable,
		vm_initializer *init, void *aux) {
			
	ASSERT (VM_TYPE(type) != VM_UNINIT)

	struct supplemental_page_table *spt = &thread_current ()->spt;

	/* Check wheter the upage is already occupied or not. */
	
	if (spt_find_page (spt, upage) == NULL) {
		/* TODO: Create the page, fetch the initialier according to the VM type,
		 * TODO: and then create "uninit" page struct by calling uninit_new. You
		 * TODO: should modify the field after calling the uninit_new. */
		struct page *p = malloc(sizeof(struct page));
		switch (type){
			case VM_FILE:
				uninit_new(p, upage, init, VM_FILE, aux, file_backed_initializer);
				break;
			case VM_ANON:
				uninit_new(p, upage, init, VM_ANON, aux, anon_initializer);
				break;
		} 
		p->writable = writable;
		/* TODO: Insert the page into the spt. */
		return spt_insert_page(spt, p);
	}
err:
	return false;
}

/* Find VA from spt and return page. On error, return NULL. */
struct page *
spt_find_page (struct supplemental_page_table *spt UNUSED, void *va UNUSED) {
	struct page p;
	struct hash_elem *founded_elem;
	
	p.va = pg_round_down(va);
	founded_elem = hash_find(&spt->vm, &p.elem);
	return founded_elem != NULL ? hash_entry(founded_elem, struct page, elem) : NULL;
}

/* Insert PAGE into spt with validation. */
bool
spt_insert_page (struct supplemental_page_table *spt UNUSED,
		struct page *page UNUSED) {
	/* TODO: Fill this function. */
	if (hash_insert(&spt->vm, &page->elem) == NULL){
		return true;
	}
	return false;
}

void
spt_remove_page (struct supplemental_page_table *spt, struct page *page) {
	// FIXME: destroy의 정의가 확실하지 않아 hash_delete()함수 사용 고려중
	vm_dealloc_page (page);

	return true;
}

/* Get the struct frame, that will be evicted. */
static struct frame *
vm_get_victim (void) {
	struct frame *victim = NULL;
	 /* TODO: The policy for eviction is up to you. */

	return victim;
}

/* Evict one page and return the corresponding frame.
 * Return NULL on error.*/
static struct frame *
vm_evict_frame (void) {
	struct frame *victim UNUSED = vm_get_victim ();
	/* TODO: swap out the victim and return the evicted frame. */

	return NULL;
}

/* palloc() and get frame. If there is no available page, evict the page
 * and return it. This always return valid address. That is, if the user pool
 * memory is full, this function evicts the frame to get the available memory
 * space.*/
static struct frame *
vm_get_frame (void) {
	/* TODO: Fill this function. */
	struct frame *frame = malloc(sizeof(struct frame));
	if (frame == NULL) {
		PANIC ("todo");
	}
	frame->kva = palloc_get_page(PAL_USER);
	frame->page = NULL; 

	ASSERT (frame != NULL);
	ASSERT (frame->page == NULL);
	return frame;
}

/* Growing the stack. */
static void
vm_stack_growth (void *addr UNUSED) {
}

/* Handle the fault on write_protected page */
static bool
vm_handle_wp (struct page *page UNUSED) {
}

/* Return true on success */
bool
vm_try_handle_fault (struct intr_frame *f UNUSED, void *addr UNUSED,
		bool user UNUSED, bool write UNUSED, bool not_present UNUSED) {
	struct thread *cur = thread_current();
	struct supplemental_page_table *spt UNUSED = &thread_current ()->spt;
	struct page *page = NULL;
	/* TODO: Validate the fault */
	/* TODO: Your code goes here */
	if (addr == NULL) exit(-1);
	if (user) { 
		// printf("User\n");
	}
	if (write) {
		// printf("Write\n");
	}
	if (not_present) {
		// printf("Not_present\n");
		vm_alloc_page(VM_ANON, addr, true);
		page = spt_find_page(spt, addr);
		return vm_do_claim_page (page);
		// return install_page(page->va, page->frame->kva, true);
	}
	return false;
}

/* Free the page.
 * DO NOT MODIFY THIS FUNCTION. */
void
vm_dealloc_page (struct page *page) {	
	destroy (page);
	free (page);
}

/* Claim the page that allocate on VA. */
bool
vm_claim_page (void *va UNUSED) {
	/* TODO: Fill this function */
	struct thread *curr = thread_current();
	struct page *page = spt_find_page(&curr->spt, va);
	if (page == NULL) {
		PANIC ("can't find page in spt");
	}

	return vm_do_claim_page (page);
}

/* Claim the PAGE and set up the mmu. */
static bool
vm_do_claim_page (struct page *page) {
	struct frame *frame = vm_get_frame ();

	/* Set links */
	frame->page = page;
	page->frame = frame;

	/* TODO: Insert page table entry to map page's VA to frame's PA. */
	if(install_page(page->va, frame->kva, page->writable)) {
		return swap_in (page, frame->kva);
	}
	return false;
}

/* Initialize new supplemental page table */
void
supplemental_page_table_init (struct supplemental_page_table *spt UNUSED) {
	hash_init(&spt->vm, vm_hash_func, vm_less_func, NULL);
}

/* Copy supplemental page table from src to dst */
bool
supplemental_page_table_copy (struct supplemental_page_table *dst UNUSED,
		struct supplemental_page_table *src UNUSED) {
			struct hash_iterator iterator;
			hash_first(&iterator, &src->vm);
			while (hash_next(&iterator)) {
				struct page *parent_page = hash_entry(hash_cur(&iterator), struct page, elem);
				if (!vm_alloc_page_with_initializer(parent_page->uninit.type, parent_page->va, parent_page->writable, parent_page->uninit.init, parent_page->uninit.aux)) {
					return false;
				}
				struct page temp_page;
				temp_page.va = parent_page->va;
				struct hash_elem *child_page_elem = hash_find(&dst->vm, &temp_page.elem);
				struct page *child_page = hash_entry(child_page_elem, struct page ,elem);

				if (parent_page->frame != NULL) {
					if(!vm_do_claim_page(child_page)) return false;
					memcpy(child_page->frame->kva, parent_page->frame->kva, PGSIZE);
				}
			} return true;
}

/* Free the resource hold by the supplemental page table */
void
supplemental_page_table_kill (struct supplemental_page_table *spt UNUSED) {
	/* TODO: Destroy all the supplemental_page_table hold by thread and
	 * TODO: writeback all the modified contents to the storage. 
	 * NOTE: 두 번째 TODO는 swap에서 사용하는게 아닐까? 
	 *  */
	hash_destroy(&spt->vm, spt_dealloc_page);
}




void spt_dealloc_page (struct hash_elem *e, void *aux){

	//  해쉬 일름 사용 -> 느낌상 페이지로 확대시키기
	struct page *page = hash_entry (e ,struct page, elem);

	// 	vm_dealloc_page로 페이지들을 하나씩 뽀수고 free시키기
	vm_dealloc_page(page);
}








static bool
install_page (void *upage, void *kpage, bool writable) {
	struct thread *t = thread_current ();

	/* Verify that there's not already a page at that virtual
	 * address, then map our page there. */
	return (pml4_get_page (t->pml4, upage) == NULL
			&& pml4_set_page (t->pml4, upage, kpage, writable));
}