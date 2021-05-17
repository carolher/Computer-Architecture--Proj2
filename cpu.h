#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_
/**
 *  cpu.h
 *  Contains various CPU and Pipeline Data structures
 *
 *  Author :
 *  Carolina Hernandez (cherna19@binghamton.edu)
 *  State University of New York, Binghamton
 */

#define IQ_Entries 8
#define LSQ_Entries 6
#define PRF 24 // Physical Registers
#define ARF 16 //Architectural Registers
#define ROB_Entries 12
#define RAT_Entries 16
#define BIS_Entries 12 //Related to Reorder Buffer
#define BTB_Entries 2  //Prediction of at least 2 branches

enum APEX_Stages
{
  F,
  DRD,
  INT1_FU,
  INT2_FU,
  MUL1_FU,
  MUL2_FU,
  MUL3_FU,
  BR,
  MEM,
  NUM_STAGES
};

/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
  char opcode[128]; // Operation Code
  int rd;           // Destination Register Address
  int rs1;          // Source-1 Register Address
  int rs2;
  int rs3; // Source-2 Register Address
  int imm; // Literal Value
} APEX_Instruction;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
  int pc;           // Program Counter
  char opcode[128]; // Operation Code
  int rs1;          // Source-1 Register Address
  int p1;           // Source-1 Physical Register Address
  int p1_valid;
  int rs2; // Source-2 Register Address
  int p2;  // Source-2 Physical Register Address
  int p2_valid;
  int rs3; // Source-3 Register Address
  int p3;  // Source-3 Physical Register Address
  int p3_valid;
  int rd;          // Architectural Destination Register Address
  int prd;         //Physical Destination Register Address
  int imm;         // Literal Value
  int rs1_value;   // Source-1 Register Value
  int rs2_value;   // Source-2 Register Value
  int rs3_value;   // Source-3 Register Value
  int buffer;      // Latch to hold some value
  int mem_address; // Computed Memory Address
  int busy;        // Flag to indicate, stage is performing some action
  int stalled;     // Flag to indicate, stage is stalled
  // enum APEX_Stages stages;
} CPU_Stage;

/*Format of IQ Entry*/
typedef struct IQ_Entry
{

  int pc;
  int opcode[128];
  int iq_size[IQ_Entries];
  int head;        //front in Array
  int tail;        //rear in Array
  int rs1;         // Source-1 Register Address
  int p1;          // Source-1 Physical Register Address
  int p1_valid;    //valid bit
  int rs2;         // Source-2 Register Address
  int p2;          // Source-2 Physical Register Address
  int p2_valid;    //valid bit
  int rs3;         // Source-3 Register Address
  int p3;          // Source-3 Physical Register Address
  int p3_valid;    //valid bit
  int rd;          // Architectural Destination Register Address
  int prd;         //Physical Destination Register Address
  int imm;         // Literal Value
  int rs1_value;   // Source-1 Register Value
  int rs2_value;   // Source-2 Register Value
  int rs3_value;   // Source-3 Register Value
  int buffer;      // Latch to hold some value
  int mem_address; // Computed Memory Address
  enum APEX_Stages stages;

} IQ_Entry;

/*Format of IQ Entry*/
typedef struct ROB_Entry
{
  int pc;
  int opcode[128];
  int rob_size[ROB_Entries];
  int head;        //front in Array
  int tail;        //rear in Array
  int rs1;         // Source-1 Register Address
  int p1;          // Source-1 Physical Register Address
  int p1_valid;    //valid bit
  int rs2;         // Source-2 Register Address
  int p2;          // Source-2 Physical Register Address
  int p2_valid;    //valid bit
  int rs3;         // Source-3 Register Address
  int p3;          // Source-3 Physical Register Address
  int p3_valid;    //valid bit
  int rd;          // Architectural Destination Register Address
  int prd;         //Physical Destination Register Address
  int imm;         // Literal Value
  int rs1_value;   // Source-1 Register Value
  int rs2_value;   // Source-2 Register Value
  int rs3_value;   // Source-3 Register Value
  int buffer;      // Latch to hold some value
  int mem_address; // Computed Memory Address
  int mem_address_valid;
} ROB_Entry;

