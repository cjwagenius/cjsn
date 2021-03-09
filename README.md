# cjsn - A small footprint ANSI C JSON parser API

## Structure:
<pre>
    <b>typedef struct cjsn cjsn_t</b>

	A cjsn-structure that holds the state of the parsing (see
	declaration below).
</pre>
## Macros:
<pre>
    Type-checks:
	<b>cjsn_isbool(x)
	cjsn_isnull(x)
	cjsn_isnum(x)
	cjsn_isarr(x)
	cjsn_isobj(x)
	cjsn_isstr(x)
	cjsn_isaos(x)</b> (array, object or string)

    Getting values from the structure:

	<b>cjsn_get_bool(c)
	cjsn_get_num(c)
	cjsn_get_str(c)
	cjsn_get_strl(c)
	cjsn_get_key(c)
	cjsn_get_keyl(c)</b>

    <b>cjsn_get_start(x)</b>

	Get start-offset of current item.

    <b>cjsn_get_end(x)</b>

	Get end-offset of current item.

    <b>cjsn_error(x)</b>

	Query the state (when cjsn_step() returns NULL) if there was
	an error.
<pre>
## Functions:

    <b>unsigned cjsn_len(const char *cx)</b>

	Returns the length of the json-item at [cx]. If [cx] isn't a
	valid json-entity, -1 is returned. If [cx] points to an array
	or object, it will be iterated and child items counted. Other-
	wise 1 is returned.

    <b>char *cjsn_obj_get(const char *cx, const char *key, struct cjsn *cj)</b>

	If [cx] points to an object, it will be iterated until a item
	with a matching key is found and the state can be queried. If
	no such key can be found, NULL is returned.
	This function only returns the first item found. If that's not
	desirable, use cjsn_step().

    <b>char *cjsn_step(const char *cx, struct cjsn *cj)</b>

	This function works a bit like strtok_r. The first time you
	call it, you apply where to start with [cx]. The return value
	is the offset of the first json entity, or NULL if none was
	found.
	If it's not an array or object, the state [cj] can be queried
	for the value right away, and further cjsn_steps will return
	NULL.
	If it's an array or object. Subsequent calls to cjsn_step()
	with [cx] set to NULL, will iterate through the child items
	and set the state accordingly. When no child items are left,
	NULL is returned.

    <b>int cjsn_type(const char *cx)</b>

	A conveniece-function for ease type-testing. This function
	returns the following values, depending on the type at [cx].
	
		'n'	null
		'b'	true or false
		'0'	number
		'"'	string
		'['	array
		'{'	object
		 0	invalid
</pre>
