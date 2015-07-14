;==============================================================================
;       Startup Code for SPC
;
;           crt0.s
;                                   blog.naver.com/mayhouse
;                                    Based on  Alteration by Piroyan.    Based on Crt0.s included in SDCC.
;==============================================================================

		.module	crt0spc
		.globl	_main

	.area	_HEADER	(ABS)

	.org	0x7cdd		; 7cdd�������� ���α׷����ּҿ� �����ּҰ� ����. 6byte
					;�׷��Ƿ� �ҽ��� 8006���� ���ķ� ����ȴ�.
					;Ȥ�� ���ϴ� �ּҷ� �����ϸ�ȴ�.
					;0x8000�̸� --code-loc 0x8006
					;0x7cdd�̸� --code-loc 0x7ce3
start:
	call	gsinit		; Initialise global variables
	jp		_main		; Jump to main()


	; Ordering of segments for the linker.
	.area	_HOME
	.area	_CODE
	.area	_GSINIT
	.area	_GSFINAL

	.area	_GSINIT
gsinit::

	; Initialization code enters here. 

	.area	_GSFINAL
	ret

	.area	_DATA
	.area	_BSS
	.area	_HEAP
	