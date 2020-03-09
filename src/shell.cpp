#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include "InputBuffer.h"
#include "Util.h"
#include "Table.h"
#include "SelectState.h"
using namespace std;


int main(int argc, char **argv) {
    InputBuffer_t *input_buffer = new_InputBuffer();
    Command_t *cmd = new_Command();
    State_t *state = new_State();
    Table_t *table = NULL;
    int cmd_type;
    vector<int> id1;
    vector<int> id2;
    vector<int> id1_sort;
    vector<int> id2_sort;
    if (argc != 2) {
        table = new_Table(NULL);
    } else {
        table = new_Table(argv[1]);
    }
    if (table == NULL) {
        return 1;
    }
    for (;;) {
        print_prompt(state);
        read_input(input_buffer);
        cmd_type = parse_input(input_buffer->buffer, cmd);
        if (cmd_type == BUILT_IN_CMD) {
            handle_builtin_cmd(table, cmd, state);
        }
        else if (cmd_type == QUERY_CMD) {
            bool selectfromlike = 0;
            size_t arg_idx = 1;
            if (!strncmp(cmd->args[0], "select", 6)) {
                while (arg_idx < cmd->args_len) {
                    if (!strncmp(cmd->args[arg_idx], "from", 4)) {
                        if (!strncmp(cmd->args[arg_idx + 1], "like", 4)) {
                            selectfromlike = 1;
                            break;
                        }
                        else
                            break;
                    }
                    arg_idx++;
                }
            }
            if (!strncmp(cmd->args[0], "insert", 6) && !strncmp(cmd->args[2], "like", 4)) {
                id1.push_back(atoi(cmd->args[3]));
                id2.push_back(atoi(cmd->args[4]));

            }
            else if (cmd->args_len > 4 && !strncmp(cmd->args[0], "select", 6) && !strncmp(cmd->args[4], "join", 4)) {
                int count = 0;
                cmd->cond1_idx = 0;
                cmd->cond2_idx = 0;
                cmd->and_op = 0;
                cmd->or_op = 0;
                cmd->length = table->len;
                for (size_t i = 0; i < cmd->length; i++)
                    cmd->idx_array[i] = i;
                if (cmd->args_len > 10) {
                    if (!strncmp(cmd->args[10], "where", 5)) {
                        cmd->cond1_idx = 11;
                        check_operator(cmd, 12, 0);
                        check_condtion(table, cmd);
                    }
                    else {
                        cmd->type = UNRECOG_CMD;
                        return table->len;
                    }
                }
                if (!strncmp(cmd->args[9], "id1", 3)) {
                    for (size_t i = 0; i < id1.size(); i++)
                        id1_sort.push_back(id1[i]);
                    sort (id1_sort.begin(), id1_sort.end());
                    for (size_t idx = 0; idx < cmd->length; idx++) {
                        if (binary_search (id1_sort.begin(), id1_sort.end(), get_User(table, cmd->idx_array[idx])->id))
                            count++;
                    }
                    id1_sort.clear();
                }
                else {
                    for (size_t i = 0; i < id2.size(); i++)
                        id2_sort.push_back(id2[i]);
                    sort (id2_sort.begin(), id2_sort.end());
                    int left;
                    int right;
                    int middle;
                    for (size_t idx = 0; idx < cmd->length; idx++) {
                        left = 0, right = id2_sort.size();
                        int num = get_User(table, cmd->idx_array[idx])->id;
                        while (left <= right) {
                            middle = (right + left) / 2;
                            if (id2_sort[middle] == num) {
                                count ++;
                                left = middle - 1;
                                right = middle + 1;
                                while (id2_sort[left] == num) {
                                    count++;
                                    left--;
                                }
                                while (id2_sort[right] == num) {
                                    count++;
                                    right++;
                                }
                                break;
                            }
                            else if (id2_sort[middle] > num) {
                                right = middle - 1;
                            }
                            else {
                                left = middle + 1;
                            }
                        }
                    }
                    id2_sort.clear();
                }
                printf("(%d)\n", count);
            }
            else if (selectfromlike) {
                cmd->cmd_args.sel_args.limit = -1;
                cmd->cmd_args.sel_args.offset = -1;
                if (cmd->args_len > arg_idx + 2) {
                    if (!strncmp(cmd->args[arg_idx + 2], "offset", 6)) {
                        offset_state_handler(cmd, arg_idx + 3);
                    }
                    else if (!strncmp(cmd->args[arg_idx + 2], "limit", 5)) {
                        limit_state_handler(cmd, arg_idx + 3);
                    }
                }
                int limit = cmd->cmd_args.sel_args.limit;
                int offset = cmd->cmd_args.sel_args.offset;
                if (offset == -1) {
                    offset = 0;
                }
                for (int like_idx = offset; like_idx < id1.size(); like_idx++) {
                    if (limit != -1 && (like_idx - offset) >= limit) {
                        break;
                    }
                    size_t idx = 1;
                    printf("(");
                    while (!(!strncmp(cmd->args[idx], "from", 4))) {
                        if (!strncmp(cmd->args[idx], "*", 1)) {
                            printf("%d, %d", id1[like_idx], id2[like_idx]);
                        }
                        else {
                            if (idx > 1) printf(", ");

                            if (!strncmp(cmd->args[idx], "id1", 3)) {
                                printf("%d", id1[like_idx]);
                            }
                            else if (!strncmp(cmd->args[idx], "id2", 3)) {
                                printf("%d", id2[like_idx]);
                            }
                        }
                        idx++;
                    }
                    printf(")\n");
                }
            }
            else
                handle_query_cmd(table, cmd);
        }
        else if (cmd_type == UNRECOG_CMD) {
            printf("Unrecognized command '%s'.\n", input_buffer->buffer);
        }
        cleanup_Command(cmd);
        clean_InputBuffer(input_buffer);
    }
    return 0;
}


