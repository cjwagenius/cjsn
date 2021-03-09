
#include <stdio.h>

#define CJSN_DEFINE
#include "../cjsn.h"

const char json[] =
"{\n"
"	\"number\": 10,\n"
"	\"array\": [ false, null ],\n"
"	\"bool\": true\n"
"}\n";

int main(int argc, char **argv) {

	char *i;
	cjsn_t cj;

	/* We know that the fist item is an object, so we don't need to test */
	cjsn_step(json, &cj);
	while ((i = cjsn_step(NULL, &cj))) {
		printf("%.*s = ", (int)cjsn_get_keyl(&cj), cjsn_get_key(&cj));
		switch (cjsn_type(i)) {
		case '0':
			printf("%f", cjsn_get_num(&cj));
			break;
		case '[':
			printf("(an array needs to be stepped though too)");
			break;
		case 'b':
			printf("%s", cjsn_get_bool(&cj) ? "true" : "false");
			break;
		}
		fputc('\n', stdout);
	}

	if ((i = cjsn_obj_get(json, "array", &cj))) {
		cjsn_step(i, &cj);
		while ((i = cjsn_step(NULL, &cj)))
			/* Getting bool out of a null... don't care */
			printf("%s ", cjsn_get_bool(&cj) ? "true" : "false");
		fputc('\n', stdout);
	}

	return 0;
}
