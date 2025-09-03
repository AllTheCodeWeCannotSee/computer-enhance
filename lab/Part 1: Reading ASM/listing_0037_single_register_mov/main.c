#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // 为了使用 isprint()

// 定义每次从文件中读取的字节数（通常是16的倍数）
#define CHUNK_SIZE 16

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "用法: %s <文件名>\n", argv[0]);
        exit(1);
    }

    const char *filename = argv[1];
    FILE *fp;

    // 1. 使用 "rb" 模式打开二进制文件
    fp = fopen(filename, "rb");

    if (fp == NULL) {
        perror("错误: 无法打开文件");
        exit(1);
    }

    // 使用 unsigned char 是处理原始字节的最佳实践
    unsigned char buffer[CHUNK_SIZE];
    size_t bytes_read = 0; // fread 返回成功读取的元素个数
    unsigned long offset = 0; // 记录当前在文件中的偏移地址

    // 2. 使用 fread 循环读取
    // fread 会返回成功读取的“块”的数量。只要它大于0，就继续循环。
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, fp)) > 0) {
        // 打印地址偏移量 (8位十六进制，用0补齐)
        printf("%08lX: ", offset);

        // 3. 以十六进制格式打印字节
        for (int i = 0; i < CHUNK_SIZE; i++) {
            if (i < bytes_read) {
                // 打印每个字节 (2位十六进制，用0补齐，大写)
                printf("%02X ", buffer[i]);
            } else {
                // 如果最后一块不足 CHUNK_SIZE，用空格填充
                printf("   ");
            }
            // 每8个字节后增加一个额外空格，更易读
            if (i == 7) {
                printf(" ");
            }
        }

        printf(" |"); // 分隔符

        // 4. (可选) 打印可打印的 ASCII 字符
        for (int i = 0; i < bytes_read; i++) {
            // isprint() 检查字符是否可打印
            if (isprint(buffer[i])) {
                printf("%c", buffer[i]);
            } else {
                printf("."); // 不可打印字符用点(.)代替
            }
        }

        printf("\n"); // 换行
        offset += bytes_read; // 更新地址偏移量
    }

    // 5. 关闭文件
    fclose(fp);

    return 0;
}