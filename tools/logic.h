#include <string>

#include "tools/data.h"

#pragma once

using namespace std;

//--------------------------------------------------
//--------------------Type class--------------------
//<type> ::= {int,real,bool}
struct type : expression {
  enum kind { integer, real, boolean };

  type( string s ) : expression( { kinder( s ) } ) {}

  kind kinder( string s ) {
    switch ( s[ 0 ] ) {
    case 'i' : return integer;
    case 'r' : return real;
    case 'b' : return boolean;
    default : assert( false ); return integer;
    }
  }

  string print() {
    switch( flat[ 0 ] ) {
    case integer : return "int";
    case real : return "real";
    case boolean : return "bool";
    default : assert( false ); return "";
    }
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Numeral, Decimal, Boolean class
//<numeral> ::= {0} | {1..9}{0..9}*
//<decimal> ::= <numeral> {.}{0..9}+
//<boolean> ::= {true,false}
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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Idn, Idp classes--------------
//<idn> ::= {a..z,A..Z}{a..z,A..Z,0..9,_}*
//<idp> ::= <idp> {'}
template< typename T >
struct variable : symbol {
  using is_referenceable = true_type;

  string name;
  size_t _hash;

  variable( eref< variable > v ) : name( deitemise( v ).name ) {
    _init();
  }
  variable( std::string n ) : name( n ) {
    _init();
  }

  void _init() {
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
    return name;
  }
};

using primed = true_type;
using unprimed = false_type;

using idn = variable< unprimed >;
using idp = variable< primed >;
//--------------------------------------------------

//--------------------------------------------------
//--------------------Id class----------------------
//<id> ::= <idn>
//       | <idp>
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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Bop class---------------------
//<bop> ::= {and,or,imply,equiv,&&,||,->,<->}
struct bop : expression {
  enum kind { band, bor, imply, equiv };

  bop( string s ) : expression( { kinder( s ) } ) {}

  kind kinder( string s ) {
    switch( s[ 0 ] ) {
    case 'a' : case '&' : return band;
    case 'o' : case '|' : return bor;
    case 'i' : case '-' : return imply;
    case 'e' : case '<' : return equiv;
    default : assert( false ); return band;
    }
  }

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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Bool-lit class----------------
//<bool-lit> ::= <id>
//             | true
//             | false
struct bool_lit : expression {
  enum kind { bl_id, bl_true, bl_false };

  bool_lit( eref< id > i ) : expression( { bl_id, i } ) {}
  bool_lit( string s ) : expression( { (s[0] == 't' ? bl_true : bl_false) } ) {}

  std::string print() {
    switch( flat[ 0 ] ) {
    case bl_id : return get< id >().print();
    case bl_true : return "true";
    case bl_false : return "false";
    default : assert( false ); return "";
    }
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Bool-term class---------------
//<bool-term> ::= <bool-lit>
//              | ( <bool-term> )
//              | <not> <bool-term>
//              | <bool-term> <bop> <bool-term>
struct bool_term : expression {
  enum kind { literal, parenthesised, negated, relation };

  bool_term( eref< bool_lit > l ) : expression( { literal, l } ) {}
  bool_term( string, eref< bool_term > t, string ) :
    expression( { parenthesised, t } ) {}
  bool_term( string, eref< bool_term > t ) :
    expression( { negated, t } ) {}
  
  bool_term( eref< bool_lit > l, eref< bop > o, eref< bool_term > r ) :
    expression( { relation, itemise< bool_term >( bool_term( l ) ), o, r } ) {}
  bool_term( string lp, eref< bool_term > l, string rp, eref< bop > o,
             eref< bool_term > r ) :
    expression( { relation, itemise< bool_term >( bool_term( lp, l, rp ) ), o, r } ) {}
  bool_term( string n, eref< bool_term > l, eref< bop > o, eref< bool_term > r ) :
    expression( { relation, itemise< bool_term >( bool_term( n, l ) ), o, r } ) {}

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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Arith-lit class---------------
//<arith-lit> ::= <id>
//              | tid
//              | <numeral>
//              | <decimal>
struct arith_lit : expression {
  enum kind { arith_id, arith_tid, arith_num, arith_dec };

  arith_lit( eref< id > i ) :      expression( { arith_id, i } ) {}
  arith_lit( string ) :            expression( { arith_tid } ) {}
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
//-------------------------------------------------

//-------------------------------------------------
//--------------------Aop class--------------------
//<aop> ::= {+,-,*,/,%}
struct aop : expression {
  enum kind { plus, minus, times, div, srem, urem };

  aop( string s ) : expression( { kinder( s ) } ) {}

  kind kinder( string s ) {
    switch( s[ 0 ] ) {
    case '+' : return plus;
    case '-' : return minus;
    case '*' : return times;
    case '/' : return div;
    case '%' : return srem;
    default : assert( false ); return plus;
    }
  }
  
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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Index-term class--------------
// <index-term> ::= <arith-lit>
//                | <idn> [ <index-term> ]
//                | <index-term> <aop> <index-term>
//                | ( <index-term> )
//                | <index-term> [ <index-term> ]
struct index_term : expression {
  enum kind { literal, array, function, parenthesised };

  index_term( eref< idn > i, string lp, vref< index_term > t1, string rp,
              eref< aop > o, vref< index_term > t2 ) : expression(
  { function, itemise( index_term( i, lp, t1, rp ) ), o, t2[ 0 ] } ) {}
  index_term( eref< arith_lit > i,
              eref< aop > o, vref< index_term > t2 ) : expression(
  { function, itemise( index_term( i             ) ), o, t2[ 0 ] } ) {}
  index_term( string lp, vref< index_term > t1, string rp,
              eref< aop > o, vref< index_term > t2 ) : expression(
  { function, itemise( index_term( lp, t1, rp    ) ), o, t2[ 0 ] } ) {}

  index_term( eref< idn > i, string, vref< index_term > t1, string ) : expression(
  { array, i,      t1[ 0 ] } ) {}
  index_term( string, vref< index_term > t1, string ) : expression(
  { parenthesised, t1[ 0 ] } ) {}
  index_term( eref< arith_lit > i ) : expression(
  { literal,             i } ) {}

  index_term( vref< index_term > v ) : expression( deitemise( v[ 0 ] ).flat ) {}

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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Array-read class--------------
//<array-read> ::= <idn> [ <index-term> ]
//               | <array-read> [ <index-term> ]
struct array_read : expression {
  array_read( eref< idn > i, string, vref< index_term > v, string ) :
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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Array-term class--------------
//<array-term> ::= <array-read>
//               | |<idn>|
struct array_term : expression {
  enum kind { array, size };

  array_term( eref< array_read > a ) : expression( { array, a } ) {}
  array_term( string, eref< idn > i, string ) : expression( { size, i } ) {}

  std::string print() {
    if ( flat[ 0 ] == array )
      return get< array_read >().print();
    else
      return "|" + get< idn >().print() + "|";
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Sign class--------------------
//<sign> ::= {-} | \epsilon
struct sign : expression {
  enum kind { sign_true, sign_false };

  sign( string s ) : expression( { (s.empty() ? sign_false : sign_true) } ) {}

  std::string print() {
    return (flat[ 0 ] == sign_true) ? "-" : "";
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Arith-term class--------------
//<arith-term> ::= <sign> <arith-lit>
//               | <sign> <array-term>
//               | ( <arith-term> )
//               | <arith-term> <aop> <arith-term>
struct arith_term : expression {
  enum kind { literal, array, parenthesised, function };

  arith_term( eref< sign > s, eref< arith_lit > l ) : expression(
  { literal,       s, l } ) {}
  arith_term( eref< sign > s, eref< array_term > a ) : expression(
  { array,         s, a } ) {}
  arith_term( string, eref< arith_term > t, string ) : expression(
  { parenthesised, t } ) {}
  arith_term( eref< sign > s, eref< arith_lit > l, eref< aop > o,
              eref< arith_term > r ) : expression(
  { function,      itemise( arith_term( s, l ) ), o, r } ) {}
  arith_term( eref< sign > s, eref< array_term > a, eref< aop > o,
              eref< arith_term > r ) : expression(
  { function,      itemise( arith_term( s, a ) ), o, r } ) {}
  arith_term( string lp, eref< arith_term > l, string rp, eref< aop > o,
              eref< arith_term > r ) : expression(
  { function,      itemise( arith_term( lp, l, rp ) ), o, r } ) {}

  arith_term( eref< arith_term > t ) : expression( deitemise( t ).flat ) {}

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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Arith-list class--------------
//<arith-list> ::= <arith-term>
//               | <arith-list> , <arith-term>
struct arith_list : expression {
  arith_list( vref< arith_term > l ) :
    expression( vconvert< size_t >( l ) ) {}

  std::string print() {
    auto v = map< vector< string > >(
      [&]( size_t e ) { return deitemise< arith_term >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Arith-list-e class------------
//<arith-list-e> ::= <arith-list>
//                 | epsilon
struct arith_list_e : expression {
  arith_list_e( string ) : expression( vsize_t() ) {}
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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Multi class-------------------
//<multi> ::= [ <arith-term> ] <multi> |
//            [ <arith-list> ] |
//            \epsilon
struct multi : expression {
  multi( string ) : expression( vsize_t() ) {}
  multi( string, vref< arith_term > a, string, vref< arith_term > l, string ) :
    expression( cat( { a.size() - 1 }, vconvert< size_t >( a ),
                     vconvert< size_t >( l ) ) ) { assert( !l.empty() ); }
  multi( string, vref< arith_term > l, string ) : expression(
    cat( { size_t( 0 ) }, vconvert< size_t >( l ) ) ) { assert( !l.empty() ); }

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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Array-write class-------------
//<array-write> ::= <idp> <multi>
struct array_write : expression {
  array_write( eref< idp > i, eref< multi > m ) : expression( { i, m } ) {}

  std::string print() {
    return printer( extractor< idp, multi >( 0 ), { " " } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Rop class---------------------
//<rop> ::= {=,!=,<=,<,>=,>}
struct rop : expression {
  enum kind { rop_equ, rop_neq, rop_leq, rop_ltn, rop_geq, rop_gtn };

  rop( string s ) : expression( { kinder( s ) } ) {}

  kind kinder( string s ) {
    switch( s[ 0 ] ) {
    case '=' : return rop_equ;
    case '!' : return rop_neq;
    case '<' : return (s.size() > 1 ? rop_leq : rop_ltn);
    case '>' : return (s.size() > 1 ? rop_geq : rop_gtn);
    default : assert( false ); return rop_equ;
    }
  }
  
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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Idn-list class----------------
//<idn-list> ::= <idn>
//             |  <idn-list> , <idn>
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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Idn-list-e class--------------
//<idn-list-e> ::= <idn-list>
//               | epsilon
struct idn_list_e : expression {
  idn_list_e( string ) : expression( vsize_t() ) {}
  idn_list_e( vref< idn > l ) :
    expression( vconvert_reverse< size_t >( l ) ) {}

  std::string print() {
    if ( flat.empty() )
      return "";
    auto v = map< vector< std::string > >(
      [&]( size_t e ) { return deitemise< idn >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Havoc class-------------------
//<havoc> ::= havoc ( <idn-list-e> )
struct havoc : expression {
  havoc( string, string, string ) : expression( vsize_t() ) {}
  havoc( string, string, vref< idn > l, string ) :
    expression( vconvert_reverse< size_t >( l ) ) {}

  std::string print() {
    if ( flat.empty() )
      return "havoc()";
    auto v = map< vector< std::string > >(
      [&]( size_t e ) { return deitemise< idn >( e ).print(); }, flat );
    return printer( { "havoc ( ", " )" },
                    { printer( v, vstr_t( v.size() - 1, ", " ) ) } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Atom class--------------------
//<atom> ::= <bool-term>
//         | <arith-term> <rop> <arith-term>
//         | <array-write> = [ <arith-list> ]
//         | <havoc>
struct atom : expression {
  enum kind { boolean_term, relation, array_equality, havoced };

  atom( eref< bool_term > b ) : expression( { boolean_term, b } ) {}
  atom( eref< arith_term > l, eref< rop > o, eref< arith_term > r ) :
    expression( { relation, l, o, r } ) {}
  atom( eref< array_write > a, string, string, eref< arith_list > l, string ) :
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
//--------------------------------------------------

//------------------------------Formulae----------------------

//--------------------------------------------------
//--------------------Quantifier class--------------
//<quantifier> ::= { forall, exists }
struct quantifier : expression {
  enum kind { forall, exists };

  quantifier( string s ) : expression( { (s == "forall" ? forall : exists) } ) {}

  string print() {
    return (flat[ 0 ] == forall) ? "forall " : "exists ";
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Quantified type class---------
//<q-type> ::= <type>
//           | <type> [ <arith-term> , <arith-term> ]
struct q_type : expression {
  enum kind { unbound, bound };

  q_type( eref< type > t ) : expression( { unbound, t } ) {}
  q_type( eref< type > t, string&, eref< arith_term > l, string&,
          eref< arith_term > u, string& ) :
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
//--------------------------------------------------

//--------------------------------------------------
//--------------------Formula class-----------------
//<formula> ::= <atom>
//            | ( <formula> )
//            | <formula> <bop> <formula>
//            | not <formula>
//            | <quantifier> <idn-list> : <q-type> . <formula>
struct formula : expression {
  enum kind { atomic, parenthesised, boolean, negated, quantified };

  formula( eref< atom > a, eref< bop > o, eref< formula > f ) :
    expression( { boolean, itemise( formula( a ) ), o, f } ) {}
  formula( string& lp, eref< formula > l, string& rp, eref< bop > o,
           eref< formula > r ) :
    expression( { boolean, itemise( formula( lp, l, rp ) ), o, r } ) {}
  formula( string& n, eref< formula > l, eref< bop > o, eref< formula > r ) :
    expression( { boolean, itemise( formula( n, l ) ), o, r } ) {}
  formula( eref< quantifier > q, eref< idn_list > i, string c, eref< q_type > t,
           string d, eref< formula > l, eref< bop > b, eref< formula > r ) :
    expression( { boolean,
                  itemise< formula >( formula( q, i, c, t, d, l ) ),
                  b, r } ) {}
  
  formula( eref< atom > a ) : expression( { atomic, a } ) {}
  formula( string&, eref< formula > f, string& ) : expression( { parenthesised, f } ) {}
  formula( string&, eref< formula > f ) : expression( { negated, f } ) {}
  formula( eref< quantifier > q, eref< idn_list > l, string&, eref< q_type > t,
           string&, eref< formula > f ) :
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
//--------------------------------------------------
