#include "rbtree.h"
#include <stdlib.h>
#include <string.h>

typedef enum rb_color {BLACK, RED} rb_color_t;

typedef struct rbnode 
{
    void *value;
    char *key;
    rb_color_t color;
    struct rbnode *left, *right, *parent;
} rbnode_t;

struct rbtree
{
    rbnode_t *root;
    size_t size;
    rb_value_free_fn free_fn;
};

static rbnode_t NIL_NODE = { .color = BLACK };
static rbnode_t *const NIL = &NIL_NODE;

static void recolor(rbnode_t *node)
{
    node->color = (node->color == BLACK) ? RED : BLACK;
}


static rbnode_t *node_create(const char *key, void *value, rbnode_t *parent)
{
    rbnode_t *node = malloc(sizeof(*node));
    if (node == NULL) {
        goto fail_node;
    }

    size_t key_len = strlen(key) + 1;
    node->key = malloc(key_len);
    if (node->key == NULL) {
        goto fail_key;
    }
    memcpy(node->key, key, key_len);

    node->value = value;
    node->color = RED;
    node->left = node->right = NIL;
    node->parent = parent;
    return node;

fail_key:
    free(node);
fail_node:
    return NULL;
}

rbtree_t *rb_create(rb_value_free_fn value_free)
{
    rbtree_t *t = malloc(sizeof(*t));
    if (t == NULL) {
        return NULL;
    }
    t->root = NIL;
    t->size = 0;
    t->free_fn = value_free;
    return t;
}

size_t rb_size(const rbtree_t *t)
{
    return t->size;
}

void *rb_find(const rbtree_t *t, const char *key)
{
    rbnode_t *current = t->root;
    while (current != NIL) {
        int cmp = strcmp(key, current->key);
        if (cmp == 0) {
            return current->value;
        }
        current = (cmp < 0) ? current->left : current->right;
    }
    return NULL;
}

/* returns 0 if node/parent form a line (same side of grandparent), 1 if a kink */
static int is_kink(rbnode_t *node, rbnode_t *parent)
{
    rbnode_t *grandparent = parent->parent;
    int parent_is_left = (parent == grandparent->left);
    int node_is_left = (node == parent->left);
    return (parent_is_left == node_is_left) ? 0 : 1;
}

static void rotate_left(rbtree_t *t, rbnode_t *parent)
{
    rbnode_t *child = parent->right;
    rbnode_t *grandparent = parent->parent;

    parent->right = child->left;
    if (child->left != NIL) {
        child->left->parent = parent;
    }

    child->parent = grandparent;
    if (grandparent == NIL) {
        t->root = child;
    } else if (parent == grandparent->left) {
        grandparent->left = child;
    } else {
        grandparent->right = child;
    }

    child->left = parent;
    parent->parent = child;
}

static void rotate_right(rbtree_t *t, rbnode_t *parent)
{
    rbnode_t *child = parent->left;
    rbnode_t *grandparent = parent->parent;

    parent->left = child->right;
    if (child->right != NIL) {
        child->right->parent = parent;
    }

    child->parent = grandparent;
    if (grandparent == NIL) {
        t->root = child;
    } else if (parent == grandparent->left) {
        grandparent->left = child;
    } else {
        grandparent->right = child;
    }

    child->right = parent;
    parent->parent = child;
}

static void insert_fixup(rbnode_t *root, rbnode_t *node)
{
    /* invariant: node is RED on every entry to this loop */
    while (node->parent->color == RED) {
        rbnode_t *parent = node->parent;
        rbnode_t *grandparent = parent->parent;
        rbnode_t *uncle = (parent == grandparent->left) ? grandparent->right : grandparent->left;

        if (uncle->color == RED) {
            recolor(parent);
            recolor(uncle);
            recolor(grandparent);
            node = grandparent;
        } else {
            break; /* uncle-black cases (rotations) added in a later step */
        }
    }
    root->color = BLACK;
}

static int insert_recursive(rbtree_t *t, rbnode_t *current, const char *key, void *value)
{
    int cmp = strcmp(key, current->key);
    if (cmp == 0) {
        if (t->free_fn != NULL) {
            t->free_fn(current->value);
        }
        current->value = value;
        return 0;
    }

    /* child holds the address of current->left/right itself, not a copy of the
     * node pointer stored there - a plain rbnode_t * would only copy that value,
     * so writing through it later couldn't change current->left/right at all. */
    rbnode_t **child = (cmp < 0) ? &current->left : &current->right;
    if (*child != NIL) {
        return insert_recursive(t, *child, key, value);
    }

    rbnode_t *node = node_create(key, value, current);
    if (node == NULL) {
        return -1;
    }
    *child = node;
    t->size++;
    return 0;
}

int rb_insert(rbtree_t *t, const char *key, void *value)
{
    if (t->root == NIL) {
        rbnode_t *node = node_create(key, value, NIL);
        if (node == NULL) {
            return -1;
        }
        t->root = node;
        t->size++;
        return 0;
    }
    return insert_recursive(t, t->root, key, value);
}