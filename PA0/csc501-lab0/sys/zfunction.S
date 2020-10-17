
.global zfunction

zfunction:
	pushl %ebp
	movl %esp,%ebp
	movl 8(%ebp),%eax 
	ANDl $0xFF803FFF,%eax
	shl $0x4, %eax	 
	ANDl $0xFFFFFFF0, %eax
	popl %ebp
	
	ret
