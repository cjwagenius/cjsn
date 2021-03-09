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
 *
 * WWW: https://github.com/cjwagenius/cjsn
 */
/**
 * A small footprint ANSI C JSON parser API
 * ---------------------------------------------------------------------------
 *  Structure:
 *
 * 	typedef struct cjsn cjsn_t
 *
 * 		A cjsn-structure that holds the state of the parsing (see
 * 		declaration below).
 *
 *
 * ---------------------------------------------------------------------------
 *  Macros:
 *
 *	Type-checks:
 *		cjsn_isbool(x)
 *		cjsn_isnull(x)
 *		cjsn_isnum(x)
 *		cjsn_isarr(x)
 *		cjsn_isobj(x)
 *		cjsn_isstr(x)
 *		cjsn_isaos(x)
 *
 *	Getting values from the structure:
 *
 *		cjsn_get_bool(c)
 *		cjsn_get_num(c)
 *		cjsn_get_str(c)
 *		cjsn_get_strl(c)
 *		cjsn_get_key(c)
 *		cjsn_get_keyl(c)
 *
 *	cjsn_get_start(x)
 *		
 *		Get start-offset of current item.
 *
 *	cjsn_get_end(x)	
 *
 * 		Get end-offset of current item.
 *
 *	cjsn_error(x)
 *
 *		Query the state (when cjsn_step() returns NULL) if there was
 *		an error.
 *
 *
 * ---------------------------------------------------------------------------
 *  Functions:
 *
 *	unsigned cjsn_len(const char *cx)
 *
 * 		Returns the length of the json-item at [cx]. If [cx] isn't a
 * 		valid json-entity, -1 is returned. If [cx] points to an array
 * 		or object, it will be iterated and child items counted. Other-
 * 		wise 1 is returned.
 *
 *
 *	char *cjsn_obj_get(const char *cx, const char *key, struct cjsn *cj)
 *
 * 		If [cx] points to an object, it will be iterated until a item
 * 		with a matching key is found and the state can be queried. If
 * 		no such key can be found, NULL is returned.
 * 		This function only returns the first item found. If that's not
 * 		desirable, use cjsn_step().
 *
 *
 *	char *cjsn_step(const char *cx, struct cjsn *cj)
 *
 *		This function works a bit like strtok_r. The first time you
 *		call it, you apply where to start with [cx]. The return value
 *		is the offset of the first json entity, or NULL if none was
 *		found.
 *		If it's not an array or object, the state [cj] can be queried
 *		for the value right away, and further cjsn_steps will return
 *		NULL.
 *		If it's an array or object. Subsequent calls to cjsn_step()
 *		with [cx] set to NULL, will iterate through the child items
 *		and set the state accordingly. When no child items are left,
 *		NULL is returned.
 *
 *
 *	int cjsn_type(const char *cx)
 *
 *		A conveniece-function for ease type-testing. This function
 *		returns the following values, depending on the type at [cx].
 *
 *			'n'	null
 *			'b'	true or false
 *			'0'	number
 *			'"'	string
 *			'['	array
 *			'{'	object
 *			 0	invalid
 *
 * ---------------------------------------------------------------------------
 **/

#ifndef CJSN_H
#define CJSN_H

#include <stdio.h>

#define cjsn_isbool(x)		(*x == 't' || *x == 'f')
#define cjsn_isnull(x)		(*x == 'n')
#define cjsn_isnum(x)		(cjsn_type(x) == '0')
#define cjsn_isarr(x)		(*x == '[')
#define cjsn_isobj(x)		(*x == '{')
#define cjsn_isstr(x)		(*x == '"')
#define cjsn_isaos(x)		(cjsn_isarr(x)||cjsn_isobj(x)||cjsn_isstr(x))
#define cjsn_error(x)		(*((x)->npos))
#define cjsn_get_bool(c)	((c)->val.i)
#define cjsn_get_num(c)		((c)->val.d)
#define cjsn_get_str(c)		((c)->val.s.ptr)
#define cjsn_get_strl(c)	((c)->val.s.len)
#define cjsn_get_key(c)		((c)->key.ptr)
#define cjsn_get_keyl(c)	((c)->key.len)
#define cjsn_get_start(x)	((x)->sp)
#define cjsn_get_end(x)		((x)->npos-1)

typedef struct cjsn {
	struct pnt {			/* parent structure      */
		char *sp;		/* start pointer         */
		char *ep;		/* end pointer           */
	} pnt;
	union val {			/* value structure       */
		int i;
		double d;
		struct s {
			char *ptr;	/* pointer to actual str */
			size_t len;	/* length of the string  */
		} s;
	} val;
	struct key {
		char *ptr;
		size_t len;
	} key;
	char *sp;		/* start pointer to value */
	char *npos;		/* next position in data  */
} cjsn_t;

extern unsigned cjsn_len(const char *cx);
extern char *cjsn_obj_get(const char *cx, const char *key, struct cjsn *cj);
extern char *cjsn_step(const char *cx, struct cjsn *cj);
extern int cjsn_type(const char *cx);

#ifdef CJSN_DEFINE

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cjsn.h"

