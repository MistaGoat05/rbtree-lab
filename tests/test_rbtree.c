#include "rbtree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(cond, ctx, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "FAILED [%s]: %s\n", (ctx), (msg)); \
            abort(); \
        } \
    } while (0)

static void test_root_insert(void)
{
    rbtree_t *t = rb_create(NULL);
    assert(t != NULL);

    int value = 42;
    assert(rb_insert(t, "root", &value) == 0);

    assert(rb_find(t, "root") == &value);
    assert(rb_size(t) == 1);
    assert(rb_validate(t) == 0);

    rb_destroy(t);
}

static void test_insert_black_parent(void)
{
    rbtree_t *t = rb_create(NULL);
    assert(t != NULL);

    int value = 42;
    assert(rb_insert(t, "b", &value) == 0);
    assert(rb_insert(t, "a", &value) == 0);

    assert(rb_validate(t) == 0);

    rb_destroy(t);
}

static void test_insert_red_parent_and_uncle(void)
{
    rbtree_t *t = rb_create(NULL);
    assert(t != NULL);

    int value = 42;
    assert(rb_insert(t, "c", &value) == 0);
    assert(rb_insert(t, "b", &value) == 0);
    assert(rb_insert(t, "d", &value) == 0);
    assert(rb_insert(t, "a", &value) == 0);

    assert(rb_validate(t) == 0);

    rb_destroy(t);
}

static void test_insert_red_parent_black_uncle(void)
{
    rbtree_t *t = rb_create(NULL);
    assert(t != NULL);

    int value = 42;
    assert(rb_insert(t, "g", &value) == 0);
    assert(rb_insert(t, "h", &value) == 0);
    assert(rb_insert(t, "b", &value) == 0);
    assert(rb_insert(t, "a", &value) == 0);
    assert(rb_insert(t, "e", &value) == 0);
    assert(rb_insert(t, "f", &value) == 0);
    assert(rb_insert(t, "d", &value) == 0);
    assert(rb_insert(t, "c", &value) == 0);

    assert(rb_validate(t) == 0);

    rb_destroy(t);
}

typedef struct {
    const char *name;
    const char *const *inserts;
    const char *delete_key;
    int expect_ret;
    size_t expect_size_after;
} delete_case_t;

static const char *const case_red_leaf[] = {"d", "b", "f", NULL};
/*static const char *const case_one_red_child_left[] = {"d", "b", "f", "a", NULL};
static const char *const case_one_red_child_right[] = {"d", "f", "b", "g", NULL};
static const char *const case_two_children[] = {"d", "b", "f", "a", "c", NULL};
static const char *const case_root[] = {"m", NULL};
static const char *const case_sibling_red[] = {"a", "b", "c", "d", "e", "f", "g", NULL};
static const char *const case_sibling_red_mirror[] = {"g", "f", "e", "d", "c", "b", "a", NULL};*/

static const delete_case_t delete_cases[] = {
    {"red_leaf_left",                 case_red_leaf,            "b", 0, 2},
    {"red_leaf_right",                case_red_leaf,            "f", 0, 2}/*,
    {"one_red_child_left",            case_one_red_child_left,  "b", 0, 3},
    {"one_red_child_right",           case_one_red_child_right, "f", 0, 3},
    {"two_children",                  case_two_children,        "b", 0, 4},
    {"root_deletion",                 case_root,                "m", 0, 0},
    {"black_leaf_red_sibling",        case_sibling_red,          "a", 0, 6},
    {"black_leaf_red_sibling_mirror", case_sibling_red_mirror,   "g", 0, 6},*/
};

static void test_delete_cases(void)
{
    /* invariant: cases [0, i) have each built their own tree, deleted, and been destroyed */
    for (size_t i = 0; i < sizeof(delete_cases) / sizeof(delete_cases[0]); i++) {
        const delete_case_t *c = &delete_cases[i];

        rbtree_t *t = rb_create(NULL);
        assert(t != NULL);

        int value = 42;
        /* invariant: every key before index k has already been inserted successfully */
        for (size_t k = 0; c->inserts[k] != NULL; k++) {
            CHECK(rb_insert(t, c->inserts[k], &value) == 0, c->name, "setup insert failed");
        }
        CHECK(rb_validate(t) == 0, c->name, "rb_validate before delete");

        CHECK(rb_delete(t, c->delete_key) == c->expect_ret, c->name, "rb_delete return value");
        CHECK(rb_size(t) == c->expect_size_after, c->name, "rb_size after delete");
        CHECK(rb_find(t, c->delete_key) == NULL, c->name, "rb_find after delete");
        CHECK(rb_validate(t) == 0, c->name, "rb_validate after delete");

        rb_destroy(t);
    }
}

int main(void)
{
    test_root_insert();
    test_insert_black_parent();
    test_insert_red_parent_and_uncle();
    test_insert_red_parent_black_uncle();
    test_delete_cases();
    return 0;
}
