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

