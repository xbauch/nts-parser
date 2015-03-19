#include "parsers/hierarchies.h"

#pragma once

//--------------------------------------------------
//--------------------Rule syntax-------------------
//<rule> ::= <formula>
//         | <call>
//         | epsilon

//----------Rule parser-------------------
template<>
const handle< rule > parser< rule > =
   attempt( caller< rule >( "rule:call",
              reference( "call", parser< call > ) ) )
|| attempt( caller< rule >( "rule:formula",
              reference( "formula", parser< formula > ) ) )
||          caller< rule >( "rule:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Transition syntax-------------
//<transition> ::= <idn> '->' <idn> <annotations> '{' <rule> '}'
//               | <idn> : <idn> '->' <idn> <annotations> '{' <rule> '}'

//----------Transition parser-------------
template<>
const handle< transition > parser< transition > =
   attempt( caller< transition >( "transition:labelled",
              reference( "idn",         parser< idn > ),
              colon_tok,
              reference( "idn",         parser< idn > ),
              arrow_tok,
              reference( "idn",         parser< idn > ),
              reference( "annotations", parser< annotations > ),
              curly_left_tok,
              reference( "rule",        parser< rule > ),
              curly_right_tok ) )
||          caller< transition >(
              "transition:unlabelled",
              reference( "idn",         parser< idn > ),
              arrow_tok,
              reference( "idn",         parser< idn > ),
              reference( "annotations", parser< annotations > ),
              curly_left_tok,
              reference( "rule",        parser< rule > ),
              curly_right_tok );
//--------------------------------------------------

template<>
const vhandle< transition > vparser_em< transition > =
   attempt( conscaller< transition >( "transitions:cons",
              empty_tok,
              reference( "v-transition", vparser_em< transition > ),
              reference( "transition",   parser< transition > ) ) )
||          lastcaller< transition >( "transitions:last",
              reference( "transition",   parser< transition > ) );

//------------------------------------------------
//--------------------Transitions syntax----------
//<transitions> ::= <transition> <transitions>
//                | epsilon

//----------Transitions parser------------
template<>
const handle< transitions > parser< transitions > =
   attempt( caller< transitions >( "transitions:non-empty",
              reference( "transition-vector", vparser_em< transition > ) ) )
||          caller< transitions >( "transitions:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Statelist syntax--------------
//<statelist> ::= <statelist> , <idn>
//              | <idn>

//----------Statelist parser--------------
template<>
const handle< statelist > parser< statelist > =
            caller< statelist >( "statelist",
              reference( "idn-vector", vparser_cm< idn > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Statelist-a syntax------------
//<statelist-a> ::= <statelist> , <annotations> <idn>
//                | <annotations> <idn>

//----------Statelist-a parser--------------
template<>
const handle< statelist_a > parser< statelist_a > =
   attempt( caller< statelist_a >( "statelist-a:prefixed",
              reference( "statelist", parser< statelist > ),
              comma_tok,
              reference( "annotations", parser< annotations > ),
              reference( "idn", parser< idn > ) ) )
||          caller< statelist_a >( "statelist-a:non-prefixed",
              reference( "annotations", parser< annotations > ),
              reference( "idn", parser< idn > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------States syntax-----------------
//<states> ::= states <statelist-a> ;
//           | epsilon

//----------States parser-----------------
template<>
const handle< states > parser< states > =
   attempt( caller< states >( "states:non-empty",
              states_tok,
              reference( "statelist-a", parser< statelist_a > ),
              semicolon_tok ) )
||          caller< states >( "states:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------States-init syntax------------
//<states-init> ::= initial <statelist> ;

//----------States-init parser------------
template<>
const handle< states_init > parser< states_init > =
   attempt( caller< states_init >( "states-init:non-empty",
              initial_tok,
              reference( "statelist", parser< statelist > ),
              semicolon_tok ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------States-fin syntax-------------
//<states-fin> ::= final <statelist> ;
//               | epsilon

//----------States-fin parser-------------
template<>
const handle< states_fin > parser< states_fin > =
   attempt( caller< states_fin >( "states-fin:non-empty",
              final_tok,
              reference( "statelist", parser< statelist > ),
              semicolon_tok ) )
||          caller< states_fin >( "states-fin:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------States-err syntax-------------
//<states-err> ::= error <statelist> ;
//               | epsilon

//----------States-err parser-------------
template<>
const handle< states_err > parser< states_err > =
   attempt( caller< states_err >( "states-err:non-empty",
              error_tok,
              reference( "statelist", parser< statelist > ),
              semicolon_tok ) )
||          caller< states_err >( "states-err:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Statemarks syntax-------------
//<statemarks> ::= <states> <states-init> <states-fin> <states-err>

//----------Statemarks parser-------------
template<>
const handle< statemarks > parser< statemarks > =
            caller< statemarks >( "statemark",
              reference( "states", parser< states > ),
              reference( "states-init", parser< states_init > ),
              reference( "states-fin", parser< states_fin > ),
              reference( "states-err", parser< states_err > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Nts-body syntax---------------
//<nts-body> ::= <decl-loc> <statemarks> <transitions>

//----------Nts-body parser---------------
template<>
const handle< nts_body > parser< nts_body > =
            caller< nts_body >( "nts-body",
              reference( "decl-loc", parser< decl_loc > ),
              reference( "statemarks", parser< statemarks > ),
              reference( "transitions", parser< transitions > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Nts-basic syntax--------------
//<Nts-basic> ::= <annotations> <idn> '{' <nts-body> '}'

//----------Nts-basic parser--------------
template<>
const handle< nts_basic > parser< nts_basic > =
            caller< nts_basic >( "nts-basic",
              reference( "annotations", parser< annotations > ),
              reference( "idn", parser< idn > ),
              curly_left_tok,
              reference( "nts-body", parser< nts_body > ),
              curly_right_tok );
//--------------------------------------------------
