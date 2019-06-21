# cjsn
An extremly lightweight & simple json-parser.

cjsn only works with the string provided. No further allocations are needed.
```
#include <stdio.h>
#include <stdlib.h>

#include "cjsn.h"

char json[] = "{ \"one\": 1, \"array\": [ true, false ], \"str\": \"string\" }";

int main(int argc, char **argv)
{
	char *cx;
	struct cjsn cj;

	cjsn_step(json, &cj);
	while ((cx = cjsn_step(NULL, &cj))) {
		fprintf(stdout, "%.*s = ",
		    cjsn_get_keyl(&cj), cjsn_get_key(&cj));
		switch (cjsn_type(cx)) {
		case 'n':
			fputs("null", stdout);
			break;
		case 'b':
			fprintf(stdout, "%s",
			    cjsn_get_bool(&cj) ? "true" : "false");
			break;
		case '0':
			fprintf(stdout, "%d", cjsn_get_num(&cj));
			break;
		case '"':
			fprintf(stdout, "%.*s",
			    cjsn_get_strl(&cj), cjsn_get_str(&cj));
			break;
		default:
			fprintf(stdout, "%s", *cx == '[' ? "array" : "object");
		}
		fputc('\n', stdout);
	}

	if (cjsn_error(&cj))
		fprintf(stderr, "error at char %i\n", cj.npos - json);
}
```

Read cjsn.h for more info.