/*Format of IQ Entry*/
typedef struct LSQ_Entry
{
  int pc;
  int opcode[128];
  int lsq_size[LSQ_Entries];
  int head;             //front in Array
  int tail;             //rear in Array
  int rs1;              // Source-1 Register Address
  int p1;               // Source-1 Physical Register Address
  int p1_valid;         //valid bit
  int rs2;              // Source-2 Register Address
  int p2;               // Source-2 Physical Register Address
  int p2_valid;         //valid bit
  int rs3;              // Source-3 Register Address
  int p3;               // Source-3 Physical Register Address
  int p3_valid;         //valid bit
  int rd;               // Architectural Destination Register Address
  int prd;              //Physical Destination Register Address
  int imm;              // Literal Value
  int rs1_value;        // Source-1 Register Value
  int rs2_value;        // Source-2 Register Value
  int rs3_value;        // Source-3 Register Value
  int buffer;           // Latch to hold some value
  int mem_address;      // Computed Memory Address
  int mem_address_valid; //valid bit
} LSQ_Entry;

/*Format of IQ Entry*/
typedef struct BTB_Entry
{
  int pc;
  int opcode[128];
  int btb_size[BTB_Entries];
  int imm;
  int memory_address;
  int mem_address_valid;
  int next_predicted_path; //Instruction to be fetched if branch is predicted correctly
  int status;  //taken or not taken
} BTB_Entry;

/*Format of Branch Instruction Stack Entry*/
typedef struct BIS_Entry
{
  int pc;
  int top;
  int bottom;
  int opcode[128];
  int bis_size[BIS_Entries];
  int imm;
  int memory_address;
  int mem_address_valid;
  int status;  //taken or not taken - save in BTB if is taken
} BIS_Entry;

/*Format of Rename Table*/
typedef struct RAT
{
  int rat_size[RAT_Entries];
  int rd;
  int rd_valid;
  int prd;
  int prd_valid;

} RAT;

/* Model of APEX CPU */
typedef struct APEX_CPU
{
  /* Clock cycles elasped */
  int clock;

  /* Current program counter */
  int pc;

  /* Integer register file */
  int regs[32];
  int regs_valid[32];

  /* Array of 5 CPU_stage */
  CPU_Stage stage[9];

  /* Code Memory where instructions are stored */
  APEX_Instruction *code_memory;
  int code_memory_size;

  /* Data Memory */
  int data_memory[4096];

  /* Some stats */
  int ins_completed;

} APEX_CPU;

// Pipeline functions

APEX_Instruction *
create_code_memory(const char *filename, int *size);

APEX_CPU *
APEX_cpu_init(const char* filename,const char *command, int no_of_cycles);

int APEX_cpu_run(APEX_CPU *cpu);

void APEX_cpu_stop(APEX_CPU *cpu);

// Stages functions

int fetch(APEX_CPU *cpu);

int decode(APEX_CPU *cpu);

int integer1_fu(APEX_CPU *cpu);

int integer2_fu(APEX_CPU *cpu);

int multiplication1_fu(APEX_CPU *cpu);

int multiplication2_fu(APEX_CPU *cpu);

int multiplication3_fu(APEX_CPU *cpu);

int branch_fu(APEX_CPU *cpu);

int memory(APEX_CPU *cpu);

// Dispatch and renaming - Methods at DRD

//sources renaming  //decoder.rename  rename.dispatch  iq.renaming.
//decode
//dispatch






// IQ - Methods - Array Queue

// int pushIQ();

// int clearIQ(); // can be a general squashing

// bool isIQempty();

// bool isIQfull();

// // ROB Queue - Methods
// // Cicular Queue

// int pushROB();

// int clearROB(); // can be a general squashing for all cicuclar queues

// bool isROBempty();

// bool isROBfull();

// // LSQ Queue  - Methods
// //Cicular Queue

// int pushLSQ();

// int clearLSQ(); // can be a general squashing for all cicuclar queues

// bool isLSQempty();

// bool isROBfull();

// // BIS - Methods
// // Cicular Queue
// int pushBIS();

// int clearBIS(); // can be a general squashing for all cicuclar queues

// bool isBISempty();

// bool isBISfull();

// // BTB - Methods

// int pushBR();

// int clearBR();

// // Common FU's - Methods

// int flushFu();

#endif
