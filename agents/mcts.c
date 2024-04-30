#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "game.h"
#include "mcts.h"
#include "util.h"

#define FRACTIONAL_BITS 20


struct node {
    int move;
    char player;
    int n_visits;
    unsigned long score;
    struct node *parent;
    struct node *children[N_GRIDS];
};

static struct node *new_node(int move, char player, struct node *parent)
{
    struct node *node = malloc(sizeof(struct node));
    node->move = move;
    node->player = player;
    node->n_visits = 0;
    node->score = 0;
    node->parent = parent;
    memset(node->children, 0, sizeof(node->children));
    return node;
}

static void free_node(struct node *node)
{
    for (int i = 0; i < N_GRIDS; i++)
        if (node->children[i])
            free_node(node->children[i]);
    free(node);
}


unsigned long fixed_sqrt(unsigned long x)
{
    if (x == 0UL)
        return 0;
    unsigned long guess = x >> 1;
    unsigned long previous_guess;

    unsigned long epsilon = 1;

    do {
        previous_guess = guess;
        guess = (guess + (x << FRACTIONAL_BITS) / guess) >> 1;
    } while (guess - previous_guess > epsilon);

    return guess;
}



unsigned long fixed_log(unsigned long x) {
    if (x == 0UL)
        return 0;

    unsigned long div = ((x + (1 << FRACTIONAL_BITS)));
    unsigned long div2 = div  >> FRACTIONAL_BITS;
     if (div2 == 0)
        div2 = 1;
    unsigned long u = (x - (1 << FRACTIONAL_BITS)) / div2;

    unsigned long sum = 0;
    unsigned long u_squared = u * u;
    u_squared += 1UL << (FRACTIONAL_BITS - 1);
    u_squared >>= FRACTIONAL_BITS;

    for (int n = 0; n < 10; n++) {
        unsigned long term = ((2 * u)) / (2 * n + 1);
        sum += term;
        u *= u_squared;
        u >>= FRACTIONAL_BITS; 
    }

    return sum;
}



static inline unsigned long uct_score(int n_total, int n_visits, unsigned long score)
{
  
    if (n_visits == 0)
        return ULONG_MAX;

    unsigned long ret = score / n_visits +
          ((fixed_sqrt(2 << FRACTIONAL_BITS) * fixed_sqrt(fixed_log(n_total << FRACTIONAL_BITS) / n_visits)) >> FRACTIONAL_BITS);
    return ret;
}

static struct node *select_move(struct node *node)
{
    struct node *best_node = NULL;
    unsigned long best_score = 0;
    for (int i = 0; i < N_GRIDS; i++) {
        if (!node->children[i])
            continue;
        unsigned long score = uct_score(node->n_visits, node->children[i]->n_visits,
                                 node->children[i]->score);
        if (score > best_score) {
            best_score = score;
            best_node = node->children[i];
        }
    }
    return best_node;
}

static unsigned long simulate(char *table, char player)
{
    char win;
    char current_player = player;
    char temp_table[N_GRIDS];
    memcpy(temp_table, table, N_GRIDS);
    while (1) {
        int *moves = available_moves(temp_table);
        if (moves[0] == -1) {
            free(moves);
            break;
        }
        int n_moves = 0;
        while (n_moves < N_GRIDS && moves[n_moves] != -1)
            ++n_moves;
        int move = moves[rand() % n_moves];
        free(moves);
        temp_table[move] = current_player;
        if ((win = check_win(temp_table)) != ' ')
            return calculate_win_value(win, player);
        current_player ^= 'O' ^ 'X';
    }
    return 1<<(FRACTIONAL_BITS-1);
}

static void backpropagate(struct node *node, unsigned long score)
{
    while (node) {
        node->n_visits++;
        node->score += score;
        node = node->parent;
        score = (1 << FRACTIONAL_BITS) - score;
    }
}

static void expand(struct node *node, char *table)
{
    int *moves = available_moves(table);
    int n_moves = 0;
    while (n_moves < N_GRIDS && moves[n_moves] != -1)
        ++n_moves;
    for (int i = 0; i < n_moves; i++) {
        node->children[i] = new_node(moves[i], node->player ^ 'O' ^ 'X', node);
    }
    free(moves);
}

double fixed_to_float(unsigned long fixed_point) {
    return (double)(fixed_point) / (1 << FRACTIONAL_BITS);
}


int mcts(char *table, char player)
{
    
    /* check function
    unsigned long sqrt = fixed_sqrt(16<<FRACTIONAL_BITS);
    printf("fixed_sqrt: %lu\n",sqrt);
    printf("float_sqrt: %f\n",fixed_to_float(sqrt));    
    
    
    unsigned long lg = fixed_log(16<<FRACTIONAL_BITS);
    printf("fixed_log: %lu\n",lg);
    printf("float_log: %f\n",fixed_to_float(lg));*/

    char win;
    struct node *root = new_node(-1, player, NULL);
    for (int i = 0; i < ITERATIONS; i++) {
        struct node *node = root;
        char temp_table[N_GRIDS];
        memcpy(temp_table, table, N_GRIDS);
        while (1) {
            if ((win = check_win(temp_table)) != ' ') {
                unsigned long score =
                    calculate_win_value(win, node->player ^ 'O' ^ 'X');
                backpropagate(node, score);
                break;
            }
            if (node->n_visits == 0) {
                unsigned long score = simulate(temp_table, node->player);
                backpropagate(node, score);
                break;
            }
            if (node->children[0] == NULL)
                expand(node, temp_table);
            node = select_move(node);
            assert(node);
            temp_table[node->move] = node->player ^ 'O' ^ 'X';
        }
    }
    struct node *best_node = NULL;
    int most_visits = -1;
    for (int i = 0; i < N_GRIDS; i++) {
        if (root->children[i] && root->children[i]->n_visits > most_visits) {
            most_visits = root->children[i]->n_visits;
            best_node = root->children[i];
        }
    }
    int best_move = best_node->move;
    free_node(root);
    return best_move;
}
