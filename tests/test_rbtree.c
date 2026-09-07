#include "rbtree.h"
#include <assert.h>

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

int main(void)
{
    test_root_insert();
    test_insert_black_parent();
    test_insert_red_parent_and_uncle();
    test_insert_red_parent_black_uncle();
    return 0;
}
