#include <string>

#include "tools/data.h"

#pragma once

using namespace std;

//--------------------------------------------------
//--------------------Tokens syntax-----------------
//<type> ::= {int,real,bool}
struct type : expression {
  enum kind { integer, real, boolean };

  type( kind k ) : expression( { k } ) {}

  string print() {
    switch( flat[ 0 ] ) {
    case integer : return "int";
    case real : return "real";
    case boolean : return "bool";
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< type > symbol_storage< type >;

template< typename T >
struct constant : expression {  
  constant( unioned< T > i ) : expression( { i } ) {}
  constant( string s ) : expression( { unioned< T >( convert( s ) ) } ) {}

  string print() {
    stringstream ss;
    unioned< T > i( flat[ 0 ] );
    ss << boolalpha;
    ss << T(i);
    return ss.str();
  }

  T convert( string s ) {
    if ( is_same< T, int >() )
      return stoi( s );
    if ( is_same< T, double >() )
      return stod( s );
    if ( is_same< T, bool >() )
      return (s == "true");
  }
};

using numeral = constant< int >;
using decimal = constant< double >;
using boolean = constant< bool >;

template<>
bimap< boolean > symbol_storage< boolean >;
template<>
bimap< numeral > symbol_storage< numeral >;
template<>
bimap< decimal > symbol_storage< decimal >;

using primed = true_type;
using unprimed = false_type;

template< typename T >
struct variable : symbol {
  using is_referenceable = true_type;

  string name;
  size_t _hash;
  
  variable( std::string n ) : name( n ) {
    std::hash< string > hfn;
    _hash = hfn( name );
  }

  size_t hash() const {
    return _hash;
  }

  bool operator==( const variable& other ) const {
    return name == other.name;
  }

  string print() {
    return name;// + (T() ? "'" : "");
  }
};

using idn = variable< unprimed >;
using idp = variable< primed >;

template<>
bimap< idp > symbol_storage< idp >;
template<>
bimap< idn > symbol_storage< idn >;

struct id : expression {
  enum kind { id_n, id_p };

  id( eref< idn > i ) : expression( { id_n, i } ) {}
  id( eref< idp > i ) : expression( { id_p, i } ) {}

  std::string print() {
    if ( flat[ 0 ] == id_n )
      return get< idn >().print();
    else
      return get< idp >().print();
  }
};

template<>
bimap< id > symbol_storage< id >;
//--------------------------------------------------

//--------------------------------------------------
//----------syntax of booleans----------------------
//<bop> ::= {and,or,imply,equiv,&&,||,->,<->}
struct bop : expression {
  enum kind { band, bor, imply, equiv };

  bop( kind k ) : expression( { k } ) {}

  std::string print() {
    switch( flat[ 0 ] ) {
    case band : return "/\\";
    case bor : return "\\/";
    case imply : return "->";
    case equiv : return "<->";
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< bop > symbol_storage< bop >;

//<bool-lit> ::= <id> |
//               true |
//               false
struct bool_lit : expression {
  enum kind { bl_id, bl_true, bl_false };

  bool_lit( eref< id > i ) : expression( { bl_id, i } ) {}
  bool_lit( kind k ) : expression( { k } ) {
    assert( k != bl_id );
  }

  std::string print() {
    switch( flat[ 0 ] ) {
    case bl_id : return get< id >().print();
    case bl_true : return "true";
    case bl_false : return "false";
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< bool_lit > symbol_storage< bool_lit >;

//<bool-term> ::= <bool-lit> |
//                ( <bool-term> ) |
//                <not> <bool-term> |
//                <bool-term> <bop> <bool-term>
struct bool_term : expression {
  enum kind { literal, parenthesised, negated, relation };

  bool_term( eref< bool_lit > l ) : expression( { literal, l } ) {}
  bool_term( eref< bool_term > t, bool n = false ) :
    expression( { (n ? negated : parenthesised), t } ) {}
  bool_term( eref< bool_lit > l, eref< bop > o, eref< bool_term > r ) :
    expression( { relation, itemise< bool_term >( bool_term( l ) ), o, r } ) {}
  bool_term( eref< bool_term > l, eref< bop > o, eref< bool_term > r ) :
    expression( { relation, itemise< bool_term >( bool_term( l ) ), o, r } ) {}

  string print() {
    switch( flat[ 0 ] ) {
    case literal : return get< bool_lit >().print();
    case parenthesised : return printer( { "(", ")" },
                                         { get< bool_term >().print() } );
    case negated : return "~" + get< bool_term >().print();
    case relation : return printer( extractor< bool_term, bop, bool_term >(),
                                    { " ", " " } );
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< bool_term > symbol_storage< bool_term >;
//--------------------------------------------------

//--------------------------------------------------
//----------Arithmetic syntax-----------------------
//<arith-lit> ::= <id> |
//                tid |
//                <numeral> |
//                <decimal>
struct arith_lit : expression {
  enum kind { arith_id, arith_tid, arith_num, arith_dec };

  arith_lit( eref< id > i ) : expression( { arith_id, i } ) {}
  arith_lit() : expression( { arith_tid } ) {}
  arith_lit( eref< numeral > n ) : expression( { arith_num, n } ) {}
  arith_lit( eref< decimal > d ) : expression( { arith_dec, d } ) {}

  std::string print() {
    switch( flat[ 0 ] ) {
    case arith_id : return get< id >().print();
    case arith_tid : return "tid";
    case arith_num : return get< numeral >().print();
    case arith_dec : return get< decimal >().print();
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< arith_lit > symbol_storage< arith_lit >;

struct aop : expression {
  enum kind { plus, minus, times, div, srem, urem };

  aop( kind k ) : expression( { k } ) {}

  std::string print() {
    switch( flat[ 0 ] ) {
    case plus : return "+";
    case minus : return "-";
    case times : return "*";
    case div : return "/";
    case srem : case urem : return "%";
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< aop > symbol_storage< aop >;

// <index-term> ::= <arith-lit>
//                | <idn> [ <index-term> ]
//                | <index-term> <aop> <index-term>
//                | ( <index-term> )
//                | <index-term> [ <index-term> ]
struct index_term : expression {
  enum kind { literal, array, function, parenthesised };

  index_term( eref< idn > i, vref< index_term > t, eref< aop > a,
              eref< index_term > s ) :
    expression( { function, itemise< index_term >( index_term( i, t ) ), a, s } ) {}
  index_term( eref< arith_lit > l, eref< aop > a, eref< index_term > t ) :
    expression( { function, itemise< index_term >( index_term( l ) ), a, t } ) {}
  // index_term( eref< index_term > t, eref< aop > a, eref< index_term > i ) :
  //   expression( { function, itemise< index_term >( index_term( t ) ), a, i } ) {}
  
  index_term( eref< idn > i, vref< index_term > t ) :
    expression( cat( { array, i }, vconvert< size_t >( t ) ) ) {}
  index_term( eref< arith_lit > l ) : expression( { literal, l } ) {}
  index_term( eref< index_term > t ) : expression( { parenthesised, t } ) {}

  index_term( eref< index_term > l, eref< aop > a, eref< index_term > r ) :
    expression( { function, l, a, r } ) {}

  string print() {
    switch ( flat[ 0 ] ) {
    case literal : return get< arith_lit >().print();
    case function : return printer( extractor< index_term, aop, index_term >(),
                                    { " ", " " } );
    case parenthesised : return printer( { "(", ")" },
                                         { get< index_term >().print() } );
    }
    if ( flat[ 0 ] != array ) {
      assert( false );
      return "";
    }
    
    auto v = map< vector< string > >(
      [&] (size_t e) { return deitemise< index_term >( e ).print(); }, flat,
      flat.begin() + 2, flat.end() );
    return get< idn >().print() +
      "[" + printer( v, vstr_t( v.size() - 1, "][" ) ) + "]";
  }
};

template<>
bimap< index_term > symbol_storage< index_term >;

// <array-read> ::= <idn> [ <index-term> ]
//                | <array-read> [ <index-term> ]
struct array_read : expression {
  array_read( eref< idn > i, vref< index_term > v ) :
    expression( cat( { i }, vconvert< size_t >( v ) ) ) {
    assert( !v.empty() );
  }

  string print() {
    auto v = map< vector< string > >(
      [&](size_t e) { return deitemise< index_term >( e ).print(); }, flat,
      flat.begin() + 1, flat.end() );
    return get< idn >( 0 ).print() +
           "[" + printer( v, vstr_t( v.size() - 1, "][" ) ) + "]";
  }
};

template<>
bimap< array_read > symbol_storage< array_read >;

//<array-term> ::= <array-read> |
//                 |<idn>|
struct array_term : expression {
  enum kind { array, size };

  array_term( eref< array_read > a ) : expression( { array, a } ) {}
  array_term( eref< idn > i ) : expression( { size, i } ) {}

  std::string print() {
    if ( flat[ 0 ] == array )
      return get< array_read >().print();
    else
      return "|" + get< idn >().print() + "|";
  }
};

template<>
bimap< array_term > symbol_storage< array_term >;


//<sign> ::= {-} | \epsilon
struct sign : expression {
  enum kind { sign_true, sign_false };

  sign( kind k ) : expression( { k } ) {}

  std::string print() {
    return (flat[ 0 ] == sign_true) ? "-" : "";
  }
};

template<>
bimap< sign > symbol_storage< sign >;

//<arith-term> ::= <sign> <arith-lit> |
//                 <sign> <array-term> |
//                 ( <arith-term> ) |
//                 <arith-term> <aop> <arith-term>
struct arith_term : expression {
  enum kind { literal, array, parenthesised, function };

  arith_term( eref< sign > s, eref< arith_lit > l ) :
    expression( { literal, s, l } ) {}
  arith_term( eref< sign > s, eref< array_term > a ) :
    expression( { array, s, a } ) {}
  arith_term( eref< arith_term > t ) : expression( { parenthesised, t } ) {}
  arith_term( eref< sign > s, eref< arith_lit > l, eref< aop > o,
              eref< arith_term > r ) :
    expression( { function, itemise< arith_term >( arith_term( s, l ) ), o, r } ) {}
  arith_term( eref< sign > s, eref< array_term > a, eref< aop > o,
              eref< arith_term > r ) :
    expression( { function, itemise< arith_term >( arith_term( s, a ) ), o, r } ) {}
  arith_term( eref< arith_term > l, eref< aop > o, eref< arith_term > r ) :
    expression( { function, itemise< arith_term >( arith_term( l ) ), o, r } ) {}

  std::string print() {
    switch( flat[ 0 ] ) {
    case literal : return printer( extractor< sign, arith_lit >(), { "" } );
    case array : return printer( extractor< sign, array_term >(), { "" } );
    case parenthesised : return printer( { "(", ")" },
                                         { get< arith_term >().print() } );
    case function : return printer( extractor< arith_term, aop, arith_term >(),
                                    { " ", " " } );
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< arith_term > symbol_storage< arith_term >;

//<arith-list> ::= <arith-term> |
//                 <arith-list> , <arith-term>
struct arith_list : expression {
  arith_list( vref< arith_term > l ) :
    expression( vconvert< size_t >( l ) ) {}

  std::string print() {
    auto v = map< vector< string > >(
      [&]( size_t e ) { return deitemise< arith_term >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};

template<>
bimap< arith_list > symbol_storage< arith_list >;

struct arith_list_e : expression {
  arith_list_e() : expression( vsize_t() ) {}
  arith_list_e( vref< arith_term > l ) :
    expression( vconvert< size_t >( l ) ) {}

  std::string print() {
    if ( flat.empty() )
      return "";
    auto v = map< vector< string > >(
      [&]( size_t e ) { return deitemise< arith_term >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};

template<>
bimap< arith_list_e > symbol_storage< arith_list_e >;

//<multi> ::= [ <arith-term> ] <multi> |
//            [ <arith-list> ] |
//            \epsilon
struct multi : expression {
  multi() : expression( vsize_t() ) {}
  multi( vref< arith_term > a, vref< arith_term > l ) :
    expression( cat( { a.size() - 1 }, vconvert< size_t >( a ),
                     vconvert< size_t >( l ) ) ) {
    assert( !l.empty() );
  }
  multi( vref< arith_term > l ) : expression( cat( { size_t( 0 ) },
                                                   vconvert< size_t >( l ) ) ) {
    assert( !l.empty() );
  }

  std::string print() {
    if ( flat.empty() )
      return "";
    auto v1 = map< vector< string > >(
      [&]( size_t e ) { return deitemise< arith_term >( e ).print(); }, flat,
      flat.begin() + 1, flat.begin() + flat[ 0 ] + 1 );
    auto v2 = map< vector< string > >(
      [&]( size_t e ) { return deitemise < arith_term >( e ).print(); }, flat,
      flat.begin() + flat[ 0 ] + 1, flat.end() );
    std::string ret;
    if( !v1.empty() )
      ret = "[" + printer( v1, vstr_t( v1.size() - 1, "][" ) ) + "]";
    return ret + "[" + printer( v2, vstr_t( v2.size() - 1, ", " ) ) + "]";
  }
};

template<>
bimap< multi > symbol_storage< multi >;

//<array-write> ::= <idp> <multi>
struct array_write : expression {
  array_write( eref< idp > i, eref< multi > m ) : expression( { i, m } ) {}

  std::string print() {
    return printer( extractor< idp, multi >( 0 ), { " " } );
  }
};

template<>
bimap< array_write > symbol_storage< array_write >;
//--------------------------------------------------

//--------------------------------------------------
//----------Atom syntax-----------------------------
//<rop> ::= {=,!=,<=,<,>=,>}
struct rop : expression {
  enum kind { rop_equ, rop_neq, rop_leq, rop_ltn, rop_geq, rop_gtn };

  rop( kind k ) : expression( { k } ) {}

  std::string print() {
    switch( flat[ 0 ] ) {
    case rop_equ : return " = ";
    case rop_neq : return " != ";
    case rop_leq : return " <= ";
    case rop_ltn : return " < ";
    case rop_geq : return " >= ";
    case rop_gtn : return " > ";
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< rop > symbol_storage< rop >;

//<idn-list> ::= <idn> |
//               <idn-list> , <idn>
struct idn_list : expression {
  idn_list( vref< idn > l ) :
    expression( vconvert< size_t >( l ) ) {
    assert( !l.empty() );
  }

  string print() {
    auto v = map< vector< string > >(
      [&]( size_t e ) { return deitemise< idn >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};

template<>
bimap< idn_list > symbol_storage< idn_list >;

//<idn-list-e> ::= <idn-list> |
//                 \epsilon
struct idn_list_e : expression {
  idn_list_e() : expression( vsize_t() ) {}
  idn_list_e( vref< idn > l ) :
    expression( vconvert< size_t >( l ) ) {}

  std::string print() {
    if ( flat.empty() )
      return "";
    auto v = map< vector< std::string > >(
      [&]( size_t e ) { return deitemise< idn >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};

template<>
bimap< idn_list_e > symbol_storage< idn_list_e >;

//<havoc> ::= havoc ( <idn-list-e> )
struct havoc : expression {
  havoc() : expression( vsize_t() ) {}
  havoc( vref< idn > l ) :
    expression( vconvert< size_t >( l ) ) {}

  std::string print() {
    if ( flat.empty() )
      return "havoc()";
    auto v = map< vector< std::string > >(
      [&]( size_t e ) { return deitemise< idn >( e ).print(); }, flat );
    return printer( { "havoc ( ", " )" },
                    { printer( v, vstr_t( v.size() - 1, ", " ) ) } );
    //return printer( { "havoc ( ", " )" }, { get< idn_list_e >( 0 ).print() } );
  }
};

template<>
bimap< havoc > symbol_storage< havoc >;

//<atom> ::= <bool-term> |
//           <arith-term> <rop> <arith-term>
//           <array-write> = [ <arith-list> ]
//           <havoc>
struct atom : expression {
  enum kind { boolean_term, relation, array_equality, havoced };

  atom( eref< bool_term > b ) : expression( { boolean_term, b } ) {}
  atom( eref< arith_term > l, eref< rop > o, eref< arith_term > r ) :
    expression( { relation, l, o, r } ) {}
  atom( eref< array_write > a, eref< arith_list > l ) :
    expression( { array_equality, a, l } ) {}
  atom( eref< havoc > h ) : expression( { havoced, h } ) {}

  std::string print() {
    switch ( flat[ 0 ] ) {
    case boolean_term : return get< bool_term >().print();
    case relation : return printer( extractor< arith_term, rop, arith_term >(),
                                    { " ", " " } );
    case array_equality : return printer( extractor< array_write, arith_list >(),
                                          { " = [", "]" } );
    case havoced : return get< havoc >().print();
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< atom > symbol_storage< atom >;
//--------------------------------------------------

//--------------------------------------------------
//----------Formulae syntax-------------------------
//<quantifier> ::= { forall,
//                   exists }
struct quantifier : expression {
  enum kind { forall, exists };

  quantifier( kind k ) : expression( { k } ) {}

  string print() {
    return (flat[ 0 ] == forall) ? "forall " : "exists ";
  }
};

template<>
bimap< quantifier > symbol_storage< quantifier >;

//<q-type> ::= <type> |
//             <type> [ <arith-term> , <arith-term> ]
struct q_type : expression {
  enum kind { unbound, bound };

  q_type( eref< type > t ) : expression( { unbound, t } ) {}
  q_type( eref< type > t, eref< arith_term > l, eref< arith_term > u ) :
    expression( { bound, t, l, u } ) {}

  std::string print() {
    switch ( flat[ 0 ] ) {
    case unbound : return get< type >().print();
    case bound : return printer( extractor< type, arith_term, arith_term >(),
                                 { "[", ", ", "]" } );
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< q_type > symbol_storage< q_type >;

//<formula> ::= <atom> |
//              ( <formula> ) |
//              <formula> <bop> <formula> |
//              not <formula> |
//              <quantifier> <idn-list> : <q-type> . <formula>
struct formula : expression {
  enum kind { atomic, parenthesised, boolean, negated, quantified };

  formula( eref< atom > a, eref< bop > b, eref< formula > f ) :
    expression( { boolean, itemise< formula >( formula( a ) ), b, f } ) {}
  formula( eref< formula > l,  eref< bop > b, eref< formula > r, bool n = false ) :
    expression( { boolean, itemise< formula >( formula( l, n ) ), b, r } ) {}
  formula( eref< quantifier > q, eref< idn_list > l, eref< q_type > t,
           eref< formula > lf, eref< bop > b, eref< formula > rf ) :
    expression( { boolean, itemise< formula >( formula( q, l, t, lf ) ), b, rf } ) {}

  
  formula( eref< atom > a ) : expression( { atomic, a } ) {}
  formula( eref< formula > f, bool n = false ) :
    expression( { (n ? negated : parenthesised), f } ) {}
  formula( eref< quantifier > q, eref< idn_list > l, eref< q_type > t,
           eref< formula > f ) :
    expression( { quantified, q, l, t, f } ) {}

  std::string print() {
    switch( flat[ 0 ] ) {
    case atomic : return get< atom >().print();
    case parenthesised :
      return printer( { "(", ")" }, { get< formula >().print() } );
    case boolean :
      return printer( extractor< formula, bop, formula >(), { " ", " " } );
    case quantified :
      return printer( extractor< quantifier, idn_list, q_type, formula >(),
                      { " ", " : ", " . " } );
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< formula > symbol_storage< formula >;
//--------------------------------------------------