#define issep(x)  (!x || isspace(x) || strchr(",]}", x))
#define skipws(x) while (isspace(*x)) x++

static char *find_closing(char *str) {

	int sch = *str;	/* start char                   */
	int ech = 0;	/* end char                     */
	int depth = 1;
	int quote = 0;	/* is parsing within quotation? */

	if (sch == '"')
		ech = '"';
	else if (sch == '[')
		ech = ']';
	else if (sch == '{')
		ech = '}';
	assert(ech != 0 && "a invalid start character left ech == 0");

	for (str += 1; *str; str += 1) {
		if ((*str == '"') && (*(str-1) != '\\')) {
			if (ech == '"')
				return str;
			quote = !quote;
		}
		if (!quote) {
			if (*str == sch) {
				depth ++;
			} else if (*str == ech) {
				depth --;
				if (!depth)
					break;
			}
		}
	}
	
	return *str ? str : NULL;
}
static char *next(char *str, char **ep, void *val) {

	double d;

	assert(str != NULL && ep != NULL && val != NULL);

	d = strtod(str, ep);
	if (*ep != str) {
		*((double*) val) = d;
		return str;
	}
	if (strchr("\"[{", *str)) {
		if (!(*ep = find_closing(str)))
			return NULL;
		*((char**) val) = str + 1;
		return str;
	}
	if (issep(str[4])) {
		if (!strncmp(str, "true", 4) || !strncmp(str, "null", 4)) {
			*((int*) val) = *str == 't' ? 1 : 0;
			*ep += 4;
			return str;
		}
	}
	if (issep(str[5]) && !strncmp(str, "false", 5)) {
		*((int*) val) = 0;
		*ep += 5;
		return str;
	}

	return NULL;
}

unsigned cjsn_len(const char *cx) {

	char *x;
	size_t l = 0;
	struct cjsn cj;

	assert(cx != NULL);

	if (!(x = cjsn_step(cx, &cj)))
		return -1;
	if (!cjsn_isarr(x) && !cjsn_isobj(x))
		return 1;
	while (cjsn_step(NULL, &cj))
		l += 1;
	
	return cjsn_error(&cj) ? -1 : l;
}
char *cjsn_obj_get(const char *cx, const char *key, struct cjsn *cj) {

	char *x;
	struct cjsn st;

	assert(cx != NULL && key != NULL);

	if (!cj)
		cj = &st;
	if (!(x = cjsn_step(cx, cj)))
		return NULL;
	while (cjsn_step(NULL, cj)) {
		if (!strncmp(key, cj->key.ptr, cj->key.len))
			return cj->sp;
	}

	return NULL;
}
char *cjsn_step(const char *cx, struct cjsn *cj) {

	char *ep;
	char *rtn;

	assert(cj != NULL && "cjsn_step: invalid argument");

	if (cx) {
		cj->pnt.sp = NULL;
		cj->pnt.ep = NULL;
		cj->npos = (char*) cx;
	}
	skipws(cj->npos);
	if (!*cj->npos || cj->npos == cj->pnt.ep) {
		cj->npos = "";
		return NULL;
	}
	
	/* if we're stepping an array/object (cj->pnt.sp != NULL). */
	if (cj->pnt.sp) {
		/**
		 * if the pnt array/object was the last type found
		 * (cj->pnt.sp == cj->sp), we're not expecting a
		 * comma before a type. Otherwise we do, and in that case
		 * we'll have to traverse it.
		 **/
		if (cj->pnt.sp != cj->sp) {
			if (*cj->npos != ',')
				return NULL;
			cj->npos ++;
			skipws(cj->npos);
		}
		/**
		 * if the parent is an object, we expect a key and a colon
		 * before any type. We handle all of it here.
		 **/
		if (*cj->pnt.sp == '{') {
			if (*next(cj->npos, &ep, &cj->key.ptr) != '"')
				return NULL;
			cj->key.len = ep - cj->key.ptr;
			cj->npos = ep + 1;
			skipws(cj->npos);
			if (*cj->npos != ':')
				return NULL;
			cj->npos ++;
			skipws(cj->npos);
		}
	}
	if (!(rtn = next(cj->npos, &ep, &cj->val.i)))
		return NULL;

	if (cjsn_isaos(rtn)) {
		/**
		 * If [cx] is not NULL, and type is an array/object, set parent
		 * values of this state.
		 **/
		if (cx && !cjsn_isstr(rtn)) {
			cj->pnt.sp = cj->sp = cj->npos;
			cj->pnt.ep = ep;
		} else {
			cj->npos = ep;
		}
		cj->npos += 1;
		cj->val.s.len = ep - cj->val.s.ptr;
	} else {
		cj->npos = ep;
	}

	return (cj->sp = rtn);
}
int cjsn_type(const char *cx) {

	char *ep;

	assert(cx != NULL);

	if (cjsn_isaos(cx) || cjsn_isnull(cx))
		return *cx;
	if (cjsn_isbool(cx))
		return 'b';
	if (strtod(cx, &ep) || ep != cx)
		return '0';

	return 0;
}

#endif /* CJSN_DEFINE */

#endif /* CJSN_H */

