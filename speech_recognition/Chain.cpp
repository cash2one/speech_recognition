
#include "StdAfx.h"
#include "Chain.h"


Chain::Chain()
    :pre_(NULL),
    next_(NULL)
{
}

Chain::~Chain()
{
}

int Chain::next(unsigned cmd, void *data, int len)
{
    if (NULL == next_)
    {
        return -1;
    }

    return next_->next(cmd, data, len);
}

int Chain::pre(unsigned cmd, void *data, int len)
{
    if (NULL == pre_)
    {
        return -1;
    }

    return pre_->pre(cmd, data, len);
}
