#include "parsers/formulae.h"

#pragma once

//--------------------------------------------------
//--------------------String syntax-----------------
//<string> ::= " <printable-char>* "

//----------String parser-----------------
template<>
const handle< astring > parser< astring > =
            caller< astring >( "astring:string",
              astring_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------A-type-val syntax-------------
//<a-type-val> ::= int : <sign> <numeral>
//               | real : <sign> <decimal>
//               | bool : <boolean>
//               | string : <string>
//               | formula : <formula>

//----------A-type-val parser-------------
template<>
const handle< a_type_val > parser< a_type_val > =
   attempt( caller< a_type_val >( "a-type-val:int",
              int_tok,
              colon_tok,
              reference( "sign",    parser< sign > ),
              reference( "numeral", parser< numeral > ) ) )
|| attempt( caller< a_type_val >( "a-type-val:real",
              real_tok,
              colon_tok,
              reference( "sign",    parser< sign > ),
              reference( "decimal", parser< decimal > ) ) )
|| attempt( caller< a_type_val >( "a-type-val:bool",
              bool_tok,
              colon_tok,
              reference( "boolean", parser< boolean > ) ) )
|| attempt( caller< a_type_val >( "a-type-val:string",
              string_tok,
              colon_tok,
              reference( "string",  parser< astring > ) ) )
||          caller< a_type_val >( "a-type-val:formula",
              formula_tok,
              colon_tok,
              reference( "formula", parser< formula > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Annotation syntax-------------
//<annotation> ::= @ <idn> : <a-type-val> ;

//----------Annotation parser-------------
template<>
const handle< annotation > parser< annotation > =
            caller< annotation >( "annotation:annotation",
              at_tok,
              reference( "idn",        parser< idn > ),
              colon_tok,
              reference( "a-type-val", parser< a_type_val > ),
              semicolon_tok );
//--------------------------------------------------

template<>
const vhandle< annotation > vparser_em< annotation > =
   attempt( conscaller< annotation >( "annotations:cons",
              empty_tok,
              reference( "v-annotation", vparser_em< annotation > ),
              reference( "annotation",   parser< annotation > ) ) )
||          lastcaller< annotation >( "annotations:last",
              reference( "annotation",   parser< annotation > ) );

//--------------------------------------------------
//--------------------Annotations syntax------------
//<annotations> ::= <annotations> <annotation>
//                | epsilon

//----------Annotations parser-------------
template<>
const handle< annotations > parser< annotations > =
   attempt( caller< annotations >( "annotations:non-empty",
              reference( "annotation-vector", vparser_em< annotation > ) ) )
||          caller< annotations >( "annotations:empty",
              empty_tok );
//--------------------------------------------------
