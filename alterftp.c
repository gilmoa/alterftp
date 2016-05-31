#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "config_file.c"
#include "debug.c"

#define CONF_PATH ".alterftp_conf"

void print_usage(char *name);
void do_init();

int curl_mkdir(char *base_path, char *path);
int curl_rmdir(char *base_path, char *path);
int curl_send(char *base_path, char *path, char *target);

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


	// other commands require credentials
	// and additional arguments
	struct creds credentials;
	if(!get_credentials(CONF_PATH, &credentials))
	{
		exit(EXIT_FAILURE);
	}

	int targets_count = argc - 2;
	char **targets = malloc(targets_count * sizeof(char *));

	int x;
	for(x = 2; x < argc; x++)
	{
		targets[x - 2] = malloc((strlen(argv[x]) + strlen(credentials.dir) + 2) * sizeof(char));
		strcpy(targets[x - 2], credentials.dir);
		strcat(targets[x - 2], "/");
		strcat(targets[x - 2], argv[x]);
	}

	// Debug arguments
	for(x = 0; x < targets_count; x++)
		print_string("target", targets[x]);

	// Compute base path url
	char base_path_adds[] = "ftp://:@ftp..altervista.org";
	size_t base_path_length = strlen(base_path_adds) +
														strlen(credentials.user) +
														strlen(credentials.pwd) +
														strlen(credentials.dir);

	char *base_path = malloc((base_path_length + 1) * sizeof(char));
	sprintf(base_path, "ftp://%s:%s@ftp.%s.altervista.org%s",
			credentials.user, credentials.pwd, credentials.user, credentials.dir);

	// MKDIR
	if(strcmp(cmd, "mkdir") == 0)
	{
		int x;
		for(x = 0; x < targets_count; x++)
		{
			if(fork() == 0)
			{
				curl_mkdir(base_path, targets[x]);
			}
		}

		while(wait(NULL) > 0);
		print_done();
	}
	// RMDIR
	else if(strcmp(cmd, "rmdir") == 0)
	{
		int x;
		for(x = 0; x < targets_count; x++)
		{
			if(fork() == 0)
			{
				curl_rmdir(base_path, targets[x]);
			}
		}

		while(wait(NULL) > 0);
		print_done();
	}
	// SEND
	else if(strcmp(cmd, "send") == 0)
	{
		int x;
		base_path[strlen(base_path) - strlen(credentials.dir)] = '\0';

		for(x = 0; x < targets_count; x++)
		{
			char *target = targets[x] + strlen(credentials.dir) + 1;
			if(fork() == 0)
			{
				curl_send(base_path, targets[x], target);
				exit(EXIT_SUCCESS);
			}
		}

		while(wait(NULL) > 0);
		print_done();
	}
	// DELETE
	else if(strcmp(cmd, "delete") == 0)
	{
		int x;
		for(x = 0; x < targets_count; x++)
		{
			if(fork() == 0)
			{
				curl_delete(base_path, targets[x]);
				exit(EXIT_SUCCESS);
			}
		}

		while(wait(NULL) > 0);
		print_done();
	}
	else
	{
		print_usage(argv[0]);
	}

	free(base_path);
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

	// Remove annoying '/'
	// Before
	if(credentials.dir[0] != '/')
	{
		char *tmp = malloc((strlen(credentials.dir) + 1) * sizeof(char));
		strcpy(tmp, "/");
		strcat(tmp, credentials.dir);
		strcpy(credentials.dir, tmp);
		free(tmp);
	}
	// After
	if(credentials.dir[strlen(credentials.dir) - 1] == '/')
		credentials.dir[strlen(credentials.dir) - 1] = '\0';

	if(!save_credentials(CONF_PATH, credentials))
		exit(EXIT_FAILURE);

	printf("All set up.\n");
}

int curl_cmd(char *base_path, char *cmd, char *arg)
{
	int r;
	char *location = malloc((strlen(base_path) + 2) * sizeof(char));
	strcpy(location, base_path);
	// strcat(location, "/");

	// printf("%s %s %s %s\n", "curl", location, cmd, arg);
	r = execlp("curl", "curl", "-sS", location, cmd, arg, (char *)NULL);
	free(location);
	return r;
}

int curl_mkdir(char *base_path, char *path)
{
	int r;
	char *target = malloc((strlen(path) + 4) * sizeof(char));
	strcpy(target, "MKD ");
	strcat(target, path + 1);

	char *base_path_c = malloc((strlen(base_path) + 2) * sizeof(char));
	strcpy(base_path_c, base_path);
	strcat(base_path_c, "/");

	r = curl_cmd(base_path_c, "-Q", target);
	free(target);
	return r;
}

int curl_rmdir(char *base_path, char *path)
{
	int r;
	char *target = malloc((strlen(path) + 4) * sizeof(char));
	strcpy(target, "RMD ");
	strcat(target, path + 1);

	char *base_path_c = malloc((strlen(base_path) + 2) * sizeof(char));
	strcpy(base_path_c, base_path);
	strcat(base_path_c, "/");

	r = curl_cmd(base_path_c, "-Q", target);
	free(target);
	return r;
}

int curl_send(char *base_path, char *path, char *target)
{
	int r = 0;
	char *base_path_c = malloc((strlen(base_path) + strlen(path) + 1) * sizeof(char));
	strcpy(base_path_c, base_path);
	strcat(base_path_c, path);

	r = curl_cmd(base_path_c, "-T", target);
	free(target);
	return r;
}

int curl_delete(char *base_path, char *path)
{
	int r;
	char *target = malloc((strlen(path) + 4) * sizeof(char));
	strcpy(target, "DELE ");
	strcat(target, path + 1);

	char *base_path_c = malloc((strlen(base_path) + 2) * sizeof(char));
	strcpy(base_path_c, base_path);
	strcat(base_path_c, "/");

	print_debug("%s %s %s\n", base_path_c, "-Q", target);
	r = curl_cmd(base_path_c, "-Q", target);
	free(target);
	return r;
}
