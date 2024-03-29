#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "instructions.h"

#define N_INSTRUCTIONS 33


#define MAKE_REG(x) x/* | 0x80000000*/
#define MAKE_IMM(x) x/* | 0x40000000*/
#define MAKE_VAL(x) x/* | 0x20000000*/

#define OP2REG(x) x & 0x01ff
#define OP2IMM(x) x & 0xffff
#define OP2VAL(x) x & 0xffffff


typedef struct {
    const char* opname;
    enum {R,I,J} format;
    uint8_t opcode;
    int n_operans;
} Icode;

Icode itable[N_INSTRUCTIONS] =  {
    {"J",   J,0x02,1},
    {"JAL", J,0x03,1},
    {"SLL", R,0x04,3},
    {"SRL", R,0x06,3},
    {"SRA", R,0x07,3},
    {"ADD", R,0x20,3},
    {"AND", R,0x24,3},
    {"OR",  R,0x25,3},
    {"SUB", R,0x22,3},
    {"XOR", R,0x26,3},
    {"SLE", R,0x2c,3},
    {"SEQ", R,0x28,3},
    {"SNE", R,0x29,3},
    {"SLT", R,0x2a,3},
    {"SUBI",I,0x0a,3},
    {"SW",  I,0x2b,3},
    {"XORI",I,0x0e,3},
    {"ADDI",I,0x08,3},
    {"ANDI",I,0x0c,3},
    {"LW",  I,0x23,3},
    {"ORI", I,0x0d,3},
    {"BEQZ",I,0x04,2},
    {"BNEZ",I,0x05,2},
    {"JALR",I,0x13,1},
    {"JR",  I,0x12,1},
    {"LHI", I,0x0f,1},
    {"SEQI",I,0x18,3},
    {"SLEI",I,0x1c,3},
    {"SLLI",I,0x14,3},
    {"SLTI",I,0x1a,3},
    {"SNEI",I,0x19,3},
    {"SRAI",I,0x17,3},
    {"SRLI",I,0x16,3},
};

int get_n_operands(Instruction i) {
    if(i.op1 == NO_OPERAND)
        return 0;
    if(i.op2 == NO_OPERAND)
        return 1;
    if(i.op3 == NO_OPERAND)
        return 2;
    return 3;
}



op_t make_op(int value, int typeflag, int line) {
    op_t op = value;
    
    switch(typeflag) {
        case OP_REG: 
            return MAKE_REG(op);

        case OP_IMM:
            return MAKE_IMM(op);
        case OP_VAL:
            // marche pas car op peut etre negatif...
            //if((op & ~0x03ffffff) != 0) {
            //    fprintf(stderr, "erreur a la ligne %d: valeur %d trop grande \n", line, op);
            //    return INVALID_OPERAND;
            //}
            return MAKE_VAL(op);

        default: 
            printf("que se passe-t-il ?? make_op(int value=%d,int typeflag=%d,int line=%d);\n", value,typeflag,line);
            return INVALID_OPERAND;
    }
}


Instruction normalize(Instruction i) {
    char buffer[512];

    strncpy(buffer, i.opname, 511);
    
    for(char* ptr = buffer; *ptr != '\0'; ++ptr)
        *ptr = toupper(*ptr);

    Instruction ret = {
        .opname = buffer,
        .op1    = i.op1,
        .op2    = i.op2,
        .op3    = i.op3,
    };


    return ret;
}

Icode* find(Instruction in) {
    Instruction normalised = normalize(in);


    for(int i = 0; i <= N_INSTRUCTIONS; ++i) {
        
        Icode* it = &itable[i];

        if(!strcmp(it->opname, normalised.opname)) {
            return it;
        }
    }
    // instruction invalide !
    return NULL;
}

/*

I           [31:26]	    I[25:21]	I[20:16]	I[15:11]	I[10:6]	    I[5:0]
R	        0x0	        Rs1	        Rs2	        Rd	        Inutilisé	Opcode
I	        Opcode	    Rs1	        Rd          immediate	immediate	immediate
J	        Opcode	    value	    value	    value	    value       value

*/

instruction_t buildRinstruction(uint32_t opcode, uint32_t Rd,uint32_t Rs1,uint32_t Rs2) {
    instruction_t ret = opcode | (Rd << 11) | (Rs2 << 16) | (Rs1 << 21);
    return ret;
}
instruction_t buildIinstruction(uint32_t opcode, uint32_t Rd,uint32_t Rs1, uint32_t imm) {
    instruction_t ret = (imm) | (Rd << 16) | (Rs1 << 21) | (opcode << 26);
    return ret;
}
instruction_t buildJinstruction(uint32_t opcode, uint32_t val) {
    instruction_t ret = (val) | (opcode << 26);
    return ret;
}

instruction_t convert(Instruction in) {
    Icode* icode = find(in);

//    printf("%d\t%s %d %d %d --> %s(%.2x): ", in.line,in.opname,in.op1,in.op2,in.op3,        icode->opname, icode->opcode);

    if(!icode) {
        fprintf(stderr, "erreur a la ligne %d: operation '%s' inconnue\n", in.line, in.opname);

        return INVALID_INSTRUCTION;
    }

    int n_operands = get_n_operands(in);
    

    if(icode->n_operans != n_operands) {
        fprintf(stderr, "erreur a la ligne %d: %s requiert %d operandes (%d donnees).\n", in.line, icode->opname, icode->n_operans, n_operands);

        return INVALID_INSTRUCTION;
    }

    switch(icode->format) {
        case R:
            return buildRinstruction(icode->opcode, OP2REG(in.op1),OP2REG(in.op2),OP2REG(in.op3));
        case I:
            switch(n_operands) {
                case 3:
                    return buildIinstruction(icode->opcode, OP2REG(in.op1),OP2REG(in.op2),OP2IMM(in.op3));
                case 2:
                    return buildIinstruction(icode->opcode, 0, OP2REG(in.op1),OP2IMM(in.op2));
                case 1:
                    if(!strcmp(icode->opname, "LHI"))
                        return buildIinstruction(icode->opcode, 0, 0,OP2IMM(in.op1));
                    else
                        return buildIinstruction(icode->opcode, 0, OP2IMM(in.op1),0);
                default:
                    assert(0);
            }
        case J:
            return buildJinstruction(icode->opcode, OP2VAL(in.op1));
    }
}
