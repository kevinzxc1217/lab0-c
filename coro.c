/* Implementing coroutines with setjmp/longjmp */

#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include <time.h>
#include <ctype.h>
#include "game.h"
#include "agents/mcts.h"
#include "agents/negamax.h"

#include <errno.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct task {
    jmp_buf env;
    struct list_head list;
    char task_name[10];
    int n;
    int i;
};

struct arg {
    int n;
    int i;
    char *task_name;
};
char table[N_GRIDS];
char ai1 = 'X';
char ai2 = 'O';
int first_move = 1;
int move = 0;

static LIST_HEAD(tasklist);
static void (**tasks)(void *);
static struct arg *args;
static int ntasks;
static jmp_buf sched;
static struct task *cur_task;

static void task_add(struct task *task)
{
    list_add_tail(&task->list, &tasklist);
}

static void task_switch()
{
    if (!list_empty(&tasklist)) {
        struct task *t = list_first_entry(&tasklist, struct task, list);
        list_del(&t->list);
        cur_task = t;
        longjmp(t->env, 1);
    }
}

void schedule(void)
{
    int i = 0;

    setjmp(sched);

    while (ntasks-- > 0) {
        struct arg arg = args[i];
        tasks[i++](&arg);
        printf("Never reached\n");
    }

    task_switch();
}

int move_record[N_GRIDS];
int move_count = 0;

void record_move(int move)
{
    move_record[move_count++] = move;
}

static void print_moves()
{
    printf("Moves: ");
    for (int i = 0; i < move_count; i++) {
        printf("%c%d", 'A' + GET_COL(move_record[i]),
               1 + GET_ROW(move_record[i]));
        if (i < move_count - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}


#define CTRL_KEY(k) ((k) & 0x1f)

struct termios orig_termios;
void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  struct termios raw = orig_termios;

  raw.c_lflag &= ~( ICANON);
//   raw.c_lflag &= ~(ECHO |  ICANON);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/* A task yields control n times */

void task1(void *arg)
{
    negamax_init();
    char c = '\0';
    struct task *task = malloc(sizeof(struct task));
    strcpy(task->task_name, ((struct arg *) arg)->task_name);
    task->n = ((struct arg *) arg)->n;
    task->i = ((struct arg *) arg)->i;
    INIT_LIST_HEAD(&task->list);

    if (setjmp(task->env) == 0) {
        task_add(task);
        longjmp(sched, 1);
    }

    task = cur_task;
    for (; task->i < task->n; task->i++) {
        if (setjmp(task->env) == 0) {
            srand(time(NULL));

            char win = check_win(table);
            if (win == 'D') {
                draw_board(table);
                printf("It is a draw!\n");
            } else if (win != ' ') {
                if(system("clear") == -1){
                    //The system method failed
                }
                draw_board(table);
                printf("%c won!\n", win);
                first_move = 1;
                move_count = 0;
                sleep(1);
                memset(table, ' ', N_GRIDS);
            }

            if(first_move){
                move = rand() % (BOARD_SIZE*BOARD_SIZE);
                first_move = 0;
            }
            else
                move = negamax_predict(table, ai1).move;          
            if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN);
            if(c == 'q'){
                break;
            }
            else if(c == 'p'){
                c = '\0';
                while(1){
                    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN);
                    if(c == 'p'){
                        c = '\0';
                        break;
                    }
                }
            }
            if (move != -1) {
                table[move] = ai1;
                record_move(move);
            }  
            if(system("clear") == -1){
                //The system method failed
            }
            draw_board(table);
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
            printf("%s\n",buffer);

            task_add(task);
            task_switch();
        }
        task = cur_task;
    }

    longjmp(sched, 1);
}

void task2(void *arg)
{
    char c = '\0';
    struct task *task = malloc(sizeof(struct task));
    strcpy(task->task_name, ((struct arg *) arg)->task_name);
    task->n = ((struct arg *) arg)->n;
    task->i = ((struct arg *) arg)->i;
    INIT_LIST_HEAD(&task->list);


    if (setjmp(task->env) == 0) {
        task_add(task);
        longjmp(sched, 1);
    }

    task = cur_task;

    for (; task->i < task->n; task->i++) {
        if (setjmp(task->env) == 0) {
            srand(time(NULL));
            char win = check_win(table);
            if (win == 'D') {
                draw_board(table);
                printf("It is a draw!\n");
            } else if (win != ' ') {
                if(system("clear") == -1){
                    //The system method failed
                }
                draw_board(table);
                printf("%c won!\n", win);
                first_move = 1;
                move_count = 0;
                sleep(1);
                memset(table, ' ', N_GRIDS);
            }
            if(first_move){
                move = rand() % (BOARD_SIZE*BOARD_SIZE);
                first_move = 0;
            }
            else
                move = mcts(table, ai2);    
            if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN);
            if(c == 'q'){
                break;
            }
            else if(c == 'p'){
                c = '\0';
                while(1){
                    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN);
                    if(c == 'p'){
                        c = '\0';
                        break;
                    }
                }
            }
            if (move != -1) {
                table[move] = ai2;
                record_move(move);
            }    
            if(system("clear") == -1){
                //The system method failed
            }
            draw_board(table);
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
            printf("%s\n",buffer);

            task_add(task);
            task_switch();
        }
        task = cur_task;
    }

    longjmp(sched, 1);
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
int coro(void)
{
    move_count = 0;
    first_move = 1;
    enableRawMode();
    void (*registered_task[])(void *) = {task1, task2};
    struct arg arg0 = {.n = 70, .i = 0, .task_name = "Task 1"};
    struct arg arg1 = {.n = 70, .i = 1, .task_name = "Task 2"};
    struct arg registered_arg[] = {arg0, arg1};
    tasks = registered_task;
    args = registered_arg;
    ntasks = ARRAY_SIZE(registered_task);
    memset(table, ' ', N_GRIDS);
    schedule();

    return 0;
}