
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "runtest.h"

#include "../cjsn.c"

void tst_skipws(void)
{
	char *p;
	char buf[] = "   null";

	p = skipws(buf+3);
	assert(p == buf+3);

	p = skipws(buf);
	assert(p == buf+3);
}
void tst_find_closing(void)
{
	char *p;
	char buf[] = "{\"a\":[\"]}\\\"\"]}";

	p = find_closing(buf);		/* '{' */
	assert(p == buf+13);
	p = find_closing(buf+5);	/* '[' */
	assert(p == buf+12);
	p = find_closing(buf+6);
	assert(p == buf+11);

	p = find_closing("\"\\\"");
	assert(p == NULL);
	p = find_closing("[[]");
	assert(p == NULL);
	p = find_closing("{{}");
	assert(p == NULL);
}
void tst_next(void)
{
	char *t;
	char *ep;
	union v { int i; double d; char *p; } v;
	char b[] = "{\"one\":[null,true,false,1.0,\"string\",fail]}";

	t = next(b, &ep, &v.p);
	assert(*t == '{' && *ep == '}');
	t = next(b+7, &ep, &v.p);
	assert(*t == '[' && *ep == ']');
	t = next(b+8, &ep, &v.i);
	assert(*t == 'n' && ep == t+3 && v.i == 0);
	t = next(b+13, &ep, &v.i);
	assert(*t == 't' && ep == t+3 && v.i == 1);
	t = next(b+18, &ep, &v.i);
	assert(*t == 'f' && ep == t+4 && v.i == 0);
	t = next(b+24, &ep, &v.d);
	assert(*t == '1' && ep == t+2 && v.d == 1.0);
	t = next(b+28, &ep, &v.p);
	assert(*t == '"' && ep == t+7 && v.p == t+1);
	t = next(b+37, &ep, &v.p);
	assert(t == NULL);
}

void tst_cjsn_step(void)
{
	struct cjsn st;

	assert(*cjsn_step("[ [ ], \"\" ]", &st) == '[');
	assert(*cjsn_step(NULL, &st) == '[');
	assert(*cjsn_step(NULL, &st) == '"');
	assert(st.val.s.len == 0);
	assert(!cjsn_step(NULL, &st) && !cjsn_error(&st));
	
	assert(*cjsn_step("{ \"one\": 0.5, \"two\": { } }", &st) == '{');
	assert(cjsn_isnum(cjsn_step(NULL, &st)));
	assert(*cjsn_step(NULL, &st) == '{');
	assert(!cjsn_step(NULL, &st) && !cjsn_error(&st));
}
void tst_cjsn_len(void)
{
	assert(cjsn_len("[1,2 3]") == -1);
	assert(cjsn_len("\"str\"") == 1);
	assert(cjsn_len("[1,2,3]") == 3);
}
void tst_cjsn_obj_get(void)
{
	char *x;
	char b[] = "{\"one\":1,\"two\":2}";

	x = cjsn_obj_get(b, "three", NULL);
	assert(x == NULL);

	x = cjsn_obj_get(b, "two", NULL);
	assert(x != NULL);
	
}
void tst_cjsn_type(void)
{
	assert(cjsn_type("null") == 'n');
	assert(cjsn_type("true") == 'b');
	assert(cjsn_type("false") == 'b');
	assert(cjsn_type("0.6") == '0');
	assert(cjsn_type("\"str\"") == '"');
	assert(cjsn_type("[ ]") == '[');
	assert(cjsn_type("{ }") == '{');
}

int main(int argc, char **argv)
{
	RUNTEST(tst_skipws);
	RUNTEST(tst_find_closing);
	RUNTEST(tst_next);

	RUNTEST(tst_cjsn_type);
	RUNTEST(tst_cjsn_step);
	RUNTEST(tst_cjsn_len);
	RUNTEST(tst_cjsn_obj_get);

	return EXIT_SUCCESS;
}
