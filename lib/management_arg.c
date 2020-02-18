#ifndef MANAGEMENT_ARG_C
#define MANAGEMENT_ARG_C

bool enable_print;

void print_syntax() {
    printf("name sw: %s\n", name_exe);
    printf("sintax: TODO\n");   //TODO
    exit(EXIT_SUCCESS);
}



void managementCommand(int i) {
	if (!strcasecmp(command[i], "-h") || !strcasecmp(command[i], "--help")) {
        print_syntax();
	} else if (!strcasecmp(command[i], "-p") || !strcasecmp(command[i], "--enable-print")) {
        enable_print = TRUE;
	}
}

void managementParam(int i) {
	if (!strcasecmp(param[i], "param") || !strcasecmp(param[i], "parameter")) {
        
	} else if (!strcasecmp(param[i], "") || !strcmp(param[i], "")) {

	}
}

void managementArg() {
	if (countParam != 0) {
		for (PRECONDITION countParam POSTCONDITION) {
			managementParam(i);
		}
	}

	if (countCommand != 0) {
		for (PRECONDITION countCommand POSTCONDITION) {
			managementCommand(i);
		}
	}
}
#endif
