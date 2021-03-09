/* This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org>
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "runtest.h"

#include "../cjsn.c"

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

	x = cjsn_obj_get(b, "three", 5, NULL);
	assert(x == NULL);

	x = cjsn_obj_get(b, "two", -1, NULL);
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
	RUNTEST(tst_find_closing);
	RUNTEST(tst_next);

	RUNTEST(tst_cjsn_type);
	RUNTEST(tst_cjsn_step);
	RUNTEST(tst_cjsn_len);
	RUNTEST(tst_cjsn_obj_get);

	return EXIT_SUCCESS;
}
