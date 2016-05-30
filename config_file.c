struct creds
{
	char user[32];
	char pwd[32];
	char address[64];
	unsigned int sumcheck;
};

unsigned int sum(struct creds *c)
{
	unsigned int r = 0;
	int x;

	for(x = 0; x < 32; x++)
	{
		r += c->user[x];
		r += c->pwd[x];
		r += c->address[x * 2];
		r += c->address[x * 2 + 1];
	}

	return r;
}

int check_config_sum(struct creds *c)
{
	unsigned int sumcheck;

	sumcheck = sum(c);

	return (sumcheck == (c->sumcheck));
}

int save_credentials(char *path, struct creds c)
{
	c.sumcheck = sum(&c);
	printf("CHECKSUM: %u\n", c.sumcheck);

	FILE *config_f = fopen(path, "wb");
	if(config_f == NULL)
	{
		perror("fopen");
		return 0;
	}
	fwrite(&c, sizeof(struct creds), 1, config_f);
	fclose(config_f);

	return 1;
}

int get_credentials(char *path, struct creds *c)
{
	FILE *config_f = fopen(path, "rb");
	if(config_f == NULL)
	{
		perror("fopen");
		return 0;
	}
	fread(c, sizeof(struct creds), 1, config_f);
	fclose(config_f);

	if(!check_config_sum(c))
	{
		printf("ERROR parsing credentials, please run <init>.\n");
		return 0;
	}

	return 1;
}
