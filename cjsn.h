
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
extern char *cjsn_obj_get(const char *cx, const char *key, struct cjsn *cj);
/** get next json entity.
 *
 * parses the next json entity from cx and updates cj accoringly.
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

