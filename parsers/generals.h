#include "Parser-Combinators/stream_iterator.hpp"
#include "tools/logic.h"

#pragma once

class reject_str {
  string const s;

public:
  using is_parser_type = true_type;
  using is_handle_type = false_type;
  using result_type = string;
  int const rank = 0;
  string const name;

  explicit reject_str( const string& s ) : s( s ), name( "\"" + s + "\"" ) {}

  template< typename Iterator, typename Range >
  bool operator() ( Iterator& i, const Range& r, string* result = nullptr ) const {
    bool ok = false;
    for ( auto it = s.begin(); it != s.end(); ++it ) {
      if ( *i == *it || i == r.last ) {
        ok = true;
        break;
      }
      ++i;
    }
    if ( i != r.last && ::isalnum( *(i+1) ) )
      ok = true;
    if ( ok && result != nullptr )
      result->append( s);
    return ok;
      
  }
};

class reject_strs {
  const unordered_set< string >& uss;

public:
  using is_parser_type = true_type;
  using is_handle_type = false_type;
  using result_type = string;
  int const rank = 0;
  string const name;

  explicit reject_strs( const unordered_set< string >& u ) : uss( u ), name( "uss" ) {}

  template< typename Iterator, typename Range >
  bool operator() ( Iterator& i, const Range& r, string* result = nullptr ) const {
    string s;
    for ( Iterator j = i; j != r.last && ::isalpha( *j ); ++j )
      s.push_back( *j );
    return (uss.count( s ) == 0);
  }
};

template< typename T >
using handle = pstream_handle< eref< T > >;

template< typename T >
const handle< T > parser;

template< typename T >
using vhandle = pstream_handle< vref< T > >;

template< typename T >
const vhandle< T > vparser_sq;
template< typename T >
const vhandle< T > vparser_cm;

template< typename T, typename... Ts >
struct builder_t {
  builder_t() {}
  void operator() ( eref< T >* res, Ts... ts ) const {
    *res = itemise( T( ts... ) );
  }
};

template< typename... Ts >
builder_t< Ts... > builder = builder_t< Ts... >();

template< typename T, typename... Ts >
const handle< T > caller( string log_name, Ts... ts ) {
  return log( log_name, all( builder< T, typename Ts::result_type... >, ts... ) );
}

template< typename T, typename P, typename S, typename... Ts >
struct consbuilder_t {
  consbuilder_t() {}
  void operator() ( vref< T >* res, Ts... ts, P p, S s ) const {
    *res = s;
    res->push_back( itemise( T( ts... ) ) );
  }
};

template< typename... Ts >
consbuilder_t< Ts... > consbuilder = consbuilder_t< Ts... >();

template< typename T, typename P, typename S, typename... Ts >
const vhandle< T > conscaller( string log_name, P p, S s, Ts... ts ) {
  return log( log_name, all( consbuilder< T, typename P::result_type,
                             typename S::result_type, typename Ts::result_type... >,
                             ts..., p, s ) );
}

template< typename T, typename... Ts >
struct lastbuilder_t {
  lastbuilder_t() {}
  void operator() ( vref< T >* res, Ts... ts ) const {
    *res = { itemise( T( ts... ) ) };
  }
};

template< typename... Ts >
lastbuilder_t< Ts... > lastbuilder = lastbuilder_t< Ts... >();

template< typename T, typename... Ts >
const vhandle< T > lastcaller( string log_name, Ts... ts ) {
  return log( log_name, all( lastbuilder< T, typename Ts::result_type... >, ts... ) );
}
