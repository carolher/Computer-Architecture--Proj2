/*
 *  cpu.c
 *  Contains APEX cpu pipeline implementation
 *
 *  Author :
 *  Carolina Hernandez (cherna19@binghamton.edu)
 *  State University of New York, Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1
int flush_and_reload_pc = 0;

/*
 * This function creates and initializes APEX cpu.
 *
 * Note : You are free to edit this function according to your
 * 				implementation
 */
APEX_CPU *
APEX_cpu_init(const char *filename, const char *command, int no_of_cycles)
{
  if (!filename && !command && !no_of_cycles)
  {
    return NULL;
  }

  APEX_CPU *cpu = malloc(sizeof(*cpu));
  if (!cpu)
  {
    return NULL;
  }

  if (ENABLE_DEBUG_MESSAGES)
  {

    if (strcmp(command, "Simulate") == 0)
    {
    }
  }

  /* Initialize PC, Registers and all pipeline stages */
  cpu->pc = 4000;
  cpu->clock = 1;
  memset(cpu->regs, 0, sizeof(int) * 32);
  memset(cpu->regs_valid, 1, sizeof(int) * 32);
  memset(cpu->stage, 0, sizeof(CPU_Stage) * NUM_STAGES);
  memset(cpu->data_memory, 0, sizeof(int) * 4000);

  /* Parse input file and create code memory */
  cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);

  if (!cpu->code_memory)
  {
    free(cpu);
    return NULL;
  }

  if (ENABLE_DEBUG_MESSAGES)
  {
    fprintf(stderr,
            "APEX_CPU : Initialized APEX CPU, loaded %d instructions\n",
            cpu->code_memory_size);
    fprintf(stderr, "APEX_CPU : Printing Code Memory\n");
    printf("%-9s %-9s %-9s %-9s %-9s %-9s\n", "opcode", "rd", "rs1", "rs2", "rs3", "imm");

    for (int i = 0; i < cpu->code_memory_size; ++i)
    {
      printf("%-9s %-9d %-9d %-9d %-9d %-9d\n",
             cpu->code_memory[i].opcode,
             cpu->code_memory[i].rd,
             cpu->code_memory[i].rs1,
             cpu->code_memory[i].rs2,
             cpu->code_memory[i].rs3,
             cpu->code_memory[i].imm);
    }
  }

  /* Make all stages busy except Fetch stage, initally to start the pipeline */
  for (int i = 1; i < NUM_STAGES; ++i)
  {
    cpu->stage[i].busy = 1;
  }

  return cpu;
}

/*
 * This function de-allocates APEX cpu.
 *
 * Note : You are free to edit this function according to your
 * 				implementation
 */
void APEX_cpu_stop(APEX_CPU *cpu)
{
  free(cpu->code_memory);
  free(cpu);
}

/* Converts the PC(4000 series) into
 * array index for code memory
 *
 * Note : You are not supposed to edit this function
 *
 */
int get_code_index(int pc)
{
  return (pc - 4000) / 4;
}

