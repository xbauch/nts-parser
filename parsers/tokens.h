#include <string>

#include "parsers/generals.h"

#pragma once

//--------------------------------------------------
//--------------------Tokens------------------------
unordered_set< string > keywords =
{ "forall", "exists", "int", "real", "bool", "true", "false", "and", "or",
  "imply", "equiv", "not", "par", "in", "out", "tid", "initial", "final",
  "error", "havoc", "string", "formula", "instances", "nts", "init" };
auto const forall_tok = tokenise_c( accept_str( "forall" ) );
auto const exists_tok = tokenise_c( accept_str( "exists" ) );

auto const int_tok =  tokenise_c( accept_str( "int"  ) );
auto const real_tok = tokenise_c( accept_str( "real" ) );
auto const bool_tok = tokenise_c( accept_str( "bool" ) );

auto const zero_tok = tokenise_c( accept( is_char( '0' ) ) );
auto const numeral_tok = tokenise_c( some( accept( is_digit ) ) );

auto const dot_tok = tokenise_c( accept( is_char( '.' ) ) );
auto const semicolon_tok = tokenise_c( accept( is_char( ';' ) ) );
auto const decimal_tok =
   (zero_tok || numeral_tok)
&& dot_tok
&& tokenise_c( some( accept( is_digit ) ) );

auto const true_tok = tokenise_c( accept_str( "true" ) );
auto const false_tok = tokenise_c( accept_str( "false" ) );
auto const boolean_tok = tokenise_c( accept_str( "false" )
                                || accept_str( "true" ) );

auto const string_tok = tokenise_c( accept_str( "string" ) );
auto const formula_tok = tokenise_c( accept_str( "formula" ) );
auto const astring_tok = tokenise_c( some( accept( is_any ) ) );
auto const at_tok = tokenise_c( accept( is_char( '@' ) ) );

auto const idn_tok =
   reject_strs( keywords )
&& accept( is_alpha )
&& tokenise_c( many( accept( is_alnum ) || accept( is_char( '_' ) ) ) );

auto const idp_tok = idn_tok && tokenise_c( accept( is_char( '\'' ) ) );

auto const and_tok =   tokenise_c( accept_str( "and" ) ||   accept_str( "&&" ) );
auto const or_tok =    tokenise_c( accept_str( "or" ) ||    accept_str( "||" ) );
auto const imply_tok = tokenise_c( accept_str( "imply" ) || accept_str( "->" ) );
auto const equiv_tok = tokenise_c( accept_str( "equiv" )
                                || accept_str( "<->" ) );
auto const not_tok =   tokenise_c( accept_str( "not" )
                                || accept_str( "!" ) );

auto const arrow_tok = tokenise_c( accept_str( "->" ) );

auto const round_left_tok = tokenise_c( accept( is_char( '(' ) ) );
auto const round_right_tok = tokenise_c( accept( is_char( ')' ) ) );
auto const curly_left_tok = tokenise_c( accept( is_char( '{' ) ) );
auto const curly_right_tok = tokenise_c( accept( is_char( '}' ) ) );
auto const square_left_tok = tokenise_c( accept( is_char( '[' ) ) );
auto const square_right_tok = tokenise_c( accept( is_char( ']' ) ) );
auto const colon_tok = tokenise_c( accept( is_char( ':' ) ) );
auto const comma_tok = tokenise_c( accept( is_char( ',' ) ) );
auto const square_right_left_tok = square_right_tok && square_left_tok;
auto const bar_tok = tokenise_c( accept( is_char( '|' ) ) );

auto const is_last = discard( tokenise_c( accept( is_eof ) ) );
auto const empty_tok =  tokenise_c( accept_str( "" ) );

auto const par_tok = tokenise_c( accept_str( "par" ) );
auto const in_tok = tokenise_c( accept_str( "in" ) );
auto const out_tok = tokenise_c( accept_str( "out" ) );
auto const tid_tok = tokenise_c( accept_str( "tid" ) );
auto const not_tid_tok = tokenise_c( reject_str( "tid" ) );

auto const initial_tok = tokenise_c( accept_str( "initial" ) );
auto const states_tok = tokenise_c( accept_str( "states" ) );
auto const final_tok = tokenise_c( accept_str( "final" ) );
auto const error_tok = tokenise_c( accept_str( "error" ) );
auto const havoc_tok = tokenise_c( accept_str( "havoc" ) );
auto const instances_tok = tokenise_c( accept_str( "instances" ) );
auto const nts_tok = tokenise_c( accept_str( "nts" ) );
auto const init_tok = tokenise_c( accept_str( "init" ) );

auto const plus_tok =  tokenise_c( accept( is_char( '+' ) ) );
auto const minus_tok = tokenise_c( accept( is_char( '-' ) ) );
auto const times_tok = tokenise_c( accept( is_char( '*' ) ) );
auto const div_tok =   tokenise_c( accept( is_char( '/' ) ) );
auto const mod_tok =   tokenise_c( accept( is_char( '%' ) ) );

auto const neq_tok = tokenise_c( accept_str( "!=" ) );
auto const geq_tok = tokenise_c( accept_str( ">=" ) );
auto const leq_tok = tokenise_c( accept_str( "<=" ) );

auto const equal_tok = tokenise_c( accept( is_char( '=' ) ) );
auto const lt_tok =    tokenise_c( accept( is_char( '<' ) ) );
auto const gt_tok =    tokenise_c( accept( is_char( '>' ) ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Type syntax------------------
//<type> ::= {int,real,bool}

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

//----------Numeral parser----------------
template<>
const handle< numeral > parser< numeral > =
            caller< numeral >( "numeral",
              numeral_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Decimal syntax---------------
//<decimal> ::= <numeral> {.}{0..9}+

//----------Decimal parser----------------
template<>
const handle< decimal > parser< decimal > =
            caller< decimal >( "decimal",
              decimal_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Boolean syntax---------------
//<boolean> ::= {true,false}

//----------Boolean parser----------------
template<>
const handle< boolean > parser< boolean > =
            caller< boolean >( "boolean",
              boolean_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Idn syntax-------------------
//<idn> ::= {a..z,A..Z}{a..z,A..Z,0..9,_}*

//----------Idn parser--------------------
template<>
const handle< idn > parser< idn > =
            caller< idn >( "idn",
              idn_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Idp syntax-------------------
//<idp> ::= <idp> {'}

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

//----------Id parser---------------------
template<>
const handle< id > parser< id > =
   attempt( caller< id >( "id:idp",
              reference( "idp", parser< idp > ) ) )
||          caller< id >( "id:idn",
              reference( "idn", parser< idn > ) );
//-------------------------------------------------
