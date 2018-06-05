#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<math.h>

int substr_count(char *str, const char *sub_str)
{
	if (str == NULL || sub_str == NULL) {
		printf("ERROR: [substr_count] parameter(s) should not be NULL!\n");
		return -1;
	}

	int n = 0;
	char *begin_ptr = str;
	char *end_ptr = NULL;

	while ((end_ptr = strstr(begin_ptr, sub_str)) != NULL) {
		end_ptr += strlen(sub_str);
		begin_ptr = end_ptr;
		n++;
	}

	return n;
}

int str_replace(const char *file_path, const char *new_str, const char *old_str)
{
	if (file_path == NULL || new_str == NULL || old_str == NULL) {
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
	ftruncate(fd, 0);

	/* count the sub-string number */
	int n = substr_count(ori_str, old_str);
	if (n == -1) {
		printf("ERROR: substring count error!\n");
		close(fd);
		free(ori_str);
		return -1;
	}

	/* malloc memory for rst_str */
	int rst_str_len = file_len + n * abs(strlen(new_str) - strlen(old_str)) + 1;
	char *rst_str = (char *)malloc(rst_str_len);
	if (rst_str == NULL) {
		printf("ERROR: malloc rst_str failed!\n");
		close(fd);
		free(ori_str);
		return -1;
	}
	memset(rst_str, 0, rst_str_len);

	char *cpy_str = rst_str;
	char *begin_ptr = ori_str;
	char *end_ptr;

	while ((end_ptr = strstr(begin_ptr, old_str)) != NULL) {
		memcpy(cpy_str, begin_ptr, end_ptr - begin_ptr);
		cpy_str += (end_ptr - begin_ptr);
		memcpy(cpy_str, new_str, strlen(new_str));
		cpy_str += strlen(new_str);
		end_ptr += strlen(old_str);
		begin_ptr = end_ptr;
	}
	strcpy(cpy_str, begin_ptr);

	printf("ori = %s\nrst = %s\n", ori_str, rst_str);

	lseek(fd, 0, SEEK_SET);
	ret = write(fd, rst_str, strlen(rst_str));
	if (ret != strlen(rst_str)) {
		printf("ERROR: write error!\n");
		close(fd);
		free(ori_str);
		free(rst_str);
		return -1;
	}

	free(rst_str);
	free(ori_str);
	close(fd);
	return 0;
}

int main()
{
	char *new_str = "happy";
	char *old_str = "scream";
	char *file_path = "./test.txt";

	if (str_replace(file_path, new_str, old_str) != 0) {
		printf("INFO: string replace failed!\n");
		return -1;
	}

	return 0;
}

