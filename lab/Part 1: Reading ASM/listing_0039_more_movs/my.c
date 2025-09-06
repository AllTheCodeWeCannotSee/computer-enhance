
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const char* registers_8bit[8] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
const char* registers_16bit[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char* mov_mod_00[8] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};

typedef struct {
    char op[50];
    char op1[50];
    char op2[50];
    int is_valid;
} Instruction;

int get_reg_int(unsigned char D, unsigned char REG, unsigned char RM) {
    int reg_int;
    if (D == 0b0) {
        reg_int = RM & 0b111;
    }  else if (D == 0b1) {
        reg_int = REG & 0b111;
    } 
    return reg_int;
}

int get_rm_int(unsigned char D, unsigned char REG, unsigned char RM) {
    int rm_int;
    if (D == 0b0 ) {
        rm_int = REG & 0b111;
    }  else if (D == 0b1) {
        rm_int = RM & 0b111;
    } 
    return rm_int;
}

char* get_reg_char(unsigned char D, unsigned char W, unsigned char REG, unsigned char RM) {
    int reg_index = get_reg_int(D, REG, RM);
    char *reg_char;
    if (W == 0b0) {
        reg_char = registers_8bit[reg_index];
    } else if (W == 0b1) {
        reg_char = registers_16bit[reg_index];
    }
    return reg_char;
}

char* get_rm_char(unsigned char D, unsigned char W, unsigned char REG, unsigned char RM) {
    int rm_index = get_rm_int(D, REG, RM);
    char *rm_char;
    if (W == 0b0) {
        rm_char = registers_8bit[rm_index];
    } else if (W == 0b1) {
        rm_char = registers_16bit[rm_index];
    }
    return rm_char;
}

// 打印一个字节的二进制
void print_bits(unsigned char byte) {
    for (int i = 7; i >= 0; i -- ) {
        printf("%d", (byte >> i) & 0b1);
    }
    printf("\n");
}

void decode_1000010_mod_00(Instruction *inst_ptr, unsigned char D, unsigned char W, unsigned char REG, unsigned char RM) {
    int rm_int = RM & 0b111;
    int reg_int = REG & 0b111;
    char * rm_char = mov_mod_00[rm_int];
    char final_rm_char[50];
    snprintf(final_rm_char, sizeof(final_rm_char), "[%s]", rm_char);

    // REG
    char * reg_char;
    if (W == 0) {
        reg_char = registers_8bit[reg_int];
    } else if (W == 1) {
        reg_char = registers_16bit[reg_int];
    }
    if (D == 0) {
        // to rm
        strcpy(inst_ptr->op1, final_rm_char);
        strcpy(inst_ptr->op2, reg_char);
    } else if (D == 1) {
        // to reg
        strcpy(inst_ptr->op1, reg_char);
        strcpy(inst_ptr->op2, final_rm_char);
    }
    return ;
}

void decode_1000010_mod_01(FILE * fp ,Instruction *inst_ptr, unsigned char D, unsigned char W, unsigned char REG, unsigned char RM) {
    // 读指令的第3个字节
    unsigned char third; fread(&third, 1, 1, fp);
    int num = third & 0b11111111;

    int rm_int = RM & 0b111;
    int reg_int = REG & 0b111;
    char * rm_char = mov_mod_00[rm_int];
    char final_rm_char[50];
    snprintf(final_rm_char, sizeof(final_rm_char), "[%s + %d]", rm_char, num);

    // REG
    char * reg_char;
    if (W == 0) {
        reg_char = registers_8bit[reg_int];
    } else if (W == 1) {
        reg_char = registers_16bit[reg_int];
    }
    if (D == 0) {
        // to rm
        strcpy(inst_ptr->op1, final_rm_char);
        strcpy(inst_ptr->op2, reg_char);
    } else if (D == 1) {
        // to reg
        strcpy(inst_ptr->op1, reg_char);
        strcpy(inst_ptr->op2, final_rm_char);
    }
    return ;

}

