#include "Parser-Combinators/parser_combinators.hpp"
#include "parsers/generals.h"

#pragma once

build_op< bop, bkind::equiv > bop_eq;
build_op< bop, bkind::imply > bop_im;
build_op< bop, bkind::bor > bop_or;
build_op< bop, bkind::band > bop_an;
//-------------------------------------------------
//--------------------Bop parser-------------------
//<bop> ::= {and,or,imply,equiv,&&,||,->,<->}
const handle< bop > equ_bop() {
  return log( "bop-equiv", all( bop_eq, equiv_tok ) );
}

const handle< bop > imp_bop() {
  return log( "bop-imply", all( bop_im, imply_tok ) );
}

const handle< bop > or_bop() {
  return log( "bop-or", all( bop_or, or_tok ) );
}

const handle< bop > and_bop() {
  return log( "bop-and", all( bop_an, and_tok ) );
}

template<>
const handle< bop > parser< bop > =
   attempt( equ_bop() )
|| attempt( imp_bop() )
|| attempt( or_bop() )
||          and_bop();
//--------------------------------------------------

build_itemise< bool_lit, id > bit_ld;
build_op< bool_lit, blkind::bl_true > bop_tru;
build_op< bool_lit, blkind::bl_false > bop_fl;

//--------------------------------------------------
//--------------------Bool-lit parser---------------
//<bool-lit> ::= <id> |
//               true |
//               false
handle< bool_lit > const id_bool_lit( handle< id > hi ) {
  return log( "bool-lit-id", all( bit_ld, hi ) );
}

handle< bool_lit > const tru_lit() {
  return log( "bool-lit-true", all( bop_tru, true_tok ) );
}

handle< bool_lit > const fal_lit() {
  return log( "bool-lit-false", all( bop_fl, false_tok ) );
}

template<>
const handle< bool_lit > parser< bool_lit > =
   attempt( tru_lit() )
|| attempt( fal_lit() )
||          id_bool_lit( reference( "id", parser< id > ) );
//-------------------------------------------------

build_binop_as< bool_term, bop, bool_lit, bool_term > bbi_bop;
build_itemise_true< bool_term, bool_term > bit_btn;
build_itemise< bool_term, bool_term > bit_btp;
build_itemise< bool_term, bool_lit > bit_btl;
build_binop_par< bool_term, bop, bool_term > bbi_bpp;

//-------------------------------------------------
//--------------------Bool-term parser-------------
//<bool-term> ::= <bool-lit> |
//                ( <bool-term> ) |
//                <not> <bool-term> |
//                <bool-term> <bop> <bool-term>
handle< bool_term > const lit_boo( handle< bool_lit > hl ) {
  return log( "bool-term-bool-lit", all( bit_btl, hl ) );
}
  
handle< bool_term > const par_boo( handle< bool_term > ht ) {
  return log( "bool-term-par", all( bit_btp, ht ) );
}

handle< bool_term > const par_boo2( handle< bool_term > ht,
                                    handle< bop > ho ) {
  return log( "bool-term-bop-par", all( bbi_bpp, ht, round_right_tok, ho, ht ) );
}

const handle< bool_term > not_boo( handle< bool_term > ht ) {
  return log( "bool-term-not", all( bit_btn, ht ) );
}

const handle< bool_term > bop_boo( handle< bool_lit > hl,
                                   handle< bop > ho,
                                   handle< bool_term > ht ) {
  return log( "bool-term-bop", all( bbi_bop, hl, ho, ht ) );
}

template<>
const handle< bool_term > parser< bool_term > =
   attempt( bop_boo( reference( "bool-lit", parser< bool_lit > ),
                     reference( "bop", parser< bop > ),
                     reference( "bool-term", parser< bool_term > ) ) )
|| attempt( discard( not_tok ) &&
            not_boo( reference( "bool-term", parser< bool_term > ) ) )
|| attempt( discard( round_left_tok )
         && par_boo2( reference( "bool-term", parser< bool_term > ),
                      reference( "bop", parser< bop > ) ) )
|| attempt( discard( round_left_tok )
         && par_boo( reference( "bool-term", parser< bool_term > ) )
         && discard( round_right_tok ) )
||          lit_boo( reference( "bool-lit", parser< bool_lit > ) );
//-------------------------------------------------
