#include "parsers/tokens.h"
#include "parsers/generals.h"
#include "parsers/terms.h"
#include "parsers/arithmetic.h"
#include "parsers/atomics.h"

#pragma once

build_op< quantifier, qkind::forall > bop_qf;
build_op< quantifier, qkind::exists > bop_qe;

//-------------------------------------------------
//--------------------Quantifier parser------------
//<quantifier> ::= { forall,
//                   exists }
const handle< quantifier > for_quant() {
  return log( "forall", all( bop_qf, forall_tok ) );
}

const handle< quantifier > exi_quant() {
  return log( "exists", all( bop_qe, exists_tok ) );
}

template<>
const handle< quantifier > parser< quantifier > =
   attempt( for_quant() )
||          exi_quant();
//--------------------------------------------------

build_itemise< q_type, type > bit_qtt;
build_itemise_three_ss< q_type, type, arith_term > bit_qta;

//--------------------------------------------------
//--------------------Quantified type parser--------
//<q-type> ::= <type> |
//             <type> [ <arith-term> , <arith-term> ]
const handle< q_type > unb_q_type( handle< type > ht ) {
  return log( "q-type-unbounded", all( bit_qtt, ht ) );
}

const handle< q_type > bou_q_type( handle< type > ht, handle< arith_term > ha ) {
  return log( "q-type-bounded",
              all( bit_qta, ht, square_left_tok, ha, comma_tok, ha ) );
}

template<>
const handle< q_type > parser< q_type > =
   attempt( bou_q_type( reference( "type", parser< type > ),
                        reference( "arith_term", parser< arith_term > ) )
         && discard( square_right_tok ) )
|| unb_q_type( reference( "type", parser< type > ) );
//--------------------------------------------------

build_itemise_three< formula, atom, bop, formula > bit_fab;
build_itemise_three< formula, formula, bop, formula > bit_ffb;
build_itemise_three_true< formula, formula, bop, formula > bit_fft;
//build_itemise_four< formula, formula, bop, formula > bit_ffb;

build_itemise< formula, atom > bit_fat;
build_itemise< formula, formula > bit_fof;
build_itemise_true< formula, formula > btt_fof;
build_formula_bop bit_fob;
build_formula bit_for;

//--------------------------------------------------
//--------------------Formula parser----------------
//<formula> ::= <atom> |
//              ( <formula> ) |
//              <formula> <bop> <formula> |
//              not <formula> |
//              <quantifier> <idn-list> : <q-type> . <formula>
const handle< formula > ato_bop_form( handle< atom > ha, handle< bop > hb,
                                      handle< formula > hf ) {
  return log( "for-ato-bop", all( bit_fab, ha, hb, hf ) );
}

const handle< formula > par_bop_form( handle< formula > hf, handle< bop > hb ) {
  return log( "for-par-bop", all( bit_ffb, hf, hb, hf ) );
}

const handle< formula > not_bop_form( handle< formula > hf, handle< bop > hb ) {
  return log( "for-not-bop", all( bit_fft, hf, hb, hf ) );
}

const handle< formula > qua_bop_form( handle< quantifier > hq,
                                      handle< idn_list > hi,
                                      handle< q_type > ht,
                                      handle< formula > hf,
                                      handle< bop > hb ) {
  return log( "for_qua_bop",
              all( bit_fob, hq, hi, colon_tok, ht, dot_tok, hf, hb, hf ) );
}

const handle< formula > ato_form( handle< atom > ha ) {
  return log( "for-ato", all( bit_fat, ha ) );
}
  
const handle< formula > par_form( handle< formula > hf ) {
  return log( "for-par", all( bit_fof, hf ) );
}

const handle< formula > not_form( handle< formula > hf ) {
  return log( "for-not", all( btt_fof, hf ) );
}

handle< formula > const qua_form( handle< quantifier > hq,
                                  handle< idn_list > hi,
                                  handle< q_type > ht,
                                  handle< formula > hf ) {
  return log( "for-qua",
              all( bit_for, hq, hi, colon_tok, ht, dot_tok, hf ) );
}

template<>
const handle< formula > parser< formula > =
   attempt( ato_bop_form( reference( "atom", parser< atom > ),
                          reference( "bop", parser< bop > ),
                          reference( "formula", parser< formula > ) ) )
|| attempt( discard( round_left_tok )
         && par_bop_form( reference( "formula", parser< formula > ),
                          reference( "bop", parser< bop > ) ) )
|| attempt( discard( not_tok )
         && not_bop_form( reference( "formula", parser< formula > ),
                          reference( "bop", parser< bop > ) ) )
|| attempt( qua_bop_form( reference( "quantifier", parser< quantifier > ),
                          reference( "idn_list", parser< idn_list > ),
                          reference( "q_type", parser< q_type > ),
                          reference( "formula", parser< formula > ),
                          reference( "bop", parser< bop > ) ) )
  
|| attempt( ato_form( reference( "atom", parser< atom > ) ) )
|| attempt( discard( round_left_tok )
         && par_form( reference( "formula", parser< formula > ) )
         && discard( round_right_tok ) )
|| attempt( discard( not_tok )
         && not_form( reference( "formula", parser< formula > ) ) )
||          qua_form( reference( "quantifier", parser< quantifier > ),
                      reference( "idn_list", parser< idn_list > ),
                      reference( "q_type", parser< q_type > ),
                      reference( "formula", parser< formula > ) );
//--------------------------------------------------
