#ifndef TOOLS_NTS2NTS_H_
#define TOOLS_NTS2NTS_H_

#pragma once

#include <iostream>

#include <libNTS/nts.hpp>
#include <libNTS/data_types.hpp>
#include <libNTS/logic.hpp>

#include "parsers/globals.h"

template< typename By, typename Fun, typename In >
auto nts_trans( Fun f, In nts_elem, int index ) ->
decltype( f( nts_elem.template get< By >( index ) ) ) {
  return f( nts_elem.template get< By >( index ) );
}

template< typename T >
std::unique_ptr< T > uniquer( T* t_pointer ) {
  return std::make_unique< T >( t_pointer );
}

template< typename By, typename Fun, typename In >
auto uniq_trans( Fun f, In i, int id ) {
  return uniquer( nts_trans< By >( f, i, id ) );
}

struct nts_2_Nts {
  using NamMap = std::map< std::string, nts::Variable* >; //one for each variable
  using BNtsMap = std::map< std::string, nts::BasicNts* >;
  using StateMap = std::map< std::string, nts::State* >;
  using uT = std::unique_ptr< nts::Term >;
  using VT = std::vector< nts::Term* >;
  using VV = std::vector< nts::Variable* >;
  using cVV = std::vector< const nts::Variable* >;
  using VS = std::vector< nts::State* >;
  using VR = std::vector< nts::Transition* >;
  using pTT = std::pair< VT, VT >;
  using tsTV = std::tuple< std::string, VT, VV >;
  using pTV = std::pair< VT, VV >;
  using Vs = std::vector< std::string >;

  using uF = std::unique_ptr< nts::Formula >;
  
  NamMap glob_vars;
  BNtsMap bntses;
  nts::Nts* n;
  
  nts_2_Nts( nts_parser::nts in );

  //processing
  void process_nts_name(   nts_parser::nts_name nn );
  void process_decl_glob(  nts_parser::decl_glob dg );
  void process_init(       nts_parser::init i );
  void process_instances(  nts_parser::instances i );
  void process_instance(   nts_parser::instance i );
  void process_nts_list(   nts_parser::nts_list nl );
  void process_nts_basic(  nts_parser::nts_basic nb );
  void process_nts_body(   nts_parser::nts_body nb,  nts::BasicNts& bn );
  
  void process_decl_loc(   nts_parser::decl_loc dl,  nts::BasicNts& bn,
                           NamMap& scope );
  void process_statemarks( nts_parser::statemarks s, nts::BasicNts& bn,
                           StateMap& sm );

  vector< nts::AnnotString* > a2a( nts_parser::annotations as );
    
  void process_annotations( nts_parser::annotations a, nts::Annotations& A );
  void process_annotation(  nts_parser::annotation a,  nts::Annotations& A );

  Vs names_from_idn_list( nts_parser::idn_list il );
  
  VV register_variables(            Vs    names, nts::DataType dt, NamMap& scope );
  nts::Variable* register_variable( string name, nts::DataType dt, NamMap& scope );
  nts::Variable* lookup_variable(   string name,                   NamMap& scope );
  
  //variables
  template< typename In >
  VV variables_from_io( In io, NamMap& scope ) {
    using TType = nts_parser::decl_io_blocks;
    if ( io.flat.empty() )
      return VV();
    return variables_from_decl_io_blocks( io.template get< TType >( 0 ), scope );
  }

  VV variables_from_decl_blocks(     nts_parser::decl_blocks db,     NamMap& scope );
  VV variables_from_decl_io_blocks(  nts_parser::decl_io_blocks db,  NamMap& scope );
  VV variables_from_decl_par_blocks( nts_parser::decl_par_blocks db, NamMap& scope );

  VV variables_from_decl(           nts_parser::decl d,            NamMap& scope );
  VV variables_from_decl_io_block(  nts_parser::decl_io_block db,  NamMap& scope );
  VV variables_from_decl_par_block( nts_parser::decl_par_block db, NamMap& scope );
  VV variables_from_decl_block(     nts_parser::decl_block db,     NamMap& scope );
  
  VV variables_from_decl_lits(     nts_parser::decl_lits dl,     nts::ScalarType st,
                                   NamMap& scope );
  VV variables_from_decl_io_lits(  nts_parser::decl_io_lits dl,  nts::ScalarType st,
                                   NamMap& scope );
  VV variables_from_decl_par_lits( nts_parser::decl_par_lits dl, nts::ScalarType st,
                                   NamMap& scope );

  //constant variables
  cVV cvariables_from_arglist( nts_parser::arglist a, NamMap& scope );

  //pointer to variable
  nts::Variable* variable_from_decl_lit(     nts_parser::decl_lit dl,
                                             nts::ScalarType st, NamMap& scope );
  nts::Variable* variable_from_decl_io_lit(  nts_parser::decl_io_lit dl,
                                             nts::ScalarType st, NamMap& scope );
  nts::Variable* variable_from_decl_par_lit( nts_parser::decl_par_lit dl,
                                             nts::ScalarType st, NamMap& scope );

