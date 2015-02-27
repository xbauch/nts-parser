#include "Parser-Combinators/parser_combinators.hpp"
#include "parsers/generals.h"

#pragma once

build_itemise< arith_lit, id > bit_ali;
build_empty< arith_lit > bue_alt;
build_itemise< arith_lit, numeral > bit_aln;
build_itemise< arith_lit, decimal > bit_ald;

//-------------------------------------------------
//--------------------Arith-lit parser-------------
//<arith-lit> ::= <id> |
//                tid |
//                <numeral> |
//                <decimal>
const handle< arith_lit > id_ari_lit( handle< id > hi ) {
  return log( "arith-lit-id", all( bit_ali, hi ) );
}

const handle< arith_lit > tid_ari_lit() {
  return log( "arith-lit-tid", all( bue_alt, tid_tok ) );
}

const handle< arith_lit > num_ari_lit( handle< numeral > hn ) {
  return log( "arith-lit-numeral", all( bit_aln, hn ) );
}

const handle< arith_lit > dec_ari_lit( handle< decimal > hd ) {
  return log( "arith-lit-decimal", all( bit_ald, hd ) );
}

template<>
const handle< arith_lit >  parser< arith_lit > =
   attempt( id_ari_lit( reference( "id", parser< id > ) ) )
|| attempt( tid_ari_lit() )
|| attempt( dec_ari_lit( reference( "decimal", parser< decimal > ) ) )
||          num_ari_lit( reference( "numeral", parser< numeral > ) );
//-------------------------------------------------

build_op< aop, akind::plus > bop_ap;
build_op< aop, akind::minus > bop_am;
build_op< aop, akind::times > bop_at;
build_op< aop, akind::div > bop_ad;
build_op< aop, akind::srem > bop_as;

//-------------------------------------------------
//--------------------Aop parser-------------------
//<aop> ::= {+,-,*,/,%}
const handle< aop > plu_aop() {
  return log( "aop-plus", all( bop_ap, plus_tok ) );
}

const handle< aop > min_aop() {
  return log( "aop-minus", all( bop_am, minus_tok ) );
}

const handle< aop > tim_rop() {
  return log( "aop-times", all( bop_at, times_tok ) );
}

const handle< aop > div_rop() {
  return log( "aop-div", all( bop_ad, div_tok ) );
}

const handle< aop > rem_rop() {
  return log( "aop-mod", all( bop_as, mod_tok ) );
}

template<>
const handle< aop > parser< aop > =
   attempt( plu_aop() )
|| attempt( min_aop() )
|| attempt( tim_rop() )
|| attempt( div_rop() )
||          rem_rop();
//------------------------------------------------

//-------------------------------------------------
//--------------------Index-term vector-parser-----

build_index_term_arr_aop_cat bit_aac;
build_index_term_lit_aop_cat bit_lac;
build_index_term_par_aop_cat bit_pac;

build_index_term_arr_cat bit_arc;
build_index_term_lit_cat bit_lrc;
build_index_term_par_cat bit_prc;

build_index_term_arr_aop bit_aro;
build_index_term_lit_aop bit_lao;
build_index_term_par_aop bit_pao;

build_index_term_arr bit_tar;
build_vectorise_con< index_term, arith_lit > bvc_ita;
build_vectorise_vcon< index_term, index_term > bvc_itt;

const vhandle< index_term > vec_ind_ter_arr_aop_cat( handle< idn > hi,
                                                     vhandle< index_term > vt,
                                                     handle< aop > ho ) {
  return log( "vec-ind-ter-arr-aop-cat",
              all( bit_aac, hi, square_left_tok, vt, square_right_tok, ho, vt,
                   square_right_left_tok, vt ) );
}

const vhandle< index_term > vec_ind_ter_lit_aop_cat( handle< arith_lit > hl,
                                                     handle< aop > ho,
                                                     vhandle< index_term > vt ) {
  return log( "vec-ind-ter-lit-aop-cat",
              all( bit_lac, hl, ho, vt, square_right_left_tok, vt ) );
}

