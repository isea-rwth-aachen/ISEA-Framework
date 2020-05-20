/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : dgl_state.h
* Creation Date : 11-11-2012
* Last Modified : Mo 08 Sep 2014 15:27:35 CEST
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _DGL_STATE_
#define _DGL_STATE_

#include "state.h"

namespace state
{

class Dgl_state : public State
{
    public:
    Dgl_state(){};
    virtual ~Dgl_state(){};
};
}    // namespace state

#endif /* _DGL_STATE_ */