static void
print_instruction(CPU_Stage *stage)
{

  if (strcmp(stage->opcode, "STORE") == 0)
  {
    //Implement logic if it's at fetch stage just don't rename the instruction
    if (F)
    {
      printf("%s,R%d,R%d,#%d ", stage->opcode, stage->rs1, stage->rs2, stage->imm);
    }
    else
    {
      printf("%s,R%d,R%d,#%d -> [%s,P%d,P%d,#%d] ",
             stage->opcode, stage->rs1, stage->rs2, stage->imm,
             stage->opcode, stage->p1, stage->p2, stage->imm);
    }
  }

  else if (strcmp(stage->opcode, "MOVC") == 0)
  {

    printf("%s,R%d,#%d ", stage->opcode, stage->rd, stage->imm);
  }

  else if (strcmp(stage->opcode, "STR") == 0)
  {
    printf("%s,R%d,R%d,R%d ", stage->opcode, stage->rs1, stage->rs2, stage->rs3);
  }

  else if (strcmp(stage->opcode, "ADD") == 0 || strcmp(stage->opcode, "SUB") == 0 || strcmp(stage->opcode, "AND") == 0 ||
           strcmp(stage->opcode, "OR") == 0 || strcmp(stage->opcode, "EX-OR") == 0 || strcmp(stage->opcode, "MUL") == 0 ||
           strcmp(stage->opcode, "LDR") == 0)
  {
    printf("%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  else if (strcmp(stage->opcode, "ADDL") == 0 || strcmp(stage->opcode, "SUBL") == 0 || strcmp(stage->opcode, "LOAD") == 0)
  {

    printf("%s,R%d,R%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode, "BZ") == 0 || strcmp(stage->opcode, "BNZ") == 0)
  {
    printf("%s,#%d", stage->opcode, stage->imm);
  }

  if (strcmp(stage->opcode, "JUMP") == 0)
  {
    printf("%s,R%d,#%d", stage->opcode, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode, "HALT") == 0 || strcmp(stage->opcode, "HALT\n") == 0)
  {
    printf("%s", stage->opcode);
  }
}

/* Debug function which dumps the cpu stage
 * content
 *
 * Note : You are not supposed to edit this function
 *
 */
static void
print_stage_content(char *name, CPU_Stage *stage)
{
  printf("%-15s: pc(%d) ", name, stage->pc);
  print_instruction(stage);
  printf("\n");
}

/*
 *  Fetch Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int fetch(APEX_CPU *cpu)
{
  CPU_Stage *stage = &cpu->stage[F];
  if (!stage->busy && !stage->stalled)
  {
    /* Store current PC in fetch latch */
    stage->pc = cpu->pc;

    /* Index into code memory using this pc and copy all instruction fields into
     * fetch latch
     */
    APEX_Instruction *current_ins = &cpu->code_memory[get_code_index(cpu->pc)];
    strcpy(stage->opcode, current_ins->opcode);
    stage->rd = current_ins->rd;
    stage->rs1 = current_ins->rs1;
    stage->rs2 = current_ins->rs2;
    stage->rs3 = current_ins->rs3;
    stage->imm = current_ins->imm;
    stage->rd = current_ins->rd;

    /* Update PC for next instruction */
    cpu->pc += 4;

    /* Copy data from fetch latch to decode latch*/
    cpu->stage[DRD] = cpu->stage[F];

    if (ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("Fetch", stage);
    }
  }
  return 0;
}

/*
 *  Decode/Rename/Dispatch Stage of APEX Pipeline
 *
 * //initialize IQ,ROB,LSQ T, stalling and BIS logic
 * 
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int decode(APEX_CPU *cpu)
{
  CPU_Stage *stage = &cpu->stage[DRD];
  if (!stage->busy && !stage->stalled)
  {
    if (strcmp(stage->opcode, "STORE") == 0)
    {
    }

    else if (strcmp(stage->opcode, "MOVC") == 0)
    {

      // printf("%s,R%d,#%d ", stage->opcode, stage->rd, stage->imm);
    }

    else if (strcmp(stage->opcode, "STR") == 0)
    {
      // printf("%s,R%d,R%d,R%d ", stage->opcode, stage->rs1, stage->rs2, stage->rs3);
    }

    else if (strcmp(stage->opcode, "ADD") == 0 || strcmp(stage->opcode, "SUB") == 0 || strcmp(stage->opcode, "AND") == 0 ||
             strcmp(stage->opcode, "OR") == 0 || strcmp(stage->opcode, "EX-OR") == 0 || strcmp(stage->opcode, "MUL") == 0 ||
             strcmp(stage->opcode, "LDR") == 0)
    {
      // printf("%s,R%d,R%d,R%d ", stage->opcode, stage->rd, stage->rs1, stage->rs2);
    }

    else if (strcmp(stage->opcode, "ADDL") == 0 || strcmp(stage->opcode, "SUBL") == 0 || strcmp(stage->opcode, "LOAD") == 0)
    {

      // printf("%s,R%d,R%d,#%d ", stage->opcode, stage->rd, stage->rs1, stage->imm);
    }

    if (strcmp(stage->opcode, "BZ") == 0 || strcmp(stage->opcode, "BNZ") == 0)
    {
      // printf("%s,#%d", stage->opcode, stage->imm);
    }

    if (strcmp(stage->opcode, "JUMP") == 0)
    {
      // printf("%s,R%d,#%d", stage->opcode, stage->rs1, stage->imm);
    }

    if (strcmp(stage->opcode, "HALT") == 0 || strcmp(stage->opcode, "HALT\n") == 0)
    {
      // printf("%s", stage->opcode);
    }
    /* Copy data from decode latch to execute latch*/
    // cpu->stage[EX] = cpu->stage[DRD];

    if (ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("Decode/Rename", stage);
    }
  }
  return 0;
}