const vhandle< index_term > vec_ind_ter_par_aop_cat( vhandle< index_term > vt,
                                                     handle< aop > ho ) {
  return log( "vec-ind-ter-par-aop-cat",
              all( bit_pac, vt, round_right_tok, ho, vt, square_right_left_tok, vt ) );
}

const vhandle< index_term > vec_ind_ter_arr_cat( handle< idn > hi,
                                                 vhandle< index_term > vt ) {
  return log( "vec-ind-ter-arr-cat",
              all( bit_arc, hi, square_left_tok, vt, square_right_tok,
                   square_right_left_tok, vt ) );
}

const vhandle< index_term > vec_ind_ter_lit_cat( handle< arith_lit > hl,
                                                 vhandle< index_term > vt ) {
  return log( "vec-ind-ter-lit-cat",
              all( bit_lrc, hl, square_right_left_tok, vt ) );
}

const vhandle< index_term > vec_ind_ter_par_cat( vhandle< index_term > vt ) {
  return log( "vec-ind-ter-par-cat",
              all( bit_prc, vt, round_right_tok, square_right_left_tok, vt ) );
}

const vhandle< index_term > vec_ind_ter_arr_aop( handle< idn > hi,
                                                 vhandle< index_term > vt,
                                                 handle< aop > ho ) {
  return log( "vec-ind-ter-arr-aop",
              all( bit_aro, hi, square_left_tok, vt, square_right_tok, ho, vt ) );
}

const vhandle< index_term > vec_ind_ter_lit_aop( handle< arith_lit > hl,
                                                 handle< aop > ho,
                                                 vhandle< index_term > vt ) {
  return log( "vec-ind-ter-lit-aop", all( bit_lao, hl, ho, vt ) );
}

const vhandle< index_term > vec_ind_ter_par_aop( vhandle< index_term > vt,
                                                 handle< aop > ho ) {
  return log( "vec-ind-ter-par-aop",
              all( bit_pao, vt, round_right_tok, ho, vt ) );
}

const vhandle< index_term > vec_ind_ter_arr( handle< idn > hi,
                                             vhandle< index_term > vt ) {
  return log( "vec-ind-ter-arr",
              all( bit_tar, hi, square_left_tok, vt ) );
}


const vhandle< index_term > vec_ind_ter_par( vhandle< index_term > vt ) {
   return log( "vec-ind-ter-par", all( bvc_itt, vt ) );
}

const vhandle< index_term > vec_ind_ter_lit( handle< arith_lit > hl ) {
  return log( "vec-ind-ter-lit", all( bvc_ita, hl ) );
}

template<>
const vhandle< index_term > vparser_sq< index_term > =
//<idn> [ <v-index-term> ] <aop> <v-index-term> ][ <v-index-term>
   attempt( vec_ind_ter_arr_aop_cat( reference( "idn", parser< idn > ),
                                     reference( "index-terms",
                                                vparser_sq< index_term > ),
                                     reference( "aop", parser< aop > ) ) )
//<arith-lit>              <aop> <v-index-term> ][ <v-index-term>
|| attempt( vec_ind_ter_lit_aop_cat( reference( "arith-lit", parser< arith_lit > ),
                                     reference( "aop", parser< aop > ),
                                     reference( "index-terms",
                                                vparser_sq< index_term > ) ) )
//( <v-index-term> )       <aop> <v-index-term> ][ <v-index-term>  
|| attempt( discard( round_left_tok )
         && vec_ind_ter_par_aop_cat( reference( "index_terms",
                                                vparser_sq< index_term > ),
                                     reference( "aop", parser< aop > ) ) )

//<idn> [ <v-index-term> ]                      ][ <v-index-term>  
|| attempt( vec_ind_ter_arr_cat( reference( "idn", parser< idn > ),
                                 reference( "index-terms",
                                            vparser_sq< index_term > ) ) )
