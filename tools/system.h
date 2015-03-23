#include <string>

#include "tools/logic.h"

#pragma once

using namespace std;

//--------------------------------------------------
//--------------------String class------------------
//<string> ::= " <printable-char>* "
struct astring : symbol {
  using is_referenceable = true_type;

  string name;
  size_t _hash;

  astring( string n ) : name( n ) {
    _init();
  }

  void _init() {
    std::hash< string > hfn;
    _hash = hfn( name );
  }

  size_t hash() const {
    return _hash;
  }

  bool operator==( const astring& other ) const {
    return name == other.name;
  }

  string print() {
    return name;
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------A-type-val class--------------
//<a-type-val> ::= int : <sign> <numeral>
//               | real : <sign> <decimal>
//               | bool : <boolean>
//               | string : <string>
//               | formula : <formula>
struct a_type_val : expression {
  enum kind { integer, real, boolean, astring, formula };

  a_type_val( string&, string&, eref< sign > s, eref< numeral > n ) :
    expression( { integer, s, n } ) {}
  a_type_val( string&, string&, eref< sign > s, eref< decimal > r ) :
    expression( { real, s, r } ) {}
  a_type_val( string&, string&, eref< ::boolean > b ) :
    expression( { boolean, b } ) {}
  a_type_val( string&, string&, eref< ::astring > s ) :
    expression( { astring, s } ) {}
  a_type_val( string&, string&, eref< ::formula > f ) :
    expression( { formula, f } ) {}

  string print() {
    switch( flat[ 0 ] ) {
    case integer : return "int : " + printer( extractor< sign, numeral >(),
                                              { " " } );
    case real : return "real : " + printer( extractor< sign, decimal >(),
                                            { " " } );
    case boolean : return "bool : " + get< ::boolean >().print();
    case astring : return "string : " + get< ::astring >().print();
    case formula : return "formula : " + get< ::formula >().print();
    default : assert( false ); return "";
    }
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Annotation class--------------
//<annotation> ::= @ <idn> : <a-type-val> ;
struct annotation : expression {
  annotation( string&, eref< idn > i, string&, eref< a_type_val > a, string& ) :
    expression( { i, a } ) {}
  annotation( eref< annotation > a ) :
    expression( deitemise( a ).flat ) {}
  
  string print() {
    return printer( { "@", ":", ";" }, extractor< idn, a_type_val >( 0 ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Annotations class-------------
//<annotations> ::= <annotations> <annotation>
//                | epsilon
struct annotations : expression {
  annotations( string& ) :
    expression( vsize_t() ) {}
  annotations( vref< annotation > a ) :
    expression( vconvert_reverse< size_t >( a ) ) {}

  string print() {
    if ( flat.empty() )
      return "";
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< annotation >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, " " ) );
  }
};
//--------------------------------------------------

using basic = compilation< ' ', annotations, idn >;

//--------------------------------------------------
//--------------------Array class-------------------
//<array> ::= <basic> [ <arith-term> ]
//          | <array> [ <arith-term> ]
struct nts_array : expression {
  nts_array( eref< basic > b, string&, vref< arith_term > v, string& ) :
    expression( cat( { b }, vconvert_reverse< size_t >( v ) ) ) {
    assert( !v.empty() );
  }

  string print() {
    auto v = map_replace< vector< string > >(
      [&](size_t e) { return deitemise< arith_term >( e ).print(); }, flat,
      flat.begin() + 1, flat.end() );
    return get< basic >( 0 ).print() +
           "[" + printer( v, vstr_t( v.size() - 1, "][" ) ) + "]";
  }
};
//--------------------------------------------------

struct empty_list : expression {
  empty_list( eref< empty_list > l ) : expression( deitemise( l ).flat ) {}
  empty_list( string& ) : expression( vsize_t() ) {}

  string print() {
    return "";
  }
};

//--------------------------------------------------
//--------------------Array-ref class---------------
//<array-ref> ::= <basic> []
//              | <array> []
//              | <array-ref> []
struct array_ref : expression {
  enum kind { ref_basic, ref_array };

  array_ref( eref< basic > b, string&, vref< empty_list > v, string& ) :
    expression( { ref_basic, b, v.size() } ) {
    assert( !v.empty() );
  }
  array_ref( eref< nts_array > a, string&, vref< empty_list > v, string& ) :
    expression( { ref_array, a, v.size() } ) {
    assert( !v.empty() );
  }

  string print() {
    return (flat[ 0 ] == ref_basic ? get< basic >().print() :
            get< nts_array >().print())
      + "[" + printer( vstr_t( flat[ 2 ], "" ),
                       vstr_t( flat[ 2 ] - 1, "][" ) )
      + "]";
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Array-pure-ref class----------
//<array-pure-ref> ::= <basic> []
//                   | <array-pure-ref> []
struct array_pure_ref : expression {
  array_pure_ref( eref< basic > b, string&, vref< empty_list > v, string& ) :
    expression( { b, v.size() } ) {
    assert( !v.empty() );
  }

  string print() {
    return get< basic >( 0 ).print()
      + "[" + printer( vstr_t( flat[ 1 ], "" ),
                       vstr_t( flat[ 1 ] - 1, "][" ) )
      + "]";
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-lit class----------------
//<decl-lit> ::= <basic>
//             | <arrray>
//             | <array-ref>
struct decl_lit : expression {
  enum kind { dl_basic, dl_array, dl_aref };
  
  decl_lit( eref< basic > b ) : expression( { dl_basic, b } ) {}
  decl_lit( eref< nts_array > b ) : expression( { dl_array, b } ) {}
  decl_lit( eref< array_ref > b ) : expression( { dl_aref, b } ) {}
  decl_lit( eref< decl_lit > d ) : expression( deitemise( d ).flat ) {}

  string print() {
    switch ( flat[ 0 ] ) {
    case dl_basic : return get< basic >().print();
    case dl_array : return get< nts_array >().print();
    case dl_aref : return get< array_ref >().print();
    default : assert( false ); return "";
    }
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-lits class---------------
//<decl-lits> ::= <decl-lits> , <decl-lit>
//              | <decl-lit>
struct decl_lits : expression {
  decl_lits( vref< decl_lit > v ) :
    expression( vconvert_reverse< size_t >( v ) ) {}

  string print() {
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< decl_lit >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-block class--------------
//<decl-block> ::= <decl-lits> : <type>
struct decl_block : expression {
  decl_block( eref< decl_block > b ) :
    expression( deitemise( b ).flat ) {}
  decl_block( eref< decl_lits > l, string&, eref< type > t ) :
    expression( { l, t } ) {}

  string print() {
    return printer( extractor< decl_lits, type >( 0 ), { ":" } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-block class--------------
//<decl-blocks> ::= <decl-blocks> , <decl-block>
//                | <decl-block>
struct decl_blocks : expression {
  decl_blocks( vref< decl_block > v ) :
    expression( vconvert_reverse< size_t >( v ) ) {}

  string print() {
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< decl_block >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-par-lit class------------
//<decl-par-lit> ::= <basic>
//                 | <array>
struct decl_par_lit : expression {
  enum kind { dpl_basic, dpl_array };
  
  decl_par_lit( eref< decl_par_lit > b ) : expression( deitemise( b ).flat ) {}
  decl_par_lit( eref< basic > b ) : expression( { dpl_basic, b } ) {}
  decl_par_lit( eref< nts_array > a ) : expression( { dpl_array, a } ) {}

  string print() {
    if ( flat[ 0 ] == dpl_basic )
      return get< basic >().print();
    else
      return get< nts_array >().print();
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-par-lits class-----------
//<decl-par-lits> ::= <decl-par-lits> , <idn>
//                  | <decl-par-lit>
struct decl_par_lits : expression {
  decl_par_lits( eref< decl_par_lit > l ) :
    expression( { l } ) {}
  decl_par_lits( eref< decl_par_lit > l, string&, vref< decl_par_lit > v ) :
    expression( cat( { l }, vconvert_reverse< size_t >( v ) ) ) {}

  string print() {
    if ( flat.size() == 1 )
      return get< decl_par_lit >( 0 ).print();
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< decl_par_lit >( e ).print(); },
      flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-par-block class----------
//<decl-par-block> ::= <decl-par-lits> : <type>
struct decl_par_block : expression {
  decl_par_block( eref< decl_par_lits > l, string&, eref< type > t ) :
    expression( { l, t } ) {}
  decl_par_block( eref< decl_par_block > b ) :
    expression( deitemise( b ).flat ) {}

  string print() {
    return printer( extractor< decl_par_lits, type >( 0 ), { ":" } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-par-block class----------
//<decl-par-blocks> ::= <decl-par-blocks> , <decl-par-block>
//                    | <decl-par-block>
struct decl_par_blocks : expression {
  decl_par_blocks( vref< decl_par_block > v ) :
    expression( vconvert_reverse< size_t >( v ) ) {}

  string print() {
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< decl_par_block >( e ).print(); },
      flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-io-lit class-------------
//<decl-io-lit> ::= <basic>
//                | <array-pure-ref>
struct decl_io_lit : expression {
  enum kind { dil_basic, dil_apr };
  
  decl_io_lit( eref< decl_io_lit > b ) : expression( deitemise( b ).flat ) {}
  decl_io_lit( eref< basic > b ) : expression( { dil_basic, b } ) {}
  decl_io_lit( eref< array_pure_ref > a ) : expression( { dil_apr, a } ) {}

  string print() {
    if ( flat[ 0 ] == dil_basic )
      return get< basic >().print();
    else
      return get< array_pure_ref >().print();
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-io-lits class------------
//<decl-io-lits> ::= <decl-io-lits> , <idn>
//                  | <decl-io-lit>
struct decl_io_lits : expression {
  decl_io_lits( eref< decl_io_lit > l ) :
    expression( { l } ) {}
  decl_io_lits( eref< decl_io_lit > l, string&, vref< decl_io_lit > v ) :
    expression( cat( { l }, vconvert_reverse< size_t >( v ) ) ) {}

  string print() {
    if ( flat.size() == 1 )
      return get< decl_io_lit >( 0 ).print();
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< decl_io_lit >( e ).print(); },
      flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-io-block class-----------
//<decl-io-block> ::= <decl-io-lits> : <type>
struct decl_io_block : expression {
  decl_io_block( eref< decl_io_lits > l, string&, eref< type > t ) :
    expression( { l, t } ) {}
  decl_io_block( eref< decl_io_block > b ) :
    expression( deitemise( b ).flat ) {}

  string print() {
    return printer( extractor< decl_io_lits, type >( 0 ), { ":" } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-io-block class-----------
//<decl-io-blocks> ::= <decl-io-blocks> , <decl-io-block>
//                    | <decl-io-block>
struct decl_io_blocks : expression {
  decl_io_blocks( vref< decl_io_block > v ) :
    expression( vconvert_reverse< size_t >( v ) ) {}

  string print() {
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< decl_io_block >( e ).print(); },
      flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl class--------------------
//<decl> ::= <decl-blocks> ;
//         | par <decl-par-blocks> ;
struct decl : expression {
  enum kind { d_seq, d_par };

  decl( eref< decl > d ) :
    expression( deitemise( d ).flat ) {}
  decl( eref< decl_blocks > b, string& ) :
    expression( { d_seq, b } ) {}
  decl( string&, eref< decl_par_blocks > b, string& ) :
    expression( { d_par, b } ) {}

  string print() {
    if ( flat[ 0 ] == d_seq )
      return get< decl_blocks >().print() + ";";
    else 
      return "par " + get< decl_par_blocks >().print() + ";";
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-glob class---------------
//<decl-glob> ::= <decl-glob> <decl>
//         | epsilon
struct decl_glob : expression {
  decl_glob( string& ) : expression( vsize_t() ) {}
  decl_glob( vref< decl > a ) : expression( vconvert_reverse< size_t >( a ) ) {}

  string print() {
    if ( flat.empty() )
      return "";
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< decl >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, "\n" ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------In class----------------------
//<in> ::= in <decl-io-blocks> ;
//         | epsilon
struct in : expression {
  in( string& ) : expression( vsize_t() ) {}
  in( string&, eref< decl_io_blocks > a, string& ) : expression( { a } ) {}

  string print() {
    if ( flat.empty() )
      return "";
    return "in" + get< decl_io_block >( 0 ).print() + ";";
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Out class---------------------
//<out> ::= out <decl-io-blocks> ;
//        | epsilon
struct out : expression {
  out( string& ) : expression( vsize_t() ) {}
  out( string&, eref< decl_io_blocks > a, string& ) : expression( { a } ) {}

  string print() {
    if ( flat.empty() )
      return "";
    return "out" + get< decl_io_block >( 0 ).print() + ";";
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-loc class----------------
//<decl-loc> ::= <decl-loc> <decl>
//             | <in> <out>
struct decl_loc : expression {
  decl_loc( string& ) :
    expression( vsize_t() ) {}
  decl_loc( eref< in > i, eref< out > o ) :
    expression( { i, o } ) {}
  decl_loc( eref< in > i, eref< out > o, vref< decl > d ) :
    expression( cat( { i, o }, vconvert_reverse< size_t >( d ) ) ) {}

  string print() {
    if ( flat.empty() )
      return "";
    if ( flat.size() == 2 )
      return printer( extractor< in, out >( 0 ), { "\n" } );
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< decl >( e ).print(); },
      flat, flat.begin() + 2, flat.end() );
    return printer( extractor< in, out >( 0 ), { "\n" } )
      + printer( vstr_t( v.size(), "\n" ), v );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Arglist class-----------------
//<arglist> ::= <arith-list>
//            | epsilon
struct arglist : expression {
  arglist( string& ) : expression( vsize_t() ) {}
  arglist( eref< arith_list > l ) : expression( { l } ) {}

  string print() {
    if ( flat.empty() )
      return "";
    return get< arith_list >( 0 ).print();
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Ret-terms class---------------
//<ret-terms> ::= <idp>
//              | <ret-terms> , <idp>
struct ret_terms : expression {
  ret_terms( vref< idp > v ) : expression( vconvert_reverse< size_t >( v ) ) {}

  string print() {
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< idp >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Retlist class-----------------
//<retlist> ::= <ret-terms>
//            | epsilon
struct retlist : expression {
  retlist( string& ) : expression( vsize_t() ) {}
  retlist( eref< ret_terms > t ) : expression( { t } ) {}

  string print() {
    if ( flat.empty() )
      return "";
    return get< ret_terms >( 0 ).print();
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Call-base class---------------
//<call-base> ::= <idn> ( <arglist> , <retlist> )
//              | <idp> = <idn> ( <arglist> )
//              | ( <ret-terms> ) = <idn> ( <arglist> )
struct call_base : expression {
  enum kind { no_assign, single_assign, multi_assign };
  
  call_base( eref< idn > i, string&, eref< arglist > a, string& ) :
    expression( { no_assign, i, a } ) {}
  call_base( eref< idp > i, string&, eref< idn > n, string&, eref< arglist > a,
             string& ) :
    expression( { single_assign, i, n, a } ) {}
  call_base( string&, eref< ret_terms > t, string&, string&, eref< idn > n,
             string&, eref< arglist > a, string& ) :
    expression( { multi_assign, t, n, a } ) {}

  string print() {
    switch ( flat[ 0 ] ) {
    case no_assign :
      return printer( extractor< idn, arglist >(),
                      { "(", ")" } );
    case single_assign :
      return printer( extractor< idp, idn, arglist >(), { " = ", "(", ")" } );
    case multi_assign :
      return printer( { "(", ") = ", "(", ")" }, extractor< ret_terms, idn,
                                                            arglist >() );
    default : assert( false ); return "";
    }
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Call class--------------------
//<call> ::= <call-base>
//         | <call-base> and <havoc>
struct call : expression {
  call( eref< call_base > b ) :
    expression( { b } ) {}
  call( eref< call_base > b, string&, eref< havoc > h ) :
    expression( { b, h } ) {}

  string print() {
    if ( flat.size() == 1 )
      return get< call_base >( 0 ).print();
    return printer( extractor< call_base, havoc >( 0 ), { " /\\ " } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Rule class--------------------
//<rule> ::= <formula>
//         | <call>
//         | epsilon
struct rule : expression {
  enum kind { r_empty, r_formula, r_call };
  
  rule( string& ) : expression( { r_empty } ) {}
  rule( eref< formula > f ) : expression( { r_formula, f } ) {}
  rule( eref< call > c ) : expression( { r_call, c } ) {}

  string print() {
    switch ( flat[ 0 ] ) {
    case r_empty : return "";
    case r_formula : return get< formula >().print();
    case r_call : return get< call >().print();
    default : assert( false ); return "";
    }
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Transition class--------------
//<transition> ::= <idn> '->' <idn> <annotations> '{' <rule> '}'
//               | <idn> : <idn> '->' <idn> <annotations> '{' <rule> '}'
struct transition : expression {
  transition( eref< idn > f, string&, eref< idn > t, eref< annotations > a,
              string&, eref< rule > r, string& ) :
    expression( { f, t, a, r } ) {}
  transition( eref< idn > l, string&, eref< idn > f, string&, eref< idn > t,
              eref< annotations > a, string&, eref< rule > r, string& ) :
    expression( { l, f, t, a, r } ) {}
  transition( eref< transition > t ) :
    expression( deitemise( t ).flat ) {}

  string print() {
    bool labelled = (flat.size() == 5);
    return (labelled ? get< idn >( 0 ).print() + " : " : "")
      + printer( extractor< idn, idn, annotations, rule >( labelled ),
                 { " -> ", " ", " {", "}" } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Transitions class-------------
//<transitions> ::= <transition> <transitions>
//                | epsilon
struct transitions : expression {
  transitions( string& ) :
    expression( vsize_t() ) {}
  transitions( vref< transition > a ) :
    expression( vconvert_reverse< size_t >( a ) ) {}

  string print() {
    if ( flat.empty() )
      return "";
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< transition >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, "\n" ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Statelist class---------------
//<statelist> ::= <statelist> , <idn>
//              | <idn>
struct statelist : expression {
  statelist( vref< idn > v ) : expression( vconvert_reverse< size_t >( v ) ) {}

  string print() {
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< idn >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Statelist-a class-------------
//<statelist-a> ::= <statelist> , <annotations> <idn>
//                | <annotations> <idn>
struct statelist_a : expression {
  statelist_a( eref< statelist > s, string&, eref< annotations > a,
               eref< idn > i ) :
    expression( { s, a, i } ) {}
  statelist_a( eref< annotations > a, eref< idn > i ) :
    expression( { a, i } ) {}

  string print() {
    bool prefixed = (flat.size() == 3);
    return (prefixed ? get< statelist >( 0 ).print() + ", " : "")
      + printer( extractor< annotations, idn >( prefixed ), { " " } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------States class------------------
//<states> ::= states <statelist-a> ;
//           | epsilon
struct states : expression {
  states( string& ) : expression( vsize_t() ) {}
  states( string&, eref< statelist_a > a, string& ) :
    expression( { a } ) {}

  string print() {
    if ( flat.empty() )
      return "";
    return printer( { "states ", ";" }, { get< statelist_a >( 0 ).print() } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------States-init class-------------
//<states-init> ::= initial <statelist> ;
struct states_init : expression {
  states_init( string&, eref< statelist > a, string& ) :
    expression( { a } ) {}

  string print() {
    return printer( { "initial ", ";" }, { get< statelist >( 0 ).print() } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------States-fin class--------------
//<states-fin> ::= final <statelist> ;
//           | epsilon
struct states_fin : expression {
  states_fin( string& ) : expression( vsize_t() ) {}
  states_fin( string&, eref< statelist > a, string& ) :
    expression( { a } ) {}

  string print() {
    if ( flat.empty() )
      return "";
    return printer( { "final ", ";" }, { get< statelist >( 0 ).print() } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------States-err class--------------
//<states-err> ::= error <statelist> ;
//           | epsilon
struct states_err : expression {
  states_err( string& ) : expression( vsize_t() ) {}
  states_err( string&, eref< statelist > a, string& ) :
    expression( { a } ) {}

  string print() {
    if ( flat.empty() )
      return "";
    return printer( { "error ", ";" }, { get< statelist >( 0 ).print() } );
  }
};
//--------------------------------------------------

using statemarks = compilation< '\n', states, states_init, states_fin,
                                states_err >;

using nts_body = compilation< '\n', decl_loc, statemarks, transitions >;

//--------------------------------------------------
//--------------------Nts-basic class---------------
//<Nts-basic> ::= <annotations> <idn> '{' <nts-body> '}'
struct nts_basic : expression {
  nts_basic( eref< nts_basic > n ) :
    expression( deitemise( n ).flat ) {}
  nts_basic( eref< annotations > a, eref< idn > i, string&, eref< nts_body > b,
              string& ) :
    expression( { a, i, b } ) {}

  string print() {
    return printer( extractor< annotations, idn, nts_body >( 0 ),
                    { "\n", " {\n", "\n}" } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Instance class----------------
//<instance> ::= <idn> [ <arith-term> ]
struct instance : expression {
  instance( eref< instance > i ) : expression( deitemise( i ).flat ) {}
  instance( eref< idn > i, string&, eref< arith_term > a, string& ) :
    expression( { i, a } ) {}

  string print() {
    return printer( extractor< idn, arith_term >( 0 ), { "[", "]" } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Inst-list class---------------
//<inst-list> ::= <instance>
//              | <inst-list> , <instance>
struct inst_list : expression {
  inst_list( vref< instance > a ) :
    expression( vconvert_reverse< size_t >( a ) ) {}

  string print() {
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< instance >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, ", " ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Instances class----------------
//<instances> ::= instances <inst-list> ;
struct instances : expression {
  instances( string&, eref< inst_list > l, string& ) : expression( { l } ) {}

  string print() {
    return printer( { "instances ", ";" }, extractor< inst_list >( 0 ) );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Nts-name class----------------
//<nts-name> ::= <annotations> nts <idn> ;
struct nts_name : expression {
  nts_name( eref< annotations > a, string&, eref< idn > i, string& ) :
    expression( { a, i } ) {}

  string print() {
    return printer( extractor< annotations, idn >( 0 ), { "\nnts ", ";" } );
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Init class--------------------
//<init> ::= init <formula> ;
struct init : expression {
  init( string&, eref< formula > f, string& ) : expression( { f } ) {}

  string print() {
    return "init " + get< formula >( 0 ).print() + ";";
  }
};
//--------------------------------------------------

//--------------------------------------------------
//--------------------Nts-list class----------------
//<nts-list> ::= <nts-basic> <nts-list>
//             | epsilon
struct nts_list : expression {
  nts_list( string& ) :
    expression( vsize_t() ) {}
  nts_list( vref< nts_basic > a ) :
    expression( vconvert_reverse< size_t >( a ) ) {}

  string print() {
    if ( flat.empty() )
      return "";
    auto v = map_replace< vector< std::string > >(
      [&]( size_t e ) { return deitemise< nts_basic >( e ).print(); }, flat );
    return printer( v, vstr_t( v.size() - 1, "\n\n" ) );
  }
};
//--------------------------------------------------

using nts = compilation< '\n', nts_name, decl_glob, init, instances, nts_list >;
