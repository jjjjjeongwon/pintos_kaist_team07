/* file.c: Implementation of memory backed file object (mmaped object). */

#include "vm/vm.h"
#include "threads/vaddr.h"

static bool file_backed_swap_in (struct page *page, void *kva);
static bool file_backed_swap_out (struct page *page);
static void file_backed_destroy (struct page *page);

/* DO NOT MODIFY this struct */
static const struct page_operations file_ops = {
	.swap_in = file_backed_swap_in,
	.swap_out = file_backed_swap_out,
	.destroy = file_backed_destroy,
	.type = VM_FILE,
};

/* The initializer of file vm */
void
vm_file_init (void) {
	// FIXME: 지금으로썬 이 함수에 뭘 넣어줘야 할지 아잘모
}

/* Initialize the file backed page */
bool
file_backed_initializer (struct page *page, enum vm_type type, void *kva) {
	/* Set up the handler */
	page->operations = &file_ops;
	// NOTE: 아직 file_page 구조체의 추가 멤버 가능성을 고려해야 함.
	// struct file_page *file_page = &page->file;
	// file_page->init = page->uninit.init;
	// file_page->type = page->uninit.type;
	// file_page->aux = page->uninit.aux;
	// file_page->page_initializer = page->uninit.page_initializer;

	return true;
}

/* Swap in the page by read contents from the file. */
static bool
file_backed_swap_in (struct page *page, void *kva) {
	struct file_page *file_page UNUSED = &page->file;
}

/* Swap out the page by writeback contents to the file. */
static bool
file_backed_swap_out (struct page *page) {
	struct file_page *file_page UNUSED = &page->file;
}

/* Destory the file backed page. PAGE will be freed by the caller. */
static void
file_backed_destroy (struct page *page) {
	struct file_page *file_page UNUSED = &page->file;
}

/* Do the mmap */
void *
do_mmap (void *addr, size_t length, int writable,
		struct file *file, off_t offset) {
	vm_alloc_page(VM_FILE, addr, writable);
	vm_claim_page(addr);
	struct page *page = spt_find_page(&thread_current()->spt, addr);
	
	printf("PAGE IS OKAY: %p\n\n\n\n\n", page);
	printf("THIS IS KVA ADDR: %p\n\n\n\n\n", page->frame->kva);

	file_seek(file, offset);
	if (file_read (file, page->frame->kva, length) != (int) length) {
		printf("readche!!!!!!!\n\n\n");
		palloc_free_page (page->frame->kva);
		return false;
	}
	memset (page->frame->kva + length, 0, PGSIZE - length);
	
	return addr;
}

/* Do the munmap */
void
do_munmap (void *addr) {
	spt_remove_page(&thread_current()->spt, spt_find_page(&thread_current()->spt ,addr));
}