//( <v-index-term> )                            ][ <v-index-term>
|| attempt( discard( round_left_tok )
         && vec_ind_ter_par_cat( reference( "index_terms",
                                            vparser_sq< index_term > ) ) )
//<arith-lit>                                   ][ <v-index-term>
|| attempt( vec_ind_ter_lit_cat( reference( "arith-lit", parser< arith_lit > ),
                                 reference( "index-terms",
                                            vparser_sq< index_term > ) ) )
                                            
//<idn> [ <v-index-term> ] <aop> <v-index-term>
|| attempt( vec_ind_ter_arr_aop( reference( "idn", parser< idn > ),
                                 reference( "index-terms", vparser_sq< index_term > ),
                                 reference( "aop", parser< aop > ) ) )

//<arith-lit>              <aop> <v-index-term>
|| attempt( vec_ind_ter_lit_aop( reference( "arith-lit", parser< arith_lit > ),
                                 reference( "aop", parser< aop > ),
                                 reference( "index-terms",
                                            vparser_sq< index_term > ) ) )
//( <v-index-term> )       <aop> <v-index-term>
|| attempt( discard( round_left_tok )
         && vec_ind_ter_par_aop( reference( "index_terms", vparser_sq< index_term > ),
                                 reference( "aop", parser< aop > ) ) )
//<idn> [ <v-index-term> ]  
|| attempt( vec_ind_ter_arr( reference( "idn", parser< idn > ),
                             reference( "index-terms", vparser_sq< index_term > ) )
         && discard( square_right_tok ) )
//( <v-index-term> )
|| attempt( discard( round_left_tok )
         && vec_ind_ter_par( reference( "index_terms", vparser_sq< index_term > ) )
         && discard( round_right_tok ) )
