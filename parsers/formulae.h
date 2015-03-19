#include "parsers/tokens.h"
#include "parsers/terms.h"
#include "parsers/arithmetic.h"
#include "parsers/atomics.h"

#pragma once

//-------------------------------------------------
//--------------------Quantifier syntax------------
//<quantifier> ::= { forall, exists }


//----------Quantifier parser-------------
template<>
const handle< quantifier > parser< quantifier > =
   attempt( caller< quantifier >( "quantifier:forall",
              forall_tok ) )
||          caller< quantifier >( "quantifier:exists",
              exists_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Quantified type syntax--------
//<q-type> ::= <type>
//           | <type> [ <arith-term> , <arith-term> ]

//----------Quantified type parser--------
template<>
const handle< q_type > parser< q_type > =
   attempt( caller< q_type >( "q-type:bounded",
              reference( "type",       parser< type > ),
              square_left_tok,
              reference( "arith_term", parser< arith_term > ),
              comma_tok,
              reference( "arith_term", parser< arith_term > ),
              square_right_tok ) )
||          caller< q_type >( "q-type:unbounded",
              reference( "type",       parser< type > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Formula syntax----------------
//<formula> ::= <atom>
//            | ( <formula> )
//            | <formula> <bop> <formula>
//            | not <formula>
//            | <quantifier> <idn-list> : <q-type> . <formula>

//----------Formula parser----------------
template<>
const handle< formula > parser< formula > =
   attempt( caller< formula >( "formula:atom-bop",
              reference( "atom",       parser< atom > ),
              reference( "bop",        parser< bop > ),
              reference( "formula",    parser< formula > ) ) )
|| attempt( caller< formula >( "formula:parenthesised-bop",
              round_left_tok,
              reference( "formula",    parser< formula > ),
              round_right_tok,
              reference( "bop",        parser< bop > ),
              reference( "formula",    parser< formula > ) ) )
|| attempt( caller< formula >( "formula:not-bop",
              not_tok,
              reference( "formula",    parser< formula > ),
              reference( "bop",        parser< bop > ),
              reference( "formula",    parser< formula > ) ) )
|| attempt( caller< formula >( "formula:quantifier-bop",
              reference( "quantifier", parser< quantifier > ),
              reference( "idn_list",   parser< idn_list > ),
              colon_tok,
              reference( "q_type",     parser< q_type > ),
              dot_tok,
              reference( "formula",    parser< formula > ),
              reference( "bop",        parser< bop > ),
              reference( "formula",    parser< formula > ) ) )
|| attempt( caller< formula >( "formula:atom",
              reference( "atom",       parser< atom > ) ) )
|| attempt( caller< formula >( "formula:parenthesised",
              round_left_tok,
              reference( "formula",    parser< formula > ),
              round_right_tok ) )
|| attempt( caller< formula >( "formula:not",
              not_tok,
              reference( "formula",    parser< formula > ) ) )
||          caller< formula >( "formula:quantifier",
              reference( "quantifier", parser< quantifier > ),
              reference( "idn_list",   parser< idn_list > ),
              colon_tok,
              reference( "q_type",     parser< q_type > ),
              dot_tok,
              reference( "formula",    parser< formula > ) );
//--------------------------------------------------
