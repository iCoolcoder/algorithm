
#include <string.h>
#include <algorithm>
#include <stdlib.h>

#pragma warning(disable : 4996)
#define LEN_LINE 80

int cmp(const void *str1, const void *str2)
{
	const char *p1 = (char *)str1, *p2 = (char *)str2;
	return strcmp(p1, p2);
}

bool FileSort(const char *file, size_t memLimit)
{
	FILE* fp;
	if ((fp = fopen(file, "r")) == NULL)
	{
		return false;
	}
	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	size_t split_cnt = (file_size + memLimit - 1) / memLimit;
	char prefix[256] = "tttt";
	char file_t[256];
	
	int fileNo;
	char *buff = new char[memLimit];
	if (NULL == buff)
		return false;

	size_t totalReadLen = 0; // already read
	size_t lineCnt = memLimit/LEN_LINE; // capability of lines of block
	fileNo = 1;

	// split into files and sort
	while (totalReadLen < file_size)
	{
		// create a new temp file
		sprintf(file_t, "%s%d.txt", prefix, fileNo);
		FILE *fp_t = fopen(file_t, "w");
		size_t read_len = 0;
		int i = 0;

		// read by lines
		while ((i < lineCnt) && fgets(buff + read_len, LEN_LINE, fp))
		{
			i++;
			totalReadLen += strlen(buff + read_len) + 1; // inlcude \n
			read_len += LEN_LINE;
		}
		// sort
		qsort(buff, i, LEN_LINE, cmp);
		int j = 0;
		read_len = 0;
		// write by lines
		while (j < i)
		{
			fputs(buff + read_len, fp_t);
			j++;
			read_len += LEN_LINE;
		}
		fclose(fp_t);
		fileNo++;
	}
	fclose(fp);

	fp = fopen(file, "w");
	FILE **fp_t = (FILE **)malloc(sizeof(FILE *) * fileNo);
	bool *flg = new bool[fileNo]; 

	fileNo--;
	for (int i = 1; i <= fileNo; i++)
	{
		sprintf(file_t, "%s%d.txt", prefix, i);
		fp_t[i-1] = fopen(file_t, "r");
		flg[i-1] = true;
	}

	// merge
	while (1)
	{
		// read a line
		for (int i = 0; i < fileNo; i++)
		{
			if (flg[i])
			{
				if (fp_t[i])
				{
					if (!fgets(buff + i * LEN_LINE, LEN_LINE, fp_t[i]))
					{
						fclose(fp_t[i]);
						sprintf(file_t, "%s%d.txt", prefix, i);
						remove(file_t);
						buff[i * LEN_LINE] = 0;
						fp_t[i] = NULL;
					}
				}
				flg[i] = false;
			}
		}

		int min = 0;

		// find minest line
		for (int i = 1; i < fileNo; i++)
		{
			if (buff[min*LEN_LINE] == 0)
                min = i;
            else if (buff[i*LEN_LINE] && strcmp(buff+min*LEN_LINE, buff+i*LEN_LINE)>0)
                min = i;
		}

		if (buff[min*LEN_LINE])
        {
            fputs(buff+min*LEN_LINE, fp);
            flg[min] = true;
        }
        else
            break;  
	}
	fclose(fp);
	return true;
}

int main(int argc)
{
	FileSort("C:\\sort\\sort.txt", 1234);
	return 0;
}

