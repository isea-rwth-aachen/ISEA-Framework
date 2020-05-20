/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : mapGetSet.h
* Creation Date : 26-08-2013
* Last Modified : Do 20 Feb 2014 19:39:03 CET
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MAPGETSET_
#define _MAPGETSET_
#include <map>
#include <functional>

class TestRectangularBlock;

template < typename First, typename Second, class Compare = std::less<First> >
class MapGetSet
{
    friend class ::TestRectangularBlock;
    public:
        MapGetSet();
        ~MapGetSet(){};
        void Add(const First& position, const Second &value);
        std::vector<Second> Get(const First& position) const;
    private:
        std::map<First, std::vector< Second >, Compare > mFirst2Second;
        typedef typename std::map<First, std::vector< Second > >::iterator iterator;
        typedef typename std::map<First, std::vector< Second > >::const_iterator const_iterator;
    protected:
};


template < typename First, typename Second, class Compare >
MapGetSet<First, Second, Compare >::MapGetSet()
{}

template < typename First, typename Second, class Compare >
void MapGetSet<First, Second, Compare>::Add(const First& position, const Second &value)
{
      iterator it = mFirst2Second.find(position) ;
      if( it == mFirst2Second.end() )
      {
          std::vector<Second> secondVector;
          secondVector.push_back(value);
          mFirst2Second[position] = secondVector;
      }
      else
          it->second.push_back(value);
}

template < typename First, typename Second, class Compare >
std::vector<Second> MapGetSet<First, Second, Compare>::Get(const First& position) const
{
      const_iterator it = mFirst2Second.find(position) ;
      if( it == mFirst2Second.end() )
          return std::vector<Second>();
      return it->second;
}


#endif /* _MAPGETSET_ */
