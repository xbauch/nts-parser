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
//--------------------------------------------------
//--------------------Expression kinds--------------
using qkind = quantifier::kind;
using tkind = type::kind;
using bkind = bop::kind;
using blkind = bool_lit::kind;
using akind = aop::kind;
using skind = sign::kind;
using rkind = rop::kind;
//--------------------------------------------------

//--------------------------------------------------
//--------------------Generic expression builders---
template< typename T, typename T::kind op >
struct build_op {
  build_op() {}
  void operator() ( eref< T >* res, string& ) const {
    *res = itemise( T( op ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Generic constant builders-----
template< typename T >
struct build_constant {
  build_constant() {}
  void operator() ( eref< T >* res, string& name ) const {
    *res = itemise( T( name ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Generic variable builders-----
template< typename T >
struct build_variable {
  build_variable() {}
  void operator() ( eref< T >* res, string& name ) const {
    *res = itemise( T( name ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Generic binop builders--------
template< typename Result, typename Operator, typename Operand >
struct build_binop {
  build_binop() {}
  void operator() ( eref< Result >* res, eref< Operand > l,
                    eref< Operator > op, eref< Operand > r ) const {
    *res = itemise( Result( l, op, r ) );
  }
};

template< typename Result, typename Operator, typename LOperand, typename ROperand >
struct build_binop_as {
  build_binop_as() {}
  void operator() ( eref< Result >* res, eref< LOperand > l,
                    eref< Operator > op, eref< ROperand > r ) const {
    *res = itemise( Result( l, op, r ) );
  }
};

template< typename Result, typename Par, typename Operator, typename LOperand,
          typename ROperand >
struct build_binop_as_par {
  build_binop_as_par() {}
  void operator() ( eref< Result >* res, eref< Par > p, eref< LOperand > l,
                    eref< Operator > op, eref< ROperand > r ) const {
    *res = itemise( Result( p, l, op, r ) );
  }
};

template< typename Result, typename Operator, typename Operand >
struct build_binop_par {
  build_binop_par() {}
  void operator() ( eref< Result >* res, eref< Operand > l, string&,
                    eref< Operator > op, eref< Operand > r ) const {
    *res = itemise( Result( l, op, r ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Generic wrappers--------------
template< typename T1, typename T2 >
struct build_itemise {
  build_itemise() {}
  void operator() ( eref< T1 >* res, eref< T2 > v ) const {
    *res = itemise( T1( v ) );
  }
};

template< typename T1, typename T2 >
struct build_itemise_true {
  build_itemise_true() {}
  void operator() ( eref< T1 >* res, eref< T2 > v ) const {
    *res = itemise( T1( v, true ) );
  }
};

template< typename R, typename T1, typename T2 >
struct build_itemise_two {
  build_itemise_two() {}
  void operator() ( eref< R >* res, eref< T1 > t1, eref< T2 > t2 ) const {
    *res = itemise( R( t1, t2 ) );
  }
};

template< typename R, typename T1, typename T2 >
struct build_itemise_two_ss {
  build_itemise_two_ss() {}
  void operator() ( eref< R >* res, eref< T1 > t1, string&, string&,
                    eref< T2 > t2 ) const {
    *res = itemise( R( t1, t2 ) );
  }
};

template< typename R, typename T1, typename T2 >
struct build_itemise_three_ss {
  build_itemise_three_ss() {}
  void operator() ( eref< R >* res, eref< T1 > t1, string&, eref< T2 > t2, string&,
                    eref< T2 > t3 ) const {
    *res = itemise( R( t1, t2, t3 ) );
  }
};

template< typename R, typename T1, typename T2, typename T3 >
struct build_itemise_three {
  build_itemise_three() {}
  void operator() ( eref< R >* res, eref< T1 > t1, eref< T2 > t2,
                    eref< T3 > t3 ) const {
    *res = itemise( R( t1, t2, t3 ) );
  }
};

template< typename R, typename T1, typename T2, typename T3 >
struct build_itemise_three_true {
  build_itemise_three_true() {}
  void operator() ( eref< R >* res, eref< T1 > t1, eref< T2 > t2,
                    eref< T3 > t3 ) const {
    *res = itemise( R( t1, t2, t3, true ) );
  }
};

/*struct build_index_term_par_aop {
  build_index_term_par_aop() {}
  void operator() ( eref< index_term >* res, eref< index_term > t1, string&,
                    eref< aop > a, eref< index_term > t2 ) const {
    *res = itemise( index_term( t1, a, t2 ) );
  }
  };*/

// template< typename R, typename T1, typename T2, typename T3, typename T4 >
// struct build_itemise_four {
//   build_itemise_four() {}
//   void operator() ( eref< R >* res, eref< T1 > t1, eref< T2 > t2,
//                     eref< T3 > t3, eref< T4 > t4 ) const {
//     *res = itemise( R( t1, t2, t3, t4 ) );
//   }
// };
 
struct build_formula_bop {
  build_formula_bop() {}
  void operator() ( eref< formula >* res, eref< quantifier > t1, eref< idn_list > t2,
                    string&, eref< q_type > t3, string&, eref< formula > t4,
                    eref< bop > t5, eref< formula > t6 ) const {
    *res = itemise( formula( t1, t2, t3, t4, t5, t6 ) );
  }
};

struct build_formula {
  build_formula() {}
  void operator() ( eref< formula >* res, eref< quantifier > t1, eref< idn_list > t2,
                    string&, eref< q_type > t3, string&, eref< formula > t4 ) const {
    *res = itemise( formula( t1, t2, t3, t4 ) );
  }
};

template< typename T >
struct build_empty {
  build_empty() {}
  void operator() ( eref< T >* res, string& ) const {
    *res = itemise( T() );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Vector parsers----------------
template< typename T >
struct build_cat {
  build_cat() {}
  void operator() ( vref< T >* res, eref< T > a, string&, vref< T > v ) const {
    *res = { a };
    copy( v.begin(), v.end(), back_inserter( *res ) );
  }
};

template< typename T >
struct build_vectorise {
  build_vectorise() {}
  void operator() ( vref< T >* res, eref< T > v ) const {
    cout << "build_vectorise" << endl;
    *res = { v };
  }
};

template< typename Res, typename T1, typename T2 >
struct build_eref_s_vref {
  build_eref_s_vref() {}
  void operator() ( eref< Res >* res, eref< T1 > t1, string&, vref< T2 > t2 ) const {
    *res = itemise( Res( t1, t2 ) );
  }
};

template< typename T1, typename T2 >
struct build_itemise_vector {
  build_itemise_vector() {}
  void operator() ( eref< T1 >* res, vref< T2 > v ) const {
    *res = itemise( T1( v ) );
  }
};

template< typename T1, typename T2 >
struct build_itemise_vvector {
  build_itemise_vvector() {}
  void operator() ( eref< T1 >* res, vref< T2 > v ) const {
    *res = v[ 0 ];
  }
};

template< typename Res, typename T1 >
struct build_vref_s_vref {
  build_vref_s_vref() {}
  void operator() ( eref< Res >* res, vref< T1 > t1, string&, vref< T1 > t2 ) const {
    *res = itemise( Res( t1, t2 ) );
  }
};

struct build_index_term_arr_aop {
  build_index_term_arr_aop() {}
  void operator() ( vref< index_term >* res, eref< idn > i, string&,
                    vref< index_term > t1, string&, eref< aop > a,
                    vref< index_term > t2 ) const {
    *res = { itemise( index_term( itemise( index_term( i, t1 ) ), a, t2[ 0 ] ) ) };
    if ( t2.size() > 1 )
      copy( t2.begin() + 1, t2.end(), back_inserter( *res ) );
  }
};

struct build_index_term_lit_aop {
  build_index_term_lit_aop() {}
  void operator() ( vref< index_term >* res, eref< arith_lit > i,
                    eref< aop > a, vref< index_term > t2 ) const {
    *res = { itemise( index_term( i, a, t2[ 0 ] ) ) };
    if ( t2.size() > 1 )
      copy( t2.begin() + 1, t2.end(), back_inserter( *res ) );
  }
};

struct build_index_term_par_aop {
  build_index_term_par_aop() {}
  void operator() ( vref< index_term >* res, vref< index_term > t1, string&,
                    eref< aop > a, vref< index_term > t2 ) const {
    *res = { itemise( index_term( itemise( index_term( t1[ 0 ] ) ), a, t2[ 0 ] ) ) };
    if ( t2.size() > 1 )
      copy( t2.begin() + 1, t2.end(), back_inserter( *res ) );
  }
};

struct build_index_term_arr {
  build_index_term_arr() {}
  void operator() ( vref< index_term >* res, eref< idn > i, string&,
                    vref< index_term > t1 ) const {
    cout << "size: " << t1.size() << endl;
    *res = { itemise( index_term( i, t1 ) ) };
  }
};

template< typename R, typename T >
struct build_vectorise_con {
  build_vectorise_con() {}
  void operator() ( vref< R >* res, eref< T > v ) const {
    *res = { itemise( R( v ) ) };
  }
};

template< typename R, typename T >
struct build_vectorise_vcon {
  build_vectorise_vcon() {}
  void operator() ( vref< R >* res, vref< T > v ) const {
    *res = { itemise( R( v[ 0 ] ) ) };
  }
};

struct build_index_term_arr_aop_cat {
  build_index_term_arr_aop_cat() {}
  void operator() ( vref< index_term >* res, eref< idn > i, string&,
                    vref< index_term > t1, string&, eref< aop > o,
                    vref< index_term > t2, string&, vref< index_term > t3 ) const {
    *res = { itemise( index_term( itemise( index_term( i, t1 ) ), o, t2[ 0 ] ) ) };
    copy( t3.begin(), t3.end(), back_inserter( *res ) );
  }
};

struct build_index_term_lit_aop_cat {
  build_index_term_lit_aop_cat() {}
  void operator() ( vref< index_term >* res, eref< arith_lit > l, eref< aop > o,
                    vref< index_term > t1, string&, vref< index_term > t2 ) const {
    cout << "fuu" << endl;
    *res = { itemise( index_term( itemise( index_term( l ) ), o, t1[ 0 ] ) ) };
    copy( t2.begin(), t2.end(), back_inserter( *res ) );
  }
};

struct build_index_term_par_aop_cat {
  build_index_term_par_aop_cat() {}
  void operator() ( vref< index_term >* res, vref< index_term > t1, string&,
                    eref< aop > o, vref< index_term > t2, string&,
                    vref< index_term > t3 ) const {
    *res = { itemise( index_term( itemise( index_term( t1[ 0 ] ) ), o, t2[ 0 ] ) ) };
    copy( t3.begin(), t3.end(), back_inserter( *res ) );
  }
};

struct build_index_term_arr_cat {
  build_index_term_arr_cat() {}
  void operator() ( vref< index_term >* res, eref< idn > i, string&,
                    vref< index_term > t1, string&, string&,
                    vref< index_term > t2 ) const {
    *res = { itemise( index_term( i, t1 ) ) };
    copy( t2.begin(), t2.end(), back_inserter( *res ) );
  }
};

struct build_index_term_lit_cat {
  build_index_term_lit_cat() {}
  void operator() ( vref< index_term >* res, eref< arith_lit > l, string&,
                    vref< index_term > t2 ) const {
    *res = {itemise( index_term( l ) ) };
    copy( t2.begin(), t2.end(), back_inserter( *res ) );
  }
};

struct build_index_term_par_cat {
  build_index_term_par_cat() {}
  void operator() ( vref< index_term >* res, vref< index_term > t1, string&,
                    string&, vref< index_term > t3 ) const {
    *res = { itemise( index_term( t1[ 0 ] ) ) };
    copy( t3.begin(), t3.end(), back_inserter( *res ) );
  }
};

//--------------------------------------------------
