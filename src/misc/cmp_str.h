#ifndef CMP_STR_H_
#define CMP_STR_H_



namespace misc
{

///cmp_str is a functor returning case insensitive comparison of a two c style string
struct cmp_str
{
    bool operator()( const char *first,
                     const char *second ) const;
};

///equal_str is a functor returning case insensitive equality of a two c style string
struct equal_str
{
    bool operator()( const char *first,
                     const char *second ) const;
};

}

#endif
