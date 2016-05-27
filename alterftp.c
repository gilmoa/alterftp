#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "debug.c"

void print_usage(char *name);
void do_init();

int main(int argc, char *argv[]) {
	// checks arguments sanity
	if(argc < 2)
	{
		print_usage(argv[0]);
		exit(EXIT_SUCCESS);
	}

	char *cmd = argv[1];

	if(strcmp(cmd, "init") != 0 && argc < 3)
	{
		print_usage(argv[0]);
		exit(EXIT_SUCCESS);
	}

	char *targets[10];
	int targets_count = argc - 2;

	// parse arguments
	if(strcmp(cmd, "init") == 0)
	{
		do_init();
		exit(EXIT_SUCCESS);
	}

	// other commands require additional arguments
	int x;
	for(x = 2; x < argc || x < 10; x++)
		targets[x - 2] = argv[x];

	


	return(EXIT_SUCCESS);
}

void print_usage(char *name)
{
	printf("%-10s%s <command> [directory | file names] files (...)\n\n", "usage:", name);
	printf("command:\n");
	printf("   %-9s setup current directory for %s\n", "init", name);
	printf("   %-9s send [file names] to ftp server\n", "send");
	printf("   %-9s delete [file names] from ftp server\n", "delete");
	printf("   %-9s list content of [path] or '/' on server\n", "list");
	printf("   %-9s create [path] directory on server\n", "mkdir");
	printf("   %-9s create [path] directory on server\n", "rmdir");
	printf("   %-9s show this, as any invalid command or syntax\n", "help");
}

void do_init()
{

}
