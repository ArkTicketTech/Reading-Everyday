一个示例程序。

- 功能：从标准输入读取文本，并对其进行修改，然后把它写到标准输出中。
- 输入数据：
	- 列范围，如：4 9 12 20 -1，这列数据的意思是，从第4列读到第9列，然后从第12列读到第20列，-1表示结束
	- 输入字符串：即你想处理的字符串
- 输出数据：
	- 处理后的字符串

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_COLS 20
#define MAX_INPUT 200

int read_column_numbers(int columns[], int max);
void rearrange(char *output, char const *input, int n_columns, int const columns[]);

int main(void)
{
	int n_columns;
	int columns[MAX_COLS];
	char input[MAX_INPUT];
	char output[MAX_INPUT];

	n_columns = read_column_numbers(columns, MAX_COLS);

	while ( gets(input) != NULL ) {
        printf("Original input: %s\n", input);
        rearrange(output, input, n_columns, columns);
        printf("Rearrange line: %s\n", output);
	}

    return EXIT_SUCCESS;
}

int read_column_numbers(int columns[], int max)
{
    int num = 0;
    int ch;

    while (num < max && scanf("%d", &columns[num]) == 1 && columns[num] >= 0)
        num += 1;

    if (num %2 != 0) {
        puts("Last column number is not paired.");
        exit(EXIT_FAILURE);
    }

    while (ch = getchar() != EOF && ch != '\n')
    ;
        
    return num; 
}

void rearrange(char *output, char const *input, int n_columns, int const columns[])
{
    int col;
    int output_col;
    int len;

    len = strlen( input );
    output_col = 0;

    for ( col = 0; col < n_columns; col += 2) {
        int nchars = columns[col + 1] - columns[col] + 1;

        if ( columns[col] >= len || output_col == MAX_INPUT - 1 )
            break;

        if ( output_col + nchars > MAX_INPUT - 1 )
            nchars = MAX_INPUT - output_col - 1;

        strncpy( output + output_col, input + columns[col], nchars );
        output_col += nchars;
    }
}
```
