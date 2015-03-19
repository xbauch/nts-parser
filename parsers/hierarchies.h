#include "parsers/declarations.h"

#pragma once

//--------------------------------------------------
//--------------------Arglist syntax----------------
//<arglist> ::= <arith-list>
//            | epsilon

//----------Arglist parser----------------
template<>
const handle< arglist > parser< arglist > =
   attempt( caller< arglist >( "arglist:non-empty",
              reference( "arith-list", parser< arith_list > ) ) )
||          caller< arglist >( "arglist:empty",
              empty_tok );
//--------------------------------------------------

template<>
const vhandle< idp > vparser_cm< idp > =
   attempt( conscaller< idp >( "idp:cons",
              comma_tok,
              reference( "v-idp", vparser_cm< idp > ),
              reference( "idp",   parser< idp > ) ) )
||          lastcaller< idp >( "idp:last",
              reference( "idp",   parser< idp > ) );

//--------------------------------------------------
//--------------------Ret-terms syntax--------------
//<ret-terms> ::= <idp>
//              | <ret-terms> , <idp>

//----------Ret-terms parser--------------
template<>
const handle< ret_terms > parser< ret_terms > =
            caller< ret_terms >( "ret-terms",
              reference( "idps", vparser_cm< idp > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Retlist syntax----------------
//<retlist> ::= <ret-terms>
//            | epsilon

//----------Retlist parser----------------
template<>
const handle< retlist > parser< retlist > =
   attempt( caller< retlist >( "retlist:non-empty",
              reference( "ret-terms", parser< ret_terms > ) ) )
||          caller< retlist >( "retlist:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Call-base syntax--------------
//<call-base> ::= <idn> ( <arglist> , <retlist> )
//              | <idp> = <idn> ( <arglist> )
//              | ( <ret-terms> ) = <idn> ( <arglist> )

//----------Call-base parser--------------
template<>
const handle< call_base > parser< call_base > =
   attempt( caller< call_base >( "call-base:no-assign",
              reference( "idn",       parser< idn > ),
              round_left_tok,
              reference( "arglist",   parser< arglist > ),
              // comma_tok,
              // reference( "retlist",   parser< retlist > ),
              round_right_tok ) )
|| attempt( caller< call_base >( "call-base:single-assign",
              reference( "idp",       parser< idp > ),
              equal_tok,
              reference( "idn",       parser< idn > ),
              round_left_tok,
              reference( "arglist",   parser< arglist > ),
              round_right_tok ) )
||          caller< call_base >( "call-base:multi-assign",
              round_left_tok,
              reference( "ret_terms", parser< ret_terms > ),
              round_right_tok,
              equal_tok,
              reference( "idn",       parser< idn > ),
              round_left_tok,
              reference( "arglist",   parser< arglist > ),
              round_right_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Call syntax-------------------
//<call> ::= <call-base>
//         | <call-base> and <havoc>

//----------Call parser-------------------
template<>
const handle< call > parser< call > =
   attempt( caller< call >( "call:havoc",
              reference( "call_base", parser< call_base > ),
              and_tok,
              reference( "havoc",     parser< havoc > ) ) )
||          caller< call >( "call:no-havoc",
              reference( "call_base", parser< call_base > ) );
//--------------------------------------------------
