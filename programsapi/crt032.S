.section .text

.global _start
_start:
	# Set up end of the stack frame linked list.
	movl $0, %ebp
	pushl %ebp # rip=0
	pushl %ebp # rbp=0
	movl %esp, %ebp

	# We need those in a moment when we call main.
	pushl %esi
	pushl %edi

	# Prepare signals, memory allocation, stdio and such.
	# call initialize_standard_library

	# Run the global constructors.
	# call _init

	# Restore argc and argv.
	popl %edi
	popl %esi

	# Run main
	call main

	# Terminate the process with the exit code.
	movl %eax, %edi
	call exit
.size _start, . - _start