int integer1_fu(APEX_CPU *cpu)
{
  CPU_Stage *stage = &cpu->stage[INT1_FU];

  if (!stage->busy && !stage->stalled)
  { // This is just a Delay Latch
    if (strcmp(stage->opcode, "MOVC") == 0 ||  strcmp(stage->opcode, "STORE") == 0 || strcmp(stage->opcode, "STR") == 0 ||
       strcmp(stage->opcode, "ADD") == 0 || strcmp(stage->opcode, "LDR") == 0 ||  strcmp(stage->opcode, "LOAD") == 0 ||
       strcmp(stage->opcode, "SUB") == 0 || strcmp(stage->opcode, "AND") == 0 || strcmp(stage->opcode, "OR") == 0 ||
       strcmp(stage->opcode, "EX-OR") == 0 || strcmp(stage->opcode, "ADDL") == 0 || strcmp(stage->opcode, "SUBL") == 0)
    {
    }
    cpu->stage[INT2_FU] = cpu->stage[INT1_FU];
    if (ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("INT1_FU_STAGE", stage);
      // flushStage(cpu, INTFU1);
    }

  }
  return 0;
}

int integer2_fu(APEX_CPU *cpu)
{
  CPU_Stage *stage = &cpu->stage[INT2_FU];

  if (!stage->busy && !stage->stalled)
  {
    if (strcmp(stage->opcode, "MOVC") == 0)
    {
      stage->buffer = stage->imm + 0;
    }

    else if (strcmp(stage->opcode, "ADD") == 0)
    {
      stage->buffer = stage->rs1_value + stage->rs2_value;
    }
    else if (strcmp(stage->opcode, "LDR") == 0)
    {
      stage->buffer = stage->rs1_value + stage->rs2_value;
      //  updtLSQ(cpu, INTFU2);
    }
    else if (strcmp(stage->opcode, "SUB") == 0)
    {
      stage->buffer = stage->rs1_value - stage->rs2_value;
    }

    else if (strcmp(stage->opcode, "AND") == 0)
    {
      stage->buffer = stage->rs1_value & stage->rs2_value;
    }

    else if (strcmp(stage->opcode, "OR") == 0)
    {
      stage->buffer = stage->rs1_value | stage->rs2_value;
    }

    else if (strcmp(stage->opcode, "EX-OR") == 0)
    {
      stage->buffer = stage->rs1_value ^ stage->rs2_value;
    }

    else if (strcmp(stage->opcode, "ADDL") == 0)
    {
      stage->buffer = stage->rs1_value + stage->imm;
    }

    else if (strcmp(stage->opcode, "SUBL") == 0)
    {
      stage->buffer = stage->rs1_value - stage->imm;
    }

    else if (strcmp(stage->opcode, "STORE") == 0)
    {
      stage->buffer = stage->rs2_value + stage->imm;

      // updtLSQ(cpu, INTFU2);
    }
    
    else if(strcmp(stage->opcode, "STR") == 0){
      stage->buffer = stage->rs2_value + stage->rs3_value;
    }

    else if (strcmp(stage->opcode, "LOAD") == 0)
    {
      stage->buffer = stage->rs1_value + stage->imm;
      // updtLSQ(cpu, INTFU2);
    }

    if (ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("INT2_FU_STAGE",stage);
    }

    // if (strcmp(stage->opcode, "") != FREE && (strcmp(stage->opcode, "LOAD") != FREE) && stage->prd != COMMIT)
    // {
    //   pushResults(cpu, INTFU2);
    // }
    // if (strcmp(stage->opcode, "") != FREE && stage->lsq_i == COMMIT)
    // {
    //   updtROB(cpu, INTFU2);
    // }

    // flushStage(cpu, INTFU2);
  }

  return 0;
}

