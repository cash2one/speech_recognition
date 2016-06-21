
#ifndef _CHAIN_H_
#define _CHAIN_H_

/**
 *\class  Chain
 *\brief  ������Ҫ��������������
 *\date   2016-05-26
 *\author �ź���
 *\par    Revision History:
 */
class Chain
{
public:
    Chain();

    ~Chain();

protected:
    Chain *pre_;  // ��һ���ڵ�
    Chain *next_; // ��һ���ڵ�

public:
    virtual int pre(unsigned cmd, void *data, int len);

    virtual int next(unsigned cmd, void *data, int len);

    Chain* getPre() { return pre_; }

    Chain* getNext() { return next_; }

    void setPre(Chain *chain) { pre_ = chain; }

    void setNext(Chain *chain) { next_ = chain; }
};

#endif
