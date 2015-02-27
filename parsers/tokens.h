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
  && accept( is_alpha )//tokenise( accept( is_alpha ) )
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

build_op< type, tkind::integer > bop_ti;
build_op< type, tkind::real > bop_tr;
build_op< type, tkind::boolean > bop_tb;

//-------------------------------------------------
//--------------------Type parser------------------
//<type> ::= {int,real,bool}
const handle< type > int_typ() {
  return log( "type-int", all( bop_ti, int_tok ) );
}

const handle< type > rea_typ() {
  return log( "type-real", all( bop_tr, real_tok ) );
}

const handle< type > boo_typ() {
  return log( "type-bool", all( bop_tb, bool_tok ) );
}

template<>
const handle< type > parser< type > =
   attempt( int_typ() )
|| attempt( rea_typ() )
||          boo_typ();
//-------------------------------------------------

const build_constant< numeral > bco_n;

//-------------------------------------------------
//--------------------Numeral parser---------------
//<numeral> ::= {0} | {1..9}{0..9}*
const handle< numeral > zer_num() {
  return log( "numeral-zero", all( bco_n, zero_tok ) );
}

const handle< numeral > non_num() {
  return log( "numeral-nonzero", all( bco_n, numeral_tok ) );
}

template<>
const handle< numeral > parser< numeral > =
   attempt( zer_num() )
||          non_num();
//-------------------------------------------------

const build_constant< decimal > bco_d;

//-------------------------------------------------
//--------------------Decimal parser---------------
//<decimal> ::= <numeral> {.}{0..9}+
const handle< decimal > dec_dec() {
  return log( "decimal", all( bco_d, decimal_tok ) );
}

template<>
const handle< decimal > parser< decimal > =
  dec_dec();
//-------------------------------------------------

const build_constant< boolean > bco_b;

//-------------------------------------------------
//--------------------Boolean parser---------------
//<boolean> ::= {true,false}
const handle< boolean > boo_boo() {
  return log( "boolean", all( bco_b, boolean_tok ) );
}

template<>
const handle< boolean > parser< boolean > =
  boo_boo();
//-------------------------------------------------

const build_variable< idn > bva_n;

//-------------------------------------------------
//--------------------Idn parser-------------------
//<idn> ::= {a..z,A..Z}{a..z,A..Z,0..9,_}*
const handle< idn > idn_idn() {
  cout << "idn" << endl;
  return log( "<idn>", all( bva_n, idn_tok ) );
}

template<>
const handle< idn > parser< idn > =
  idn_idn();
//-------------------------------------------------

const build_variable< idp > bva_p;

//-------------------------------------------------
//--------------------Idp parser-------------------
//<idp> ::= <idp> {'}
const handle< idp > idp_idp() {
  return log( "<idp>", all( bva_p, idp_tok ) );
}

template<>
const handle< idp > parser< idp > =
  idp_idp();
//-------------------------------------------------

const build_itemise< id, idn > bit_in;
const build_itemise< id, idp > bit_ip;

//-------------------------------------------------
//--------------------Id parser--------------------
//<id> ::= <idn> |
//         <idp>
const handle< id > idn_id( handle< idn > hi ) {
  return log( "<id-idn>", all( bit_in, hi ) );
}

const handle< id > idp_id( handle< idp > hi ) {
  return log( "<id-idp>", all( bit_ip, hi ) );
}

template<>
const handle< id > parser< id > =
   attempt( idp_id( reference( "idp", parser< idp > ) ) )
||          idn_id( reference( "idn", parser< idn > ) );
//-------------------------------------------------