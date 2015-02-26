#include <functional>
#include <vector>

#pragma once

using namespace std;

template< typename L, typename R >
L fold( function< L (L, R) > op, L fst, vector< R > vec ) {
  for ( auto& e : vec ) {
    fst = op( fst, e );
  }
  return fst;
}

template< typename T >
vector< T > zip( vector< T > one, vector< T > two ) {
  vector< T > ret;
  vector< T >& shorter = (one.size() < two.size()) ? one : two;
  vector< T >& longer = (one.size() < two.size()) ? two : one;
  int i;
  for( i = 0; i < shorter.size(); i++ ) {
    ret.push_back( one[ i ] );
    ret.push_back( two[ i ] );
  }
  for( ; i < longer.size(); i++ )
    ret.push_back( longer[ i ] );
  return ret;
}