int multiplication1_fu(APEX_CPU *cpu)
{
  
  CPU_Stage *stage = &cpu->stage[MUL1_FU];
  if (!stage->busy && !stage->stalled)
  {
    if (strcmp(stage->opcode, "MUL") == 0)
    {
      // stage->stalled = NONSTALLED;
    }
    cpu->stage[MUL2_FU] = cpu->stage[MUL1_FU];
    if (ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("MUL1_FU_STAGE", stage);
    }
  }
  return 0;
}

int multiplication2_fu(APEX_CPU *cpu)
{

   CPU_Stage *stage = &cpu->stage[MUL2_FU];
  if (!stage->busy && !stage->stalled)
  {
    if (strcmp(stage->opcode, "MUL") == 0)
    {
      // stage->stalled = NONSTALLED;
    }
    cpu->stage[MUL3_FU] = cpu->stage[MUL2_FU];
    if (ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("MUL2_FU_STAGE", stage);
    }
  }
  return 0;
}

int multiplication3_fu(APEX_CPU *cpu)
{

  CPU_Stage *stage = &cpu->stage[MUL3_FU];
  if (!stage->busy && !stage->stalled)
  {
    if (strcmp(stage->opcode, "MUL") == 0)
    {
      stage->buffer = stage->rs1_value * stage->rs2_value;
      // stage->stalled = NONSTALLED;
      // pushResults(cpu, MULFU3);
      // updtROB(cpu, MULFU3);
    }
    if (ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("MUL3_FU_STAGE", stage);
      // flushStage(cpu, MULFU2);
    }
  }
  return 0;
}

int branch_fu(APEX_CPU *cpu)
{

  CPU_Stage *stage = &cpu->stage[BR];

     /* Store */
    if (strcmp(stage->opcode, "BZ") == 0)
    {
    }

    /* LOAD */
    if (strcmp(stage->opcode, "BNZ") == 0)
    {
    }

    /* LDR */
    if (strcmp(stage->opcode, "JUMP") == 0)
    {
    }
  return 0;
}

/*
 *  Memory Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int memory(APEX_CPU *cpu)
{
  CPU_Stage *stage = &cpu->stage[MEM];
  if (!stage->busy && !stage->stalled)
  {


     /* Store */
    if (strcmp(stage->opcode, "STORE") == 0)
    {
    }

    /* LOAD */
    if (strcmp(stage->opcode, "LOAD") == 0)
    {
    }

    /* LDR */
    if (strcmp(stage->opcode, "LDR") == 0)
    {
    }

    /* STR */
    if (strcmp(stage->opcode, "STR") == 0)
    {
    }

    /* Copy data from decode latch to execute latch*/
    // cpu->stage[WB] = cpu->stage[MEM];

    if (ENABLE_DEBUG_MESSAGES)
    {
      print_stage_content("Memory", stage);
    }
  }
  return 0;
}

// /*
//  *  Writeback Stage of APEX Pipeline
//  *
//  *  Note : You are free to edit this function according to your
//  * 				 implementation
//  */
// int
// writeback(APEX_CPU* cpu)
// {
//   CPU_Stage* stage = &cpu->stage[WB];
//   if (!stage->busy && !stage->stalled) {

//     /* Update register file */
//     if (strcmp(stage->opcode, "MOVC") == 0) {
//       cpu->regs[stage->rd] = stage->buffer;
//     }

//     cpu->ins_completed++;

//     if (ENABLE_DEBUG_MESSAGES) {
//       print_stage_content("Writeback", stage);
//     }
//   }
//   return 0;
// }

/*
 *  APEX CPU simulation loop
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int APEX_cpu_run(APEX_CPU *cpu)
{
  while (cpu->clock <= cpu->code_memory_size)
  {

    /* All the instructions committed, so exit */
    if (cpu->ins_completed == cpu->code_memory_size)
    {
      printf("(apex) >> Simulation Complete");
      break;
    }

    if (ENABLE_DEBUG_MESSAGES)
    {
      printf("--------------------------------\n");
      printf("Clock Cycle #: %d\n", cpu->clock);
      printf("--------------------------------\n");
    }

    // writeback(cpu);
    // memory(cpu);

    branch_fu(cpu);
    multiplication3_fu(cpu);
    multiplication2_fu(cpu);
    multiplication1_fu(cpu);
    integer2_fu(cpu);
    integer1_fu(cpu);
    decode(cpu);
    fetch(cpu);
    cpu->clock++;
  }

  return 0;
}