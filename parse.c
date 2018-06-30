#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<math.h>

#define KEYLEN 20
#define VALUELEN 100

int config_read(char *config_str, int len)
{
	char *key_word = "option";
	char *ptr = NULL;
	char *t = strtok(config_str, "\r\n");
	char key[KEYLEN] = {0};
	char value[VALUELEN] = {0};
	int i = 0;
	
	
	while (t != NULL) {
		/* ignore comment */
		if (strstr(t, "#") != NULL) {
			printf("ignore comment. [%s]\n", t);
			t = strtok(NULL, "\r\n");
			continue;
		}
		
		if ((ptr = strstr(t, key_word)) != NULL) {
			ptr += strlen(key_word);
			
			/* skip blank characters */
			while (*ptr == ' ' || *ptr == '\t')
				ptr++;
			
			/* get key */
			i = 0;
			memset(key, 0, KEYLEN);
			while (*ptr != ' ' && *ptr != '\t') {
				key[i] = *ptr;
				i++;
				ptr++;
			}
			
			/* skip blank characters */
			while (*ptr == ' ' || *ptr == '\t')
				ptr++;
			
			/* get value */
			i = 0;
			memset(value, 0, VALUELEN);
			while (*ptr != ' ' && *ptr != '\t') {
				value[i] = *ptr;
				i++;
				ptr++;
			}
			
			printf("key = %s, value = %s\n", key, value);
		}
		
		t = strtok(NULL, "\r\n");
	}
	return 0;
}

int str_handle(const char *file_path)
{
	if (file_path == NULL) {
		printf("ERROR: [str_replace] parameter(s) should not be NULL!\n");
		return -1;
	}

	int fd = open(file_path, O_RDWR);
	if (fd == -1) {
		printf("ERROR: file open error!\n");
		return -1;
	}

	/* file size */
	int file_len = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	/* malloc memory for ori_str */
	char *ori_str = (char *)malloc(file_len + 1);
	if (ori_str == NULL) {
		printf("ERROR: malloc ori_str failed!\n");
		close(fd);
		return -1;
	}
	memset(ori_str, 0, file_len + 1);
	
	/* begin to read file */
	ssize_t ret = read(fd, ori_str, file_len);
	if (ret != file_len) {
		printf("ERROR: read file error!\n");
		close(fd);
		free(ori_str);
		return -1;
	}
	//ftruncate(fd, 0);

	config_read(ori_str, strlen(ori_str));

	lseek(fd, 0, SEEK_SET);


	free(ori_str);
	close(fd);
	return 0;
}

int main()
{
	char *file_path = "configuration.txt";

	if (str_handle(file_path) != 0) {
		printf("INFO: string handle failed!\n");
		return -1;
	}

	return 0;
}

