#include "parsers/basics.h"

#pragma once

//--------------------------------------------------
//--------------------Instance syntax---------------
//<instance> ::= <idn> [ <arith-term> ]

//----------Instance parser-------------------
template<>
const handle< instance > parser< instance > =
            caller< instance >( "instance",
              reference( "idn", parser< idn > ),
              square_left_tok,
              reference( "arith-term", parser< arith_term > ),
              square_right_tok );
//--------------------------------------------------

template<>
const vhandle< instance > vparser_cm< instance > =
   attempt( conscaller< instance >( "instances:cons",
              comma_tok,
              reference( "v-instance", vparser_cm< instance > ),
              reference( "instance",   parser< instance > ) ) )
||          lastcaller< instance >( "instances:last",
              reference( "instance",   parser< instance > ) );

//------------------------------------------------
//--------------------Inst-list syntax------------
//<inst-list> ::= <instance>
//              | <inst-list> , <instance>

//----------Inst-list parser--------------
template<>
const handle< inst_list > parser< inst_list > =
            caller< inst_list >( "inst-list",
              reference( "instance-vector", vparser_cm< instance > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Instances syntax--------------
//<instances> ::= instances <inst-list> ;

//----------Instances parser------------------
template<>
const handle< instances > parser< instances > =
            caller< instances >( "instances",
              instances_tok,
              reference( "inst-list", parser< inst_list > ),
              semicolon_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Nts-name syntax---------------
//<nts-name> ::= <annotations> nts <idn> ;

//----------Nts-name parser-------------------
template<>
const handle< nts_name > parser< nts_name > =
            caller< nts_name >( "nts-name",
              reference( "annotations", parser< annotations > ),
              nts_tok,
              reference( "idn", parser< idn > ),
              semicolon_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Init syntax-------------------
//<init> ::= init <formula> ;

//----------Init parser-------------------
template<>
const handle< init > parser< init > =
            caller< init >( "init",
              init_tok,
              reference( "formula", parser< formula > ),
              semicolon_tok );
//--------------------------------------------------

template<>
const vhandle< nts_basic > vparser_em< nts_basic > =
   attempt( conscaller< nts_basic >( "nts-basics:cons",
              empty_tok,
              reference( "v-nts-basic", vparser_em< nts_basic > ),
              reference( "nts-basic",   parser< nts_basic > ) ) )
||          lastcaller< nts_basic >( "nts-basics:last",
              reference( "nts_basic",   parser< nts_basic > ) );

//------------------------------------------------
//--------------------Nts-list syntax-------------
//<nts-list> ::= <nts-basic> <nts-list>
//             | epsilon

//----------Nts-list parser---------------
template<>
const handle< nts_list > parser< nts_list > =
   attempt( caller< nts_list >( "nts-list:non-empty",
              reference( "nts-basic-vector", vparser_em< nts_basic > ) ) )
||          caller< nts_list >( "nts-list:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Nts syntax--------------------
//<nts> ::= <nts-name> <decl-glob> <init> <instances> <nts-list>

//----------Nts parser--------------------
template<>
const handle< nts > parser< nts > =
            caller< nts >( "nts",
              reference( "nts-name", parser< nts_name > ),
              reference( "decl-glob", parser< decl_glob > ),
              reference( "init", parser< init > ),
              reference( "instances", parser< instances > ),
              reference( "nts-list", parser< nts_list > ) );
//--------------------------------------------------
