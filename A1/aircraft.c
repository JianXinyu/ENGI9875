#include <stdlib.h>
#include <string.h>
#include "aircraft.h"

struct aircraft{
    enum aircraft_kind kind_;
    char *regcode_;
    unsigned int refcount_;
};

struct aircraft* ac_create(enum aircraft_kind kind, const char *reg)
{
    struct aircraft *ac = malloc(sizeof(struct aircraft));
    ac->kind_ = kind;
    ac->regcode_ = strdup(reg);
    ac->refcount_ = 1;
    return ac;
}

void ac_hold(struct aircraft *ac)
{
    ac->refcount_++;
}

void ac_release(struct aircraft *ac)
{
    if(ac->refcount_) ac->refcount_--;
    else free(ac);
}

unsigned int ac_refcount(const struct aircraft *ac)
{
    return ac->refcount_;
}

enum aircraft_kind	ac_kind(const struct aircraft *ac)
{
    return ac->kind_;
}

const char* ac_registration(const struct aircraft *ac)
{
    return ac->regcode_;
}