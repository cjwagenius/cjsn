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
} cjsn;

#define cjsn_isbool(x)		(cjsn_type(x) == 'b')
#define cjsn_isnum(x)		(cjsn_type(x) == '0')
#define cjsn_isarr(x)		(*x == '[')
#define cjsn_isobj(x)		(*x == '{')
#define cjsn_isstr(x)		(*x == '"')
#define cjsn_error(x)		(*((x)->npos))
#define cjsn_get_bool(c)	((c)->val.i)
#define cjsn_get_num(c)		((c)->val.d)
#define cjsn_get_str(c)		((c)->val.s.ptr)
#define cjsn_get_strl(c)	((c)->val.s.len)
#define cjsn_get_key(c)		((c)->key.ptr)
#define cjsn_get_keyl(c)	((c)->key.len)
#define cjsn_get_start(x)	((x)->sp)
#define cjsn_get_end(x)		((x)->npos-1)

/** get length of a array/object
 **/
extern unsigned cjsn_len(const char *cx);
/** get a item from an object.
 **/
extern char *cjsn_obj_get(const char *cx, const char *key, size_t keyl,
			  struct cjsn *cj);

/** get next json entity.
 *
 * parses the next json entity from cx and updates cj accoringly. if cx is
 * NULL, step will continue after the last found entity.
 * 
 * returns:
 * 	pointer to the first char of the entity, if one is found.
 * 	NULL if parsing is complete or an error occured.
 *
 * if cj->npos != '\0' when NULL is returned, it points to where the error
 * occured.
 *
 **/
extern char *cjsn_step(const char *cx, struct cjsn *cj);
/** get type of json entity at cx.
 *
 * Returns:
 * 	'n' = null
 * 	'b' = bool
 * 	'0' = number
 * 	'"' = string
 * 	'{' = array
 * 	'{' = object
 * 	 0  = none (failure)
 **/
extern int cjsn_type(const char *cx);

