#include "Parser-Combinators/parser_combinators.hpp"
#include "parsers/generals.h"

#pragma once

build_op< rop, rkind::rop_equ > bop_equ;
build_op< rop, rkind::rop_neq > bop_ne;
build_op< rop, rkind::rop_leq > bop_le;
build_op< rop, rkind::rop_ltn > bop_lt;
build_op< rop, rkind::rop_geq > bop_ge;
build_op< rop, rkind::rop_gtn > bop_gt;

//-------------------------------------------------
//--------------------Rop parser-------------------
//<rop> ::= {=,!=,<=,<,>=,>}
handle< rop > const equ_rop() {
  return log( "=", all( bop_equ, equal_tok ) );
}

handle< rop > const neq_rop() {
  return log( "!=", all( bop_ne, neq_tok ) );
}

handle< rop > const leq_rop() {
  return log( "<=", all( bop_le, leq_tok ) );
}

handle< rop > const ltn_rop() {
  return log( "<", all( bop_lt, lt_tok ) );
}

handle< rop > const geq_rop() {
  return log( ">=", all( bop_ge, geq_tok ) );
}

handle< rop > const gtn_rop() {
  return log( ">", all( bop_gt, gt_tok ) );
}

template<>
const handle< rop > parser< rop > =
   attempt( equ_rop() )
|| attempt( neq_rop() )
|| attempt( leq_rop() )
|| attempt( ltn_rop() )
|| attempt( geq_rop() )
||          gtn_rop();
//-------------------------------------------------

build_cat< idn > buc_idn;
build_vectorise< idn > buv_idn;

const vhandle< idn > cat_idn_cm( handle< idn > hi, vhandle< idn > vi ) {
  return log( "idn-list-rec", all( buc_idn, hi, comma_tok, vi ) );
}

const vhandle< idn > vec_idn( handle< idn > hi ) {
  return log( "idn-list-last", all( buv_idn, hi ) );
}

template<>
const vhandle< idn > vparser_cm< idn > =
   attempt( cat_idn_cm( reference( "elem", parser< idn > ),
                        reference( "sep", vparser_cm< idn > ) ) )
||          vec_idn( reference( "elem", parser< idn > ) );

build_itemise_vector< idn_list, idn > biv_ili;

//-------------------------------------------------
//--------------------Idn-list parser--------------
//<idn-list> ::= <idn> |
//               <idn-list> , <idn>
handle< idn_list > const non_idn_list( vhandle< idn > vi ) {
  return log( "idn-list", all( biv_ili, vi ) );
}

template<>
const handle< idn_list > parser< idn_list > =
  non_idn_list( reference( "idn", vparser_cm< idn > ) );
//-------------------------------------------------

build_itemise_vector< idn_list_e, idn > biv_ile;
build_empty< idn_list_e > bue_ile;

//-------------------------------------------------
//--------------------Idn-list-e parser------------
//<idn-list-e> ::= <idn-list> |
//                 epsilon
const handle< idn_list_e > non_idn_list_e( vhandle< idn > vi ) {
  return log( "idn-list-nonempty", all( biv_ile, vi ) );
}

const handle< idn_list_e > emp_idn_list_e() {
  return log( "idn-list-empty", all( bue_ile, empty_tok ) );
}

template<>
const handle< idn_list_e > parser< idn_list_e > =
   attempt( non_idn_list_e( reference( "idn", vparser_cm< idn > ) ) )
||          emp_idn_list_e();
//-------------------------------------------------

build_itemise_vector< havoc, idn > biv_hai;
build_empty< havoc > bue_hav;

//-------------------------------------------------
//--------------------Havoc parser-----------------
//<havoc> ::= havoc ( <idn-list-e> )
const handle< havoc > non_havo( vhandle< idn > vi ) {
  return log( "havoc ( <idn-list> )", all( biv_hai, vi ) );
}

const handle< havoc > emp_havo() {
  return log( "havoc ()", all( bue_hav, empty_tok ) );
}

template<>
const handle< havoc > parser< havoc > =
   discard( havoc_tok )
&& discard( round_left_tok )
&& ( attempt( non_havo( reference( "idn", vparser_cm< idn > ) ) )
  ||          emp_havo() )
&& discard( round_right_tok );
//-------------------------------------------------

build_itemise< atom, bool_term > bit_atb;
build_binop< atom, rop, arith_term > bbi_ara;
build_itemise_two_ss< atom, array_write, arith_list > bis_aaa;
build_itemise< atom, havoc > bit_ath;

//-------------------------------------------------
//--------------------Atom parser------------------
//<atom> ::= <bool-term> |
//           <arith-term> <rop> <arith-term>
//           <array-write> = [ <arith-list> ]
//           <havoc>
handle< atom > const boo_atom( handle< bool_term > ht ) {
  return log( "atom-bool-term", all( bit_atb, ht ) );
}

handle< atom > const rop_atom( handle< arith_term > ht, handle< rop > hr ) {
  return log( "atom-rop", all( bbi_ara, ht, hr, ht ) );
}
  
const handle< atom > arr_atom( handle< array_write > ha, handle< arith_list > hl ) {
  return log( "atom-array-equals", all( bis_aaa, ha, equal_tok, square_left_tok, hl ) );
}

const handle< atom > hav_atom( handle< havoc > hh ) {
  return log( "atom-havoc", all( bit_ath, hh ) );
}

template<>
handle< atom > const parser< atom > =
//   attempt( boo_atom( reference( "bool-term", parser< bool_term > ) ) )
   attempt( rop_atom( reference( "arith-term", parser< arith_term > ),
                      reference( "rop", parser< rop > ) ) )
|| attempt( arr_atom( reference( "array-write", parser< array_write > ),
                      reference( "arith-list", parser< arith_list > ) )
         && discard( square_right_tok ) )
|| attempt( hav_atom( reference( "havoc", parser< havoc > ) ) )
|| boo_atom( reference( "bool-term", parser< bool_term > ) );
//-------------------------------------------------
