	.file	"c:\lcc\include\stdarg.h"
_$M0:
	.file	"c:\lcc\include\stdio.h"
_$M1:
	.file	"c:\lcc\include\stddef.h"
_$M2:
	.file	"c:\lcc\include\stdlib.h"
_$M3:
	.file	"c:\pak\pakfile.h"
_$M4:
	.text
	.type	_vsscanf,function
_vfscanf:
	pushl	%ebp
	movl	%esp,%ebp
	subl	$12,%esp
	pushl	%esi
	pushl	%edi
	.line	10
	.line	11
	pushl	12(%ebp)
	call	_numOfArgs
	addl	$4,%esp
	movl	%eax,-12(%ebp)
	.line	12
	pushl	-12(%ebp)
	pushl	16(%ebp)
	call	_va_listToArray
	addl	$8,%esp
	movl	%eax,-8(%ebp)
	.line	15
	movl	-12(%ebp),%edi
	subl	$1,%edi
	movl	%edi,-4(%ebp)
	jmp	_$7
_$4:
	.line	16
	movl	-4(%ebp),%edi
	movl	-8(%ebp),%esi
	pushl	(%esi,%edi,4)
	.line	15
	decl	-4(%ebp)
_$7:
	cmpl	$0,-4(%ebp)
	jge	_$4
	.line	19
	pushl	-8(%ebp)
	call	_free
	addl	$4,%esp
	.line	21
	pushl	12(%ebp)
	pushl	8(%ebp)
	call	_sscanf
	movl	-12(%ebp), %ebx
	imull	$4, %ebx
	addl	$8, %ebx
	addl	%ebx, %esp
	.line	22
	popl	%edi
	popl	%esi
	leave
	ret
_$10:
	.size	_vsscanf,_$10-_vsscanf
	.globl	_vsscanf
	.extern	_numOfArgs
	.extern	_va_listToArray
	.extern	_free
	.extern	_sscanf