void decode_1000010_mod_10(FILE * fp, Instruction *inst_ptr, unsigned char D, unsigned char W, unsigned char REG, unsigned char RM) {
    // 读指令的第3个字节
    unsigned char third; fread(&third, 1, 1, fp);
    // 读指令的第4个字节
    unsigned char fourth; fread(&fourth, 1, 1, fp);
    int num = third | (fourth << 8);

    int rm_int = RM & 0b111;
    int reg_int = REG & 0b111;
    char * rm_char = mov_mod_00[rm_int];
    char final_rm_char[50];
    snprintf(final_rm_char, sizeof(final_rm_char), "[%s + %d]", rm_char, num);

    // REG
    char * reg_char;
    if (W == 0) {
        reg_char = registers_8bit[reg_int];
    } else if (W == 1) {
        reg_char = registers_16bit[reg_int];
    }
    if (D == 0) {
        // to rm
        strcpy(inst_ptr->op1, final_rm_char);
        strcpy(inst_ptr->op2, reg_char);
    } else if (D == 1) {
        // to reg
        strcpy(inst_ptr->op1, reg_char);
        strcpy(inst_ptr->op2, final_rm_char);
    }
    return ;
}

void decode_1000010_mod_11(Instruction *inst_ptr, unsigned char D, unsigned char W, unsigned char REG, unsigned char RM) {
    int reg_index, rm_index;
    char * reg_char = get_reg_char(D, W, REG, RM);
    char * rm_char = get_rm_char(D, W, REG, RM);
    strcpy(inst_ptr->op1, reg_char);
    strcpy(inst_ptr->op2, rm_char);
    return ;
}



void decode_1000010(Instruction *inst_ptr, unsigned char first, FILE *fp) {
    // 确定 op = mov
    strcpy(inst_ptr->op, "mov");
    // 读指令的第二个字节
    unsigned char second;
    fread(&second, 1, 1, fp);
    // D, W, MOD, REG, R/M
    unsigned char D = (first >> 1) & 0b1;
    unsigned char W = first & 0b1;
    unsigned char MOD = (second >> 6) & 0b11;
    unsigned char REG = (second >> 3) & 0b111;
    unsigned char RM = second & 0b111;
    // 根据 MOD 解析指令
    switch(MOD) {
        case 0b00: decode_1000010_mod_00(inst_ptr, D, W, REG, RM); break;
        case 0b01: decode_1000010_mod_01(fp, inst_ptr, D, W, REG, RM); break;
        case 0b10: decode_1000010_mod_10(fp, inst_ptr, D, W, REG, RM); break;
        case 0b11: decode_1000010_mod_11(inst_ptr, D, W, REG, RM); break;
    }
}


void decode_1011(Instruction *inst_ptr, unsigned char first, FILE * fp) {
    // 确定 op = mov
    strcpy(inst_ptr->op, "mov");
    unsigned char W = (first >> 3) & 0b1;
    unsigned char REG = first & 0b111;
    int reg_int = REG & 0b111;


    if (W == 0) {
        unsigned char second;
        fread(&second, 1, 1, fp);

        int num = second & 0b11111111;
        char imm[50];
        snprintf(imm, sizeof(imm), "%d", num);
        strcpy(inst_ptr->op1, registers_8bit[reg_int]);
        strcpy(inst_ptr->op2, imm);

    } else if (W == 1) {
        unsigned char second;
        fread(&second, 1, 1, fp);
        unsigned char third;
        fread(&third, 1, 1, fp);

        int num = second | (third << 8);
        char imm[50];
        snprintf(imm, sizeof(imm), "%d", num);
        strcpy(inst_ptr->op1, registers_16bit[reg_int]);
        strcpy(inst_ptr->op2, imm);
    }
}


Instruction decode_one_instruction(FILE *fp) {
    Instruction inst = {0};
    // 1. 读第一个字节
    unsigned char first;
    // EOF
    if (fread(&first, 1, 1, fp) == 0) {
        inst.is_valid = 0;
        return inst;
    }
    // valid
    inst.is_valid = 1;
    // 解析
    if ((first >> 2) == 0b100010) {
        // 100010 mov
        decode_1000010(&inst, first, fp);
        return inst;
    } 
    if ((first >> 4) == 0b1011) {
        // 1011 mov
        decode_1011(&inst, first, fp);
        return inst;
    }
}

void print_instruction(Instruction * decoded_ptr) {
    if (decoded_ptr->is_valid == 1) {
        printf("%s %s, %s\n", decoded_ptr->op, decoded_ptr->op1, decoded_ptr->op2);
    }
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
    // 3. 循环读取指令
    while (!feof(fp)) {
        Instruction decoded = decode_one_instruction(fp);
        print_instruction(&decoded);
    }
    return 0;
}