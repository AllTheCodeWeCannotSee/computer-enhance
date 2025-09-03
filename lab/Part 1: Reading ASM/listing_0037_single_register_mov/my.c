
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE  2
const char* registers_8bit[8] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char* registers_16bit[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};

typedef struct {
    char op[10];
    char op1[10];
    char op2[10];
} Instruction;

const char * get_op(short int bits_16) {
    unsigned char byte1 = (bits_16 >> 8) & 0xFF;
    
    switch (byte1) {
        case 0b10001000:
        case 0b10001001:
        case 0b10001010:
        case 0b10001011:
            return "mov";
        default:
            return "unknown";
    }
}

const char* get_op1(short int bits_16) {
    unsigned char byte1 = (bits_16 >> 8) & 0b11111111;
    unsigned char byte2 = bits_16 & 0b11111111;
    unsigned char d = byte1 >> 1 & 0b1;
    unsigned char w = byte1 & 0b1;
    int reg = (byte2 >> 3) & 0b111;
    int rm = byte2 & 0b111;

    char* op1;
    // return reg
    if (d == 0b1) {
        // 8bit
        if (w == 0b0) {
            op1 = registers_8bit[reg];
        }
        //16bit
        if (w == 0b1) {
            op1 = registers_16bit[reg];
        }

    }
    // return rm
    if (d == 0b0) {
        // 8bit
        if (w == 0b0) {
            op1 = registers_8bit[rm];
        }
        //16bit
        if (w == 0b1) {
            op1 = registers_16bit[rm];
        }
    }
    return op1;
}

const char* get_op2(short int bits_16) {
    unsigned char byte1 = (bits_16 >> 8) & 0b11111111;
    unsigned char byte2 = bits_16 & 0b11111111;
    unsigned char d = byte1 >> 1 & 0b1;
    unsigned char w = byte1 & 0b1;
    int reg = (byte2 >> 3) & 0b111;
    int rm = byte2 & 0b111;

    char* op2;
    // return reg
    if (d == 0b1) {
        // 8bit
        if (w == 0b0) {
            op2 = registers_8bit[rm];
        }
        //16bit
        if (w == 0b1) {
            op2 = registers_16bit[rm];
        }

    }
    // return rm
    if (d == 0b0) {
        // 8bit
        if (w == 0b0) {
            op2 = registers_8bit[reg];
        }
        //16bit
        if (w == 0b1) {
            op2 = registers_16bit[reg];
        }
    }
    return op2;
}


Instruction decode(short int bits_16) {
    Instruction res = {0};
    // 1. op
    const char* op = get_op(bits_16);
    // 2. op1
    const char* op1 = get_op1(bits_16);
    // 3. op2
    const char* op2 = get_op2(bits_16);
    // 返回结果
    strcpy(res.op, op);
    strcpy(res.op1, op1);
    strcpy(res.op2, op2);
    return res;
}

void bits_to_print(short int bits_16) {
    Instruction res = decode(bits_16);
    printf("%s %s, %s\n", &res.op, &res.op1, &res.op2);
}

int main(int argc, char *argv[]) {
    // 1. handle arg error
    if (argc < 2) {
        fprintf(stderr, "%s need 2 args\n", argv[0]);
        exit(1);
    }
    // 2. read file
    const char *filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("can't open file");
        exit(1);
    }
    // 3. 读一行
    unsigned char buffer[CHUNK_SIZE];
    while (fread(buffer, 1, CHUNK_SIZE, fp) > 0) {
        // 大端
        short int big_endian_val = (short int) (buffer[1] | (buffer[0] << 8));
        bits_to_print(big_endian_val);
    }
    return 0;
}