#include <utility> //pair
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <cassert>

#include "tools/jenkins.h"
#include "tools/functional.h"

#pragma once

using namespace std;

template< typename T, typename = typename T::is_hashable_type >
struct hasher {
  size_t operator()( const T& t ) const {
    return t.hash();
  }
};

template< typename T, typename = typename T::is_equalable_type >
struct equaler {
  size_t operator()( const T& t1, const T& t2 ) const {
    return t1 == t2;
  }
};

template< typename T >
struct bimap {
  using SB = pair< size_t, bool >;
  using Hash = hasher< T >;
  using Equal = equaler< T >;
  
  unordered_map< T, size_t, Hash, Equal > t2u;
  vector< T > u2t;

  SB insert( const T& t ) {
    size_t id = u2t.size();
    pair< T, size_t > p( t, id );
    auto suc = t2u.insert( p );
    SB retVal;
    if ( suc.second ) {
      u2t.push_back( t );
      return { id, true };
    }
    return { suc.first->second, false };
  }

  const T& get_value( size_t u ) const {
    assert( u < u2t.size() );
    return u2t[ u ];
  }

  size_t size() {
    return u2t.size();
  }
};

template< typename T >
bimap< T > symbol_storage = bimap< T >();

template< typename T, typename F = typename T::is_referenceable >
struct eref {
  using reference_type = T;

  size_t id;

  eref() {}
  eref( size_t _i ) : id( _i ) {}
  eref( eref& r ) : id( r.id ) {}
  eref( const eref& r ) : id( r.id ) {}
  eref& operator=( eref other ) {
    swap( *this, other );
    return *this;
  }

  friend void swap( eref& fst, eref& snd ) {
    std::swap( fst.id, snd.id );
  }
  
  operator size_t() const {
    return id;
  }
};

template< typename T >
T deitemise( const eref< T >& r ) {
  return symbol_storage< T >.get_value( r );
}

template< typename T >
eref< T > itemise( const T& r ) {
  return symbol_storage< T >.insert( r ).first;
}

template< typename T >
using vref = std::vector< eref< T > >;

template< typename T, char delim = ',', char open = '[', char close = ']' >
std::ostream& operator<<( std::ostream& o, const vref< T >& x ) {
  o << open;
  for ( auto i = x.begin(); i != x.end(); i++ ) {
    if ( i != x.begin() )
      o << delim << " ";
    o << deitemise< T >( *i ).print();
  }
  o << close;
  return o;
}

struct symbol {
  using is_hashable_type = std::true_type;
  using is_equalable_type = std::true_type;
};

struct expression : symbol {
  using is_referenceable = true_type;

  using vsize_t = vector< size_t >;
  using vstr_t = vector< string >;

  vsize_t flat;
  size_t _hash;

  size_t _h() {
    auto p = jenkins::SpookyHash::Hash128( (void*)flat.data(),
                                           sizeof( size_t ) * flat.size(), 0, 0 );
    return p.first ^ p.second;
  }

  expression() = delete;
  expression( vsize_t v ) : flat( v ), _hash( _h() ) {}

  string print() { return ""; }

  size_t hash() const {
    return _hash;
  }

  bool operator==( const expression& other ) const {
    return flat == other.flat;
  }

  template< typename T >
  T get( size_t id = 1 ) {
    return deitemise< T >( flat[ id ] );
  }

  string printer( vstr_t fst, vstr_t snd ) {
    vstr_t s = zip( fst, snd );
    return fold< string, string >(
      [](string s1, string s2) -> string { return s1 + s2; }, "", s );
  }

  template< typename... Ts >
  string printer( const tuple< Ts... >& in, vstr_t snd ) {
    return printer( for_each< string, Ts... >( [] (auto e) { return e.print(); },
                                               in ),
                    snd );
  }

  template< typename FType, typename SType, typename... RType >
  tuple< FType, SType, RType... > extract( size_t fst ) {
    return tuple_cat( make_tuple( deitemise< FType >( flat[ fst ] ) ),
                      extract< SType, RType... >( fst + 1 ) );
  }

  template< typename T >
  tuple< T > extract( size_t fst ) {
    return make_tuple( deitemise< T >( flat[ fst ] ) );
  }

  template< typename... Ts >
  tuple< Ts... > extractor( size_t fst = 1 ) {
    return extract< Ts... >( fst );
  }

};

template< typename T >
struct unioned {
  union {
    T i;
    size_t v;
  };

  unioned() = delete;
  unioned( size_t _v ) : v( _v ) {}
  unioned( T _i ) : i( _i ) {}
  operator T() const {
    return i;
  }
  operator size_t() const {
    return v;
  }
};