//<arith-lit>
||          vec_ind_ter_lit( reference( "arith-lit", parser< arith_lit > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Index-term vector-parser-----
build_itemise_vvector< index_term, index_term > biv_iti;

const handle< index_term > ind_ter_vec( vhandle< index_term > vt ) {
  return log( "", all( biv_iti, vt ) );
}

template<>
const handle< index_term > parser< index_term > =
  ind_ter_vec( reference( "index-terms", vparser_sq< index_term > ) );
build_eref_s_vref< array_read, idn, index_term > bes_aia;
//-------------------------------------------------

//-------------------------------------------------
//--------------------Array-read parser------------
//<array-read> ::= <idn> [ <arith-lit> ] |
//                 <array-read> [ <arith-lit> ]
const handle< array_read > arr_read( handle< idn > hi, vhandle< index_term > vl ) {
  return log( "array-read-vec",
              all( bes_aia, hi, square_left_tok, vl ) );
}

template<>
const handle< array_read > parser< array_read > =
   arr_read( reference( "idn", parser< idn > ),
             reference( "arith-lits", vparser_sq< index_term > ) )
&& discard( square_right_tok );
//-------------------------------------------------

build_itemise< array_term, array_read > bit_ata;
build_itemise< array_term, idn > bit_ati;

//-------------------------------------------------
//--------------------Array-term parser------------
//<array-term> ::= <array-read> |
//                 |<idn>|
const handle< array_term > read_arr( handle< array_read > ha ) {
  return log( "array-term-array-read", all( bit_ata, ha ) );
}

const handle< array_term > size_arr( handle< idn > hi ) {
  return log( "array-term-size", all( bit_ati, hi ) );
}

template<>
const handle< array_term > parser< array_term > =
   attempt( read_arr( reference( "array-read", parser< array_read > ) ) )
||          ( discard( bar_tok )
           && size_arr( reference( "idn", parser< idn > ) ) 
           && discard( bar_tok ) );
//-------------------------------------------------

build_op< sign, skind::sign_true > bop_st;
build_op< sign, skind::sign_false > bop_sf;

//-------------------------------------------------
//--------------------Sign parser------------------
//<sign> ::= {-} | epsilon
handle< sign > const tru_sig() {
  return log( "sign-true", all( bop_st, minus_tok ) );
}

handle< sign > const fal_sig() {
  return log( "sign-false", all( bop_sf, empty_tok ) );
}

template<>
const handle< sign > parser< sign > =
   attempt( tru_sig() )
||          fal_sig();
//-------------------------------------------------

build_binop_as_par< arith_term, sign, aop, arith_lit, arith_term > bbi_asl;
build_binop_as_par< arith_term, sign, aop, array_term, arith_term > bbi_asa;
build_binop_par< arith_term, aop, arith_term > bbi_apt;
build_itemise< arith_term, arith_term > bit_ara;
build_itemise_two< arith_term, sign, array_term > bit_asa;
build_itemise_two< arith_term, sign, arith_lit > bit_asl;

//-------------------------------------------------
//--------------------Arith-term parser------------
//<arith-term> ::= <sign> <arith-lit> |
//                 <sign> <array-term> |
//                 ( <arith-term> ) |
//                 <arith-term> <aop> <arith-term>
const handle< arith_term > par_aop_ari_ter( handle< arith_term > hat,
                                             handle< aop > hap ) {
  return log( "arit-term-par-aop", all( bbi_apt, hat, round_right_tok, hap, hat ) );
}

const handle< arith_term > par_ari_ter( handle< arith_term > hat ) {
  return log( "arit-term-par", all( bit_ara, hat ) );
}

const handle< arith_term > lit_aop_ari_term( handle< sign > has,
                                             handle< arith_lit > hal,
                                             handle< aop > hap,
                                             handle< arith_term > hat ) {
  return log( "arit-term-lit-aop", all( bbi_asl, has, hal, hap, hat ) );
}

const handle< arith_term > arr_aop_ari_term( handle< sign > has,
                                             handle< array_term > haa,
                                             handle< aop > hap,
                                             handle< arith_term > hat ) {
  return log( "arit-term-arr-aop", all( bbi_asa, has, haa, hap, hat ) );
}

const handle< arith_term > arr_ari_term( handle< sign > has,
                                         handle< array_term > haa ) {
  return log( "arit-term-arr", all( bit_asa, has, haa ) );
}

const handle< arith_term > lit_ari_term( handle< sign > has,
                                         handle< arith_lit > hal ) {
  return log( "arit-term-arr", all( bit_asl, has, hal ) );
}

template<>
const handle< arith_term > parser< arith_term > =
   attempt( discard( round_left_tok )
         && par_aop_ari_ter( reference( "arith-term", parser< arith_term > ),
                             reference( "aop", parser< aop > ) ) )
|| attempt( discard( round_left_tok )
         && par_ari_ter( reference( "arith-term", parser< arith_term > ) )
         && discard( round_right_tok ) )
|| attempt( lit_aop_ari_term( reference( "sign", parser< sign > ),
                              reference( "arith-lit", parser< arith_lit > ),
                              reference( "aop", parser< aop > ),
                              reference( "arith-term", parser< arith_term > ) ) )
|| attempt( arr_aop_ari_term( reference( "sign", parser< sign > ),
                              reference( "array-term", parser< array_term > ),
                              reference( "aop", parser< aop > ),
                              reference( "arith-term", parser< arith_term > ) ) )
|| attempt( arr_ari_term( reference( "sign", parser< sign > ),
                          reference( "array-term", parser< array_term > ) ) )
||          lit_ari_term( reference( "sign", parser< sign > ),
                          reference( "arith-lit", parser< arith_lit > ) );
//-------------------------------------------------

build_cat< arith_term > buc_art;
build_vectorise< arith_term > buv_art;

const vhandle< arith_term > cat_arit_term_cm( handle< arith_term > ha,
                                              vhandle< arith_term > va ) {
  return log( "cat", all( buc_art, ha, comma_tok, va ) );
}

const vhandle< arith_term > vec_arit_term( handle< arith_term > ha ) {
  return log( "con", all( buv_art, ha ) );
}

template<>
const vhandle< arith_term > vparser_cm< arith_term > =
   attempt( cat_arit_term_cm( reference( "arith-term", parser< arith_term > ),
                              reference( "v-arith-term", vparser_cm< arith_term > ) ) )
||          vec_arit_term( reference( "arith-term", parser< arith_term > ) );

build_itemise_vector< arith_list, arith_term > bit_ala;

//-------------------------------------------------
//--------------------Arith-list parser------------
const handle< arith_list > non_arith_list( vhandle< arith_term > vt ) {
  return log( "arith-list", all( bit_ala, vt ) );
}

template<>
const handle< arith_list > parser< arith_list > =
  non_arith_list( reference( "arith_terms", vparser_cm< arith_term > ) );
//-------------------------------------------------

build_itemise_vector< arith_list_e, arith_term > bit_ale;
build_empty< arith_list_e > bue_ale;

//-------------------------------------------------
//--------------------Arith-list-e parser----------
//<arith-list> ::= <arith-term> |
//                 <arith-list> , <arith-term>
const handle< arith_list_e > non_arith_list_e( vhandle< arith_term > vt ) {
  return log( "arith-list-e-nonempty",
              all( bit_ale, vt ) );
}

const handle< arith_list_e > emp_arith_list_e() {
  return log( "arith-list-e-empty", all( bue_ale, empty_tok ) );
}

template<>
const handle< arith_list_e > parser< arith_list_e > =
   attempt( non_arith_list_e( reference( "arith_terms", vparser_cm< arith_term > ) ) )
||          emp_arith_list_e();
//-------------------------------------------------

const vhandle< arith_term > cat_arit_term_sq( handle< arith_term > ha,
                                              vhandle< arith_term > va ) {
  return log( "cat", all( buc_art, ha, square_right_left_tok, va ) );
}

template<>
const vhandle< arith_term > vparser_sq< arith_term > =
   attempt( cat_arit_term_sq( reference( "arith-term", parser< arith_term > ),
                              reference( "v-arith-term", vparser_sq< arith_term > ) ) )
||          vec_arit_term( reference( "arith-term", parser< arith_term > ) );


build_vref_s_vref< multi, arith_term > bvs_mat;
build_itemise_vector< multi, arith_term > biv_mat;
build_empty< multi > bue_mul;

//-------------------------------------------------
//--------------------Multi parser-----------------
//<multi> ::= [ <arith-term> ] <multi> |
//            [ <arith-list> ] |
//            epsilon
const handle< multi > non_multi( vhandle< arith_term > v1,
                                 vhandle< arith_term > v2 ) {
  return log( "multi-nonempty", all( bvs_mat, v1, comma_tok, v2 ) );
}

const handle< multi > alm_non_multi( vhandle< arith_term > va ) {
  return log( "multi-list-nonempty", all( biv_mat, va ) );
}

const handle< multi > emp_multi() {
  return log( "multi-empty", all( bue_mul, empty_tok ) );
}

template<>
const handle< multi > parser< multi > =
   attempt( discard( square_left_tok )
         && non_multi( reference( "arith_terms", vparser_sq< arith_term > ),
                       reference( "arith_terms", vparser_cm< arith_term > ) )
         && discard( square_right_tok ) )
|| attempt( discard( square_left_tok )
         && alm_non_multi( reference( "arith-terms", vparser_cm< arith_term > ) )
         && discard( square_right_tok ) )
||          emp_multi();
//-------------------------------------------------

build_itemise_two< array_write, idp, multi > bit_aim;
//-------------------------------------------------
//--------------------Array-write parser-----------
//<array-write> ::= <idp> <multi>
const handle< array_write > arr_wrt( handle< idp > hi, handle< multi > hm ) {
  return log( "array-write", all( bit_aim, hi, hm ) );
}

template<>
const handle< array_write > parser< array_write > =
  arr_wrt( reference( "idp", parser< idp > ),
           reference( "multi", parser< multi > ) );
//-------------------------------------------------
