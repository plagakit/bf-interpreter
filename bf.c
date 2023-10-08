#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    uint16_t dp;
    uint8_t* memory;
} BF;

BF createBF() 
{
    BF bf;
    bf.dp = 0;

    bf.memory = (uint8_t*)calloc(UINT16_MAX, sizeof(uint8_t));
    if (bf.memory == NULL) 
    {
        fprintf(stderr, "Not enough memory!\n");
        exit(1);
    }
    
    return bf;
}

void freeBF(BF* bf) 
{
    free(bf->memory);
}

void printData(BF* bf, uint16_t to) 
{
    for (uint16_t i = 0; i <= to; i++)
        printf("%d ", bf->memory[i]);
}

uint8_t getByteInput() 
{
    printf("==> ");

    uint8_t byte;
    while ((byte = getchar()) == '\n'); // make sure byte isnt newline from stdin

    return byte;
}

void run(BF* bf, const char* line) 
{
    bool printed = false;
    for (char* ip = (char*)line; *ip != '\0'; ip++) 
    {
        switch (*ip) 
        {
            case '>': bf->dp++; break;
            case '<': bf->dp--; break;
            case '+': bf->memory[bf->dp]++; break;
            case '-': bf->memory[bf->dp]--; break;

            case '.':
                printf("%c", bf->memory[bf->dp]); 
                printed = true;
                break;

            case ',': 
                bf->memory[bf->dp] = getByteInput();
                break;

            case '[': {
                if (bf->memory[bf->dp] == 0) 
                {
                    int nestLevel = 0;
                    for (char* s = ip + 1; *s != '\0'; s++) 
                    {
                        if (*s == '[')
                            nestLevel++;
                        else if (*s == ']') 
                        {
                            if (nestLevel == 0) 
                            {
                                ip = s;
                                break;
                            } 
                            else
                                nestLevel--;
                        }
                    }
                }
                break;
            }

            case ']': {
                if (bf->memory[bf->dp] != 0) 
                {
                    int nestLevel = 0;
                    for (char* s = ip - 1; s >= line; s--) 
                    {
                        if (*s == ']')
                            nestLevel++;
                        else if (*s == '[') 
                        {
                            if (nestLevel == 0) 
                            {
                                ip = s;
                                break;
                            } 
                            else
                                nestLevel--;
                        }
                    }
                }
                break;
            }
            
            // Custom characters added by me 
            case 'X': freeBF(bf); exit(0);
            case '|': printf("%d", bf->memory[bf->dp]); printed = true; break;
            case '#': printData(bf, bf->dp); printed = true; break;
        }
    }

    if (printed)
        printf("\n");
}

void runFile(const char* path)
{
	// Open the file
	FILE* file = fopen(path, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "Could not open file \"%s\".\n", path);
		exit(1);
	}

	// Find the end of the file to get size
	fseek(file, 0L, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	// Create buffer
	char* buffer = (char*)malloc(fileSize + 1); // + 1 for null char
	if (buffer == NULL)
	{
		fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
		exit(1);
	}

    // Write file to buffer
	size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
	if (bytesRead < fileSize)
	{
		fprintf(stderr, "Could not read file \"%s\".\n", path);
		exit(1);
	}
	buffer[bytesRead] = '\0';
	fclose(file);
	
    // Run buffer
    BF bf = createBF();
    run(&bf, (const char*)buffer);

    freeBF(&bf);
    free(buffer);
    exit(0);
}

void runLines() 
{
    BF bf = createBF();

    // REPL
    char line[1024];
    for (;;)
    {
        printf("> ");
   
        // Write input to buffer
        if (!fgets(line, sizeof(line), stdin)) 
        {
            freeBF(&bf);
            exit(0);
        }

        // Run input
        run(&bf, line);

        // Flush input
        int flush;
        while ((flush = getchar()) != '\n' && flush != EOF);
    }
}

int main(int argc, const char* argv[]) 
{
    if (argc == 1)
        runLines();
    else if (argc == 2)
        runFile(argv[1]);
    else 
    {
        printf("Usage: bf [file]");
        exit(1);
    }
}