#include <functional>
#include <vector>
#include <tuple>
#include <algorithm>

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

template< typename Fun, typename OutT, typename... Ts, size_t... Is >
vector< OutT > for_each( const tuple< Ts... >& tuple, Fun fun,
                         index_sequence< Is... > ) {
  vector< OutT > ret_vec = { OutT(), fun( get< Is >( tuple ) )... };
  return vector< OutT >( ret_vec.begin() + 1, ret_vec.end() );
}

template< typename OutT, typename... Ts, typename F >
vector< OutT > for_each( F f, const tuple< Ts... >& tuple ) {
  return for_each< F, OutT >( tuple, f, make_index_sequence< sizeof...( Ts ) >() );
}

template< typename F, typename T >
void map_inplace( F f, std::vector< T >& in ) {
  for ( auto& e : in ) {
    f( e );
  }
}

template< typename Out, typename F, typename In >
Out map( F f, In&, typename In::iterator fst, typename In::iterator lst ) {
  Out o;
  for( ; fst != lst; ++fst ) 
    o.push_back( f( *fst ) );
  return o;
}

template< typename Out, typename F, typename In >
Out map( F f, In in ) { return map< Out >( f, in, in.begin(), in.end() ); }
 
template< typename T >
T cat( T first, T second ) {
  std::copy( second.begin(), second.end(), std::back_inserter( first ) );
  return first;
}

template< typename T, typename... Vs >
T cat( T first, T second, Vs... vs ) {
  std::copy( second.begin(), second.end(), std::back_inserter( first ) );
  return cat( first, vs... );
}

template< typename Out, typename In >
vector< Out > vconvert( vector< In > in ) {
  return map< vector< Out > >( []( auto a ) { return Out( a ); }, in );
}

template< typename Out, typename In >
vector< Out > vconvert_reverse( vector< In > in ) {
  vector< Out > o = map< vector< Out > >( []( auto a ) { return Out( a ); }, in );
  reverse( o.begin(), o.end() );
  return o;
}
