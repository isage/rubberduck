@
@ safeRW.S - CreepNT's "MemAccessError area"-based peek/poke primitives
@
@ SceExcpmgr accepts registration of a code region ("MemAccessError area")
@ which can contain small sequences of instructions that perform a single
@ read or write to memory. If a DABT occurs in this area, SceExcpmgr will
@ set r0=0x80022005 (ERROR_INVALID_MEMORY_ACCESS) then resume execution two
@ instructions after the faulting one instead of triggering a kernel panic.
@ This effectively skipping both the faulting instruction, and the one right
@ after it.
@
@ On success, the "mov r0, #0" is executed and these primitives return 0.
@ On failure, SceExcpmgr skips the "mov r0, #0" and an error is returned.
@
@ N.B.: code in the MemAccessError area must be in Thumb mode.
@
	.thumb
	.syntax unified
	.cpu cortex-a9

	@ Used by C code for registration with SceExcpmgr
	.global _safeRWRegionStart
	.global _safeRWRegionEnd
_safeRWRegionStart:

	@ _safeReadX routines attempt to read *r0 into r1,
	@ returning 0 in r0 on success (data in r1) and an
	@ error code in r0 otherwise (r1 = garbage).
	@ No register other than r0 and r1 is clobbered.
_safeRead8:
	ldrb	r1, [r0]
	mov		r0, #0
	bx		lr

_safeRead16:
	ldrh	r1, [r0]
	mov		r0, #0
	bx		lr

_safeRead32:
	ldr		r1, [r0]
	mov		r0, #0
	bx		lr

	.type safeWrite8, %function
	.global safeWrite8
safeWrite8:
	strb	r1, [r0]
	mov		r0, #0
	bx		lr

	.type safeWrite16, %function
	.global safeWrite16
safeWrite16:
	strh	r1, [r0]
	mov		r0, #0
	bx		lr

	.type safeWrite32, %function
	.global safeWrite32
safeWrite32:
	str		r1, [r0]
	mov		r0, #0
	bx		lr

_safeRWRegionEnd:
	@ N.B.: this isn't really needed
	.size _safeRWRegionStart, . - _safeRWRegionStart

	@ These functions should only touch safe pointers,
	@ so they don't have to be in the MemAccessError area.
	.type safeRead8, %function
	.global safeRead8
safeRead8:
	push	{ r1, lr }		@ save output pointer & return address
	bl		_safeRead8		@ call safe wrapper
	pop		{ r3, lr }
	cmp		r0, #0
	it		eq				@ on success, store result
	strbeq	r1, [r3]
	bx		lr

	.type safeRead16, %function
	.global safeRead16
safeRead16:
	push	{ r1, lr }		@ save output pointer & return address
	bl		_safeRead16		@ call safe wrapper
	pop		{ r3, lr }
	cmp		r0, #0
	it		eq				@ on success, store result
	strheq	r1, [r3]
	bx		lr

	.type safeRead32, %function
	.global safeRead32
safeRead32:
	push	{ r1, lr }		@ save output pointer & return address
	bl		_safeRead32		@ call safe wrapper
	pop		{ r3, lr }
	cmp		r0, #0
	it		eq				@ on success, store result
	streq	r1, [r3]
	bx		lr

