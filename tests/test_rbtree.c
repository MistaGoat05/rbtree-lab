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

    rb_destroy(t);
}

int main(void)
{
    test_root_insert();
    return 0;
}
