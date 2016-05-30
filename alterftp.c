#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "config_file.c"
#include "debug.c"

#define CONF_PATH ".alterftp_conf"

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

	// parse arguments
	if(strcmp(cmd, "init") == 0)
	{
		do_init();
		exit(EXIT_SUCCESS);
	}


	// other commands require additional arguments
	int targets_count = argc - 2;
	char **targets = malloc(targets_count * sizeof(char *));

	int x;
	for(x = 2; x < argc; x++)
	{
		targets[x - 2] = malloc((strlen(argv[x]) + 2) * sizeof(char));
		strcpy(targets[x - 2], "/");
		strcat(targets[x - 2], argv[x]);
	}

	for(x = 0; x < targets_count; x++)
	{
		print_string("target", targets[x]);
	}

	struct creds credentials;
	// //
	if(!get_credentials(CONF_PATH, &credentials))
	{
		free(targets);
		exit(EXIT_FAILURE);
	}

	printf("ftp://%s:%s@ftp.%s.altervista.org/%s\n", credentials.user, credentials.pwd, credentials.user, credentials.dir);

	free(targets);
	exit(EXIT_SUCCESS);
}

void print_usage(char *name)
{
	printf("%-10s%s <command> [directory | file names] files (...)\n\n", "usage:", name);
	printf("command:\n");
	printf("   %-9s setup current directory for %s\n", 						"init", name);
	printf("   %-9s send [file names] to ftp server\n", 							"send");
	printf("   %-9s delete [file names] from ftp server\n", 					"delete");
	printf("   %-9s list content of [path] or '/' on server\n", 			"list");
	printf("   %-9s create [path] directory on server\n", 						"mkdir");
	printf("   %-9s create [path] directory on server\n", 						"rmdir");
	printf("   %-9s show this, as any invalid command or syntax\n", 	"help");
}

void do_init()
{
	struct creds credentials;
	char c_dir[64];
	if(getcwd(c_dir, sizeof(c_dir)) != NULL)
	{
		printf("Initializing in '%s'.\n", c_dir);
	}
	else
	{
		perror("getcwd");
		exit(EXIT_FAILURE);
	}

	printf("%10s: ", "username");
	scanf("%s", credentials.user);

	printf("%10s: ", "password");
	scanf("%s", credentials.pwd);

	printf("%10s: ", "directory");
	scanf("%s", credentials.dir);

	if(credentials.dir[strlen(credentials.dir) - 1] == '/')
	{
		credentials.dir[strlen(credentials.dir) - 1] = '\0';
	}

	if(!save_credentials(CONF_PATH, credentials))
		exit(EXIT_FAILURE);

	printf("All set up.\n");
}
