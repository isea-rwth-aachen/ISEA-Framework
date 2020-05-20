#ifndef _VISITOR_
#define _VISITOR_

// forward declarations
namespace aging
{
class AgingBase;
class CalendarianAging;
class CyclicalAging;
class AnodeOverhang;
class GenericAging;
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
    virtual void VisitCalendarianAging( aging::CalendarianAging &calendarianAging ) = 0;
    virtual void VisitCyclicalAging( aging::CyclicalAging &cyclicalAging ) = 0;
    virtual void VisitAnodeOverhang( aging::AnodeOverhang &anodeOverhang ) = 0;
    virtual void VisitGenericAging( aging::GenericAging &genericAging ) = 0;
};
}    // namespace visitor

#endif /* _VISITOR_ */
