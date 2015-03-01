#include "Parser-Combinators/parser_combinators.hpp"
#include "parsers/generals.h"

#pragma once

//-------------------------------------------------
//--------------------Bop syntax-------------------
//<bop> ::= {and,or,imply,equiv,&&,||,->,<->}

//----------Bop builders------------------
template<>
builder_t< bop, string >
  builder< bop, string >;

//----------Bop parser--------------------
template<>
const handle< bop > parser< bop > =
   attempt( caller< bop >( "bop:equiv",
                           equiv_tok ) )
|| attempt( caller< bop >( "bop:imply",
                           imply_tok ) )
|| attempt( caller< bop >( "bop:or",
                           or_tok ) )
||          caller< bop >( "bop:and",
                           and_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Bool-lit syntax---------------
//<bool-lit> ::= <id>
//             | true
//             | false

//----------Bool-lit builders-------------
template<>
builder_t< bool_lit, eref< id > >
  builder< bool_lit, eref< id > >;

template<>
builder_t< bool_lit, string >
  builder< bool_lit, string >;

//----------Bool-lit parser---------------
template<>
const handle< bool_lit > parser< bool_lit > =
   attempt( caller< bool_lit >( "bool-lit:true",
                                true_tok ) )
|| attempt( caller< bool_lit >( "bool-lit:false",
                                false_tok ) )
||          caller< bool_lit >( "bool-lit:id",
                                reference( "id", parser< id > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Bool-term syntax-------------
//<bool-term> ::= <bool-lit>
//              | ( <bool-term> )
//              | <not> <bool-term>
//              | <bool-term> <bop> <bool-term>

//----------Bool-term builders------------
template<>
builder_t< bool_term, eref< bool_lit >, eref< bop >, eref< bool_term > >
  builder< bool_term, eref< bool_lit >, eref< bop >, eref< bool_term > >;
template<>
builder_t< bool_term, string, eref< bool_term >, string, eref< bop >,
           eref< bool_term > >
  builder< bool_term, string, eref< bool_term >, string, eref< bop >,
           eref< bool_term > >;
template<>
builder_t< bool_term, string, eref< bool_term >, eref< bop >, eref< bool_term > >
  builder< bool_term, string, eref< bool_term >, eref< bop >, eref< bool_term > >;

template<>
builder_t< bool_term, eref< bool_lit > >
  builder< bool_term, eref< bool_lit > >;
template<>
builder_t< bool_term, string, eref< bool_term >, string >
  builder< bool_term, string, eref< bool_term >, string >;
template<>
builder_t< bool_term, string, eref< bool_term > >
  builder< bool_term, string, eref< bool_term > >;

//----------Bool-term parser--------------
template<>
const handle< bool_term > parser< bool_term > =
   attempt( caller< bool_term >( "bool-term:lit-bop",
                                 reference( "bool-lit", parser< bool_lit > ),
                                 reference( "bop", parser< bop > ),
                                 reference( "bool-term", parser< bool_term > ) ) )
|| attempt( caller< bool_term >( "bool-term:not-bop",
                                 not_tok,
                                 reference( "bool-term", parser< bool_term > ),
                                 reference( "bop", parser< bop > ),
                                 reference( "bool-term", parser< bool_term > ) ) )
|| attempt( caller< bool_term >( "bool-term:not",
                                 not_tok,
                                 reference( "bool-term", parser< bool_term > ) ) )
|| attempt( caller< bool_term >( "bool-term:par-bop",
                                 round_left_tok,
                                 reference( "bool-term", parser< bool_term > ),
                                 round_right_tok,
                                 reference( "bop", parser< bop > ),
                                 reference( "bool-term", parser< bool_term > ) ) )
|| attempt( caller< bool_term >( "bool-term:par",
                                 round_left_tok,
                                 reference( "bool-term", parser< bool_term > ),
                                 round_right_tok ) )
||          caller< bool_term >( "bool-term:lit",
                                 reference( "bool-lit", parser< bool_lit > ) );
//-------------------------------------------------
