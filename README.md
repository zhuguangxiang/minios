minios
======

Embedded Operating System Just Funny

*v001:*

    This version includes basic task scheduler, high serivce routine for device
    interrupt and timer. see app/app.c for test

*v002:*

	+ adjust some directories for more reasonable. port -->> hal
	+ move interrupt stack from code section to bss section, reduce bin size
	+ .h, .c and .S file format
    + stack check

*v003:*

	+ adjust directories
    + format comments
    + minios project framework

*v0.04*

    + version number change to vx.yz
    + wait queue implemented
    + interrupt is divided into ISR and HSR(enabled interrupt mode)
    + init various modules using gcc section

*v0.05*

    + mutex, condition variable, semaphore and message queue implemented
	+ adjust init function of mutex, condition, semaphore and msg_queue.
	+ TODO:msg_queue maybe changed to message buffer not message pointer.

*v0.06*

	+ linux fifo
	+ msg_queue uses fifo instead of message pointer
	+ mem_pool implementation

*v0.07*

	+ minios virtual filesystem from linux kernel and ecos
	+ ramfs impl from ecos and linux kernel
	+ memory pool changed from stack impl to queue impl
	+ memory pool supports twice free check and wild pointer check
	+ update to v0.07



