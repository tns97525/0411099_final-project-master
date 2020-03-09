#ifndef COMMAND_H
#define COMMAND_H

enum { 
    UNRECOG_CMD,
    BUILT_IN_CMD,
    QUERY_CMD,
};

enum {
    INSERT_CMD = 100,
    SELECT_CMD,
    UPDATE_CMD,
    DELETE_CMD,
};

typedef struct {
    char name[256];
    int len;
    unsigned char type;
} CMD_t;

extern CMD_t cmd_list[];

typedef struct SelectArgs {
    char **fields;
    size_t fields_len;
    int offset;
    int limit;
    int aggre[100];
    int aggre_len;
} SelectArgs_t;

typedef union {
    SelectArgs_t sel_args;
} CmdArg_t;

typedef struct Command {
    unsigned char type;
    char **args;
    size_t args_len;
    size_t args_cap;
    size_t and_op;
    size_t or_op;
    size_t cond1_idx;
    size_t cond2_idx;
    size_t operator_array[2];
    size_t length;
    size_t idx_array[200000];
    CmdArg_t cmd_args;
} Command_t;

Command_t* new_Command();
int add_Arg(Command_t *cmd, const char *arg);
int add_select_field(Command_t *cmd, const char *argument);
void cleanup_Command(Command_t *cmd);

#endif