  //names
  string name_from_a_type_val(     nts_parser::a_type_val atv );
  string name_from_decl_io_lit(    nts_parser::decl_io_lit dl );
  string name_from_decl_par_lit(   nts_parser::decl_par_lit dl );
  string name_from_array_pure_ref( nts_parser::array_pure_ref ar );
  string name_from_arith_term(     nts_parser::arith_term at );
  string name_from_id(             nts_parser::id i );
  string name_from_idp(            nts_parser::idp i );
  string name_from_decl_lit(       nts_parser::decl_lit dl );
  string name_from_array_ref(      nts_parser::array_ref ar );
  string name_from_basic(          nts_parser::basic b );

  //transitions
  VR transitions_from_transitions( nts_parser::transitions t, NamMap& scope,
                                   StateMap& sm, nts::BasicNts& bn );

  //pointer to transition
  nts::Transition* transition_from_transition( nts_parser::transition t,
                                               NamMap& scope, StateMap& sm,
                                               nts::BasicNts& bn  );

  //states
  template< typename T >
  VS states_from( T s ) {
    if ( s.flat.empty() )
      return VS();
    return states_from_statelist( s.template get< nts_parser::statelist >( 0 ) );
  }
  VS states_from_states( nts_parser::states s ) {
    if ( s.flat.empty() )
      return VS();
    return states_from_statelist_a( s.get< nts_parser::statelist_a >( 0 ) );
  }

  VS states_from_statelist_a( nts_parser::statelist_a s );
  VS states_from_statelist(   nts_parser::statelist s );

  template< typename Fun >
  void reg_states( VS vs, Fun f, nts::BasicNts& bn, StateMap& sm ) {
    for ( auto s : vs ) {
      s->insert_to( bn );
      sm[ s->name ] = s;
      f( s );
    }
  }
  
  //datatype
  nts::DataType datatype_from_decl_io_lit(  nts_parser::decl_io_lit dl,
                                                           nts::ScalarType st );
  nts::DataType datatype_from_decl_par_lit( nts_parser::decl_par_lit dl,
                                            NamMap& scope, nts::ScalarType st );
  nts::DataType datatype_from_decl_lit(     nts_parser::decl_lit dl,
                                            NamMap& scope, nts::ScalarType st );

  //pointer to transition rule
  std::unique_ptr< nts::TransitionRule >
  transition_rule_from_rule( nts_parser::rule r, NamMap& scope );

  //tuple< name, terms, variables >
  tsTV triple_from_call(      nts_parser::call c,      NamMap& scope );
  tsTV triple_from_call_base( nts_parser::call_base c, NamMap& scope );

  //pair< terms, variables >
  pTV pair_from_arglist(    nts_parser::arglist a,     NamMap& scope );
  pTV pair_from_arith_list( nts_parser::arith_list al, NamMap& scope );

  //terms
  VT terms_from_ret_terms(    nts_parser::ret_terms t,   NamMap& scope );
  VT terms_from_arith_list(   nts_parser::arith_list al, NamMap& scope );
  VT arr_size_from_array(     nts_parser::nts_array a,   NamMap& scope );
  VT arr_size_from_array_ref( nts_parser::array_ref ar,  NamMap& scope );
  
  //pointer to formula
  uF formula_from_init(       nts_parser::init i                      );
  uF formula_from_formula(    nts_parser::formula f,    NamMap& scope );
  uF formula_from_bool_term(  nts_parser::bool_term bt, NamMap& scope );
  uF formula_from_assignment( nts_parser::assignment a, NamMap& scope );
  uF formula_from_havoc(      nts_parser::havoc h,      NamMap& scope );
  uF formula_from_bool_lit(   nts_parser::bool_lit bl,  NamMap& scope );
  uF formula_from_atom(       nts_parser::atom a,       NamMap& scope );

  //quantifier
  nts::Quantifier quantifier_from_quantifier( nts_parser::quantifier q );

  //quantified type
  nts::QuantifiedType quantifiedtype_from_q_type( nts_parser::q_type qt,
                                                  NamMap& scope );
  
  //boolean operator
  nts::BoolOp boolop_from_bop( nts_parser::bop o );

  //relational operator
  nts::RelationOp relationop_from_rop( nts_parser::rop r );
  nts::ArithOp arithop_from_aop( nts_parser::aop o );

  //pointer to term
  uT term_from_bool(       bool b                                   );
  uT term_from_idn(        nts_parser::idn i,         NamMap& scope );
  uT term_from_idp(        nts_parser::idp i,         NamMap& scope );
  uT term_from_id(         nts_parser::id i,          NamMap& scope );
  uT term_from_index_term( nts_parser::index_term at, NamMap& scope );
  uT term_from_arith_term( nts_parser::arith_term at, NamMap& scope );
  uT term_from_array_term( nts_parser::array_term at, NamMap& scope );
  uT term_from_array_read( nts_parser::array_read ar, NamMap& scope );
  uT term_from_arith_lit(  nts_parser::arith_lit al,  NamMap& scope );

  //pair< terms, terms >
  pTT pterms_from_multi( nts_parser::multi m, NamMap& scope );

  //scalar type
  nts::ScalarType scalartype_from_type( nts_parser::type t );
};

#include "tools/nts2Nts.cpp"

#endif
