#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "SelectState.h"


///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id", 2)) {
                printf("%d", user->id);
            } else if (!strncmp(sel_args->fields[idx], "name", 4)) {
                printf("%s", user->name);
            } else if (!strncmp(sel_args->fields[idx], "email", 5)) {
                printf("%s", user->email);
            } else if (!strncmp(sel_args->fields[idx], "age", 3)) {
                printf("%d", user->age);
            }
        }
    }
    printf(")\n");
}

void print_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd) {

//	printf("!!!\n\n");
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;
    size_t aggre_len = cmd->cmd_args.sel_args.aggre_len;
    size_t aggre_idx = 0;

    if (offset == -1) {
        offset = 0;
    }

    if (cmd->cond1_idx != 0) {
        check_condtion(table, cmd);
    }
    /*
        if (idxList) {
            for (idx = offset; idx < idxListLen; idx++) {
                if (limit != -1 && (idx - offset) >= limit) {
                    break;
                }
                print_user(get_User(table, idxList[idx]), &(cmd->cmd_args.sel_args));
            }
        } else {*/

    if (aggre_len != 0) {
//		printf("~~~");
        if (offset == 0 && (limit == -1 || limit >= 1)) {
            printf("(");
            for (aggre_idx = 0; aggre_idx < aggre_len; aggre_idx++) {
                int sum = 0;
                if (aggre_idx > 0) printf(", ");

                if (cmd->cmd_args.sel_args.aggre[aggre_idx] == 1) {
                    for (idx = 0; idx < cmd->length; idx++) {
                        sum = sum + get_User(table, cmd->idx_array[idx])->age;
                    }
                    printf("%.3f", (double) sum / cmd->length);
                }
                else if (cmd->cmd_args.sel_args.aggre[aggre_idx] == 2) {
                    for (idx = 0; idx < cmd->length; idx++) {
                        sum = sum + get_User(table, cmd->idx_array[idx])->id;
                    }
                    printf("%.3f", (double) sum / cmd->length);
                }
                else if (cmd->cmd_args.sel_args.aggre[aggre_idx] == 3) {
                    for (idx = 0; idx < cmd->length; idx++) {
                        sum = sum + get_User(table, cmd->idx_array[idx])->age;
                    }
                    printf("%d", sum);
                }
                else if (cmd->cmd_args.sel_args.aggre[aggre_idx] == 4) {
                    for (idx = 0; idx < cmd->length; idx++) {
                        sum = sum + get_User(table, cmd->idx_array[idx])->id;
                    }
                    printf("%d", sum);
                }
                else if (cmd->cmd_args.sel_args.aggre[aggre_idx] == 5) {
                    printf("%ld", cmd->length);
                }
            }
            printf(")\n");
        }
    }
    else {
//		printf("\n%d  %d  %d\n",offset,limit,cmd->length);
        for (idx = offset; idx < cmd->length; idx++) {
            if (limit != -1 && (idx - offset) >= limit) {
                break;
            }
//			printf("HIHIHI");
            print_user(get_User(table, cmd->idx_array[idx]), &(cmd->cmd_args.sel_args));
        }
    }
    // }
}
///
/// Print the users for given offset and limit restriction
///
int check_id(User_t *user, Command_t *cmd, int cond, int idx) {
    if (cmd->operator_array[idx] == 1) {
        if (user->id == (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 2) {
        if (user->id != (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 3) {
        if (user->id > (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 4) {
        if (user->id < (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 5) {
        if (user->id >= (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 6) {
        if (user->id <= (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    return 0;
}

int check_name(User_t *user, Command_t *cmd, int cond, int idx) {
    if (cmd->operator_array[idx] == 1) {
        if (!strcmp(user->name, cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 2) {
        if (!strcmp(user->name, cmd->args[cond]))
            return 0;
        else
            return 1;
    }
    return 0;
}

int check_email(User_t *user, Command_t *cmd, int cond, int idx) {
    if (cmd->operator_array[idx] == 1) {
        if (!strcmp(user->email, cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 2) {
        if (!strcmp(user->email, cmd->args[cond]))
            return 0;
        else
            return 1;
    }
    return 0;
}

int check_age(User_t *user, Command_t *cmd, int cond, int idx) {
    if (cmd->operator_array[idx] == 1) {
        if (user->age == (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 2) {
        if (user->age != (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 3) {
        if (user->age > (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 4) {
        if (user->age < (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 5) {
        if (user->age >= (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    else if (cmd->operator_array[idx] == 6) {
        if (user->age <= (unsigned) atoi(cmd->args[cond]))
            return 1;
        else
            return 0;
    }
    return 0;
}

void check_condtion(Table_t *table, Command_t *cmd)
{
    int cond1_idx = cmd->cond1_idx;
    int cond2_idx = cmd->cond2_idx;
    int logical_length = table->len;
    size_t logical[logical_length];
    int length = cmd->length;
    int i = 0;
    size_t idx;
    for (idx = 0; idx < table->len; idx++) {
        if (!strncmp(cmd->args[cond1_idx], "id", 2)) {
            if (check_id(get_User(table, idx), cmd, cond1_idx + 2, 0)) {
                cmd->idx_array[i] = idx;
                i++;
            }
        }
        else if (!strncmp(cmd->args[cond1_idx], "name", 4)) {
            if (check_name(get_User(table, idx), cmd, cond1_idx + 2, 0)) {
                cmd->idx_array[i] = idx;
                i++;
            }
        }
        else if (!strncmp(cmd->args[cond1_idx], "email", 5)) {
            if (check_email(get_User(table, idx), cmd, cond1_idx + 2, 0)) {
                cmd->idx_array[i] = idx;
                i++;
            }
        }
        else if (!strncmp(cmd->args[cond1_idx], "age", 3)) {
            if (check_age(get_User(table, idx), cmd, cond1_idx + 2, 0)) {
                cmd->idx_array[i] = idx;
                i++;
            }
        }
    }
    length = i;
    if (cmd->and_op == 1) {
        int i = 0;
        for (idx = 0; idx < table->len; idx++) {
            if (!strncmp(cmd->args[cond2_idx], "id", 2)) {
                if (check_id(get_User(table, idx), cmd, cond2_idx + 2, 1)) {
                    logical[i] = idx;
                    i++;
                }
            }
            else if (!strncmp(cmd->args[cond2_idx], "name", 4)) {
                if (check_name(get_User(table, idx), cmd, cond2_idx + 2, 1)) {
                    logical[i] = idx;
                    i++;
                }
            }
            else if (!strncmp(cmd->args[cond2_idx], "email", 5)) {
                if (check_email(get_User(table, idx), cmd, cond2_idx + 2, 1)) {
                    logical[i] = idx;
                    i++;
                }
            }
            else if (!strncmp(cmd->args[cond2_idx], "age", 3)) {
                if (check_age(get_User(table, idx), cmd, cond2_idx + 2, 1)) {
                    logical[i] = idx;
                    i++;
                }
            }
        }
        logical_length = i;
        int and_index = 0;
        for (int i = 0; i < length; i++) {
            for (int j = 0; j < logical_length; j++) {
                if (cmd->idx_array[i] == logical[j]) {
                    cmd->idx_array[and_index] = cmd->idx_array[i];
                    and_index++;
                    break;
                }
            }
        }
        length = and_index;
    }
    else if (cmd->or_op == 1) {
        i = 0;
        for (idx = 0; idx < table->len; idx++) {
            if (!strncmp(cmd->args[cond2_idx], "id", 2)) {
                if (check_id(get_User(table, idx), cmd, cond2_idx + 2, 1)) {
                    logical[i] = idx;
                    i++;
                }
            }
            else if (!strncmp(cmd->args[cond2_idx], "name", 4)) {
                if (check_name(get_User(table, idx), cmd, cond2_idx + 2, 1)) {
                    logical[i] = idx;
                    i++;
                }
            }
            else if (!strncmp(cmd->args[cond2_idx], "email", 5)) {
                if (check_email(get_User(table, idx), cmd, cond2_idx + 2, 1)) {
                    logical[i] = idx;
                    i++;
                }
            }
            else if (!strncmp(cmd->args[cond2_idx], "age", 3)) {
                if (check_age(get_User(table, idx), cmd, cond2_idx + 2, 1)) {
                    logical[i] = idx;
                    i++;
                }
            }
        }
        logical_length = i;
        int same_idx;
        for (int i = 0; i < logical_length; i++) {
            same_idx = 0;
            for (int j = 0; j < length; j++) {
                if (logical[i] == cmd->idx_array[j]) {
                    same_idx = 1;
                    break;
                }
            }
            if (same_idx == 0) {
                for (int j = length - 1; j >= 0; j--) {
                    if (logical[i] < cmd->idx_array[j]) {
                        cmd->idx_array[j + 1] = cmd->idx_array[j];
                        if (j == 0) {
                            cmd->idx_array[j] = logical[i];
                        }
                    }
                    else {
                        cmd->idx_array[j + 1] = logical[i];
                        break;
                    }
                }
                length++;
            }
        }
    }
    cmd->length = length;

}


///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " ,\n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        archive_table(table);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    } else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(Table_t *table, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd(table, cmd);
        return INSERT_CMD;
    }
    else if (!strncmp(cmd->args[0], "select", 6)) {
        cmd->cond1_idx = 0;
        cmd->cond2_idx = 0;
        cmd->and_op = 0;
        cmd->or_op = 0;
        cmd->length = table->len;
        for (size_t i = 0; i < cmd->length; i++)
            cmd->idx_array[i] = i;
        handle_select_cmd(table, cmd);
        return SELECT_CMD;
    }
    else if (!strncmp(cmd->args[0], "update", 6)) {
        cmd->cond1_idx = 0;
        cmd->cond2_idx = 0;
        cmd->and_op = 0;
        cmd->or_op = 0;
        cmd->length = table->len;
        for (size_t i = 0; i < cmd->length; i++)
            cmd->idx_array[i] = i;
        handle_update_cmd(table, cmd);
        return UPDATE_CMD;
    }
    else if (!strncmp(cmd->args[0], "delete", 6)) {
        cmd->cond1_idx = 0;
        cmd->cond2_idx = 0;
        cmd->and_op = 0;
        cmd->or_op = 0;
        cmd->length = table->len;
        for (size_t i = 0; i < cmd->length; i++)
            cmd->idx_array[i] = i;
        handle_delete_cmd(table, cmd);
        return DELETE_CMD;
    }
    else {
        return UNRECOG_CMD;
    }
}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;
    User_t *user = command_to_User(cmd);
    if (user) {
        ret = add_User(table, user);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}

///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd, 1);

    print_users(table, NULL, 0, cmd);
    return table->len;
}

int handle_update_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = UPDATE_CMD;
    if (cmd->args_len > 6) {
        if (!strncmp(cmd->args[6], "where", 5)) {
            cmd->cond1_idx = 7;
            check_operator(cmd, 8, 0);
            check_condtion(table, cmd);

        }
        else {
            cmd->type = UNRECOG_CMD;
            return table->len;
        }
    }
    for (size_t idx = 0; idx < cmd->length; idx++) {
        if (!strncmp(cmd->args[3], "id", 2)){
            int repeat_key = 0;
            for(size_t i = 0; i<cmd->length; i++)
                if(get_User(table, cmd->idx_array[i])->id == (unsigned) atoi(cmd->args[5]))
                    repeat_key = 1;
            if(repeat_key != 1)
                get_User(table, cmd->idx_array[idx])->id = atoi(cmd->args[5]);
        }
        else if (!strncmp(cmd->args[3], "name", 4))
            strcpy(get_User(table, cmd->idx_array[idx])->name, cmd->args[5]);
        else if (!strncmp(cmd->args[3], "email", 5))
            strcpy(get_User(table, cmd->idx_array[idx])->email, cmd->args[5]);
        else if (!strncmp(cmd->args[3], "age", 3))
            get_User(table, cmd->idx_array[idx])->age = atoi(cmd->args[5]);
    }
    return table->len;
}

int handle_delete_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = DELETE_CMD;
    if (cmd->args_len > 3) {
        if (!strncmp(cmd->args[3], "where", 5)) {
            cmd->cond1_idx = 4;
            check_operator(cmd, 5, 0);
            check_condtion(table, cmd);
        }
        else {
            cmd->type = UNRECOG_CMD;
            return table->len;
        }
    }
    if (cmd->length != 0) {
        size_t old_table_len = table->len;
        table->len = 0;
        for (size_t reserve_idx = 0; reserve_idx < old_table_len; reserve_idx++) {
            int delete_idx = 0;
            for (size_t idx = 0; idx < cmd->length; idx++) {
                if (reserve_idx == cmd->idx_array[idx]) {
                    delete_idx = 1;
                    break;
                }
            }
            if (delete_idx == 0) {
                get_User(table, table->len)->id = get_User(table, reserve_idx)->id;
                strcpy(get_User(table, table->len)->name, get_User(table, reserve_idx)->name);
                strcpy(get_User(table, table->len)->email, get_User(table, reserve_idx)->email);
                get_User(table, table->len)->age = get_User(table, reserve_idx)->age;
                table->len = (table->len) + 1;
            }
        }
        cmd->length = table->len;
        if (cmd->length != 0) {
            for (size_t idx = 0; idx < cmd->length; idx++) {
                cmd->idx_array[idx] = idx;
            }
        }
    }
    return table->len;
}


///
/// Show the help messages
///
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
                       "\n"
                       "## Built-in Commands\n"
                       "\n"
                       "  * .exit\n"
                       "\tThis cmd archives the table, if the db file is specified, then exit.\n"
                       "\n"
                       "  * .output\n"
                       "\tThis cmd change the output strategy, default is stdout.\n"
                       "\n"
                       "\tUsage:\n"
                       "\t    .output (<file>|stdout)\n\n"
                       "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
                       "\n"
                       "  * .load\n"
                       "\tThis command loads records stored in <DB file>.\n"
                       "\n"
                       "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
                       "\n"
                       "\tUsage:\n"
                       "\t    .load <DB file>\n\n"
                       "\n"
                       "  * .help\n"
                       "\tThis cmd displays the help messages.\n"
                       "\n"
                       "## Query Commands\n"
                       "\n"
                       "  * insert\n"
                       "\tThis cmd inserts one user record into table.\n"
                       "\n"
                       "\tUsage:\n"
                       "\t    insert <id> <name> <email> <age>\n"
                       "\n"
                       "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
                       "\n"
                       "  * select\n"
                       "\tThis cmd will display all user records in the table.\n"
                       "\n";
    printf("%s", msg);
}

