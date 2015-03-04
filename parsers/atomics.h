#include "Parser-Combinators/parser_combinators.hpp"
#include "parsers/generals.h"

#pragma once

//-------------------------------------------------
//--------------------Rop parser-------------------
//<rop> ::= {=,!=,<=,<,>=,>}

//----------Rop parser--------------------
template<>
const handle< rop > parser< rop > =
   attempt( caller< rop >( "rop:=",
                           equal_tok ) )
|| attempt( caller< rop >( "rop:!=",
                           neq_tok ) )
|| attempt( caller< rop >( "rop:<=",
                           leq_tok ) )
|| attempt( caller< rop >( "rop:<",
                           lt_tok ) )
|| attempt( caller< rop >( "rop:>=",
                           geq_tok ) )
||          caller< rop >( "rop:>",
                           gt_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Idn-list vector syntax-------
//<v-idn-list> ::= <idn> , <v-idn-list>
//               | <idn>

//----------Idn-list vector parser--------
template<>
const vhandle< idn > vparser_cm< idn > =
   attempt( conscaller< idn >( "idns:cons",
                               comma_tok,
                               reference( "idns", vparser_cm< idn > ),
                               reference( "idn",  parser< idn > ) ) )
||          lastcaller< idn >( "idns:last",
                               reference( "idn", parser< idn > ) );
//-------------------------------------------------                     

//-------------------------------------------------
//--------------------Idn-list syntax--------------
//<idn-list> ::= <idn>
//             |  <idn-list> , <idn>

//----------Idn-list parser---------------
template<>
const handle< idn_list > parser< idn_list > =
            caller< idn_list >( "idn-list:non-empty",
                                reference( "idns", vparser_cm< idn > ) );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Idn-list-e syntax------------
//<idn-list-e> ::= <idn-list>
//               | epsilon

//----------Idn-list-e parser-------------
template<>
const handle< idn_list_e > parser< idn_list_e > =
   attempt( caller< idn_list_e >( "idn-list-e:non-empty",
                                  reference( "idns", vparser_cm< idn > ) ) )
||          caller< idn_list_e >( "idn-list-e:empty", empty_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Havoc parser-----------------
//<havoc> ::= havoc ( <idn-list-e> )

//----------Havoc parser------------------
template<>
const handle< havoc > parser< havoc > =
   attempt( caller< havoc >( "havoc:non-empty",
                             havoc_tok,
                             round_left_tok,
                             reference( "idns", vparser_cm< idn > ),
                             round_right_tok ) )
||          caller< havoc >( "havoc:empty",
                             havoc_tok,
                             round_left_tok,
                             round_right_tok );
//-------------------------------------------------

//-------------------------------------------------
//--------------------Atom syntax------------------
//<atom> ::= <bool-term>
//         | <arith-term> <rop> <arith-term>
//         | <array-write> = [ <arith-list> ]
//         | <havoc>

//----------Atom parser-------------------
template<>
handle< atom > const parser< atom > =
   attempt( caller< atom >( "atom:rop",
                            reference( "arith-term", parser< arith_term > ),
                            reference( "rop",        parser< rop > ),
                            reference( "arith-term", parser< arith_term > ) ) )
|| attempt( caller< atom >( "atom:array",
                            reference( "array-write", parser< array_write > ),
                            equal_tok,
                            square_left_tok,
                            reference( "arith-list",  parser< arith_list > ),
                            square_right_tok ) )
|| attempt( caller< atom >( "atom:havoc",
                            reference( "havoc", parser< havoc > ) ) )
||          caller< atom >( "atom:bool-term",
                            reference( "bool-term", parser< bool_term > ) );
//-------------------------------------------------
