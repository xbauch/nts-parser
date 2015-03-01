#include <string>
#include <unordered_set>

#include "Parser-Combinators/parser_combinators.hpp"
#include "parsers/generals.h"

#pragma once

//--------------------------------------------------
//--------------------Tokens------------------------
unordered_set< string > keywords =
{ "forall", "exists", "int", "real", "bool", "true", "false", "and", "or", "imply",
  "equiv", "not", "par", "in", "out", "tid", "initial", "final", "error", "havoc" };
auto const forall_tok = tokenise( accept_str( "forall" ) );
auto const exists_tok = tokenise( accept_str( "exists" ) );

auto const int_tok =  tokenise( accept_str( "int"  ) );
auto const real_tok = tokenise( accept_str( "real" ) );
auto const bool_tok = tokenise( accept_str( "bool" ) );

auto const zero_tok = tokenise( accept( is_char( '0' ) ) );
auto const numeral_tok = tokenise( some( accept( is_digit ) ) );

auto const dot_tok = tokenise( accept( is_char( '.' ) ) );
auto const semicolon_tok = tokenise( accept( is_char( ';' ) ) );
auto const decimal_tok =
   (zero_tok || numeral_tok)
&& dot_tok
&& tokenise( some( accept( is_digit ) ) );

auto const true_tok = tokenise( accept_str( "true" ) );
auto const false_tok = tokenise( accept_str( "false" ) );
auto const boolean_tok = tokenise( accept_str( "false" ) || accept_str( "true" ) );

auto const idn_tok =
   reject_strs( keywords )
&& accept( is_alpha )
&& tokenise( many( accept( is_alnum ) || accept( is_char( '_' ) ) ) );

auto const idp_tok = idn_tok && tokenise( accept( is_char( '\'' ) ) );

auto const and_tok =   tokenise( accept_str( "and" ) ||   accept_str( "&&" ) );
auto const or_tok =    tokenise( accept_str( "or" ) ||    accept_str( "||" ) );
auto const imply_tok = tokenise( accept_str( "imply" ) || accept_str( "->" ) );
auto const equiv_tok = tokenise( accept_str( "equiv" ) || accept_str( "<->" ) );
auto const not_tok =   tokenise( accept_str( "not" ) ||   accept_str( "!" ) );

auto const round_left_tok = tokenise( accept( is_char( '(' ) ) );
auto const round_right_tok = tokenise( accept( is_char( ')' ) ) );
auto const curly_left_tok = tokenise( accept( is_char( '{' ) ) );
auto const curly_right_tok = tokenise( accept( is_char( '}' ) ) );
auto const square_left_tok = tokenise( accept( is_char( '[' ) ) );
auto const square_right_tok = tokenise( accept( is_char( ']' ) ) );
auto const colon_tok = tokenise( accept( is_char( ':' ) ) );
auto const comma_tok = tokenise( accept( is_char( ',' ) ) );
auto const square_right_left_tok = square_right_tok && square_left_tok;
auto const bar_tok = tokenise( accept( is_char( '|' ) ) );

auto const is_last = discard( tokenise( accept( is_eof ) ) );
auto const empty_tok =  tokenise( accept_str( "" ) );

auto const par_tok = tokenise( accept_str( "par" ) );
auto const in_tok = tokenise( accept_str( "in" ) );
auto const out_tok = tokenise( accept_str( "out" ) );
auto const tid_tok = tokenise( accept_str( "tid" ) );
auto const not_tid_tok = tokenise( reject_str( "tid" ) );

auto const initial_tok = tokenise( accept_str( "initial" ) );
auto const final_tok = tokenise( accept_str( "final" ) );
auto const error_tok = tokenise( accept_str( "error" ) );
auto const havoc_tok = tokenise( accept_str( "havoc" ) );

auto const plus_tok =  tokenise( accept( is_char( '+' ) ) );
auto const minus_tok = tokenise( accept( is_char( '-' ) ) );
auto const times_tok = tokenise( accept( is_char( '*' ) ) );
auto const div_tok =   tokenise( accept( is_char( '/' ) ) );
auto const mod_tok =   tokenise( accept( is_char( '%' ) ) );

auto const neq_tok = tokenise( accept_str( "!=" ) );
auto const geq_tok = tokenise( accept_str( ">=" ) );
auto const leq_tok = tokenise( accept_str( "<=" ) );

auto const equal_tok = tokenise( accept( is_char( '=' ) ) );
auto const lt_tok =    tokenise( accept( is_char( '<' ) ) );
auto const gt_tok =    tokenise( accept( is_char( '>' ) ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Type syntax------------------
//<type> ::= {int,real,bool}

//----------Type builders-----------------
template<>
builder_t< type, string >
  builder< type, string >;

//----------Type parser-------------------
template<>
const handle< type > parser< type > =
   attempt( caller< type >( "type:int",
                            int_tok ) )
|| attempt( caller< type >( "type:real",
                            real_tok ) )
||          caller< type >( "type:boolean",
                            bool_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Numeral syntax---------------
//<numeral> ::= {0} | {1..9}{0..9}*

//----------Numeral builders--------------
template<>
builder_t< numeral, string >
  builder< numeral, string >;

//----------Numeral parser----------------
template<>
const handle< numeral > parser< numeral > =
            caller< numeral >( "numeral",
                               numeral_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Decimal syntax---------------
//<decimal> ::= <numeral> {.}{0..9}+

//----------Decimal builders--------------
template<>
builder_t< decimal, string >
  builder< decimal, string >;

//----------Decimal parser----------------
template<>
const handle< decimal > parser< decimal > =
            caller< decimal >( "decimal",
                               decimal_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Boolean syntax---------------
//<boolean> ::= {true,false}

//----------Boolean builders--------------
template<>
builder_t< boolean, string >
  builder< boolean, string >;

//----------Boolean parser----------------
template<>
const handle< boolean > parser< boolean > =
            caller< boolean >( "boolean",
                               boolean_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Idn syntax-------------------
//<idn> ::= {a..z,A..Z}{a..z,A..Z,0..9,_}*

//----------Idn builders------------------
template<>
builder_t< idn, string >
  builder< idn, string >;

//----------Idn parser--------------------
template<>
const handle< idn > parser< idn > =
            caller< idn >( "idn",
                           idn_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Idp syntax-------------------
//<idp> ::= <idp> {'}

//----------Idp builders------------------
template<>
builder_t< idp, string >
  builder< idp, string >;

//----------Idp parser--------------------
template<>
const handle< idp > parser< idp > =
            caller< idp >( "idp",
                           idp_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Id syntax--------------------
//<id> ::= <idn>
//       | <idp>

//----------Id builders-------------------
template<>
builder_t< id, eref< idp > >
  builder< id, eref< idp > >;

template<>
builder_t< id, eref< idn > >
  builder< id, eref< idn > >;

//----------Id parser---------------------
template<>
const handle< id > parser< id > =
   attempt( caller< id >( "id:idp",
                          reference( "idp", parser< idp > ) ) )
||          caller< id >( "id:idn",
                          reference( "idn", parser< idn > ) );
//-------------------------------------------------
