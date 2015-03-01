#include "Parser-Combinators/parser_combinators.hpp"
#include "parsers/generals.h"

#pragma once

//-------------------------------------------------
//--------------------Arith-lit parser-------------
//<arith-lit> ::= <id>
//              | tid
//              | <numeral>
//              | <decimal>

//----------Arith-lit builders------------
template<>
builder_t< arith_lit, eref< id > >
  builder< arith_lit, eref< id > >;
template<>
builder_t< arith_lit, string >
  builder< arith_lit, string >;
template<>
builder_t< arith_lit, eref< decimal > >
  builder< arith_lit, eref< decimal > >;
template<>
builder_t< arith_lit, eref< numeral > >
  builder< arith_lit, eref< numeral > >;

//----------Arith-lit parser--------------
template<>
const handle< arith_lit >  parser< arith_lit > =
   attempt( caller< arith_lit >( "arith-lit:id",
                                 reference( "id", parser< id > ) ) )
|| attempt( caller< arith_lit >( "arith-lit:tid",
                                 tid_tok ) )
|| attempt( caller< arith_lit >( "arith-lit:dec",
                                 reference( "decimal", parser< decimal > ) ) )
||          caller< arith_lit >( "arith-lit:num",
                                 reference( "numeral", parser< numeral > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Aop parser-------------------
//<aop> ::= {+,-,*,/,%}

//----------Aop builders------------------
template<>
builder_t< aop, string >
  builder< aop, string >;

//----------Aop parser--------------------
template<>
const handle< aop > parser< aop > =
   attempt( caller< aop >( "aop:plus",
                           plus_tok ) )
|| attempt( caller< aop >( "aop:minus",
                           minus_tok ) )
|| attempt( caller< aop >( "aop:times",
                           times_tok ) )
|| attempt( caller< aop >( "aop:div",
                           div_tok ) )
||          caller< aop >( "aop:mod",
                           mod_tok );
//------------------------------------------------

//-------------------------------------------------
//--------------------Index-term vector syntax-----
// <v-index-term> ::= <index-term> ][ <v-index-term>
//                  | <index-term>

//----------Index-term vector builders----
template<>
consbuilder_t< index_term, string, vref< index_term >, eref< idn >, string,
               vref< index_term >, string, eref< aop >, vref< index_term > >
  consbuilder< index_term, string, vref< index_term >, eref< idn >, string,
               vref< index_term >, string, eref< aop >, vref< index_term > >;
template<>
consbuilder_t< index_term, string, vref< index_term >, eref< arith_lit >,
               eref< aop >, vref< index_term > >
  consbuilder< index_term, string, vref< index_term >, eref< arith_lit >,
               eref< aop >, vref< index_term > >;
template<>
consbuilder_t< index_term, string, vref< index_term >, string, vref< index_term >,
               string, eref< aop >, vref< index_term > >
  consbuilder< index_term, string, vref< index_term >, string, vref< index_term >,
               string, eref< aop >, vref< index_term > >;
template<>
consbuilder_t< index_term, string, vref< index_term >, eref< idn >, string,
               vref< index_term >, string >
  consbuilder< index_term, string, vref< index_term >, eref< idn >, string,
               vref< index_term >, string >;
template<>
consbuilder_t< index_term, string, vref< index_term >, eref< arith_lit > >
  consbuilder< index_term, string, vref< index_term >, eref< arith_lit > >;
template<>
consbuilder_t< index_term, string, vref< index_term >, string, vref< index_term >,
               string >
  consbuilder< index_term, string, vref< index_term >, string, vref< index_term >,
               string>;

template<>
lastbuilder_t< index_term, eref< idn >, string,
               vref< index_term >, string, eref< aop >, vref< index_term > >
  lastbuilder< index_term, eref< idn >, string,
               vref< index_term >, string, eref< aop >, vref< index_term > >;
template<>
lastbuilder_t< index_term, eref< arith_lit >, eref< aop >, vref< index_term > >
  lastbuilder< index_term, eref< arith_lit >, eref< aop >, vref< index_term > >;
template<>
lastbuilder_t< index_term, string, vref< index_term >, string, eref< aop >,
               vref< index_term > >
  lastbuilder< index_term, string, vref< index_term >, string, eref< aop >,
               vref< index_term > >;
template<>
lastbuilder_t< index_term, eref< idn >, string, vref< index_term >, string >
  lastbuilder< index_term, eref< idn >, string, vref< index_term >, string >;
template<>
lastbuilder_t< index_term, eref< arith_lit > >
  lastbuilder< index_term, eref< arith_lit > >;
template<>
lastbuilder_t< index_term, string, vref< index_term >, string >
  lastbuilder< index_term, string, vref< index_term >, string >;

//----------Index-term vector parser------
template<>
const vhandle< index_term > vparser_sq< index_term > =
   attempt( conscaller< index_term >(
              "v-index-term:array-aop-cons",
              square_right_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              reference( "idn",         parser< idn > ),
              square_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              square_right_tok,
              reference( "aop",         parser< aop > ),
              reference( "index-terms", vparser_sq< index_term > ) ) )
|| attempt( conscaller< index_term >(
              "v-index-term:lit-aop-cons",
              square_right_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              reference( "arith-lit",   parser< arith_lit > ),
              reference( "aop",         parser< aop > ),
              reference( "index-terms", vparser_sq< index_term > ) ) )
|| attempt( conscaller< index_term >(
              "v-index-term:par-aop-cons",
              square_right_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              round_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              round_right_tok,
              reference( "aop",         parser< aop > ),
              reference( "index-terms", vparser_sq< index_term > ) ) )
|| attempt( conscaller< index_term >(
              "v-index-term:array-cons",
              square_right_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              reference( "idn",         parser< idn > ),
              square_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              square_right_tok ) )
|| attempt( conscaller< index_term >(
              "v-index-term:par-cons",
              square_right_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              round_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              round_right_tok ) )
|| attempt( conscaller< index_term >(
              "v-index-term:lit-cons",
              square_right_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              reference( "arith-lit",   parser< arith_lit > ) ) )
            
|| attempt( lastcaller< index_term >(
              "v-index-term:array-aop",
              reference( "idn",         parser< idn > ),
              square_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              square_right_tok,
              reference( "aop",         parser< aop > ),
              reference( "index-terms", vparser_sq< index_term > ) ) )
|| attempt( lastcaller< index_term >(
              "v-index-term:lit-aop",
              reference( "arith-lit",   parser< arith_lit > ),
              reference( "aop",         parser< aop > ),
              reference( "index-terms", vparser_sq< index_term > ) ) )
|| attempt( lastcaller< index_term >(
              "v-index-term:par-aop",
              round_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              round_right_tok,
              reference( "aop",         parser< aop > ),
              reference( "index-terms", vparser_sq< index_term > ) ) )
|| attempt( lastcaller< index_term >(
              "v-index-term:array",
              reference( "idn",         parser< idn > ),
              square_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              square_right_tok ) )
|| attempt( lastcaller< index_term >(
              "v-index-term:par",
              round_left_tok,
              reference( "index-terms", vparser_sq< index_term > ),
              round_right_tok ) )
||          lastcaller< index_term >(
              "v-index-term:lit",
              reference( "arith-lit", parser< arith_lit > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Index-term syntax------------
// <index-term> ::= <arith-lit>
//                | <idn> [ <index-term> ]
//                | <index-term> <aop> <index-term>
//                | ( <index-term> )
//                | <index-term> [ <index-term> ]

//----------Index-term builders-----------
template<>
builder_t< index_term, vref< index_term > >
  builder< index_term, vref< index_term > >;

//----------Index-term parser-------------
template<>
const handle< index_term > parser< index_term > =
  attempt( caller< index_term >( "index-term",
                                 vparser_sq< index_term > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Array-read syntax------------
//<array-read> ::= <idn> [ <index-term> ]
//               | <array-read> [ <index-term> ]

//----------Array-read builders-----------
template<>
builder_t< array_read, eref< idn >, string, vref< index_term >, string >
  builder< array_read, eref< idn >, string, vref< index_term >, string >;

//----------Array-read parser-------------
template<>
const handle< array_read > parser< array_read > =
            caller< array_read >( "array-read",
                                  reference( "idn", parser< idn > ),
                                  square_left_tok,
                                  reference( "index-terms", vparser_sq< index_term > ),
                                  square_right_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Array-term parser------------
//<array-term> ::= <array-read>
//               | |<idn>|

//----------Array-term builders-----------
template<>
builder_t< array_term, eref< array_read > >
  builder< array_term, eref< array_read > >;

template<>
builder_t< array_term, string, eref< idn >, string >
  builder< array_term, string, eref< idn >, string >;

//----------Array-term parser-------------
template<>
const handle< array_term > parser< array_term > =
   attempt( caller< array_term >( "array-term:read",
                                  reference( "array-read", parser< array_read > ) ) )
||          caller< array_term >( "array-term:size",
                                  bar_tok,
                                  reference( "idn", parser< idn > ),
                                  bar_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Sign syntax------------------
//<sign> ::= {-} | epsilon

//----------Sign builders-----------------
template<>
builder_t< sign, string >
  builder< sign, string >;

//----------Sign parser-------------------
template<>
const handle< sign > parser< sign > =
   attempt( caller< sign >( "sign:true",
                            minus_tok ) )
||          caller< sign >( "sign:false",
                            empty_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Arith-term syntax------------
//<arith-term> ::= <sign> <arith-lit>
//               | <sign> <array-term>
//               | ( <arith-term> )
//               | <arith-term> <aop> <arith-term>

//----------Arith-term buiders------------
template<>
builder_t< arith_term, string, eref< arith_term >, string, eref< aop >,
           eref< arith_term > >
  builder< arith_term, string, eref< arith_term >, string, eref< aop >,
           eref< arith_term > >;
template<>
builder_t< arith_term, string, eref< arith_term >, string >
  builder< arith_term, string, eref< arith_term >, string >;
template<>
builder_t< arith_term, eref< sign >, eref< arith_lit >, eref< aop >,
           eref< arith_term > >
  builder< arith_term, eref< sign >, eref< arith_lit >, eref< aop >,
           eref< arith_term > >;
template<>
builder_t< arith_term, eref< sign >, eref< array_term >, eref< aop >,
           eref< arith_term > >
  builder< arith_term, eref< sign >, eref< array_term >, eref< aop >,
           eref< arith_term > >;
template<>
builder_t< arith_term, eref< sign >, eref< arith_lit > >
  builder< arith_term, eref< sign >, eref< arith_lit > >;
template<>
builder_t< arith_term, eref< sign >, eref< array_term > >
  builder< arith_term, eref< sign >, eref< array_term > >;


//----------Arith-term parser--------------
template<>
const handle< arith_term > parser< arith_term > =
   attempt( caller< arith_term >( "arith-term:par-aop",
                                  round_left_tok,
                                  reference( "arith-term", parser< arith_term > ),
                                  round_right_tok,
                                  reference( "aop",        parser< aop > ),
                                  reference( "arith-term", parser< arith_term > ) ) )
|| attempt( caller< arith_term >( "arith-term:par",
                                  round_left_tok,
                                  reference( "arith-term", parser< arith_term > ),
                                  round_right_tok ) )
|| attempt( caller< arith_term >( "arith-term:lit-aop",
                                  reference( "sign", parser< sign > ),
                                  reference( "arith-lit", parser< arith_lit > ),
                                  reference( "aop", parser< aop > ),
                                  reference( "arith-term", parser< arith_term > ) ) )
|| attempt( caller< arith_term >( "arith-term:arr-aop",
                                  reference( "sign", parser< sign > ),
                                  reference( "array-term", parser< array_term > ),
                                  reference( "aop", parser< aop > ),
                                  reference( "arith-term", parser< arith_term > ) ) )
|| attempt( caller< arith_term >( "arith-term:arr",
                                  reference( "sign", parser< sign > ),
                                  reference( "array-term", parser< array_term > ) ) )
||          caller< arith_term >( "arith-term:lit",
                                  reference( "sign", parser< sign > ),
                                  reference( "arith-lit", parser< arith_lit > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Arith-term vector syntax-----
//<v-arith-term> ::= <arith-term> , <v-arith-term>
//                 | <arith-term>

//----------Arith-term vector parser------
template<>
const vhandle< arith_term > vparser_cm< arith_term > =
   attempt( conscaller< arith_term >(
              "arith-terms:cons",
              comma_tok,
              reference( "v-arith-term", vparser_cm< arith_term > ),
              reference( "arith-term", parser< arith_term > ) ) )
||          lastcaller< arith_term >(
              "arith-term:last",
              reference( "arith-term", parser< arith_term > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Arith-list syntax------------
//<arith-list> ::= <arith-term>
//               | <arith-list> , <arith-term>

//----------Arith-list builders-----------
template<>
builder_t< arith_list, vref< arith_term > >
  builder< arith_list, vref< arith_term > >;

//----------Arith-list parser-------------
template<>
const handle< arith_list > parser< arith_list > =
  caller< arith_list >( "arith-list",
                        reference( "arith_terms", vparser_cm< arith_term > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Arith-list-e syntax----------
//<arith-list-e> ::= <arith-list>
//                 | epsilon

//----------Arith-list-e builders---------
template<>
builder_t< arith_list_e, vref< arith_term > >
  builder< arith_list_e, vref< arith_term > >;

template<>
builder_t< arith_list_e, string >
  builder< arith_list_e, string >;

//----------Arith-list-e parser-----------
template<>
const handle< arith_list_e > parser< arith_list_e > =
   attempt( caller< arith_list_e >(
              "arith-list-e:non-empty",
              reference( "arith_terms", vparser_cm< arith_term > ) ) )
||          caller< arith_list_e >(
              "arith-list-e:empty",
              empty_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Arith-term vector syntax-----
// <v-arith-term> ::= <arith-term> ][ <v-arith-term>
//                  | <arith-term>

//----------Arith-term vector builders----
template<>
consbuilder_t< arith_term, string, vref< arith_term >, eref< arith_term > >
  consbuilder< arith_term, string, vref< arith_term >, eref< arith_term > >;

template<>
lastbuilder_t< arith_term, eref< arith_term > >
  lastbuilder< arith_term, eref< arith_term > >;

//----------Arith-term vector parser------
template<>
const vhandle< arith_term > vparser_sq< arith_term > =
   attempt( conscaller< arith_term >(
              "arith-terms:cons",
              square_right_left_tok,
              reference( "arith-terms", vparser_sq< arith_term > ),
              reference( "arith-term", parser< arith_term > ) ) )
||          lastcaller< arith_term >(
              "arith-terms:last",
              reference( "arith-term", parser< arith_term > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Multi syntax-----------------
//<multi> ::= [ <arith-term> ] <multi> |
//            [ <arith-list> ] |
//            epsilon

//----------Multi builders----------------
template<>
builder_t< multi, string, vref< arith_term >, string, vref< arith_term >, string >
  builder< multi, string, vref< arith_term >, string, vref< arith_term >, string >;

template<>
builder_t< multi, string, vref< arith_term >, string >
  builder< multi, string, vref< arith_term >, string >;

template<>
builder_t< multi, string >
  builder< multi, string >;

//----------Multi parser------------------
template<>
const handle< multi > parser< multi > =
   attempt( caller< multi >( "multi:non-non-empty",
                             square_left_tok,
                             reference( "arith_terms", vparser_sq< arith_term > ),
                             comma_tok,
                             reference( "arith_terms", vparser_cm< arith_term > ),
                             square_right_tok ) )
|| attempt( caller< multi >( "multi:non-empty",
                             square_left_tok,
                             reference( "arith_terms", vparser_cm< arith_term > ),
                             square_right_tok ) )
||          caller< multi >( "multi:empty",
                             empty_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Array-write syntax-----------
//<array-write> ::= <idp> <multi>

//----------Array-write builders----------
template<>
builder_t< array_write, eref< idp >, eref< multi > >
  builder< array_write, eref< idp >, eref< multi > >;

//----------Array-write parser------------
template<>
const handle< array_write > parser< array_write > =
            caller< array_write >( "array-write",
                                   reference( "idp", parser< idp > ),
                                   reference( "multi", parser< multi > ) );
//-------------------------------------------------
