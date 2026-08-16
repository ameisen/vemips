	.abicalls
	.option	pic0
	.section	.mdebug.abi32,"",@progbits
	.nan	2008
	.module	fp=64
	.text
	.file	"test.cpp"
	.globl	_Z4foo0v                        # -- Begin function _Z4foo0v
	.p2align	2
	.type	_Z4foo0v,@function
	.set	nomicromips
	.set	nomips16
	.ent	_Z4foo0v
_Z4foo0v:                               # @_Z4foo0v
	.frame	$fp,16,$ra
	.mask 	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	.set	noat
# %bb.0:
	addiu	$sp, $sp, -16
	sw	$ra, 12($sp)                    # 4-byte Folded Spill
	sw	$fp, 8($sp)                     # 4-byte Folded Spill
	move	$fp, $sp
	lw	$1, 4($fp)
	addiu	$1, $1, 1
	sw	$1, 4($fp)
	lw	$2, 4($fp)
	move	$sp, $fp
	lw	$fp, 8($sp)                     # 4-byte Folded Reload
	lw	$ra, 12($sp)                    # 4-byte Folded Reload
	addiu	$sp, $sp, 16
	jrc	$ra
	.set	at
	.set	macro
	.set	reorder
	.end	_Z4foo0v
$func_end0:
	.size	_Z4foo0v, $func_end0-_Z4foo0v
                                        # -- End function
	.globl	_Z4bar0v                        # -- Begin function _Z4bar0v
	.p2align	2
	.type	_Z4bar0v,@function
	.set	nomicromips
	.set	nomips16
	.ent	_Z4bar0v
_Z4bar0v:                               # @_Z4bar0v
	.frame	$fp,8,$ra
	.mask 	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	.set	noat
# %bb.0:
	addiu	$sp, $sp, -8
	sw	$ra, 4($sp)                     # 4-byte Folded Spill
	sw	$fp, 0($sp)                     # 4-byte Folded Spill
	move	$fp, $sp
	move	$sp, $fp
	lw	$fp, 0($sp)                     # 4-byte Folded Reload
	lw	$ra, 4($sp)                     # 4-byte Folded Reload
	j	_Z4foo0v
	addiu	$sp, $sp, 8
	.set	at
	.set	macro
	.set	reorder
	.end	_Z4bar0v
$func_end1:
	.size	_Z4bar0v, $func_end1-_Z4bar0v
                                        # -- End function
	.ident	"clang version 21.1.4 (https://github.com/vemips/llvm-project.git bcd9f445c34dfe0cb658e317c6fda4c60d992d1f)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.text
