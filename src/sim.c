#include <stdio.h>
#include "shell.h"
void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
    bool Update_flag = FALSE;
    uint32_t opcode = (instruction & 0xFC000000) >> 26;
    if (opcode == 0) {
        uint32_t funct = instruction & 0x3F;
        uint32_t rs = (instruction & 0x03E00000) >> 21;
        uint32_t rt = (instruction & 0x001F0000) >> 16;
        uint32_t rd = (instruction & 0x0000F800) >> 11;
        uint32_t shamt = (instruction & 0x000007C0) >> 6;

        switch (funct) {
            uint32_t HI, LO;
            case 0x00: // SLL
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
                break;
            case 0x02: // SRL
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
                break;
            case 0x03: // SRA
                NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> shamt;
                break;
            case 0x04: // SLLV
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
                break;
            case 0x06: // SRLV
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                break;
            case 0x07: // SRAV
                NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
                break;
            case 0x08: // JR
                NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                break;
            case 0x09: // JALR
                NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
                NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                break;
            case 0x20: // ADD
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                break;
            case 0x21: // ADDU
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                break;      
            case 0x22: // SUB
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                break;
            case 0x23: // SUBU
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                break;
            case 0x24: // AND
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
                break;
            case 0x25: // OR
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
                break;
            case 0x26: // XOR
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
                break;
            case 0x27: // NOR
                NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
                break;
            case 0x2A: // SLT
                NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rs] < (int32_t)CURRENT_STATE.REGS[rt];
                break;
            case 0x2B: // SLTU
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt];
                break;
            case 0x18: // MULT
                NEXT_STATE.HI = ((int64_t)CURRENT_STATE.REGS[rs] * (int64_t)CURRENT_STATE.REGS[rt]) >> 32;
                NEXT_STATE.LO = ((int64_t)CURRENT_STATE.REGS[rs] * (int64_t)CURRENT_STATE.REGS[rt]) & 0xFFFFFFFF;
                break;
            case 0x19: // MULTU
                NEXT_STATE.HI = ((uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt]) >> 32;
                NEXT_STATE.LO = ((uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt]) & 0xFFFFFFFF;
                break;
            case 0x1A: // DIV
                if (CURRENT_STATE.REGS[rt] != 0) {
                    NEXT_STATE.LO = (int32_t)CURRENT_STATE.REGS[rs] / (int32_t)CURRENT_STATE.REGS[rt];
                    NEXT_STATE.HI = (int32_t)CURRENT_STATE.REGS[rs] % (int32_t)CURRENT_STATE.REGS[rt];
                } else {
                    printf("Error: division by zero\n");
                }
                break;
            case 0x1B: // DIVU
                if (CURRENT_STATE.REGS[rt] != 0) {
                    NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
                    NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
                } else {
                    printf("Error: division by zero\n");
                }
                break;
            case 0x10: // MFHI
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[HI];
                break;
            case 0x12: // MFLO
                NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[LO];
                break;
            case 0x11: // MTHI
                NEXT_STATE.REGS[HI] = CURRENT_STATE.REGS[rs];
                break;
            case 0x13: // MTLO
                NEXT_STATE.REGS[LO] = CURRENT_STATE.REGS[rs];
                break;
            default:
                printf("Invalid instruction at %08x\n", CURRENT_STATE.PC);
                break;
        }
    } 
    else if (opcode == 2 || opcode == 3) {
        uint32_t address = instruction & 0x03FFFFFF;
        uint32_t targetAddress = (CURRENT_STATE.PC & 0xF0000000) | (address << 2);
        int addressBitCount = 0;
        int powerOf10 = 1;
        while (targetAddress) {
            addressBitCount++;
            targetAddress /= 10;
            powerOf10  = powerOf10 * 10;
        }
        targetAddress = (CURRENT_STATE.PC & 0xF0000000) | (address << 2);
        switch (opcode) {
            case 2: // J
                NEXT_STATE.PC = CURRENT_STATE.PC/powerOf10 *powerOf10 + targetAddress;
                Update_flag = TRUE;
                break;
            case 3: // JAL
                NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                NEXT_STATE.PC = CURRENT_STATE.PC/powerOf10 *powerOf10 + targetAddress;
                Update_flag = TRUE;
                break;
            default:
                printf("Invalid instruction at %08x\n", CURRENT_STATE.PC);
                break;
        }
    }
    else {
        uint32_t rs = (instruction & 0x03E00000) >> 21;
        uint32_t rt = (instruction & 0x001F0000) >> 16;
        int16_t imm = instruction & 0xFFFF;
        uint32_t address = CURRENT_STATE.REGS[rs] + (int32_t)imm;
        switch (opcode) {
            case 0x1: // BGEZ or BLTZAL
                if(rt == 0x1) {
                    if((int32_t)CURRENT_STATE.REGS[rs] >= 0) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
                    }
                } else if(rt == 0x10) { // BLTZAL
                    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                    if((int32_t)CURRENT_STATE.REGS[rs] < 0) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
                    }
                }
                break;
            case 0x4: // BEQ
                if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]){
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
                    Update_flag=TRUE;
                }
                break;
            case 0x5: // BNE
                if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]){
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
                    Update_flag = TRUE;
                }
                break;
            case 0x6: // BLEZ
                if((int32_t)CURRENT_STATE.REGS[rs] <= 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
                }
                break;
            case 0x7: // BGTZ
                if((int32_t)CURRENT_STATE.REGS[rs] > 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
                }
                break;
            case 0x8: // ADDI
                NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
                break;
            case 0x9: // ADDIU
                NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + (uint32_t)imm;
                break;
            case 0xA: // SLTI
                NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] < (int32_t)imm ? 1 : 0;
                break;
            case 0xB: // SLTIU
                NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < (uint32_t)imm ? 1 : 0;
                break;
            case 0xC: // ANDI
                NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & (uint32_t)imm;
                break;
            case 0xD: // ORI
                NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | (uint32_t)imm;
                break;
            case 0xE: // XORI
                NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ (uint32_t)imm;
                break;
            case 0xF: // LUI
                NEXT_STATE.REGS[rt] = (uint32_t)imm << 16;
                break;
            case 0x20: // LB
                NEXT_STATE.REGS[rt] = (int32_t)(int8_t)mem_read_32(address);
                break;
            case 0x21: //LH
                NEXT_STATE.REGS[rt] = (int32_t)(int16_t)mem_read_32(address);
                break;
            case 0x23: // LW
                NEXT_STATE.REGS[rt] = mem_read_32(address);
                break;
            case 0x25: // LHU
                NEXT_STATE.REGS[rt] = mem_read_32(address) & 0xFFFF;
                break;
            case 0x28: // SB
                mem_write_32(address, (uint32_t)(NEXT_STATE.REGS[rt] & 0xFF));
                break;
            case 0x29: //SH
                mem_write_32(address, (uint32_t)(NEXT_STATE.REGS[rt] & 0xFFFF));
                break;
            case 0x2B: // SW
                mem_write_32(address,NEXT_STATE.REGS[rt]);
                break;
            default:
                printf("Invalid instruction at %08x\n", CURRENT_STATE.PC);
                break;
        }
        
    }
    // Update
    if(!Update_flag) NEXT_STATE.PC = CURRENT_STATE.PC + 4 ;
}