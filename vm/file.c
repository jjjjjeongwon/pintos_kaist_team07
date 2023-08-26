/* file.c: Implementation of memory backed file object (mmaped object). */

#include "vm/vm.h"
#include "threads/vaddr.h"
#include <stdio.h>
#include "userprog/process.h"


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

static bool
lazy_load_segment (struct page *page, void *aux) {
	struct lazy_load_data *ll_data = (struct lazy_load_data *) aux;
	/* TODO: Load the segment from the file */
	/* TODO: This called when the first page fault occurs on address VA. */
	/* TODO: VA is available when calling this function. */
	uint8_t *kpage = page->frame->kva;
	struct file *file = ll_data->lazy_load_file;
	size_t page_read_bytes = ll_data->page_read_bytes;
	size_t page_zero_bytes = PGSIZE - page_read_bytes;
	off_t ofs = ll_data->ofs;

	file_seek (file, ofs);
	if (file_read (file, kpage, page_read_bytes) != (int) page_read_bytes) {
			palloc_free_page (kpage);
			return false;
		}
	memset (kpage + page_read_bytes, 0, page_zero_bytes);
	return true;
}

/* Do the mmap */
void *
do_mmap (void *addr, size_t length, int writable,
		struct file *file, off_t offset) {
	
	void * start_addr = addr;
	// ASSERT ((read_bytes + zero_bytes) % PGSIZE == 0);
	// ASSERT (pg_ofs (upage) == 0);
	// ASSERT (ofs % PGSIZE == 0);
	size_t read_bytes = file_length(file);
	file = file_reopen(file);

	while (read_bytes > 0 /* || zero_bytes > 0 */) {
		/* Do calculate how to fill this page.
		 * We will read PAGE_READ_BYTES bytes from FILE
		 * and zero the final PAGE_ZERO_BYTES bytes. */
		size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
		size_t page_zero_bytes = PGSIZE - page_read_bytes;

		/* TODO: Set up aux to pass information to the lazy_load_segment. */
		struct lazy_load_data *lazy_load_data =  (struct lazy_load_data*)malloc(sizeof(struct lazy_load_data));
		lazy_load_data->lazy_load_file = file_reopen(file);
		lazy_load_data->page_read_bytes = page_read_bytes;
		// lazy_load_data->page_zero_bytes = page_zero_bytes;
		lazy_load_data->ofs = offset;

		void *aux = lazy_load_data;
		if (!vm_alloc_page_with_initializer (VM_FILE, addr,
					writable, lazy_load_segment, aux))
			return NULL;

		/* Advance. */
		read_bytes -= page_read_bytes;
		offset += page_read_bytes;
		addr += PGSIZE;
	}
	return start_addr;
}

/* Do the munmap */
void
do_munmap (void *addr) {
	spt_remove_page(&thread_current()->spt, spt_find_page(&thread_current()->spt ,addr));
}
