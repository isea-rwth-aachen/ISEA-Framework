#ifndef _STATE_
#define _STATE_

namespace state
{

class State
{
    public:
    State(){};
    virtual ~State(){};

    /// Deprecated
    virtual double GetValue() const = 0;
    virtual double GetLastValue() const = 0;
    virtual double operator()() const { return GetValue(); };
};
}

#endif
