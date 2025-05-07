#ifndef _VISITOR_
#define _VISITOR_

// forward declarations
namespace aging
{
class AgingBase;
class CalendarianAging;
class CyclicalAging;
class CalendarianAging2;
class CyclicalAging2;
class AnodeOverhang;
class GenericAging;
class AgingLAM;
class AgingLLI;
}    // namespace aging

namespace visitor
{

template < typename BaseClass >
class VisitorBase
{
};

template < typename BaseClass >
class Visitor : VisitorBase< BaseClass >
{
};

// visitor for aging::AgingBase
template <>
class Visitor< aging::AgingBase > : VisitorBase< aging::AgingBase >
{
    public:
    virtual void Visit( aging::CalendarianAging &aging ) = 0;
    virtual void Visit( aging::CyclicalAging &aging ) = 0;
    virtual void Visit( aging::CalendarianAging2 &aging ) = 0;
    virtual void Visit( aging::CyclicalAging2 &aging ) = 0;
    virtual void Visit( aging::AnodeOverhang &aging ) = 0;
    virtual void Visit( aging::GenericAging &aging ) = 0;
    virtual void Visit( aging::AgingLAM &aging ) = 0;
    virtual void Visit( aging::AgingLLI &aging ) = 0;
};
}    // namespace visitor

#endif /* _VISITOR_ */
