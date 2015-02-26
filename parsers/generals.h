#include "Parser-Combinators/stream_iterator.hpp"
#include "tools/logic.h"

#pragma once

template< typename T >
using handle = pstream_handle< eref< T > >;

template< typename T >
const handle< T > parser;

//--------------------------------------------------
//--------------------Expression kinds--------------
using qkind = quantifier::kind;
using tkind = type::kind;
using bkind = bop::kind;
using blkind = bool_lit::kind;
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
//--------------------------------------------------
