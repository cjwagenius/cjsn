
#define RUNTEST(x) {\
	printf("%-30s", #x);\
	fflush(stdout);\
	x();\
	printf("ok\n");\
}

