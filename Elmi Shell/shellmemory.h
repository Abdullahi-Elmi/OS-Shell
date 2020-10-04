//Abdullahi Elmi
//260727124

typedef struct ShellMemory {
	char *var;
	char *value;
	struct ShellMemory *next;
} ShellMemory;

void initShellMemory();
void clearShellMemory();
char *get(const char *var);
void add(const char *var, const char *value);
ShellMemory *find(const char *key);
