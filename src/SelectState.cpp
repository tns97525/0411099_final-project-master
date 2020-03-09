#include <string.h>
#include <stdlib.h>
#include "Command.h"
#include "SelectState.h"

void field_state_handler(Command_t *cmd, size_t arg_idx) {
    cmd->cmd_args.sel_args.fields = NULL;
    cmd->cmd_args.sel_args.fields_len = 0;
    cmd->cmd_args.sel_args.limit = -1;
    cmd->cmd_args.sel_args.offset = -1;
    int aggre_len = 0;
    while(arg_idx < cmd->args_len) {
        if (!strncmp(cmd->args[arg_idx], "*", 1)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "id", 2)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "name", 4)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "email", 5)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "age", 3)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if(!strncmp(cmd->args[arg_idx], "count", 5)) {
            cmd->cmd_args.sel_args.aggre[aggre_len] = 5;
            aggre_len++;
        } else if(!strncmp(cmd->args[arg_idx], "avg", 3)) {
            if(!strncmp(cmd->args[arg_idx], "avg(age)", 8))
                cmd->cmd_args.sel_args.aggre[aggre_len] = 1;
            else if(!strncmp(cmd->args[arg_idx], "avg(id)", 7))
                cmd->cmd_args.sel_args.aggre[aggre_len] = 2;
            aggre_len++;
        } else if(!strncmp(cmd->args[arg_idx], "sum", 3)) {
            if(!strncmp(cmd->args[arg_idx], "sum(age)", 8))
                cmd->cmd_args.sel_args.aggre[aggre_len] = 3;
            else if(!strncmp(cmd->args[arg_idx], "sum(id)", 7))
                cmd->cmd_args.sel_args.aggre[aggre_len] = 4;
            aggre_len++;
        } else if (!strncmp(cmd->args[arg_idx], "from", 4)) {
            cmd->cmd_args.sel_args.aggre_len = aggre_len;
            table_state_handler(cmd, arg_idx+1);
            return;
        } else {
            cmd->type = UNRECOG_CMD;
            return;
        }
        arg_idx += 1;
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void table_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len && !strncmp(cmd->args[arg_idx], "user", 4)) {

        arg_idx++;
        if (arg_idx == cmd->args_len) {
            return;
        }
        else{
            if(!strncmp(cmd->args[arg_idx], "where", 5)){
                cmd->cond1_idx = arg_idx+1;
                check_operator(cmd, arg_idx+2, 0);
                return;
            }
            else if (!strncmp(cmd->args[arg_idx], "offset", 6)) {
                offset_state_handler(cmd, arg_idx+1);
                return;
            }
            else if (!strncmp(cmd->args[arg_idx], "limit", 5)) {
                limit_state_handler(cmd, arg_idx+1);
                return;
            }
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void check_operator(Command_t *cmd, size_t arg_idx, int idx) {
    if(!strncmp(cmd->args[arg_idx], "=", 1)) cmd->operator_array[idx] = 1;
    else if(!strncmp(cmd->args[arg_idx], "!=", 2)) cmd->operator_array[idx] = 2;
    else if(!strncmp(cmd->args[arg_idx], ">=", 2)) cmd->operator_array[idx] = 5;
    else if(!strncmp(cmd->args[arg_idx], "<=", 2)) cmd->operator_array[idx] = 6;
    else if(!strncmp(cmd->args[arg_idx], ">", 1)) cmd->operator_array[idx] = 3;
    else if(!strncmp(cmd->args[arg_idx], "<", 1)) cmd->operator_array[idx] = 4;
    

    if (arg_idx+2 == cmd->args_len) {
        return;
    }
    else{
        if(!strncmp(cmd->args[arg_idx+2], "and", 3)){
            cmd->and_op = 1;
            cmd->cond2_idx = arg_idx+3;
            check_operator(cmd, arg_idx+4, 1);
            return;
        }
        else if(!strncmp(cmd->args[arg_idx+2], "or", 2)){
            cmd->or_op = 1;
            cmd->cond2_idx = arg_idx+3;
            check_operator(cmd, arg_idx+4, 1);
            return;
        }
        else if (!strncmp(cmd->args[arg_idx+2], "offset", 6)) {
            offset_state_handler(cmd, arg_idx+3);
            return;
        }
        else if (!strncmp(cmd->args[arg_idx+2], "limit", 5)) {
            limit_state_handler(cmd, arg_idx+3);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;  
}

void offset_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.offset = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        } else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "limit", 5)) {

            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void limit_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.limit = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}
