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
  bool_term( eref< bool_term > l, eref< bop > o, eref< bool_term > r ) :
    expression( { relation, l, o, r } ) {}

  std::string print() {
    switch( flat[ 0 ] ) {
    case literal : return get< bool_lit >().print();
    case parenthesised : return printer( { "(", ")" },
                                         { get< bool_term >().print() } );
    case negated : return "! " + get< bool_term >().print();
    case relation : return printer( extract< bool_term, bop, bool_term >(),
                                    { " ", " " } );
    default : assert( false ); return "";
    }
  }
};

template<>
bimap< bool_term > symbol_storage< bool_term >;
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
//--------------------------------------------------
