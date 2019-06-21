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

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cjsn.h"

#define issep(x) (!x || isspace(x) || strchr(",]}", x))

static char *skipws(const char *str)
{
	assert(str != NULL);

	while (isspace(*str))
		str++;

	return (char*) str;
}
static char *find_closing(char *str)
{
	int sch;	/* start char */
	int ech = 0;	/* end char   */
	int depth = 1;
	int quote = 0;

	assert(str != NULL);

	sch = *str;
	if (sch == '"')
		ech = '"';
	else if (sch == '[')
		ech = ']';
	else if (sch == '{')
		ech = '}';
	assert(ech != 0); /* invalid start character left ech == 0 */

	for (str += 1; *str; str += 1) {
		if ((*str == '"') && (*(str-1) != '\\')) {
			if (ech == '"')
				return str;
			quote = !quote;
		} else if (quote) {
			continue;
		} else if (*str == sch) {
			depth += 1;
		} else if (*str == ech) {
			depth -= 1;
			if (!depth)
				break;
		}
	}
	
	return *str ? str : NULL;
}
static char *next(char *str, char **endptr, void *val)
{
	char *ep;
	double d;

	assert(str != NULL && endptr != NULL && val != NULL);

	if ((d = strtod(str, &ep)) || ep != str) {
		*((double*) val) = d;
	} else if (strchr("\"[{", *str)) {
		if (!(ep = find_closing(str)))
			return NULL;
		*((char**) val) = str+1;
		ep += 1;
	} else if (!strncmp(str, "true", 4) && issep(*(ep = str+4))) {
		*((int*) val) = 1;
	} else if ((!strncmp(str, "null", 4) && issep(*(ep = str+4))) ||
	    (!strncmp(str, "false", 5) && issep(*(ep = str+5)))) {
		*((int*) val) = 0;
	} else {
		return NULL;
	}

	if (endptr) *endptr = ep-1;

	return str;
}

unsigned cjsn_len(const char *cx)
{
	char *x;
	size_t l = 0;
	struct cjsn cj;

	assert(cx != NULL);

	if (!(x = cjsn_step(cx, &cj)))
		return -1;
	if (*x != '[' && *x != '{')
		return 1;
	while (cjsn_step(NULL, &cj))
		l += 1;
	
	return cjsn_error(&cj) ? -1 : l;
}
char *cjsn_obj_get(const char *cx, const char *key, struct cjsn *cj)
{
	char *x;
	struct cjsn st;

	assert(cx != NULL && key != NULL);

	if (!cj)
		cj = &st;
	if (!(x = cjsn_step(cx, cj)))
		return NULL;
	while (cjsn_step(NULL, cj)) {
		if (!strncmp(key, cj->key.ptr, cj->key.len) &&
		    !key[cj->key.len])
			return cj->sp;
	}

	return NULL;
}
char *cjsn_step(const char *cx, struct cjsn *cj)
{
	char *ep;
	char *rtn;

	assert(cj != NULL && "cjsn_step: invalid argument");

	if (cx) {
		cj->pnt.sp = cj->pnt.ep = NULL;
		cj->npos = (char*) cx;
	}
	if (!*(cj->npos = skipws(cj->npos)) || cj->npos == cj->pnt.ep) {
		cj->npos = "";
		return NULL;
	}
	
	/* if we're stepping an array/object (cj->pnt.sp != NULL). */
	if (cj->pnt.sp) {
		/* if the pnt array/object was the last type found
		 * (cj->pnt.sp == cj->sp), we're not expecting a
		 * comma before a type. Otherwise we do, and in that case
		 * we'll have to traverse it.
		 **/
		if (cj->pnt.sp != cj->sp) {
			if (*cj->npos != ',')
				return NULL;
			cj->npos += 1;
			cj->npos = skipws(cj->npos);
		}
		/* if the parent is an object, we expect a key and a colon
		 * before any type. We handle all of it here.
		 **/
		if (*cj->pnt.sp == '{') {
			if (*next(cj->npos, &ep, &cj->key.ptr) != '"')
				return NULL;
			/*cj->key.ptr = cj->npos+1;*/
			cj->key.len = ep - (cj->npos+1);
			cj->npos = ep+1;
			if (*(cj->npos = skipws(cj->npos)) != ':')
				return NULL;
			cj->npos += 1;
			cj->npos = skipws(cj->npos);
		}
	}
	if ((rtn = next(cj->npos, &ep, &cj->val.i))) {
		/* if cx is not NULL, and type is an array/object, set parent
		 * values in the state.
		 **/
		if (cx && (*rtn == '[' || *rtn == '{')) {
			cj->pnt.sp = cj->sp = cj->npos;
			cj->pnt.ep = ep;
			cj->npos += 1;
		} else {
			if (*rtn == '"')
				cj->val.s.len = ep - (rtn+1);
			cj->sp = cj->npos;
			cj->npos = ep+1;
		}
	}

	return rtn;
}
int cjsn_type(const char *cx)
{
	char *ep;

	assert(cx != NULL);

	if (strchr("\"[{n", *cx))
		return *cx;
	if (*cx == 't' || *cx == 'f')
		return 'b';
	if (strtod(cx, &ep) || ep != cx)
		return '0';

	return 0;
}

