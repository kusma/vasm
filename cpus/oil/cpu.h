/* cpu.h OIL cpu-description header-file */
/* (c) in 2007 by Volker Barthelmann */


/* maximum number of operands in one mnemonic */
#define MAX_OPERANDS 5

/* maximum number of mnemonic-qualifiers per mnemonic */
#define MAX_QUALIFIERS 0

/* maximum number of additional command-line-flags for this cpu */

/* data type to represent a target-address */
typedef int32_t taddr;
typedef uint32_t utaddr;

#define LITTLEENDIAN 0
#define BIGENDIAN 1
#define VASM_CPU_OIL 1

/* minimum instruction alignment */
#define INST_ALIGN 4

/* default alignment for n-bit data */
#define DATA_ALIGN(n) ((n)<=8?1:4)

/* operand class for n-bit data definitions */
#define DATA_OPERAND(n) OP_ADDR

#define ALLOW_EMPTY_OPS  1

#define cc reg

/* type to store each operand */
typedef struct {
  int type;
  expr *value;
} operand;

/* operand-types */
#define OP_NONE      0
#define OP_ADDR      1
#define OP_ADDR_IND  2
#define OP_INDEX     3
#define OP_DELEXT    4
#define OP_UE        5
#define OP_VALUE     6
#define OP_WPOS		7	
#define OP_NUMBITS	8
#define OP_INIBITS	9
#define OP_STRUCTP	10
#define OP_DEVICE       11

typedef struct {
  unsigned int opcode;
  unsigned int encoding;
} mnemonic_extension;

/* encoding types */
#define EN_NORMAL    0
#define EN_JIF       16
