/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#ifndef EMBED_DATA_H
#define EMBED_DATA_H

__asm__(
".altmacro\n" \
".macro binfile p q\n" \
"	.global \\p\n" \
"\\p:\n" \
"	.incbin \\q\n" \
"\\p&_end:\n" \
"	.byte 0\n" \
"	.global \\p&_len\n" \
"\\p&_len:\n" \
"	.int(\\p&_end - \\p)\n" \
".endm\n\t"
);

#ifdef __cplusplus
	extern "C" {
#endif

#define BINDATA(n, s) \
__asm__("\n\n.data\n\tbinfile " #n " \"" #s "\"\n"); \
extern char n; \
extern int n##_len;

#ifdef __cplusplus
	}
#endif

#endif
