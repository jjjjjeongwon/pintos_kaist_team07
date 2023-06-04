/*  유저 프로세스가 일부 커널 기능에 접근하려고 할때마다 시스템 콜이 호출됩니다. 이게 시스템 콜 핸들러의 기본 구조입니다. 
현재 상태에서는 이때 단지 메세지를 출력하고 유저 프로세스를 종료시키게 되어있습니다. 
이번 프로젝트의 part2에서 시스템 콜이 필요로 하는 다른 일을 수행하는 코드를 수행하게 될 겁니다.*/
#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "threads/flags.h"
#include "intrinsic.h"
#include "threads/init.h"
#include <filesys/filesys.h>
#include <filesys/file.h>
#include "userprog/process.h"

typedef int pid_t;

void syscall_entry (void);
void syscall_handler (struct intr_frame *);
void halt (void);
void exit (int status);
pid_t fork (const char *thread_name);
int exec (const char *cmd_line);
int wait (pid_t pid);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned size);
int write (int fd, const void *buffer, unsigned size);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);
void check_address(void *addr);
void get_argument(void *esp, int *arg, int count);

/* System call.
 *
 * Previously system call services was handled by the interrupt handler
 * (e.g. int 0x80 in linux). However, in x86-64, the manufacturer supplies
 * efficient path for requesting the system call, the `syscall` instruction.
 *
 * The syscall instruction works by reading the values from the the Model
 * Specific Register (MSR). For the details, see the manual. */

#define MSR_STAR 0xc0000081         /* Segment selector msr */
#define MSR_LSTAR 0xc0000082        /* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* Mask for the eflags */

void
syscall_init (void) {
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48  |
			((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t) syscall_entry);

	/* The interrupt service rountine should not serve any interrupts
	 * until the syscall_entry swaps the userland stack to the kernel
	 * mode stack. Therefore, we masked the FLAG_FL. */
	write_msr(MSR_SYSCALL_MASK,
			FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
}

/* The main system call interface */
void syscall_handler(struct intr_frame *f UNUSED)
{
	// TODO: Your implementation goes here.
	printf("system call!\n");
	check_address(f->rsp);
	int syscall_num = f->R.rax;

	switch (syscall_num)
	{
	case SYS_HALT: /* Halt the operating system. */
		printf("halt!\n");
		halt();
		break;
	case SYS_EXIT: /* Terminate this process. */
		printf("exit!\n");
		exit(f->R.rdi);
		break;
	case SYS_FORK: /* Clone current process. */
		printf("fork!\n");
		f->R.rax = fork(f->R.rdi);
		break;
	case SYS_EXEC: /* Switch current process. */
		printf("exec!\n");
		f->R.rax = exec(f->R.rdi);
		break;
	case SYS_WAIT: /* Wait for a child process to die. */
		printf("wait!\n");
		f->R.rax = wait(f->R.rdi);
		break;
	case SYS_CREATE: /* Create a file. */
		printf("create!\n");
		f->R.rax = create(f->R.rdi, f->R.rsi);
		break;
	case SYS_REMOVE: /* Delete a file. */
		printf("remove!\n");
		f->R.rax = remove(f->R.rdi);
		break;
	case SYS_OPEN: /* Open a file. */
		printf("open!\n");
		f->R.rax = open(f->R.rdi);
		break;
	case SYS_FILESIZE: /* Obtain a file's size. */
		printf("open!\n");
		f->R.rax = open(f->R.rdi);
		break;
	case SYS_READ: /* Read from a file. */
		printf("read!\n");
		f->R.rax = read(f->R.rdi, f->R.rsi, f->R.rdx);
		break;
	case SYS_WRITE: /* Write to a file. */
		printf("write!\n");
		f->R.rax = write(f->R.rdi, f->R.rsi, f->R.rdx);
		break;
	case SYS_SEEK: /* Change position in a file. */
		printf("seek!\n");
		seek(f->R.rdi, f->R.rsi);
		break;
	case SYS_TELL: /* Report current position in a file. */
		printf("tell\n");
		f->R.rax = tell(f->R.rdi);
		break;
	case SYS_CLOSE: /* Close a file. */
		printf("close\n");
		close(f->R.rdi);
		break;
	default:
		printf("Wrong system call number\n");
		break;
	}
	thread_exit();
}

void halt (void) {
	power_off();
}

void exit (int status) {
	struct thread *cur = thread_current (); 
    printf("%s: exit(%d)\n" , cur -> name , status);
    thread_exit();
}

pid_t fork (const char *thread_name) {
	return process_fork(thread_name, NULL);
}

int exec (const char *cmd_line) {
	process_exec(cmd_line);
}

int wait (pid_t pid) {

}

bool create (const char *file, unsigned initial_size) {
	return filesys_create(file, initial_size);
}

bool remove (const char *file) {
	return filesys_remove(file);
}

int open (const char *file) {
	return file_open(file);
}

int filesize (int fd) {
	
}

int read (int fd, void *buffer, unsigned size) {
	file_read()
}

int write (int fd, const void *buffer, unsigned size) {

}

void seek (int fd, unsigned position) {

}

unsigned tell (int fd) {
	
}

void close (int fd) {

}

/* 주소 값이 유저 영역에서 사용하는 주소 값인지 확인 하는 함수
유저 영역을 벗어난 영역일 경우 프로세스 종료(exit(-1)) */
void check_address(void *addr) {
	if (addr <0x8048000 || 0xc0000000 < addr) {
		printf("Address out of user area");
		exit(-1);
	}
}

/* 유저 스택에 있는 인자들을 커널에 저장하는 함수
스택 포인터(rsp)에 count(인자의 개수) 만큼의 데이터를 arg에 저장 */
void get_argument(void *rsp, int *arg, int count) {

}