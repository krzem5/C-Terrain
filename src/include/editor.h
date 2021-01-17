#ifndef EDITOR_H
#define EDITOR_H
#include <stdint.h>
#include <limits.h>
#include <signal.h>



#define bool _Bool
#define false 0
#define true 1
#ifdef NULL
#undef NULL
#endif
#define NULL ((void*)0)
#define assert(x) \
	do{ \
		if (!(x)){ \
			printf("%s:%i (%s): %s: Assertion Failed\n",__FILE__,__LINE__,__func__,#x); \
			raise(SIGABRT); \
		} \
	} while (0)



void run_editor(void);



void init_editor(void);



void update_editor(double dt);



#endif
