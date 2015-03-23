#include "parsers/annotations.h"

#pragma once

//--------------------------------------------------
//--------------------Basic syntax------------------
//<basic> ::= <annotations> <idn>

//----------Basic parser------------------
template<>
const handle< basic > parser< basic > =
            caller< basic >( "basic:basic",
              reference( "annotations", parser< annotations > ),
              reference( "idn",         parser< idn > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Array syntax------------------
//<array> ::= <basic> [ <arith-term> ]
//          | <array> [ <arith-term> ]

//----------Basic parser------------------
template<>
const handle< nts_array > parser< nts_array > =
            caller< nts_array >( "nts-array",
              reference( "basic",       parser< basic > ),
              square_left_tok,
              reference( "arith-terms", vparser_sq< arith_term > ),
              square_right_tok );
//--------------------------------------------------

template<>
const handle< empty_list > parser< empty_list > =
            caller< empty_list >( "empty-list",
              empty_tok );
template<>
const vhandle< empty_list > vparser_sq< empty_list > =
   attempt( conscaller< empty_list >( "empty-list:cons",
              square_right_left_tok,
              reference( "v-empty-list", vparser_sq< empty_list > ),
              reference( "empty-list",   parser< empty_list > ) ) )
||          lastcaller< empty_list >( "empty-list:last",
              reference( "empty-list",   parser< empty_list > ) );

//--------------------------------------------------
//--------------------Array-ref syntax--------------
//<array-ref> ::= <basic> []
//              | <array> []
//              | <array-ref> []

//----------Array-ref parser------------------
template<>
const handle< array_ref > parser< array_ref > =
   attempt( caller< array_ref >( "array-ref:basic",
              reference( "basic",       parser< basic > ),
              square_left_tok,
              reference( "arith-terms", vparser_sq< empty_list > ),
              square_right_tok ) )
||          caller< array_ref >( "array-ref:array",
              reference( "nts_array",   parser< nts_array > ),
              square_left_tok,
              reference( "arith-terms", vparser_sq< empty_list > ),
              square_right_tok );

//--------------------------------------------------

//--------------------------------------------------
//--------------------Array-pure-ref syntax---------
//<array-pure-ref> ::= <basic> []
//                   | <array-pure-ref> []

//----------Array-pure-ref parser---------
template<>
const handle< array_pure_ref > parser< array_pure_ref > =
            caller< array_pure_ref >( "array-pure-ref",
              reference( "basic",       parser< basic > ),
              square_left_tok,
              reference( "arith-terms", vparser_sq< empty_list > ),
              square_right_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-lit syntax---------------
//<decl-lit> ::= <basic>
//             | <arrray>
//             | <array-ref>

//----------Decl-lit parser---------------
template<>
const handle< decl_lit > parser< decl_lit > =
   attempt( caller< decl_lit >( "decl-lit:array-ref",
              reference( "array-ref", parser< array_ref > ) ) )
|| attempt( caller< decl_lit >( "decl-lit:array",
              reference( "array",     parser< nts_array > ) ) )
||          caller< decl_lit >( "decl-lit:basic",
              reference( "basic",     parser< basic > ) );
//--------------------------------------------------

template<>
const vhandle< decl_lit > vparser_cm< decl_lit > =
   attempt( conscaller< decl_lit >( "decl-lits:cons",
              comma_tok,
              reference( "v-decl-lit", vparser_cm< decl_lit > ),
              reference( "decl-lit",   parser< decl_lit > ) ) )
||          lastcaller< decl_lit >( "decl-lits:last",
              reference( "decl-lit",   parser< decl_lit > ) );

//--------------------------------------------------
//--------------------Decl-lits syntax--------------
//<decl-lits> ::= <decl-lits> , <decl-lit>
//              | <decl-lit>

//----------Decl-lits parser--------------
template<>
const handle< decl_lits > parser< decl_lits > =
            caller< decl_lits >( "decl-lits",
              reference( "decl-lit-vector", vparser_cm< decl_lit > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-block syntax-------------
//<decl-block> ::= <decl-lits> : <type>

//----------Decl-block parser--------------
template<>
const handle< decl_block > parser< decl_block > =
            caller< decl_block >( "decl-block",
              reference( "decl-lits", parser< decl_lits > ),
              colon_tok,
              reference( "type",      parser< type > ) );
//--------------------------------------------------

template<>
const vhandle< decl_block > vparser_cm< decl_block > =
   attempt( conscaller< decl_block >( "decl-block:cons",
              comma_tok,
              reference( "v-decl-block", vparser_cm< decl_block > ),
              reference( "decl-block",   parser< decl_block > ) ) )
||          lastcaller< decl_block >( "decl-block:last",
              reference( "decl-block",   parser< decl_block > ) );

//--------------------------------------------------
//--------------------Decl-blocks syntax------------
//<decl-blocks> ::= <decl-blocks> , <decl-block>
//                | <decl-block>

//----------Decl-blocks parser--------------
template<>
const handle< decl_blocks > parser< decl_blocks > =
            caller< decl_blocks >( "decl-blocks",
              reference( "decl-block-vector", vparser_cm< decl_block > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-par-lit syntax-----------
//<decl-par-lit> ::= <basic>
//                 | <array>

//----------Decl-par-lit parser-----------
template<>
const handle< decl_par_lit > parser< decl_par_lit > =
   attempt( caller< decl_par_lit >( "decl-par-lit:array",
              reference( "array", parser< nts_array > ) ) )
||          caller< decl_par_lit >( "decl-par-lit:basic",
              reference( "basic", parser< basic > ) );
//--------------------------------------------------

template<>
const vhandle< decl_par_lit > vparser_cm< decl_par_lit > =
   attempt( conscaller< decl_par_lit >( "decl-par-lit:cons",
              comma_tok,
              reference( "v-decl-par-lit", vparser_cm< decl_par_lit > ),
              reference( "decl-par-lit",   parser< decl_par_lit > ) ) )
||          lastcaller< decl_par_lit >( "decl-par-lit:last",
              reference( "decl-par-lit",   parser< decl_par_lit > ) );

//--------------------------------------------------
//--------------------Decl-par-lits syntax----------
//<decl-par-lits> ::= <decl-par-lits> , <decl-par-lit>
//                  | <decl-par-lit>

//----------Decl-par-lits parser----------
template<>
const handle< decl_par_lits > parser< decl_par_lits > =
   attempt( caller< decl_par_lits >( "decl-par-lits:non-empty",
              reference( "decl-par-lit",  parser< decl_par_lit > ),
              comma_tok,
              reference( "decl-par-lits", vparser_cm< decl_par_lit > ) ) )
||          caller< decl_par_lits >( "decl-par-lits:empty",
              reference( "decl-par-lit",  parser< decl_par_lit > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-par-block syntax---------
//<decl-par-block> ::= <decl-par-lits> : <type>

//----------Decl-par-block parser---------
template<>
const handle< decl_par_block > parser< decl_par_block > =
            caller< decl_par_block >(
              "decl-par-block",
              reference( "decl-par-lits", parser< decl_par_lits > ),
              colon_tok,
              reference( "type",          parser< type > ) );
//--------------------------------------------------

template<>
const vhandle< decl_par_block > vparser_cm< decl_par_block > =
   attempt( conscaller< decl_par_block >( "decl-par-block:cons",
              comma_tok,
              reference( "v-decl-par-block", vparser_cm< decl_par_block > ),
              reference( "decl-par-block",   parser< decl_par_block > ) ) )
||          lastcaller< decl_par_block >( "decl-par-block:last",
              reference( "decl-par-block",   parser< decl_par_block > ) );

//--------------------------------------------------
//--------------------Decl-par-blocks syntax--------
//<decl-par-blocks> ::= <decl-par-blocks> , <decl-par-block>
//                    | <decl-par-block>

//----------Decl-par-blocks parser--------
template<>
const handle< decl_par_blocks > parser< decl_par_blocks > =
            caller< decl_par_blocks >( "decl-par-blocks",
              reference( "decl-par-block-vector",
                         vparser_cm< decl_par_block > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-io-lit syntax------------
//<decl-io-lit> ::= <basic>
//                | <array-pure-ref>

//----------Decl-io-lit parser------------
template<>
const handle< decl_io_lit > parser< decl_io_lit > =
   attempt( caller< decl_io_lit >( "decl-io-lit:array-pure-ref",
              reference( "array-pure-ref", parser< array_pure_ref > ) ) )
||          caller< decl_io_lit >( "decl-io-lit:basic",
              reference( "basic",          parser< basic > ) );
//--------------------------------------------------

template<>
const vhandle< decl_io_lit > vparser_cm< decl_io_lit > =
   attempt( conscaller< decl_io_lit >( "decl-io-lit:cons",
              comma_tok,
              reference( "v-decl-io-lit", vparser_cm< decl_io_lit > ),
              reference( "decl-io-lit",   parser< decl_io_lit > ) ) )
||          lastcaller< decl_io_lit >( "decl-io-lit:last",
              reference( "decl-io-lit",   parser< decl_io_lit > ) );

//--------------------------------------------------
//--------------------Decl-io-lits syntax-----------
//<decl-io-lits> ::= <decl-io-lits> , <idn>
//                  | <decl-io-lit>

//----------Decl-io-lits parser----------
template<>
const handle< decl_io_lits > parser< decl_io_lits > =
   attempt( caller< decl_io_lits >( "decl-io-lits:non-empty",
              reference( "decl-io-lit",  parser< decl_io_lit > ),
              comma_tok,
              reference( "decl-io-lits", vparser_cm< decl_io_lit > ) ) )
||          caller< decl_io_lits >( "decl-io-lits:empty",
              reference( "decl-io-lit",  parser< decl_io_lit > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-io-block syntax---------
//<decl-io-block> ::= <decl-io-lits> : <type>

//----------Decl-io-block parser---------
template<>
const handle< decl_io_block > parser< decl_io_block > =
            caller< decl_io_block >( "decl-io-block",
              reference( "decl-io-lits", parser< decl_io_lits > ),
              colon_tok,
              reference( "type",         parser< type > ) );
//--------------------------------------------------

template<>
const vhandle< decl_io_block > vparser_cm< decl_io_block > =
   attempt( conscaller< decl_io_block >( "decl-io-block:cons",
              comma_tok,
              reference( "v-decl-io-block", vparser_cm< decl_io_block > ),
              reference( "decl-io-block",   parser< decl_io_block > ) ) )
||          lastcaller< decl_io_block >( "decl-io-block:last",
              reference( "decl-io-block",   parser< decl_io_block > ) );

//--------------------------------------------------
//--------------------Decl-io-blocks syntax---------
//<decl-io-blocks> ::= <decl-io-blocks> , <decl-io-block>
//                    | <decl-io-block>

//----------Decl-io-blocks parser---------
template<>
const handle< decl_io_blocks > parser< decl_io_blocks > =
            caller< decl_io_blocks >( "decl-io-blocks",
              reference( "decl-io-block-vector",
                         vparser_cm< decl_io_block > ) );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl syntax-------------------
//<decl> ::= <decl-blocks> ;
//         | par <decl-par-blocks> ;

//----------Decl parser-------------------
template<>
const handle< decl > parser< decl > =
   attempt( caller< decl >( "decl:par",
              par_tok,
              reference( "decl-par-blocks", parser< decl_par_blocks > ),
              semicolon_tok ) )
||          caller< decl >( "decl:seq",
              reference( "decl-blocks",     parser< decl_blocks > ),
              semicolon_tok );
//--------------------------------------------------

template<>
const vhandle< decl > vparser_em< decl > =
   attempt( conscaller< decl >( "decls:cons",
              empty_tok,
              reference( "v-decl", vparser_em< decl > ),
              reference( "decl",   parser< decl > ) ) )
||          lastcaller< decl >( "decls:last",
              reference( "decl",   parser< decl > ) );

//--------------------------------------------------
//--------------------Decl-glob syntax--------------
//<decl-glob> ::= <decl-glob> <decl>
//              | epsilon

//----------Decl-glob parser--------------
template<>
const handle< decl_glob > parser< decl_glob > =
   attempt( caller< decl_glob >( "decl-glob:non-empty",
              reference( "decl-vector", vparser_em< decl > ) ) )
||          caller< decl_glob >( "decl-glob:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------In syntax---------------------
//<in> ::= in <decl-io-blocks> ;
//       | epsilon

//----------In parser---------------------
template<>
const handle< in > parser< in > =
   attempt( caller< in >( "in:non-empty",
              in_tok,
              reference( "decl-io-blocks", parser< decl_io_blocks > ),
              semicolon_tok ) )
||          caller< in >( "in:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Out syntax--------------------
//<out> ::= out <decl-io-blocks> ;
//        | epsilon

//----------Out parser---------------------
template<>
const handle< out > parser< out > =
   attempt( caller< out >( "out:non-empty",
              out_tok,
              reference( "decl-io-blocks", parser< decl_io_blocks > ),
              semicolon_tok ) )
||          caller< out >( "out:empty",
              empty_tok );
//--------------------------------------------------

//--------------------------------------------------
//--------------------Decl-loc syntax---------------
//<decl-loc> ::= <decl-loc> <decl>
//             | <in> <out>
//             | epsilon 

//----------Decl-loc parser--------------
template<>
const handle< decl_loc > parser< decl_loc > =
   attempt( caller< decl_loc >( "decl-loc:non-empty",
              reference( "in",          parser< in > ),
              reference( "out",         parser< out > ),
              reference( "decl-vector", vparser_em< decl > ) ) )
|| attempt( caller< decl_loc >( "decl-loc:empty",
              reference( "in",          parser< in > ),
              reference( "out",         parser< out > ) ) )
||      caller< decl_loc >( "decl-loc:emptier",
              empty_tok );
//--------------------------------------------------
