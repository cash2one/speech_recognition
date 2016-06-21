
#ifndef _CHAIN_H_
#define _CHAIN_H_

/**
 *\class  Chain
 *\brief  该类主要是责任链基础类
 *\date   2016-05-26
 *\author 张海涛
 *\par    Revision History:
 */
class Chain
{
public:
    Chain();

    ~Chain();

protected:
    Chain *pre_;  // 上一个节点
    Chain *next_; // 下一个节点

public:
    virtual int pre(unsigned cmd, void *data, int len);

    virtual int next(unsigned cmd, void *data, int len);

    Chain* getPre() { return pre_; }

    Chain* getNext() { return next_; }

    void setPre(Chain *chain) { pre_ = chain; }

    void setNext(Chain *chain) { next_ = chain; }
};

#endif
