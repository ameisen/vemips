
#ifdef GET_DIAG_ARRAYS
static const int16_t DiagArrays[] = {
  /* Empty */ -1,
  /* DiagArray1 */ diag::warn_pragma_message, -1,
  /* DiagArray2 */ diag::pp_hash_warning, -1,
  /* DiagArray3 */ diag::warn_cfstring_truncated, -1,
  /* DiagArray5 */ diag::warn_independentclass_attribute, diag::warn_ptr_independentclass_attribute, -1,
  /* DiagArray6 */ diag::warn_nsobject_attribute, -1,
  /* DiagArray7 */ diag::warn_function_always_inline_attribute_mismatch, diag::warn_sme_locally_streaming_has_vl_args_returns, diag::warn_sme_streaming_pass_return_vl_to_non_streaming, -1,
  /* DiagArray9 */ diag::warn_abs_too_small, diag::warn_pointer_abs, diag::warn_unsigned_abs, diag::warn_wrong_absolute_value_type, -1,
  /* DiagArray10 */ diag::warn_abstract_final_class, -1,
  /* DiagArray11 */ diag::warn_abstract_vbase_init_ignored, -1,
  /* DiagArray13 */ diag::warn_taking_address_of_packed_member, -1,
  /* DiagArray14 */ diag::ext_typecheck_addrof_temporary, -1,
  /* DiagArray16 */ diag::warn_not_xl_compatible, diag::warn_pragma_align_not_xl_compatible, -1,
  /* DiagArray17 */ diag::ext_alias_template_in_declarative_nns, -1,
  /* DiagArray18 */ diag::warn_param_mismatched_alignment, -1,
  /* DiagArray20 */ diag::warn_alloca, -1,
  /* DiagArray21 */ diag::warn_alloca_align_alignof, -1,
  /* DiagArray22 */ diag::warn_always_inline_coroutine, -1,
  /* DiagArray23 */ diag::warn_ambiguous_suitable_delete_function_found, -1,
  /* DiagArray24 */ diag::warn_misplaced_ellipsis_vararg, -1,
  /* DiagArray25 */ diag::warn_pp_ambiguous_macro, -1,
  /* DiagArray26 */ diag::ext_nested_name_member_ref_lookup_ambiguous, -1,
  /* DiagArray27 */ diag::ext_ovl_ambiguous_oper_binary_reversed, -1,
  /* DiagArray28 */ diag::warn_incompatible_analyzer_plugin_api, -1,
  /* DiagArray29 */ diag::warn_android_unversioned_fallback, -1,
  /* DiagArray30 */ diag::warn_arith_conv_mixed_anon_enum_types, -1,
  /* DiagArray31 */ diag::ext_abstract_pack_declarator_parens, -1,
  /* DiagArray32 */ diag::warn_apinotes_message, -1,
  /* DiagArray34 */ diag::warn_arc_bridge_cast_nonarc, -1,
  /* DiagArray35 */ diag::warn_arc_possible_repeated_use_of_weak, -1,
  /* DiagArray36 */ diag::warn_arc_object_memaccess, -1,
  /* DiagArray37 */ diag::warn_arc_perform_selector_leaks, -1,
  /* DiagArray38 */ diag::warn_arc_repeated_use_of_weak, -1,
  /* DiagArray39 */ diag::warn_arc_retain_cycle, -1,
  /* DiagArray40 */ diag::warn_arc_literal_assign, diag::warn_arc_retained_assign, diag::warn_arc_retained_property_assign, -1,
  /* DiagArray41 */ diag::warn_argument_invalid_range, -1,
  /* DiagArray42 */ diag::warn_argument_undefined_behaviour, -1,
  /* DiagArray43 */ diag::warn_arm_interrupt_vfp_clobber, -1,
  /* DiagArray44 */ diag::warn_array_index_exceeds_bounds, diag::warn_array_index_exceeds_max_addressable_bounds, diag::warn_array_index_precedes_bounds, diag::warn_ptr_arith_exceeds_max_addressable_bounds, diag::warn_static_array_too_small, -1,
  /* DiagArray45 */ diag::warn_ptr_arith_exceeds_bounds, diag::warn_ptr_arith_precedes_bounds, -1,
  /* DiagArray46 */ diag::warn_array_comparison, -1,
  /* DiagArray47 */ diag::warn_array_comparison_cxx26, -1,
  /* DiagArray48 */ diag::warn_inconsistent_array_form, -1,
  /* DiagArray50 */ diag::warn_asm_mismatched_size_modifier, -1,
  /* DiagArray51 */ diag::warn_not_in_enum_assignment, -1,
  /* DiagArray52 */ diag::warn_assume_side_effects, -1,
  /* DiagArray54 */ diag::warn_atimport_in_framework_header, -1,
  /* DiagArray55 */ diag::warn_atomic_member_access, -1,
  /* DiagArray56 */ diag::warn_atomic_op_misaligned, diag::warn_atomic_op_oversized, -1,
  /* DiagArray57 */ diag::warn_atomic_implicit_seq_cst, -1,
  /* DiagArray58 */ diag::warn_atomic_op_has_invalid_memory_order, -1,
  /* DiagArray60 */ diag::warn_atomic_property_rule, -1,
  /* DiagArray61 */ diag::warn_attribute_packed_for_bitfield, -1,
  /* DiagArray62 */ diag::warn_attr_min_eq_max, -1,
  /* DiagArray63 */ diag::warn_fe_backend_warning_attr, diag::warn_toc_unsupported_type, -1,
  /* DiagArray65 */ diag::ext_c23_auto_non_plain_identifier, -1,
  /* DiagArray66 */ diag::warn_drv_disabling_vptr_no_rtti_default, -1,
  /* DiagArray68 */ diag::ext_auto_storage_class, -1,
  /* DiagArray69 */ diag::warn_auto_var_is_id, -1,
  /* DiagArray70 */ diag::warn_availability_and_unavailable, diag::warn_availability_fuchsia_unavailable_minor, diag::warn_availability_on_static_initializer, diag::warn_availability_swift_unavailable_deprecated_only, diag::warn_availability_unknown_environment, diag::warn_availability_unknown_platform, diag::warn_availability_version_ordering, diag::warn_expected_consistent_version_separator, diag::warn_mismatched_availability, diag::warn_mismatched_availability_override, diag::warn_mismatched_availability_override_unavail, -1,
  /* DiagArray71 */ diag::warn_drv_avr_family_linking_stdlibs_not_implemented, diag::warn_drv_avr_libc_not_found, diag::warn_drv_avr_linker_section_addresses_not_implemented, diag::warn_drv_avr_mcu_not_specified, diag::warn_drv_avr_stdlib_not_linked, -1,
  /* DiagArray72 */ diag::warn_fe_backend_plugin, diag::warn_fe_backend_resource_limit, -1,
  /* DiagArray73 */ diag::backslash_newline_space, -1,
  /* DiagArray74 */ diag::warn_bad_function_cast, -1,
  /* DiagArray76 */ diag::ext_rvalue_to_reference_access_ctor, diag::ext_rvalue_to_reference_temp_copy_no_viable, -1,
  /* DiagArray77 */ diag::ext_decomp_decl_cond, -1,
  /* DiagArray78 */ diag::ext_bit_int, diag::ext_cxx_bitint_suffix, -1,
  /* DiagArray79 */ diag::warn_impcast_bitfield_precision_constant, -1,
  /* DiagArray80 */ diag::warn_bitfield_too_small_for_enum, diag::warn_signed_bitfield_enum_conversion, diag::warn_unsigned_bitfield_assigned_signed_enum, -1,
  /* DiagArray81 */ diag::warn_bitfield_width_exceeds_type_width, -1,
  /* DiagArray82 */ diag::warn_precedence_bitwise_conditional, -1,
  /* DiagArray83 */ diag::warn_bitwise_instead_of_logical, -1,
  /* DiagArray84 */ diag::warn_bitwise_op_in_bitwise_op, -1,
  /* DiagArray85 */ diag::warn_block_capture_autoreleasing, -1,
  /* DiagArray86 */ diag::warn_impcast_bool_to_null_pointer, -1,
  /* DiagArray88 */ diag::warn_bitwise_negation_bool, -1,
  /* DiagArray89 */ diag::warn_counted_by_attr_elt_type_unknown_size, -1,
  /* DiagArray90 */ diag::warn_braces_around_init, -1,
  /* DiagArray91 */ diag::warn_incompatible_branch_protection_option, diag::warn_target_unsupported_branch_protection_attribute, diag::warn_unsupported_branch_protection, diag::warn_unsupported_branch_protection_spec, -1,
  /* DiagArray92 */ diag::warn_objc_invalid_bridge, diag::warn_objc_invalid_bridge_to_cf, -1,
  /* DiagArray93 */ diag::warn_assume_aligned_too_great, -1,
  /* DiagArray94 */ diag::ext_pp_redef_builtin_macro, diag::ext_pp_undef_builtin_macro, -1,
  /* DiagArray95 */ diag::warn_builtin_chk_overflow, -1,
  /* DiagArray96 */ diag::warn_implicit_decl_requires_sysheader, -1,
  /* DiagArray97 */ diag::warn_zero_size_struct_union_compat, -1,
  /* DiagArray101 */ diag::ext_old_implicitly_unsigned_long_cxx, diag::warn_auto_storage_class, diag::warn_cxx11_compat_user_defined_literal, diag::warn_cxx11_keyword, diag::warn_cxx11_right_shift_in_template_arg, diag::warn_explicit_instantiation_inline_0x, diag::warn_explicit_instantiation_must_be_global_0x, diag::warn_explicit_instantiation_out_of_scope_0x, diag::warn_explicit_instantiation_unqualified_wrong_namespace_0x, diag::warn_old_implicitly_unsigned_long_cxx, -1,
  /* DiagArray102 */ diag::warn_deprecated_string_literal_conversion, -1,
  /* DiagArray104 */ diag::warn_cxx11_compat_reserved_user_defined_literal, -1,
  /* DiagArray105 */ diag::ext_alias_declaration, diag::ext_array_size_conversion, diag::ext_auto_type_specifier, diag::ext_cxx11_enum_fixed_underlying_type, diag::ext_defaulted_deleted_function, diag::ext_enumerator_list_comma_cxx, diag::ext_explicit_conversion_functions, diag::ext_extern_template, diag::ext_for_range, diag::ext_generalized_initializer_lists, diag::ext_lambda, diag::ext_nested_name_spec_is_enum, diag::ext_nonclass_type_friend, diag::ext_nonstatic_member_init, diag::ext_override_control_keyword, diag::ext_ref_qualifier, diag::ext_rvalue_reference, diag::ext_scoped_enum, diag::ext_static_data_member_in_union, diag::ext_template_arg_object_internal, diag::ext_template_outside_of_template, diag::ext_template_parameter_default_in_function_template, diag::ext_typename_outside_of_template, diag::ext_unelaborated_friend_type, diag::ext_variadic_templates, diag::warn_ext_cxx11_attributes, -1,
  /* DiagArray106 */ diag::ext_extra_semi_cxx11, -1,
  /* DiagArray107 */ diag::ext_inline_namespace, -1,
  /* DiagArray108 */ diag::ext_cxx11_longlong, -1,
  /* DiagArray109 */ diag::ext_cce_narrowing, diag::ext_init_list_constant_narrowing, diag::ext_init_list_type_narrowing, diag::ext_init_list_variable_narrowing, diag::warn_init_list_constant_narrowing, diag::warn_init_list_type_narrowing, diag::warn_init_list_variable_narrowing, -1,
  /* DiagArray110 */ diag::ext_init_list_constant_narrowing_const_reference, diag::ext_init_list_type_narrowing_const_reference, diag::ext_init_list_variable_narrowing_const_reference, -1,
  /* DiagArray111 */ diag::ext_cxx14_attr, -1,
  /* DiagArray112 */ diag::ext_binary_literal_cxx14, -1,
  /* DiagArray115 */ diag::ext_constexpr_body_invalid_stmt, diag::ext_constexpr_body_multiple_return, diag::ext_constexpr_local_var, diag::ext_constexpr_type_definition, diag::ext_decltype_auto_type_specifier, diag::ext_init_capture, diag::ext_variable_template, -1,
  /* DiagArray116 */ diag::ext_cxx17_attr, -1,
  /* DiagArray118 */ diag::warn_cxx17_compat_exception_spec_in_signature, -1,
  /* DiagArray120 */ diag::ext_auto_new_list_init, diag::ext_constexpr_if, diag::ext_constexpr_on_lambda_cxx17, diag::ext_cxx_static_assert_no_message, diag::ext_decomp_decl, diag::ext_fold_expression, diag::ext_for_range_begin_end_types_differ, diag::ext_hex_literal_invalid, diag::ext_init_statement, diag::ext_inline_variable, diag::ext_multi_using_declaration, diag::ext_nested_namespace_definition, diag::ext_ns_enum_attribute, diag::ext_star_this_lambda_capture_cxx17, diag::ext_template_template_param_typename, diag::ext_using_attribute_ns, diag::ext_using_declaration_pack, -1,
  /* DiagArray125 */ diag::ext_cxx20_attr, -1,
  /* DiagArray126 */ diag::warn_cxx17_compat_implicit_typename, diag::warn_cxx20_compat_aggregate_init_with_ctors, diag::warn_cxx20_compat_consteval, diag::warn_cxx20_compat_constinit, diag::warn_cxx20_compat_explicit_bool, diag::warn_cxx20_compat_requires_explicit_init_non_aggregate, diag::warn_cxx20_compat_spaceship, diag::warn_cxx20_compat_use_of_unaddressable_function, diag::warn_cxx20_compat_utf8_string, diag::warn_cxx20_keyword, -1,
  /* DiagArray128 */ diag::ext_cxx_designated_init, -1,
  /* DiagArray129 */ diag::ext_adl_only_template_id, diag::ext_bitfield_member_init, diag::ext_capture_binding, diag::ext_constexpr_body_invalid_stmt_cxx20, diag::ext_constexpr_ctor_missing_init, diag::ext_constexpr_function_try_block_cxx20, diag::ext_constexpr_local_var_no_init, diag::ext_constexpr_union_ctor_no_init, diag::ext_decomp_decl_spec, diag::ext_defaulted_comparison, diag::ext_equals_this_lambda_capture_cxx20, diag::ext_explicit_bool, diag::ext_for_range_init_stmt, diag::ext_implicit_typename, diag::ext_init_capture_pack, diag::ext_inline_nested_namespace_definition, diag::ext_lambda_template_parameter_list, diag::ext_pointer_to_const_ref_member_on_rvalue, diag::ext_using_decl_scoped_enumerator, diag::ext_using_enum_declaration, diag::warn_cxx17_compat_aggregate_init_paren_list, -1,
  /* DiagArray130 */ diag::ext_cxx23_attr, -1,
  /* DiagArray132 */ diag::err_static_lambda, diag::ext_alias_in_init_statement, diag::ext_consteval_if, diag::ext_constexpr_body_invalid_stmt_cxx23, diag::ext_constexpr_static_var, diag::ext_cxx23_pp_directive, diag::ext_cxx23_size_t_suffix, diag::ext_cxx_label_end_of_compound_statement, diag::ext_lambda_missing_parens, diag::ext_operator_overload_static, -1,
  /* DiagArray133 */ diag::ext_decl_attrs_on_lambda, -1,
  /* DiagArray134 */ diag::ext_cxx26_raw_string_literal_character_set, diag::ext_cxx_static_assert_user_generated_message, diag::ext_decl_attrs_on_binding, diag::ext_delete_with_message, diag::ext_pack_indexing, diag::ext_placeholder_var_definition, diag::ext_variadic_friends, -1,
  /* DiagArray146 */ diag::warn_cxx11_compat_binary_literal, -1,
  /* DiagArray148 */ diag::warn_cxx98_compat_alias_declaration, diag::warn_cxx98_compat_alignas, diag::warn_cxx98_compat_alignof, diag::warn_cxx98_compat_attribute, diag::warn_cxx98_compat_auto_type_specifier, diag::warn_cxx98_compat_constexpr, diag::warn_cxx98_compat_ctor_list_init, diag::warn_cxx98_compat_decltype, diag::warn_cxx98_compat_defaulted_deleted_function, diag::warn_cxx98_compat_delegating_ctor, diag::warn_cxx98_compat_empty_scalar_initializer, diag::warn_cxx98_compat_empty_sizeless_initializer, diag::warn_cxx98_compat_enum_fixed_underlying_type, diag::warn_cxx98_compat_enum_nested_name_spec, diag::warn_cxx98_compat_explicit_conversion_functions, diag::warn_cxx98_compat_for_range, diag::warn_cxx98_compat_friend_is_member, diag::warn_cxx98_compat_generalized_initializer_lists, diag::warn_cxx98_compat_goto_into_protected_scope, diag::warn_cxx98_compat_indirect_goto_in_protected_scope, diag::warn_cxx98_compat_initializer_list_init, diag::warn_cxx98_compat_inline_namespace, diag::warn_cxx98_compat_lambda, diag::warn_cxx98_compat_less_colon_colon, diag::warn_cxx98_compat_literal_operator, diag::warn_cxx98_compat_literal_ucn_control_character, diag::warn_cxx98_compat_literal_ucn_escape_basic_scs, diag::warn_cxx98_compat_noexcept_decl, diag::warn_cxx98_compat_noexcept_expr, diag::warn_cxx98_compat_non_static_member_use, diag::warn_cxx98_compat_nonclass_type_friend, diag::warn_cxx98_compat_nonstatic_member_init, diag::warn_cxx98_compat_nontrivial_union_or_anon_struct_member, diag::warn_cxx98_compat_nullptr, diag::warn_cxx98_compat_override_control_keyword, diag::warn_cxx98_compat_pass_non_pod_arg_to_vararg, diag::warn_cxx98_compat_raw_string_literal, diag::warn_cxx98_compat_ref_qualifier, diag::warn_cxx98_compat_reference_list_init, diag::warn_cxx98_compat_rvalue_reference, diag::warn_cxx98_compat_scoped_enum, diag::warn_cxx98_compat_sfinae_access_control, diag::warn_cxx98_compat_static_assert, diag::warn_cxx98_compat_static_data_member_in_union, diag::warn_cxx98_compat_switch_into_protected_scope, diag::warn_cxx98_compat_template_arg_extra_parens, diag::warn_cxx98_compat_template_arg_null, diag::warn_cxx98_compat_template_arg_object_internal, diag::warn_cxx98_compat_template_outside_of_template, diag::warn_cxx98_compat_template_parameter_default_in_function_template, diag::warn_cxx98_compat_trailing_return_type, diag::warn_cxx98_compat_two_right_angle_brackets, diag::warn_cxx98_compat_typename_outside_of_template, diag::warn_cxx98_compat_unelaborated_friend_type, diag::warn_cxx98_compat_unicode_literal, diag::warn_cxx98_compat_unicode_type, diag::warn_cxx98_compat_using_decl_constructor, diag::warn_cxx98_compat_variadic_templates, -1,
  /* DiagArray149 */ diag::warn_cxx98_compat_temp_copy, -1,
  /* DiagArray150 */ diag::warn_cxx98_compat_top_level_semi, -1,
  /* DiagArray151 */ diag::warn_cxx98_compat_template_arg_local_type, -1,
  /* DiagArray152 */ diag::warn_cxx98_compat_array_size_conversion, diag::warn_cxx98_compat_cast_fn_obj, diag::warn_cxx98_compat_empty_fnmacro_arg, diag::warn_cxx98_compat_enumerator_list_comma, diag::warn_cxx98_compat_extern_template, diag::warn_cxx98_compat_longlong, diag::warn_cxx98_compat_no_newline_eof, diag::warn_cxx98_compat_pp_line_too_big, diag::warn_cxx98_compat_variadic_macro, -1,
  /* DiagArray153 */ diag::warn_cxx98_compat_template_arg_unnamed_type, -1,
  /* DiagArray154 */ diag::ext_anonymous_union, diag::ext_c11_anonymous_struct, diag::ext_c11_feature, diag::ext_typecheck_compare_complete_incomplete_pointers, -1,
  /* DiagArray155 */ diag::warn_c23_compat_utf8_string, diag::warn_c23_keyword, -1,
  /* DiagArray156 */ diag::ext_binary_literal, diag::ext_c23_bitint_suffix, diag::ext_c23_enum_fixed_underlying_type, diag::ext_c23_enum_value_not_int, diag::ext_c23_pp_directive, diag::ext_c_empty_initializer, diag::ext_c_label_end_of_compound_statement, diag::ext_c_label_followed_by_declaration, diag::ext_c_nullptr, diag::ext_c_static_assert_no_message, diag::ext_parameter_name_omitted_c23, diag::ext_pp_embed_directive, diag::ext_type_defined_in_offsetof, diag::warn_ext_c23_attributes, -1,
  /* DiagArray159 */ diag::ext_c2y_alignof_incomplete_array, diag::ext_c2y_case_range, diag::ext_c2y_generic_with_type_arg, diag::ext_c2y_imaginary_constant, diag::ext_c2y_increment_complex, -1,
  /* DiagArray160 */ diag::warn_c99_compat_unicode_id, diag::warn_c99_compat_unicode_literal, diag::warn_c99_keyword, diag::warn_old_implicitly_unsigned_long, -1,
  /* DiagArray161 */ diag::ext_designated_init, diag::ext_designated_init_array, diag::ext_designated_init_brace_elision, diag::ext_designated_init_mixed, diag::ext_designated_init_nested, -1,
  /* DiagArray162 */ diag::ext_aggregate_init_not_constant, diag::ext_c99_array_usage, diag::ext_c99_compound_literal, diag::ext_c99_feature, diag::ext_c99_flexible_array_member, diag::ext_c99_variable_decl_in_for_loop, diag::ext_c99_whitespace_required_after_macro_name, diag::ext_empty_fnmacro_arg, diag::ext_enumerator_list_comma_c, diag::ext_hex_constant_invalid, -1,
  /* DiagArray163 */ diag::warn_call_to_pure_virtual_member_function_from_ctor_dtor, -1,
  /* DiagArray164 */ diag::warn_called_once_gets_called_twice, diag::warn_called_once_never_called, diag::warn_called_once_never_called_when, -1,
  /* DiagArray165 */ diag::warn_cast_align, -1,
  /* DiagArray166 */ diag::warn_cast_calling_conv, -1,
  /* DiagArray168 */ diag::warn_cast_function_type, -1,
  /* DiagArray169 */ diag::warn_cast_function_type_strict, -1,
  /* DiagArray170 */ diag::warn_cast_pointer_from_sel, -1,
  /* DiagArray171 */ diag::warn_cast_qual, diag::warn_cast_qual2, -1,
  /* DiagArray172 */ diag::ext_bad_cxx_cast_qualifiers_away_incoherent, -1,
  /* DiagArray174 */ diag::warn_omp_section_is_char, diag::warn_subscript_is_char, -1,
  /* DiagArray175 */ diag::warn_drv_yc_multiple_inputs_clang_cl, diag::warn_drv_ycyu_different_arg_clang_cl, diag::warn_pp_hdrstop_filename_ignored, diag::warn_pp_macro_def_mismatch_with_pch, -1,
  /* DiagArray176 */ diag::warn_conv_to_base_not_used, diag::warn_conv_to_self_not_used, diag::warn_conv_to_void_not_used, -1,
  /* DiagArray177 */ diag::warn_pass_class_arg_to_vararg, -1,
  /* DiagArray178 */ diag::warn_cmse_nonsecure_union, -1,
  /* DiagArray179 */ diag::warn_comma_operator, -1,
  /* DiagArray180 */ diag::escaped_newline_block_comment_end, diag::ext_line_comment, diag::ext_multi_line_line_comment, diag::warn_nested_block_comment, -1,
  /* DiagArray182 */ diag::ext_typecheck_comparison_of_distinct_pointers, -1,
  /* DiagArray183 */ diag::warn_completion_handler_called_twice, diag::warn_completion_handler_never_called, diag::warn_completion_handler_never_called_when, -1,
  /* DiagArray184 */ diag::ext_complex_component_init, -1,
  /* DiagArray186 */ diag::warn_compound_token_split_by_macro, -1,
  /* DiagArray187 */ diag::warn_compound_token_split_by_whitespace, -1,
  /* DiagArray188 */ diag::ext_typecheck_cond_pointer_integer_mismatch, -1,
  /* DiagArray189 */ diag::warn_maybe_uninit_var, -1,
  /* DiagArray190 */ diag::warn_module_config_macro_undef, -1,
  /* DiagArray191 */ diag::warn_impcast_integer_precision_constant, -1,
  /* DiagArray192 */ diag::warn_is_constant_evaluated_always_true_constexpr, -1,
  /* DiagArray193 */ diag::warn_logical_instead_of_bitwise, -1,
  /* DiagArray194 */ diag::warn_cxx14_compat_constexpr_not_const, -1,
  /* DiagArray195 */ diag::warn_attr_on_unconsumable_class, diag::warn_loop_state_mismatch, diag::warn_param_return_typestate_mismatch, diag::warn_param_typestate_mismatch, diag::warn_return_typestate_for_unconsumable_type, diag::warn_return_typestate_mismatch, diag::warn_use_in_invalid_state, diag::warn_use_of_temp_in_invalid_state, -1,
  /* DiagArray196 */ diag::warn_hlsl_impcast_vector_truncation, diag::warn_impcast_complex_scalar, diag::warn_impcast_vector_scalar, diag::warn_opencl_generic_address_space_arg, diag::warn_template_arg_negative, diag::warn_template_arg_too_large, -1,
  /* DiagArray198 */ diag::warn_non_aligned_allocation_function, -1,
  /* DiagArray199 */ diag::warn_coroutine_handle_address_invalid_return_type, -1,
  /* DiagArray200 */ diag::warn_coroutine_promise_unhandled_exception_required_with_exceptions, -1,
  /* DiagArray201 */ diag::warn_unreachable_default, -1,
  /* DiagArray203 */ diag::warn_objc_cdirective_format_string, -1,
  /* DiagArray204 */ diag::warn_ctad_maybe_unsupported, -1,
  /* DiagArray206 */ diag::warn_ctu_incompat_triple, -1,
  /* DiagArray207 */ diag::warn_attribute_argument_n_negative, diag::warn_cuda_attr_lambda_position, diag::warn_kern_is_inline, diag::warn_kern_is_method, diag::warn_pragma_unroll_cuda_value_in_parens, -1,
  /* DiagArray208 */ diag::warn_default_atomic_custom_getter_setter, -1,
  /* DiagArray209 */ diag::ext_cxx11_attr_placement, -1,
  /* DiagArray210 */ diag::warn_dangling_variable, -1,
  /* DiagArray211 */ diag::warn_dangling_pointer_assignment, -1,
  /* DiagArray212 */ diag::warn_dangling_lifetime_pointer_assignment, -1,
  /* DiagArray213 */ diag::warn_dangling_reference_captured, diag::warn_dangling_reference_captured_by_unknown, -1,
  /* DiagArray214 */ diag::warn_dangling_else, -1,
  /* DiagArray215 */ diag::warn_bind_ref_member_to_parameter, diag::warn_dangling_member, diag::warn_init_ptr_member_to_parameter_addr, diag::warn_new_dangling_reference, -1,
  /* DiagArray216 */ diag::warn_dangling_lifetime_pointer, diag::warn_dangling_lifetime_pointer_member, -1,
  /* DiagArray217 */ diag::warn_new_dangling_initializer_list, -1,
  /* DiagArray218 */ diag::warn_drv_darwin_sdk_invalid_settings, -1,
  /* DiagArray219 */ diag::warn_pp_date_time, -1,
  /* DiagArray220 */ diag::warn_dealloc_in_category, -1,
  /* DiagArray221 */ diag::warn_debug_compression_unavailable, -1,
  /* DiagArray222 */ diag::ext_mixed_decls_code, diag::warn_mixed_decls_code, -1,
  /* DiagArray223 */ diag::warn_decls_in_multiple_modules, -1,
  /* DiagArray224 */ diag::warn_defaulted_comparison_deleted, diag::warn_defaulted_method_deleted, -1,
  /* DiagArray225 */ diag::warn_drv_delayed_template_parsing_after_cxx20, -1,
  /* DiagArray226 */ diag::warn_delegating_ctor_cycle, -1,
  /* DiagArray227 */ diag::warn_delete_abstract_non_virtual_dtor, -1,
  /* DiagArray228 */ diag::ext_delete_void_ptr_operand, diag::warn_delete_incomplete, -1,
  /* DiagArray229 */ diag::warn_delete_non_virtual_dtor, -1,
  /* DiagArray231 */ diag::ext_delimited_escape_sequence, -1,
  /* DiagArray232 */ diag::warn_deprecated_lax_vec_conv_all, -1,
  /* DiagArray233 */ diag::warn_O4_is_O3, diag::warn_access_decl_deprecated, diag::warn_drv_deprecated_arg, diag::warn_drv_deprecated_custom, diag::warn_drv_treating_input_as_cxx, diag::warn_omp_depend_in_ordered_deprecated, diag::warn_omp_minus_in_reduction_deprecated, diag::warn_option_invalid_ocl_version, diag::warn_vector_long_decl_spec_combination, -1,
  /* DiagArray234 */ diag::warn_deprecated_altivec_src_compat, -1,
  /* DiagArray235 */ diag::warn_arith_conv_mixed_anon_enum_types_cxx20, -1,
  /* DiagArray236 */ diag::warn_depr_array_comparison, -1,
  /* DiagArray237 */ diag::warn_deprecated_noreturn_spelling, diag::warn_type_attribute_deprecated_on_decl, diag::warn_vector_mode_deprecated, -1,
  /* DiagArray238 */ diag::warn_deprecated_builtin, -1,
  /* DiagArray239 */ diag::warn_deprecated_comma_subscript, -1,
  /* DiagArray240 */ diag::warn_deprecated_copy, -1,
  /* DiagArray242 */ diag::warn_deprecated_copy_with_dtor, -1,
  /* DiagArray243 */ diag::warn_deprecated_copy_with_user_provided_copy, -1,
  /* DiagArray244 */ diag::warn_deprecated_copy_with_user_provided_dtor, -1,
  /* DiagArray245 */ diag::warn_deprecated_for_co_await, -1,
  /* DiagArray246 */ diag::warn_atl_uuid_deprecated, diag::warn_cstyle_param, diag::warn_deprecated, diag::warn_deprecated_fwdclass_message, diag::warn_deprecated_message, diag::warn_property_method_deprecated, -1,
  /* DiagArray247 */ diag::warn_exception_spec_deprecated, -1,
  /* DiagArray248 */ diag::warn_comparison_mixed_enum_types_cxx20, -1,
  /* DiagArray249 */ diag::warn_conditional_mixed_enum_types_cxx20, -1,
  /* DiagArray250 */ diag::warn_arith_conv_mixed_enum_types_cxx20, -1,
  /* DiagArray251 */ diag::warn_arith_conv_enum_float_cxx20, -1,
  /* DiagArray252 */ diag::warn_deprecated_def, diag::warn_unavailable_def, -1,
  /* DiagArray253 */ diag::warn_increment_bool, -1,
  /* DiagArray254 */ diag::warn_deprecated_literal_operator_id, -1,
  /* DiagArray255 */ diag::warn_deprecated_missing_comma_before_ellipsis, -1,
  /* DiagArray256 */ diag::warn_deprecated_module_dot_map, -1,
  /* DiagArray257 */ diag::warn_drv_deprecated_arg_no_relaxed_template_template_args, -1,
  /* DiagArray258 */ diag::warn_non_prototype_changes_behavior, diag::warn_strict_uses_without_prototype, -1,
  /* DiagArray259 */ diag::warn_objc_isa_assign, diag::warn_objc_isa_use, -1,
  /* DiagArray260 */ diag::warn_objc_pointer_masking, -1,
  /* DiagArray261 */ diag::warn_objc_pointer_masking_performSelector, -1,
  /* DiagArray262 */ diag::warn_drv_deprecated_arg_ofast, diag::warn_drv_deprecated_arg_ofast_for_flang, -1,
  /* DiagArray263 */ diag::warn_pragma_deprecated_macro_use, -1,
  /* DiagArray264 */ diag::warn_deprecated_redundant_constexpr_static_def, -1,
  /* DiagArray265 */ diag::warn_deprecated_register, -1,
  /* DiagArray266 */ diag::warn_deprecated_this_capture, -1,
  /* DiagArray267 */ diag::warn_ext_int_deprecated, -1,
  /* DiagArray268 */ diag::warn_deprecated_increment_decrement_volatile, diag::warn_deprecated_simple_assign_volatile, diag::warn_deprecated_volatile_param, diag::warn_deprecated_volatile_return, diag::warn_deprecated_volatile_structured_binding, -1,
  /* DiagArray270 */ diag::warn_direct_ivar_access, -1,
  /* DiagArray271 */ diag::pp_disabled_macro_expansion, -1,
  /* DiagArray274 */ diag::warn_conflicting_param_modifiers, diag::warn_conflicting_ret_type_modifiers, -1,
  /* DiagArray276 */ diag::warn_remainder_division_by_zero, -1,
  /* DiagArray277 */ diag::warn_attribute_dll_redeclaration, -1,
  /* DiagArray278 */ diag::warn_attribute_dllexport_explicit_instantiation_decl, -1,
  /* DiagArray279 */ diag::warn_attribute_dllimport_static_field_definition, -1,
  /* DiagArray280 */ diag::warn_doc_api_container_decl_mismatch, diag::warn_doc_block_command_duplicate, diag::warn_doc_block_command_empty_paragraph, diag::warn_doc_container_decl_mismatch, diag::warn_doc_function_method_decl_mismatch, diag::warn_doc_html_start_tag_expected_ident_or_greater, diag::warn_doc_html_start_tag_expected_quoted_string, diag::warn_doc_inline_command_not_enough_arguments, diag::warn_doc_param_duplicate, diag::warn_doc_param_invalid_direction, diag::warn_doc_param_not_attached_to_a_function_decl, diag::warn_doc_param_not_found, diag::warn_doc_returns_attached_to_a_void_function, diag::warn_doc_returns_not_attached_to_a_function_decl, diag::warn_doc_tparam_duplicate, diag::warn_doc_tparam_not_attached_to_a_template_decl, diag::warn_doc_tparam_not_found, diag::warn_not_a_doxygen_trailing_member_comment, diag::warn_splice_in_doxygen_comment, diag::warn_verbatim_block_end_without_start, -1,
  /* DiagArray281 */ diag::warn_doc_deprecated_not_sync, -1,
  /* DiagArray282 */ diag::warn_doc_html_end_forbidden, diag::warn_doc_html_end_unbalanced, diag::warn_doc_html_missing_end_tag, diag::warn_doc_html_start_end_mismatch, -1,
  /* DiagArray283 */ diag::warn_doc_param_spaces_in_direction, -1,
  /* DiagArray284 */ diag::warn_correct_comment_command_name, diag::warn_unknown_comment_command_name, -1,
  /* DiagArray285 */ diag::ext_dollar_in_identifier, -1,
  /* DiagArray286 */ diag::warn_impcast_double_promotion, -1,
  /* DiagArray287 */ diag::ext_dtor_name_ambiguous, diag::ext_dtor_named_in_wrong_scope, diag::ext_qualified_dtor_named_in_lexical_scope, -1,
  /* DiagArray288 */ diag::ext_destructor_typedef_name, -1,
  /* DiagArray289 */ diag::ext_duplicate_declspec, diag::ext_warn_duplicate_declspec, diag::warn_attribute_address_multiple_identical_qualifiers, diag::warn_duplicate_declspec, -1,
  /* DiagArray290 */ diag::warn_duplicate_enum_values, -1,
  /* DiagArray291 */ diag::warn_method_param_declaration, -1,
  /* DiagArray292 */ diag::warn_duplicate_method_decl, -1,
  /* DiagArray293 */ diag::warn_duplicate_protocol_def, -1,
  /* DiagArray294 */ diag::warn_drv_dxc_missing_dxv, -1,
  /* DiagArray295 */ diag::warn_dyn_class_memaccess, -1,
  /* DiagArray296 */ diag::ext_dynamic_exception_spec, -1,
  /* DiagArray297 */ diag::warn_eagerly_load_for_standard_cplusplus_modules, -1,
  /* DiagArray299 */ diag::ext_enum_base_in_type_specifier, -1,
  /* DiagArray300 */ diag::ext_elaborated_enum_class, -1,
  /* DiagArray301 */ diag::ext_embedded_directive, -1,
  /* DiagArray302 */ diag::warn_empty_for_body, diag::warn_empty_if_body, diag::warn_empty_range_based_for_body, diag::warn_empty_switch_body, diag::warn_empty_while_body, -1,
  /* DiagArray303 */ diag::ext_decomp_decl_empty, -1,
  /* DiagArray304 */ diag::warn_empty_init_statement, -1,
  /* DiagArray305 */ diag::ext_empty_translation_unit, -1,
  /* DiagArray306 */ diag::warn_incomplete_encoded_type, -1,
  /* DiagArray308 */ diag::warn_comparison_mixed_enum_types, -1,
  /* DiagArray309 */ diag::warn_conditional_mixed_enum_types, -1,
  /* DiagArray310 */ diag::warn_comparison_of_mixed_enum_types_switch, -1,
  /* DiagArray311 */ diag::warn_impcast_different_enum_types, -1,
  /* DiagArray312 */ diag::warn_arith_conv_mixed_enum_types, diag::zzzz_warn_conv_mixed_enum_types_cxx26, -1,
  /* DiagArray313 */ diag::warn_arith_conv_enum_float, -1,
  /* DiagArray314 */ diag::ext_enum_too_large, diag::ext_enumerator_increment_too_large, -1,
  /* DiagArray315 */ diag::warn_cdtor_function_try_handler_mem_expr, diag::warn_exception_caught_by_earlier_handler, diag::warn_throw_in_noexcept_func, -1,
  /* DiagArray316 */ diag::ext_excess_initializers, diag::ext_excess_initializers_for_sizeless_type, diag::ext_excess_initializers_in_char_array_initializer, diag::ext_initializer_string_for_char_array_too_long, -1,
  /* DiagArray317 */ diag::warn_anyx86_excessive_regsave, -1,
  /* DiagArray318 */ diag::warn_exit_time_destructor, -1,
  /* DiagArray319 */ diag::warn_defined_in_function_type_macro, diag::warn_defined_in_object_type_macro, -1,
  /* DiagArray320 */ diag::warn_experimental_header_unit, -1,
  /* DiagArray321 */ diag::warn_openmp_experimental, -1,
  /* DiagArray322 */ diag::warn_direct_initialize_call, diag::warn_direct_super_initialize_call, -1,
  /* DiagArray323 */ diag::warn_arc_strong_pointer_objc_pointer, -1,
  /* DiagArray324 */ diag::ext_explicit_specialization_storage_class, -1,
  /* DiagArray326 */ diag::warn_zero_size_struct_union_in_extern_c, -1,
  /* DiagArray327 */ diag::warn_extern_init, -1,
  /* DiagArray329 */ diag::warn_namespace_member_extra_qualification, -1,
  /* DiagArray330 */ diag::ext_extra_semi, diag::warn_extra_semi_after_mem_fn_def, -1,
  /* DiagArray331 */ diag::warn_null_statement, -1,
  /* DiagArray332 */ diag::ext_pp_extra_tokens_at_eol, diag::warn_omp_extra_tokens_at_eol, -1,
  /* DiagArray333 */ diag::warn_missing_symbol_graph_dir, -1,
  /* DiagArray334 */ diag::ext_template_spec_extra_headers, -1,
  /* DiagArray335 */ diag::warn_final_dtor_non_final_class, -1,
  /* DiagArray336 */ diag::warn_pragma_final_macro, -1,
  /* DiagArray337 */ diag::warn_fixedpoint_constant_overflow, -1,
  /* DiagArray338 */ diag::warn_flag_enum_constant_out_of_range, -1,
  /* DiagArray339 */ diag::ext_flexible_array_in_array, diag::ext_flexible_array_in_struct, -1,
  /* DiagArray340 */ diag::warn_impcast_float_integer, -1,
  /* DiagArray341 */ diag::warn_floatingpoint_eq, -1,
  /* DiagArray342 */ diag::warn_impcast_float_to_integer, diag::warn_impcast_float_to_integer_out_of_range, -1,
  /* DiagArray343 */ diag::warn_impcast_float_to_integer_zero, -1,
  /* DiagArray344 */ diag::warn_redundant_loop_iteration, diag::warn_variables_not_in_loop_body, -1,
  /* DiagArray345 */ diag::warn_format_P_no_precision, diag::warn_format_P_with_objc_pointer, diag::warn_format_argument_needs_cast, diag::warn_format_bool_as_character, diag::warn_format_conversion_argument_type_mismatch, diag::warn_format_invalid_annotation, diag::warn_format_invalid_positional_specifier, diag::warn_format_mix_positional_nonpositional_args, diag::warn_format_nonsensical_length, diag::warn_format_string_is_wide_literal, diag::warn_format_zero_positional_specifier, diag::warn_missing_format_string, diag::warn_printf_ObjCflags_without_ObjCConversion, diag::warn_printf_asterisk_missing_arg, diag::warn_printf_asterisk_wrong_type, diag::warn_printf_empty_objc_flag, diag::warn_printf_format_string_contains_null_char, diag::warn_printf_format_string_not_null_terminated, diag::warn_printf_ignored_flag, diag::warn_printf_incomplete_specifier, diag::warn_printf_invalid_objc_flag, diag::warn_printf_narg_not_supported, diag::warn_printf_nonsensical_flag, diag::warn_printf_nonsensical_optional_amount, diag::warn_printf_positional_arg_exceeds_data_args, diag::warn_scanf_nonzero_width, diag::warn_scanf_scanlist_incomplete, -1,
  /* DiagArray346 */ diag::warn_printf_data_arg_not_used, -1,
  /* DiagArray347 */ diag::warn_printf_insufficient_data_args, -1,
  /* DiagArray348 */ diag::warn_format_invalid_conversion, -1,
  /* DiagArray349 */ diag::warn_format_non_standard, diag::warn_format_non_standard_conversion_spec, diag::warn_format_non_standard_positional_arg, -1,
  /* DiagArray350 */ diag::warn_format_nonliteral, -1,
  /* DiagArray351 */ diag::warn_format_overflow, -1,
  /* DiagArray352 */ diag::warn_format_overflow_non_kprintf, -1,
  /* DiagArray353 */ diag::warn_format_argument_needs_cast_pedantic, diag::warn_format_conversion_argument_type_mismatch_pedantic, -1,
  /* DiagArray354 */ diag::warn_format_nonliteral_noargs, -1,
  /* DiagArray355 */ diag::warn_format_conversion_argument_type_mismatch_signedness, -1,
  /* DiagArray356 */ diag::warn_format_truncation, -1,
  /* DiagArray357 */ diag::warn_format_truncation_non_kprintf, -1,
  /* DiagArray358 */ diag::warn_format_conversion_argument_type_mismatch_confusion, -1,
  /* DiagArray360 */ diag::warn_empty_format_string, -1,
  /* DiagArray362 */ diag::warn_fortify_scanf_overflow, diag::warn_fortify_source_overflow, diag::warn_fortify_source_size_mismatch, diag::warn_fortify_strlen_overflow, -1,
  /* DiagArray363 */ diag::warn_four_char_character_literal, -1,
  /* DiagArray364 */ diag::warn_frame_address, -1,
  /* DiagArray365 */ diag::warn_fe_backend_frame_larger_than, diag::warn_fe_frame_larger_than, -1,
  /* DiagArray367 */ diag::warn_framework_include_private_from_public, -1,
  /* DiagArray368 */ diag::warn_free_nonheap_object, -1,
  /* DiagArray369 */ diag::ext_enum_friend, -1,
  /* DiagArray370 */ diag::warn_function_def_in_objc_container, -1,
  /* DiagArray371 */ diag::warn_conflicting_func_effects, diag::warn_func_effect_calls_expr_without_effect, diag::warn_func_effect_calls_func_without_effect, diag::warn_func_effect_violation, diag::warn_invalid_add_func_effects, diag::warn_mismatched_func_effect_override, diag::warn_mismatched_func_effect_redeclaration, -1,
  /* DiagArray372 */ diag::warn_dispatch_body_ignored, diag::warn_multiversion_duplicate_entries, diag::warn_target_clone_duplicate_options, diag::warn_target_clone_no_impact_options, -1,
  /* DiagArray373 */ diag::warn_drv_fuse_ld_path, -1,
  /* DiagArray376 */ diag::ext_clang_diagnose_if, diag::ext_clang_enable_if, diag::ext_warn_gnu_final, diag::warn_attribute_on_function_definition, diag::warn_break_binds_to_switch, diag::warn_cleanup_ext, diag::warn_gcc_attribute_location, diag::warn_gcc_ignores_type_attr, diag::warn_gcc_requires_variadic_function, diag::warn_gcc_variable_decl_in_for_loop, diag::warn_loop_ctrl_binds_to_inner, -1,
  /* DiagArray377 */ diag::warn_global_constructor, diag::warn_global_destructor, -1,
  /* DiagArray378 */ diag::warn_drv_global_isel_incomplete, diag::warn_drv_global_isel_incomplete_opt, -1,
  /* DiagArray380 */ diag::ext_alignof_expr, -1,
  /* DiagArray381 */ diag::ext_gnu_anonymous_struct, -1,
  /* DiagArray382 */ diag::ext_array_init_parens, -1,
  /* DiagArray383 */ diag::ext_auto_type, -1,
  /* DiagArray385 */ diag::ext_gnu_case_range, -1,
  /* DiagArray386 */ diag::ext_integer_complex, -1,
  /* DiagArray387 */ diag::ext_array_init_copy, -1,
  /* DiagArray388 */ diag::ext_gnu_conditional_expr, -1,
  /* DiagArray389 */ diag::ext_gnu_array_range, diag::ext_gnu_missing_equal_designator, diag::ext_gnu_old_style_field_designator, -1,
  /* DiagArray391 */ diag::ext_empty_struct_union, diag::ext_flexible_array_empty_aggregate_gnu, diag::ext_no_named_members_in_struct_union, -1,
  /* DiagArray392 */ diag::ext_flexible_array_init, -1,
  /* DiagArray393 */ diag::ext_flexible_array_union_gnu, -1,
  /* DiagArray394 */ diag::ext_expr_not_ice, diag::ext_in_class_initializer_non_constant, diag::ext_vla_folded_to_constant, -1,
  /* DiagArray395 */ diag::ext_gnu_imaginary_constant, -1,
  /* DiagArray396 */ diag::ext_pp_include_next_directive, -1,
  /* DiagArray397 */ diag::warn_gnu_inline_cplusplus_without_extern, -1,
  /* DiagArray398 */ diag::ext_gnu_address_of_label, diag::ext_gnu_indirect_goto, -1,
  /* DiagArray399 */ diag::ext_pp_gnu_line_directive, -1,
  /* DiagArray400 */ diag::warn_gnu_null_ptr_arith, -1,
  /* DiagArray402 */ diag::ext_gnu_ptr_func_arith, diag::ext_gnu_subscript_void_type, diag::ext_gnu_void_ptr, -1,
  /* DiagArray403 */ diag::ext_forward_ref_enum_def, -1,
  /* DiagArray404 */ diag::ext_gnu_statement_expr, -1,
  /* DiagArray405 */ diag::ext_gnu_statement_expr_macro, -1,
  /* DiagArray406 */ diag::ext_in_class_initializer_float_type, -1,
  /* DiagArray407 */ diag::ext_string_literal_operator_template, -1,
  /* DiagArray408 */ diag::ext_typecheck_cast_to_union, -1,
  /* DiagArray409 */ diag::ext_variable_sized_type_in_struct, -1,
  /* DiagArray410 */ diag::ext_pp_line_zero, -1,
  /* DiagArray411 */ diag::ext_paste_comma, -1,
  /* DiagArray412 */ diag::warn_maybe_capture_bad_target_this_ptr, -1,
  /* DiagArray413 */ diag::warn_header_guard, -1,
  /* DiagArray414 */ diag::warn_using_directive_in_header, -1,
  /* DiagArray415 */ diag::warn_excess_precision_not_supported, -1,
  /* DiagArray416 */ diag::warn_hip_omp_target_directives, -1,
  /* DiagArray417 */ diag::warn_ignored_hip_only_option, -1,
  /* DiagArray418 */ diag::ext_hlsl_auto_type_specifier, -1,
  /* DiagArray419 */ diag::warn_hlsl_availability, diag::warn_hlsl_availability_unavailable, -1,
  /* DiagArray420 */ diag::warn_hlsl_langstd_minimal, -1,
  /* DiagArray421 */ diag::ext_hlsl_access_specifiers, diag::ext_hlsl_lambda, -1,
  /* DiagArray422 */ diag::warn_condition_is_idiomatic_assignment, -1,
  /* DiagArray423 */ diag::ext_cannot_use_trivial_abi, diag::warn_alias_to_weak_alias, diag::warn_alias_with_section, diag::warn_aligned_attr_underaligned, diag::warn_attr_abi_tag_namespace, diag::warn_attribute_after_definition_ignored, diag::warn_attribute_cmse_entry_static, diag::warn_attribute_dllexport_explicit_instantiation_def, diag::warn_attribute_exclude_from_explicit_instantiation_local_class, diag::warn_attribute_has_no_effect_on_compile_time_if, diag::warn_attribute_has_no_effect_on_infinite_loop, diag::warn_attribute_hybrid_patchable_non_extern, diag::warn_attribute_iboutlet, diag::warn_attribute_ignored, diag::warn_attribute_ignored_for_field_of_type, diag::warn_attribute_ignored_no_calls_in_stmt, diag::warn_attribute_ignored_non_function_pointer, diag::warn_attribute_ignored_on_inline, diag::warn_attribute_ignored_on_non_definition, diag::warn_attribute_invalid_on_definition, diag::warn_attribute_needs_aggregate, diag::warn_attribute_no_decl, diag::warn_attribute_nonnull_no_pointers, diag::warn_attribute_nonnull_parm_no_args, diag::warn_attribute_not_on_decl, diag::warn_attribute_pointer_or_reference_only, diag::warn_attribute_pointers_only, diag::warn_attribute_precede_definition, diag::warn_attribute_return_pointers_only, diag::warn_attribute_return_pointers_refs_only, diag::warn_attribute_sentinel_named_arguments, diag::warn_attribute_sentinel_not_variadic, diag::warn_attribute_type_not_supported, diag::warn_attribute_type_not_supported_global, diag::warn_attribute_unknown_visibility, diag::warn_attribute_void_function_method, diag::warn_attribute_weak_on_field, diag::warn_attribute_weak_on_local, diag::warn_attribute_wrong_decl_type, diag::warn_attribute_wrong_decl_type_str, diag::warn_attributes_likelihood_ifstmt_conflict, diag::warn_block_literal_attributes_on_omitted_return_type, diag::warn_cconv_unsupported, diag::warn_const_attr_with_pure_attr, diag::warn_cuda_maxclusterrank_sm_90, diag::warn_cxx11_gnu_attribute_on_type, diag::warn_declspec_allocator_nonpointer, diag::warn_declspec_attribute_ignored, diag::warn_deprecated_anonymous_namespace, diag::warn_deprecated_ignored_on_using, diag::warn_dllimport_dropped_from_inline_function, diag::warn_duplicate_attribute, diag::warn_duplicate_attribute_exact, diag::warn_function_attribute_ignored_in_stmt, diag::warn_function_stmt_attribute_precedence, diag::warn_gc_attribute_weak_on_local, diag::warn_gnu_inline_attribute_requires_inline, diag::warn_ignored_ms_inheritance, diag::warn_ignored_objc_externally_retained, diag::warn_import_on_definition, diag::warn_internal_linkage_local_storage, diag::warn_interrupt_attribute_invalid, diag::warn_microsoft_qualifiers_ignored, diag::warn_mig_server_routine_does_not_return_kern_return_t, diag::warn_mismatched_import, diag::warn_mmap_unknown_attribute, diag::warn_nocf_check_attribute_ignored, diag::warn_noderef_on_non_pointer_or_array, diag::warn_nothrow_attribute_ignored, diag::warn_ns_attribute_wrong_parameter_type, diag::warn_ns_attribute_wrong_return_type, diag::warn_objc_direct_ignored, diag::warn_objc_direct_property_ignored, diag::warn_opencl_attr_deprecated_ignored, diag::warn_pure_function_returns_void, diag::warn_require_const_init_added_too_late, diag::warn_riscv_repeated_interrupt_attribute, diag::warn_sycl_kernel_invalid_template_param_type, diag::warn_sycl_kernel_num_of_function_params, diag::warn_sycl_kernel_num_of_template_params, diag::warn_sycl_kernel_return_type, diag::warn_transparent_union_attribute_field_size_align, diag::warn_transparent_union_attribute_floating, diag::warn_transparent_union_attribute_not_definition, diag::warn_transparent_union_attribute_zero_fields, diag::warn_type_attribute_wrong_type, diag::warn_unhandled_ms_attribute_ignored, diag::warn_unsupported_target_attribute, diag::warn_unused_result_typedef_unsupported_spelling, diag::warn_wrong_clang_attr_namespace, -1,
  /* DiagArray424 */ diag::warn_missing_sdksettings_for_availability_checking, -1,
  /* DiagArray425 */ diag::warn_drv_pch_ignoring_gch_dir, diag::warn_drv_pch_ignoring_gch_file, -1,
  /* DiagArray426 */ diag::warn_drv_unsupported_opt_for_target, diag::warn_ignored_gcc_optimization, -1,
  /* DiagArray427 */ diag::warn_pragma_intrinsic_builtin, -1,
  /* DiagArray429 */ diag::warn_clause_expected_string, diag::warn_pragma_align_expected_equal, diag::warn_pragma_align_invalid_option, diag::warn_pragma_comment_ignored, diag::warn_pragma_debug_dependent_argument, diag::warn_pragma_debug_missing_argument, diag::warn_pragma_debug_missing_command, diag::warn_pragma_debug_unexpected_argument, diag::warn_pragma_debug_unexpected_command, diag::warn_pragma_debug_unknown_module, diag::warn_pragma_expected_action_or_r_paren, diag::warn_pragma_expected_colon, diag::warn_pragma_expected_colon_r_paren, diag::warn_pragma_expected_comma, diag::warn_pragma_expected_identifier, diag::warn_pragma_expected_init_seg, diag::warn_pragma_expected_integer, diag::warn_pragma_expected_lparen, diag::warn_pragma_expected_non_wide_string, diag::warn_pragma_expected_predicate, diag::warn_pragma_expected_punc, diag::warn_pragma_expected_rparen, diag::warn_pragma_expected_section_label_or_name, diag::warn_pragma_expected_section_name, diag::warn_pragma_expected_section_push_pop_or_name, diag::warn_pragma_expected_string, diag::warn_pragma_extra_tokens_at_eol, diag::warn_pragma_force_cuda_host_device_bad_arg, diag::warn_pragma_fp_ignored, diag::warn_pragma_init_seg_unsupported_target, diag::warn_pragma_invalid_action, diag::warn_pragma_invalid_argument, diag::warn_pragma_invalid_specific_action, diag::warn_pragma_missing_argument, diag::warn_pragma_ms_fenv_access, diag::warn_pragma_ms_struct, diag::warn_pragma_options_align_reset_failed, diag::warn_pragma_options_expected_align, diag::warn_pragma_pack_invalid_alignment, diag::warn_pragma_pack_malformed, diag::warn_pragma_pop_failed, diag::warn_pragma_pop_macro_no_push, diag::warn_pragma_unknown_extension, diag::warn_pragma_unsupported_action, diag::warn_pragma_unsupported_extension, diag::warn_pragma_unused_expected_var, diag::warn_pragma_unused_expected_var_arg, diag::warn_pragma_unused_undeclared_var, diag::warn_stdc_unknown_rounding_mode, -1,
  /* DiagArray430 */ diag::ext_typecheck_function_qualifiers_unspecified, diag::warn_arc_lifetime_result_type, diag::warn_block_literal_qualifiers_on_omitted_return_type, diag::warn_qual_base_type, diag::warn_qual_return_type, diag::warn_typecheck_function_qualifiers_ignored, -1,
  /* DiagArray431 */ diag::warn_typecheck_reference_qualifiers, -1,
  /* DiagArray433 */ diag::warn_auto_implicit_atomic_property, diag::warn_implicit_atomic_property, -1,
  /* DiagArray434 */ diag::warn_impcast_integer_float_precision_constant, -1,
  /* DiagArray435 */ diag::warn_impcast_floating_point_to_bool, -1,
  /* DiagArray436 */ diag::ext_implicit_exception_spec_mismatch, -1,
  /* DiagArray437 */ diag::warn_unannotated_fallthrough, -1,
  /* DiagArray438 */ diag::warn_unannotated_fallthrough_per_function, -1,
  /* DiagArray439 */ diag::warn_impcast_fixed_point_range, -1,
  /* DiagArray440 */ diag::warn_impcast_float_precision, diag::warn_impcast_float_result_precision, -1,
  /* DiagArray441 */ diag::ext_implicit_function_decl_c99, diag::ext_implicit_lib_function_decl, diag::ext_implicit_lib_function_decl_c99, diag::warn_builtin_unknown, diag::warn_implicit_function_decl, -1,
  /* DiagArray442 */ diag::ext_missing_type_specifier, diag::ext_param_not_declared, diag::warn_missing_type_specifier, -1,
  /* DiagArray443 */ diag::warn_impcast_high_order_zero_bits, diag::warn_impcast_integer_precision, -1,
  /* DiagArray444 */ diag::warn_impcast_integer_float_precision, -1,
  /* DiagArray445 */ diag::warn_implicitly_retains_self, -1,
  /* DiagArray446 */ diag::ext_integer_literal_too_large_for_signed, -1,
  /* DiagArray448 */ diag::warn_import_implementation_partition_unit_in_interface_unit, -1,
  /* DiagArray449 */ diag::ext_pp_import_directive, -1,
  /* DiagArray450 */ diag::warn_inaccessible_base_class, -1,
  /* DiagArray451 */ diag::warn_pp_include_angled_in_module_purview, -1,
  /* DiagArray452 */ diag::pp_include_next_absolute_path, -1,
  /* DiagArray453 */ diag::pp_include_next_in_primary, -1,
  /* DiagArray454 */ diag::warn_deep_exception_specs_differ, diag::warn_incompatible_exception_specs, -1,
  /* DiagArray455 */ diag::ext_typecheck_convert_incompatible_function_pointer, -1,
  /* DiagArray456 */ diag::warn_typecheck_convert_incompatible_function_pointer_strict, -1,
  /* DiagArray457 */ diag::warn_redecl_library_builtin, -1,
  /* DiagArray458 */ diag::warn_section_msvc_compat, -1,
  /* DiagArray459 */ diag::warn_cxx_ms_struct, diag::warn_npot_ms_struct, -1,
  /* DiagArray460 */ diag::ext_typecheck_convert_incompatible_pointer, -1,
  /* DiagArray461 */ diag::ext_nested_pointer_qualifier_mismatch, diag::ext_typecheck_convert_discards_qualifiers, diag::warn_bad_cxx_cast_nested_pointer_addr_space, -1,
  /* DiagArray462 */ diag::warn_property_types_are_incompatible, -1,
  /* DiagArray463 */ diag::warn_incompatible_sysroot, -1,
  /* DiagArray464 */ diag::warn_mmap_incomplete_framework_module_declaration, -1,
  /* DiagArray465 */ diag::warn_undef_method_impl, -1,
  /* DiagArray467 */ diag::warn_implicit_decl_no_jmp_buf, -1,
  /* DiagArray468 */ diag::warn_missing_submodule, diag::warn_mmap_umbrella_dir_not_found, diag::warn_uncovered_module_header, -1,
  /* DiagArray469 */ diag::warn_redeclaration_without_attribute_prev_attribute_ignored, diag::warn_redeclaration_without_import_attribute, -1,
  /* DiagArray470 */ diag::warn_inconsistent_destructor_marked_not_override_overriding, -1,
  /* DiagArray471 */ diag::warn_inconsistent_function_marked_not_override_overriding, -1,
  /* DiagArray472 */ diag::ext_increment_bool, -1,
  /* DiagArray473 */ diag::warn_infinite_recursive_function, -1,
  /* DiagArray475 */ diag::ext_initializer_overrides, diag::ext_initializer_union_overrides, diag::warn_initializer_overrides, -1,
  /* DiagArray476 */ diag::ext_out_of_line_qualified_id_type_names_constructor, -1,
  /* DiagArray478 */ diag::warn_fe_inline_asm, -1,
  /* DiagArray479 */ diag::warn_inline_namespace_reopened_noninline, -1,
  /* DiagArray480 */ diag::ext_operator_new_delete_declared_inline, -1,
  /* DiagArray481 */ diag::warn_dylib_symbol_flags_mismatch, diag::warn_glob_did_not_match, diag::warn_header_availability_mismatch, diag::warn_header_hidden_symbol, diag::warn_header_symbol_flags_mismatch, diag::warn_header_symbol_missing, diag::warn_library_hidden_symbol, diag::warn_library_missing_symbol, diag::warn_no_such_excluded_header_file, diag::warn_platform_mismatch, diag::warn_rpaths_mismatch, diag::warn_rpaths_missing, diag::warn_target, -1,
  /* DiagArray482 */ diag::warn_explicit_instantiation_after_specialization, -1,
  /* DiagArray483 */ diag::ext_typecheck_convert_int_pointer, diag::ext_typecheck_convert_pointer_int, -1,
  /* DiagArray485 */ diag::warn_enum_constant_in_bool_context, diag::warn_left_shift_in_bool_context, -1,
  /* DiagArray486 */ diag::warn_int_to_pointer_cast, -1,
  /* DiagArray487 */ diag::warn_int_to_void_pointer_cast, -1,
  /* DiagArray488 */ diag::warn_integer_constant_overflow, -1,
  /* DiagArray489 */ diag::warn_drv_missing_plugin_arg, diag::warn_drv_missing_plugin_name, diag::warn_drv_msp430_hwmult_mismatch, diag::warn_drv_msp430_hwmult_no_device, diag::warn_drv_msp430_hwmult_unsupported, diag::warn_drv_object_size_disabled_O0, diag::warn_drv_optimization_value, diag::warn_fe_backend_invalid_feature_flag, diag::warn_fe_backend_readonly_feature_flag, diag::warn_target_unrecognized_env, diag::warn_target_unsupported_extension, -1,
  /* DiagArray490 */ diag::ext_constexpr_function_never_constant_expr, -1,
  /* DiagArray491 */ diag::warn_invalid_feature_combination, -1,
  /* DiagArray492 */ diag::warn_invalid_asm_cast_lvalue, -1,
  /* DiagArray493 */ diag::warn_iboutlet_object_type, diag::warn_iboutletcollection_property_assign, -1,
  /* DiagArray494 */ diag::warn_invalid_initializer_from_system_header, -1,
  /* DiagArray495 */ diag::warn_invalid_ios_deployment_target, -1,
  /* DiagArray496 */ diag::warn_attribute_no_builtin_invalid_builtin_name, -1,
  /* DiagArray497 */ diag::warn_falloff_noreturn_function, diag::warn_noreturn_function_has_return_expr, -1,
  /* DiagArray498 */ diag::ext_offsetof_non_pod_type, diag::ext_offsetof_non_standardlayout_type, -1,
  /* DiagArray499 */ diag::warn_drv_unable_to_find_directory_expected, -1,
  /* DiagArray500 */ diag::ext_partial_spec_not_more_specialized_than_primary, -1,
  /* DiagArray502 */ diag::ext_empty_character, diag::ext_unterminated_char_or_string, -1,
  /* DiagArray503 */ diag::warn_bad_character_encoding, diag::warn_bad_string_encoding, -1,
  /* DiagArray504 */ diag::warn_invalid_specialization, -1,
  /* DiagArray505 */ diag::warn_static_assert_message_constexpr, -1,
  /* DiagArray506 */ diag::ext_pp_bad_paste_ms, -1,
  /* DiagArray507 */ diag::warn_unevaluated_string_prefix, -1,
  /* DiagArray508 */ diag::warn_invalid_utf8_in_comment, -1,
  /* DiagArray509 */ diag::warn_jump_out_of_seh_finally, -1,
  /* DiagArray510 */ diag::ext_keyword_as_ident, -1,
  /* DiagArray511 */ diag::warn_pp_macro_hides_keyword, -1,
  /* DiagArray512 */ diag::ext_param_promoted_not_compatible_with_prototype, -1,
  /* DiagArray513 */ diag::ext_token_used, -1,
  /* DiagArray514 */ diag::warn_parameter_size, diag::warn_return_value_size, -1,
  /* DiagArray515 */ diag::warn_hlsl_deprecated_register_type_b, diag::warn_hlsl_deprecated_register_type_i, diag::warn_hlsl_register_type_c_packoffset, diag::warn_hlsl_user_defined_type_missing_member, -1,
  /* DiagArray517 */ diag::warn_fe_linking_module, -1,
  /* DiagArray518 */ diag::warn_impcast_literal_float_to_integer, diag::warn_impcast_literal_float_to_integer_out_of_range, -1,
  /* DiagArray519 */ diag::warn_float_compare_literal, diag::warn_float_overflow, diag::warn_float_underflow, -1,
  /* DiagArray520 */ diag::ext_template_arg_local_type, -1,
  /* DiagArray521 */ diag::warn_logical_not_on_lhs_of_check, -1,
  /* DiagArray522 */ diag::warn_logical_and_in_logical_or, -1,
  /* DiagArray523 */ diag::ext_c99_longlong, -1,
  /* DiagArray525 */ diag::ext_pp_macro_redef, -1,
  /* DiagArray526 */ diag::ext_main_invalid_linkage_specification, diag::ext_main_used, diag::ext_noreturn_main, diag::ext_variadic_main, diag::warn_main_one_arg, diag::warn_main_redefined, diag::warn_main_returns_bool_literal, diag::warn_static_main, -1,
  /* DiagArray527 */ diag::ext_main_returns_nonint, -1,
  /* DiagArray528 */ diag::warn_has_warning_invalid_option, -1,
  /* DiagArray529 */ diag::ext_many_braces_around_init, -1,
  /* DiagArray530 */ diag::warn_drv_math_errno_enabled_after_veclib, -1,
  /* DiagArray531 */ diag::ext_mathematical_notation, -1,
  /* DiagArray532 */ diag::warn_max_tokens, diag::warn_max_tokens_total, -1,
  /* DiagArray533 */ diag::warn_max_unsigned_zero, -1,
  /* DiagArray534 */ diag::warn_suspicious_sizeof_memset, -1,
  /* DiagArray535 */ diag::warn_memsize_comparison, -1,
  /* DiagArray536 */ diag::warn_non_contravariant_param_types, diag::warn_non_covariant_ret_types, -1,
  /* DiagArray538 */ diag::ext_ms_abstract_keyword, -1,
  /* DiagArray539 */ diag::ext_anonymous_record_with_type, diag::ext_ms_anonymous_record, -1,
  /* DiagArray540 */ diag::ext_ms_cast_fn_obj, diag::ext_ms_impcast_fn_obj, -1,
  /* DiagArray541 */ diag::ext_charize_microsoft, -1,
  /* DiagArray542 */ diag::ext_comment_paste_microsoft, -1,
  /* DiagArray543 */ diag::ext_default_init_const, -1,
  /* DiagArray544 */ diag::ext_pp_operator_used_as_macro_name, -1,
  /* DiagArray545 */ diag::ext_param_default_argument_redefinition, -1,
  /* DiagArray546 */ diag::warn_attribute_section_drectve, -1,
  /* DiagArray547 */ diag::ext_ctrl_z_eof_microsoft, -1,
  /* DiagArray548 */ diag::ext_ms_forward_ref_enum, -1,
  /* DiagArray549 */ diag::ext_enumerator_too_large, -1,
  /* DiagArray550 */ diag::ext_ellipsis_exception_spec, diag::ext_incomplete_in_exception_spec, diag::ext_mismatched_exception_spec, diag::ext_mismatched_exception_spec_explicit_instantiation, diag::ext_override_exception_spec, -1,
  /* DiagArray551 */ diag::warn_microsoft_dependent_exists, -1,
  /* DiagArray552 */ diag::ext_ms_explicit_constructor_call, -1,
  /* DiagArray553 */ diag::warn_member_extra_qualification, -1,
  /* DiagArray554 */ diag::ext_ms_c_enum_fixed_underlying_type, -1,
  /* DiagArray555 */ diag::ext_flexible_array_empty_aggregate_ms, diag::ext_flexible_array_union_ms, -1,
  /* DiagArray556 */ diag::ext_goto_into_protected_scope, -1,
  /* DiagArray557 */ diag::ext_ms_ambiguous_direct_base, -1,
  /* DiagArray558 */ diag::ext_pp_include_search_ms, -1,
  /* DiagArray559 */ diag::ext_init_from_predefined, -1,
  /* DiagArray560 */ diag::warn_ms_inline_non_function, -1,
  /* DiagArray561 */ diag::ext_mutable_reference, -1,
  /* DiagArray562 */ diag::ext_pure_function_definition, -1,
  /* DiagArray563 */ diag::ext_static_non_static, -1,
  /* DiagArray564 */ diag::ext_ms_sealed_keyword, -1,
  /* DiagArray565 */ diag::ext_ms_static_assert, -1,
  /* DiagArray566 */ diag::ext_string_literal_from_predefined, -1,
  /* DiagArray567 */ diag::ext_explicit_instantiation_duplicate, diag::ext_found_in_dependent_base, diag::ext_found_later_in_class, diag::ext_ms_delayed_template_argument, diag::ext_ms_deref_template_argument, diag::ext_ms_template_spec_redecl_out_of_scope, diag::ext_ms_template_type_arg_missing_typename, diag::ext_static_out_of_line, diag::ext_undeclared_unqual_id_with_dependent_base, diag::ext_unqualified_base_class, -1,
  /* DiagArray568 */ diag::ext_template_param_shadow, -1,
  /* DiagArray569 */ diag::ext_union_member_of_reference_type, -1,
  /* DiagArray570 */ diag::ext_friend_tag_redecl_outside_namespace, -1,
  /* DiagArray571 */ diag::ext_ms_using_declaration_inaccessible, -1,
  /* DiagArray572 */ diag::ext_pseudo_dtor_on_void, -1,
  /* DiagArray573 */ diag::warn_profile_data_misexpect, -1,
  /* DiagArray574 */ diag::warn_misleading_indentation, -1,
  /* DiagArray575 */ diag::warn_mismatched_delete_new, -1,
  /* DiagArray576 */ diag::warn_conflicting_param_types, -1,
  /* DiagArray577 */ diag::warn_conflicting_ret_types, -1,
  /* DiagArray578 */ diag::warn_struct_class_previous_tag_mismatch, diag::warn_struct_class_tag_mismatch, -1,
  /* DiagArray579 */ diag::warn_missing_braces, -1,
  /* DiagArray580 */ diag::ext_constinit_missing, -1,
  /* DiagArray581 */ diag::ext_no_declarators, diag::ext_standalone_specifier, diag::ext_typedef_without_a_name, diag::warn_standalone_specifier, -1,
  /* DiagArray582 */ diag::warn_missing_designated_field_initializers, -1,
  /* DiagArray583 */ diag::ext_missing_exception_specification, -1,
  /* DiagArray584 */ diag::warn_missing_field_initializers, -1,
  /* DiagArray586 */ diag::warn_missing_include_dirs, -1,
  /* DiagArray587 */ diag::warn_missing_method_return_type, -1,
  /* DiagArray588 */ diag::warn_drv_missing_multilib, -1,
  /* DiagArray589 */ diag::warn_overriding_method_missing_noescape, -1,
  /* DiagArray590 */ diag::warn_suggest_noreturn_block, diag::warn_suggest_noreturn_function, -1,
  /* DiagArray591 */ diag::warn_cconv_knr, -1,
  /* DiagArray592 */ diag::warn_missing_prototype, -1,
  /* DiagArray593 */ diag::warn_missing_selector_name, -1,
  /* DiagArray594 */ diag::warn_missing_sysroot, -1,
  /* DiagArray595 */ diag::missing_template_arg_list_after_template_kw, -1,
  /* DiagArray596 */ diag::warn_missing_variable_declarations, -1,
  /* DiagArray597 */ diag::warn_omp_assume_attribute_string_unknown_suggested, -1,
  /* DiagArray598 */ diag::warn_hlsl_packoffset_mix, -1,
  /* DiagArray599 */ diag::remark_module_build, diag::remark_module_build_done, diag::remark_module_lock_failure, diag::remark_module_lock_timeout, -1,
  /* DiagArray600 */ diag::warn_module_conflict, diag::warn_module_system_bit_conflict, -1,
  /* DiagArray601 */ diag::warn_module_config_mismatch, -1,
  /* DiagArray602 */ diag::warn_duplicate_module_file_extension, -1,
  /* DiagArray603 */ diag::remark_module_import, -1,
  /* DiagArray604 */ diag::ext_module_import_in_extern_c, -1,
  /* DiagArray605 */ diag::remark_pp_include_directive_modular_translation, -1,
  /* DiagArray606 */ diag::remark_module_lock, -1,
  /* DiagArray607 */ diag::ext_equivalent_internal_linkage_decl_in_modules, -1,
  /* DiagArray608 */ diag::ext_module_import_not_at_top_level_noop, -1,
  /* DiagArray612 */ diag::warn_drv_msvc_not_found, -1,
  /* DiagArray613 */ diag::warn_drv_multi_gpu_arch, -1,
  /* DiagArray614 */ diag::warn_multichar_character_literal, -1,
  /* DiagArray615 */ diag::warn_vbase_moved_multiple_times, -1,
  /* DiagArray616 */ diag::warn_fp_nan_inf_when_disabled, -1,
  /* DiagArray618 */ diag::ext_anonymous_record_with_anonymous_type, -1,
  /* DiagArray620 */ diag::warn_operator_new_returns_null, -1,
  /* DiagArray621 */ diag::ext_no_newline_eof, diag::warn_no_newline_eof, -1,
  /* DiagArray622 */ diag::warn_dereference_of_noderef_type, diag::warn_dereference_of_noderef_type_no_decl, diag::warn_noderef_to_dereferenceable_pointer, -1,
  /* DiagArray624 */ diag::ext_non_c_like_anon_struct_in_typedef, -1,
  /* DiagArray626 */ diag::warn_non_literal_null_pointer, -1,
  /* DiagArray627 */ diag::warn_non_modular_include_in_framework_module, -1,
  /* DiagArray628 */ diag::warn_non_modular_include_in_module, -1,
  /* DiagArray629 */ diag::warn_cannot_pass_non_pod_arg_to_vararg, diag::warn_non_pod_vararg_with_format_string, diag::warn_second_parameter_to_va_arg_not_pod, diag::warn_second_parameter_to_va_arg_ownership_qualified, -1,
  /* DiagArray630 */ diag::warn_alignment_not_power_of_two, -1,
  /* DiagArray631 */ diag::warn_non_virtual_dtor, -1,
  /* DiagArray632 */ diag::warn_null_arg, diag::warn_null_ret, -1,
  /* DiagArray634 */ diag::pp_nonportable_path, -1,
  /* DiagArray635 */ diag::warn_apinotes_private_case, -1,
  /* DiagArray636 */ diag::warn_apinotes_private_case_system, -1,
  /* DiagArray637 */ diag::warn_sycl_kernel_name_not_a_class_type, -1,
  /* DiagArray638 */ diag::pp_nonportable_system_path, -1,
  /* DiagArray639 */ diag::warn_neon_vector_initializer_non_portable, -1,
  /* DiagArray640 */ diag::warn_cstruct_memaccess, -1,
  /* DiagArray641 */ diag::warn_cxxstruct_memaccess, -1,
  /* DiagArray642 */ diag::warn_nsconsumed_attribute_mismatch, -1,
  /* DiagArray643 */ diag::warn_nsreturns_retained_attribute_mismatch, -1,
  /* DiagArray644 */ diag::warn_null_in_arithmetic_operation, diag::warn_null_in_comparison_operation, -1,
  /* DiagArray645 */ diag::null_in_char_or_string, diag::null_in_file, -1,
  /* DiagArray646 */ diag::warn_impcast_null_pointer_to_integer, -1,
  /* DiagArray647 */ diag::warn_binding_null_to_reference, diag::warn_indirection_through_null, -1,
  /* DiagArray648 */ diag::warn_pointer_arith_null_ptr, -1,
  /* DiagArray649 */ diag::warn_pointer_sub_null_ptr, -1,
  /* DiagArray650 */ diag::warn_conflicting_nullability_attr_overriding_param_types, diag::warn_conflicting_nullability_attr_overriding_ret_types, diag::warn_mismatched_nullability_attr, diag::warn_null_resettable_setter, diag::warn_nullability_duplicate, -1,
  /* DiagArray651 */ diag::warn_nullability_missing, -1,
  /* DiagArray652 */ diag::warn_nullability_missing_array, -1,
  /* DiagArray653 */ diag::warn_nullability_declspec, -1,
  /* DiagArray654 */ diag::ext_nullability, -1,
  /* DiagArray655 */ diag::warn_nullability_inferred_on_nested_type, -1,
  /* DiagArray656 */ diag::warn_nullability_lost, -1,
  /* DiagArray657 */ diag::warn_offload_incompatible_redeclare, -1,
  /* DiagArray658 */ diag::warn_autosynthesis_property_ivar_match, -1,
  /* DiagArray659 */ diag::warn_impcast_constant_value_to_objc_bool, -1,
  /* DiagArray660 */ diag::warn_objc_boxing_invalid_utf8_string, -1,
  /* DiagArray661 */ diag::warn_objc_circular_container, -1,
  /* DiagArray663 */ diag::warn_objc_designated_init_missing_super_call, diag::warn_objc_designated_init_non_designated_init_call, diag::warn_objc_designated_init_non_super_designated_init_call, diag::warn_objc_implementation_missing_designated_init_override, diag::warn_objc_secondary_init_missing_init_call, diag::warn_objc_secondary_init_super_init_call, -1,
  /* DiagArray664 */ diag::warn_nsdictionary_duplicate_key, -1,
  /* DiagArray665 */ diag::warn_dup_category_def, -1,
  /* DiagArray666 */ diag::warn_superclass_variable_sized_type_not_at_end, diag::warn_variable_sized_ivar_visibility, -1,
  /* DiagArray667 */ diag::warn_forward_class_redefinition, -1,
  /* DiagArray668 */ diag::warn_ivars_in_interface, -1,
  /* DiagArray669 */ diag::warn_objc_literal_comparison, -1,
  /* DiagArray670 */ diag::warn_impcast_objective_c_literal_to_bool, diag::warn_objc_collection_literal_element, -1,
  /* DiagArray671 */ diag::warn_pp_objc_macro_redef_ignored, -1,
  /* DiagArray672 */ diag::warn_messaging_unqualified_id, -1,
  /* DiagArray673 */ diag::warn_class_method_not_found, diag::warn_class_method_not_found_with_typo, diag::warn_inst_method_not_found, diag::warn_instance_method_not_found_with_typo, diag::warn_instance_method_on_class_found, diag::warn_root_inst_method_not_found, -1,
  /* DiagArray674 */ diag::warn_missing_explicit_synthesis, -1,
  /* DiagArray675 */ diag::warn_objc_missing_super_call, -1,
  /* DiagArray676 */ diag::warn_multiple_method_decl, -1,
  /* DiagArray677 */ diag::warn_objc_property_retain_of_block, -1,
  /* DiagArray678 */ diag::warn_objc_pointer_cxx_catch_fragile, -1,
  /* DiagArray679 */ diag::warn_objc_property_assign_on_object, -1,
  /* DiagArray680 */ diag::warn_impl_required_for_class_property, diag::warn_impl_required_in_category_for_class_property, diag::warn_setter_getter_impl_required, diag::warn_setter_getter_impl_required_in_category, -1,
  /* DiagArray681 */ diag::warn_property_implicitly_mismatched, -1,
  /* DiagArray682 */ diag::warn_cocoa_naming_owned_rule, -1,
  /* DiagArray683 */ diag::warn_objc_property_default_assign_on_object, diag::warn_objc_property_no_assignment_attribute, -1,
  /* DiagArray684 */ diag::warn_autosynthesis_property_in_superclass, diag::warn_no_autosynthesis_property, diag::warn_no_autosynthesis_shared_ivar_property, -1,
  /* DiagArray685 */ diag::warn_category_method_impl_match, -1,
  /* DiagArray686 */ diag::warn_auto_synthesizing_protocol_property, -1,
  /* DiagArray687 */ diag::warn_objc_redundant_qualified_class_type, -1,
  /* DiagArray688 */ diag::warn_objc_readonly_property_has_setter, -1,
  /* DiagArray690 */ diag::warn_objc_redundant_literal_use, -1,
  /* DiagArray691 */ diag::warn_objc_root_class_missing, -1,
  /* DiagArray693 */ diag::warn_impcast_float_to_objc_signed_char_bool, -1,
  /* DiagArray694 */ diag::warn_impcast_int_to_objc_signed_char_bool, -1,
  /* DiagArray695 */ diag::warn_objc_string_literal_comparison, -1,
  /* DiagArray696 */ diag::warn_concatenated_nsarray_literal, -1,
  /* DiagArray697 */ diag::warn_objc_unsafe_perform_selector, -1,
  /* DiagArray698 */ diag::warn_odr_different_num_template_parameters, diag::warn_odr_different_template_parameter_kind, diag::warn_odr_field_type_inconsistent, diag::warn_odr_function_type_inconsistent, diag::warn_odr_ivar_type_inconsistent, diag::warn_odr_non_type_parameter_type_inconsistent, diag::warn_odr_objc_method_num_params_inconsistent, diag::warn_odr_objc_method_param_type_inconsistent, diag::warn_odr_objc_method_result_type_inconsistent, diag::warn_odr_objc_method_variadic_inconsistent, diag::warn_odr_objc_property_impl_kind_inconsistent, diag::warn_odr_objc_property_type_inconsistent, diag::warn_odr_objc_superclass_inconsistent, diag::warn_odr_objc_synthesize_ivar_inconsistent, diag::warn_odr_parameter_pack_non_pack, diag::warn_odr_tag_type_inconsistent, diag::warn_odr_variable_multiple_def, diag::warn_odr_variable_type_inconsistent, -1,
  /* DiagArray699 */ diag::warn_old_style_cast, -1,
  /* DiagArray702 */ diag::warn_acc_deprecated_alias_name, -1,
  /* DiagArray703 */ diag::warn_acc_if_self_conflict, -1,
  /* DiagArray704 */ diag::ext_opencl_ext_vector_type_rgba_selector, -1,
  /* DiagArray706 */ diag::ext_omp_attributes, -1,
  /* DiagArray707 */ diag::warn_omp_alignment_not_power_of_two, diag::warn_omp_allocate_thread_on_task_target_directive, diag::warn_omp_ctx_incompatible_property_for_selector, diag::warn_omp_ctx_incompatible_score_for_property, diag::warn_omp_ctx_incompatible_selector_for_set, diag::warn_omp_ctx_selector_without_properties, diag::warn_omp_declare_variant_ctx_mutiple_use, diag::warn_omp_declare_variant_ctx_not_a_property, diag::warn_omp_declare_variant_ctx_not_a_selector, diag::warn_omp_declare_variant_ctx_not_a_set, diag::warn_omp_declare_variant_expected, diag::warn_omp_declare_variant_string_literal_or_identifier, diag::warn_omp_linear_step_zero, diag::warn_omp_more_one_device_type_clause, diag::warn_omp_more_one_interop_type, diag::warn_omp_more_one_omp_all_memory, diag::warn_omp_unknown_assumption_clause_missing_id, diag::warn_omp_unknown_assumption_clause_without_args, diag::warn_omp_used_different_allocator, -1,
  /* DiagArray708 */ diag::warn_omp_invalid_attribute_for_ompx_attributes, -1,
  /* DiagArray709 */ diag::ext_omp_loop_not_canonical_init, diag::warn_omp_loop_64_bit_var, -1,
  /* DiagArray710 */ diag::warn_omp_non_trivial_type_mapped, -1,
  /* DiagArray711 */ diag::warn_drv_omp_offload_target_duplicate, diag::warn_omp_declare_target_after_first_use, diag::warn_omp_not_in_target_context, -1,
  /* DiagArray712 */ diag::warn_throw_not_valid_on_target, diag::warn_try_not_valid_on_target, -1,
  /* DiagArray713 */ diag::warn_drv_fine_grained_bitfield_accesses_ignored, diag::warn_drv_fjmc_for_elf_only, diag::warn_drv_jmc_requires_debuginfo, diag::warn_drv_loongarch_conflicting_implied_val, diag::warn_drv_moutline_atomics_unsupported_opt, diag::warn_drv_moutline_unsupported_opt, diag::warn_drv_needs_hvx, diag::warn_drv_ps_force_pic, diag::warn_drv_unsupported_diag_option_for_flang, diag::warn_drv_unsupported_longcalls, diag::warn_drv_unsupported_openmp_library, diag::warn_drv_unsupported_option_for_flang, diag::warn_drv_unsupported_option_for_offload_arch_req_feature, diag::warn_drv_unsupported_option_for_processor, diag::warn_drv_unsupported_option_for_target, diag::warn_drv_unsupported_pic_with_mabicalls, diag::warn_target_override_arm64ec, -1,
  /* DiagArray714 */ diag::ext_typecheck_ordered_comparison_of_function_pointers, diag::warn_typecheck_ordered_comparison_of_function_pointers, -1,
  /* DiagArray715 */ diag::ext_out_of_line_declaration, -1,
  /* DiagArray716 */ diag::ext_use_out_of_scope_declaration, -1,
  /* DiagArray717 */ diag::warn_overaligned_type, -1,
  /* DiagArray719 */ diag::ext_string_too_long, -1,
  /* DiagArray720 */ diag::warn_overloaded_shift_in_comparison, -1,
  /* DiagArray721 */ diag::warn_overloaded_virtual, -1,
  /* DiagArray723 */ diag::warn_fe_override_module, -1,
  /* DiagArray724 */ diag::warn_conflicting_overriding_param_modifiers, diag::warn_conflicting_overriding_param_types, diag::warn_conflicting_overriding_ret_type_modifiers, diag::warn_conflicting_overriding_ret_types, diag::warn_conflicting_overriding_variadic, diag::warn_non_contravariant_overriding_param_types, diag::warn_non_covariant_overriding_ret_types, -1,
  /* DiagArray725 */ diag::warn_drv_overriding_option, -1,
  /* DiagArray726 */ diag::warn_unnecessary_packed, -1,
  /* DiagArray727 */ diag::warn_unpacked_field, -1,
  /* DiagArray728 */ diag::warn_padded_struct_anon_field, diag::warn_padded_struct_field, diag::warn_padded_struct_size, -1,
  /* DiagArray729 */ diag::warn_padded_struct_anon_bitfield, diag::warn_padded_struct_bitfield, -1,
  /* DiagArray730 */ diag::warn_condition_is_assignment, diag::warn_consecutive_comparison, diag::warn_precedence_bitwise_rel, diag::warn_precedence_conditional, -1,
  /* DiagArray731 */ diag::warn_equality_with_extra_parens, -1,
  /* DiagArray733 */ diag::remark_fe_backend_optimization_remark, -1,
  /* DiagArray734 */ diag::remark_fe_backend_optimization_remark_analysis, diag::remark_fe_backend_optimization_remark_analysis_aliasing, diag::remark_fe_backend_optimization_remark_analysis_fpcommute, -1,
  /* DiagArray735 */ diag::warn_fe_backend_optimization_failure, -1,
  /* DiagArray736 */ diag::remark_fe_backend_optimization_remark_missed, -1,
  /* DiagArray737 */ diag::warn_module_uses_date_time, -1,
  /* DiagArray738 */ diag::warn_pch_vfsoverlay_mismatch, -1,
  /* DiagArray739 */ diag::ext_aggregate_init_not_constant, diag::ext_anonymous_record_with_type, diag::ext_anonymous_struct_union_qualified, diag::ext_array_size_conversion, diag::ext_auto_new_list_init, diag::ext_binary_literal, diag::ext_c23_enum_fixed_underlying_type, diag::ext_c23_enum_value_not_int, diag::ext_c99_array_usage, diag::ext_c99_compound_literal, diag::ext_c99_feature, diag::ext_c99_flexible_array_member, diag::ext_c99_variable_decl_in_for_loop, diag::ext_c_empty_initializer, diag::ext_c_missing_varargs_arg, diag::ext_c_nullptr, diag::ext_cast_fn_obj, diag::ext_clang_diagnose_if, diag::ext_clang_enable_if, diag::ext_cxx11_enum_fixed_underlying_type, diag::ext_cxx26_raw_string_literal_character_set, diag::ext_cxx_missing_varargs_arg, diag::ext_delete_with_message, diag::ext_designated_init, diag::ext_dtor_name_ambiguous, diag::ext_dtor_named_in_wrong_scope, diag::ext_duplicate_declspec, diag::ext_ellipsis_exception_spec, diag::ext_empty_fnmacro_arg, diag::ext_enumerator_list_comma_c, diag::ext_enumerator_list_comma_cxx, diag::ext_explicit_instantiation_without_qualified_id, diag::ext_expr_not_ice, diag::ext_extern_template, diag::ext_extra_semi, diag::ext_forward_ref_enum, diag::ext_freestanding_complex, diag::ext_gnu_array_range, diag::ext_hex_constant_invalid, diag::ext_hex_literal_invalid, diag::ext_ident_list_in_param, diag::ext_in_class_initializer_non_constant, diag::ext_integer_complement_complex, diag::ext_internal_in_extern_inline_quiet, diag::ext_line_comment, diag::ext_main_used, diag::ext_mixed_decls_code, diag::ext_multi_line_line_comment, diag::ext_named_variadic_macro, diag::ext_no_newline_eof, diag::ext_nonstandard_escape, diag::ext_ns_enum_attribute, diag::ext_opencl_double_without_pragma, diag::ext_paste_comma, diag::ext_pointer_to_const_ref_member_on_rvalue, diag::ext_pp_bad_vaargs_use, diag::ext_pp_comma_expr, diag::ext_pp_ident_directive, diag::ext_pp_line_too_big, diag::ext_pp_opencl_variadic_macros, diag::ext_pp_warning_directive, diag::ext_return_has_void_expr, diag::ext_rvalue_to_reference_access_ctor, diag::ext_rvalue_to_reference_temp_copy_no_viable, diag::ext_sizeof_alignof_function_type, diag::ext_sizeof_alignof_void_type, diag::ext_subscript_non_lvalue, diag::ext_thread_before, diag::ext_type_defined_in_offsetof, diag::ext_typecheck_addrof_void, diag::ext_typecheck_cast_nonscalar, diag::ext_typecheck_comparison_of_fptr_to_void, diag::ext_typecheck_cond_one_void, diag::ext_typecheck_convert_pointer_void_func, diag::ext_typecheck_ordered_comparison_of_pointer_and_zero, diag::ext_variadic_macro, diag::ext_vla, diag::ext_vla_cxx_in_gnu_mode, diag::ext_vla_cxx_in_gnu_mode_static_assert, diag::warn_defined_in_function_type_macro, diag::warn_ext_c23_attributes, diag::warn_ext_cxx11_attributes, diag::warn_format_conversion_argument_type_mismatch_pedantic, diag::warn_kern_is_method, diag::warn_strict_prototypes, -1,
  /* DiagArray740 */ diag::warn_opencl_unsupported_core_feature, diag::warn_pragma_extension_is_core, -1,
  /* DiagArray742 */ diag::warn_perf_constraint_implies_noexcept, -1,
  /* DiagArray743 */ diag::warn_pessimizing_move_on_initialization, diag::warn_pessimizing_move_on_return, -1,
  /* DiagArray744 */ diag::ext_sizeof_alignof_function_type, diag::ext_sizeof_alignof_void_type, diag::warn_sub_ptr_zero_size_types, -1,
  /* DiagArray745 */ diag::warn_cast_nonnull_to_bool, diag::warn_impcast_pointer_to_bool, -1,
  /* DiagArray746 */ diag::warn_pointer_compare, -1,
  /* DiagArray747 */ diag::ext_typecheck_comparison_of_pointer_integer, -1,
  /* DiagArray748 */ diag::ext_typecheck_convert_incompatible_pointer_sign, -1,
  /* DiagArray749 */ diag::warn_pointer_to_enum_cast, -1,
  /* DiagArray750 */ diag::warn_pointer_to_int_cast, -1,
  /* DiagArray751 */ diag::ext_typecheck_cond_incompatible_pointers, -1,
  /* DiagArray752 */ diag::warn_poison_system_directories, -1,
  /* DiagArray753 */ diag::warn_potentially_direct_selector_expression, -1,
  /* DiagArray754 */ diag::warn_side_effects_typeid, -1,
  /* DiagArray755 */ diag::warn_pragma_attribute_unused, -1,
  /* DiagArray756 */ diag::pp_pragma_once_in_main_file, -1,
  /* DiagArray757 */ diag::warn_pragma_pack_modified_after_include, diag::warn_pragma_pack_no_pop_eof, -1,
  /* DiagArray758 */ diag::warn_pragma_pack_non_default_at_include, -1,
  /* DiagArray759 */ diag::pp_pragma_sysheader_in_main_file, -1,
  /* DiagArray760 */ diag::warn_no_support_for_eval_method_source_on_m32, diag::warn_redefine_extname_not_applied, -1,
  /* DiagArray761 */ diag::warn_cxx11_compat_constexpr_body_invalid_stmt, diag::warn_cxx11_compat_constexpr_body_multiple_return, diag::warn_cxx11_compat_constexpr_body_no_return, diag::warn_cxx11_compat_constexpr_local_var, diag::warn_cxx11_compat_constexpr_type_definition, diag::warn_cxx11_compat_decltype_auto_type_specifier, diag::warn_cxx11_compat_deduced_return_type, diag::warn_cxx11_compat_digit_separator, diag::warn_cxx11_compat_generic_lambda, diag::warn_cxx11_compat_init_capture, diag::warn_cxx11_compat_variable_template, -1,
  /* DiagArray763 */ diag::warn_cxx14_compat_class_template_argument_deduction, diag::warn_cxx14_compat_constexpr_if, diag::warn_cxx14_compat_constexpr_on_lambda, diag::warn_cxx14_compat_decomp_decl, diag::warn_cxx14_compat_fold_expression, diag::warn_cxx14_compat_init_statement, diag::warn_cxx14_compat_inline_variable, diag::warn_cxx14_compat_nested_namespace_definition, diag::warn_cxx14_compat_star_this_lambda_capture, diag::warn_cxx14_compat_static_assert_no_message, diag::warn_cxx14_compat_template_nontype_parm_auto_type, diag::warn_cxx14_compat_template_template_param_typename, diag::warn_cxx14_compat_u8_character_literal, diag::warn_cxx14_compat_using_attribute_ns, diag::warn_cxx17_compat_multi_using_declaration, diag::warn_cxx17_compat_using_declaration_pack, diag::warn_for_range_begin_end_types_differ, -1,
  /* DiagArray764 */ diag::warn_cxx14_compat_ns_enum_attribute, diag::warn_cxx17_hex_literal, -1,
  /* DiagArray765 */ diag::warn_cxx17_compat_adl_only_template_id, diag::warn_cxx17_compat_bitfield_member_init, diag::warn_cxx17_compat_capture_binding, diag::warn_cxx17_compat_constexpr_body_invalid_stmt, diag::warn_cxx17_compat_constexpr_ctor_missing_init, diag::warn_cxx17_compat_constexpr_function_try_block, diag::warn_cxx17_compat_constexpr_local_var_no_init, diag::warn_cxx17_compat_constexpr_union_ctor_no_init, diag::warn_cxx17_compat_constexpr_virtual, diag::warn_cxx17_compat_ctad_for_alias_templates, diag::warn_cxx17_compat_decomp_decl_spec, diag::warn_cxx17_compat_defaulted_comparison, diag::warn_cxx17_compat_defaulted_method_type_mismatch, diag::warn_cxx17_compat_equals_this_lambda_capture, diag::warn_cxx17_compat_explicit_bool, diag::warn_cxx17_compat_for_range_init_stmt, diag::warn_cxx17_compat_init_capture_pack, diag::warn_cxx17_compat_inline_nested_namespace_definition, diag::warn_cxx17_compat_lambda_def_ctor_assign, diag::warn_cxx17_compat_lambda_template_parameter_list, diag::warn_cxx17_compat_missing_varargs_arg, diag::warn_cxx17_compat_spaceship, diag::warn_cxx17_compat_template_nontype_parm_type, diag::warn_cxx17_compat_unicode_type, diag::warn_cxx17_compat_using_decl_class_member_enumerator, diag::warn_cxx17_compat_using_decl_non_member_enumerator, diag::warn_cxx17_compat_using_decl_scoped_enumerator, diag::warn_cxx17_compat_using_enum_declaration, -1,
  /* DiagArray766 */ diag::warn_cxx17_compat_designated_init, diag::warn_cxx17_compat_pointer_to_const_ref_member_on_rvalue, -1,
  /* DiagArray767 */ diag::ext_subscript_overload, diag::warn_cxx20_alias_in_init_statement, diag::warn_cxx20_compat_auto_expr, diag::warn_cxx20_compat_consteval_if, diag::warn_cxx20_compat_constexpr_body_invalid_stmt, diag::warn_cxx20_compat_constexpr_var, diag::warn_cxx20_compat_decl_attrs_on_lambda, diag::warn_cxx20_compat_label_end_of_compound_statement, diag::warn_cxx20_compat_operator_overload_static, diag::warn_cxx20_compat_size_t_suffix, diag::warn_cxx20_compat_static_lambda, diag::warn_cxx23_compat_pp_directive, diag::warn_cxx23_compat_warning_directive, diag::warn_cxx23_delimited_escape_sequence, -1,
  /* DiagArray769 */ diag::warn_cxx20_compat_static_assert_user_generated_message, diag::warn_cxx23_compat_decl_attrs_on_binding, diag::warn_cxx23_delete_with_message, diag::warn_cxx23_pack_indexing, diag::warn_cxx23_placeholder_var_definition, diag::warn_cxx23_variadic_friends, diag::warn_cxx26_compat_raw_string_literal_character_set, diag::warn_pre_cxx26_ambiguous_pack_indexing_type, -1,
  /* DiagArray773 */ diag::warn_c11_compat_keyword, -1,
  /* DiagArray775 */ diag::warn_c17_compat_ellipsis_only_parameter, diag::warn_c17_compat_enum_fixed_underlying_type, diag::warn_c17_compat_enum_value_not_int, diag::warn_c17_compat_static_assert_no_message, diag::warn_c17_compat_u8_character_literal, diag::warn_c23_compat_binary_literal, diag::warn_c23_compat_bitint_suffix, diag::warn_c23_compat_digit_separator, diag::warn_c23_compat_empty_initializer, diag::warn_c23_compat_keyword, diag::warn_c23_compat_label_end_of_compound_statement, diag::warn_c23_compat_label_followed_by_declaration, diag::warn_c23_compat_literal_ucn_control_character, diag::warn_c23_compat_literal_ucn_escape_basic_scs, diag::warn_c23_compat_pp_directive, diag::warn_c23_compat_warning_directive, diag::warn_compat_pp_embed_directive, diag::warn_pre_c23_compat_attributes, -1,
  /* DiagArray779 */ diag::warn_c2y_compat_alignof_incomplete_array, diag::warn_c2y_compat_generic_with_type_arg, diag::warn_c2y_compat_increment_complex, diag::warn_c23_compat_case_range, diag::warn_c23_compat_imaginary_constant, -1,
  /* DiagArray781 */ diag::warn_omp51_compat_attributes, -1,
  /* DiagArray782 */ diag::ext_predef_outside_function, -1,
  /* DiagArray783 */ diag::warn_private_extern, -1,
  /* DiagArray784 */ diag::warn_use_of_private_header_outside_module, -1,
  /* DiagArray785 */ diag::warn_mmap_mismatched_private_module_name, diag::warn_mmap_mismatched_private_submodule, diag::warn_mmap_redundant_export_as, diag::warn_no_priv_submodule_use_toplevel, -1,
  /* DiagArray786 */ diag::warn_profile_data_missing, -1,
  /* DiagArray787 */ diag::warn_profile_data_out_of_date, -1,
  /* DiagArray788 */ diag::warn_profile_data_unprofiled, -1,
  /* DiagArray789 */ diag::warn_property_access_suggest, -1,
  /* DiagArray790 */ diag::warn_property_attr_mismatch, diag::warn_property_attribute, diag::warn_property_redecl_getter_mismatch, diag::warn_readonly_property, -1,
  /* DiagArray791 */ diag::warn_unimplemented_protocol_method, -1,
  /* DiagArray792 */ diag::warn_protocol_property_mismatch, -1,
  /* DiagArray793 */ diag::warn_avx_calling_convention, -1,
  /* DiagArray794 */ diag::warn_ptrauth_auth_null_pointer, diag::warn_ptrauth_sign_null_pointer, -1,
  /* DiagArray795 */ diag::err_func_returning_qualified_void, -1,
  /* DiagArray796 */ diag::warn_quoted_include_in_framework_header, -1,
  /* DiagArray798 */ diag::warn_for_range_ref_binds_ret_temp, -1,
  /* DiagArray799 */ diag::warn_for_range_const_ref_binds_temp_built_from_ref, diag::warn_for_range_copy, -1,
  /* DiagArray800 */ diag::warn_var_decl_not_read_only, -1,
  /* DiagArray801 */ diag::warn_auto_readonly_iboutlet_property, -1,
  /* DiagArray802 */ diag::warn_bad_receiver_type, -1,
  /* DiagArray803 */ diag::warn_receiver_forward_class, diag::warn_receiver_forward_instance, -1,
  /* DiagArray804 */ diag::ext_member_redeclared, -1,
  /* DiagArray805 */ diag::warn_sycl_entry_point_redundant_declaration, -1,
  /* DiagArray806 */ diag::warn_consteval_if_always_true, -1,
  /* DiagArray808 */ diag::warn_redundant_move_on_return, -1,
  /* DiagArray809 */ diag::warn_redundant_parens_around_declarator, -1,
  /* DiagArray810 */ diag::ext_register_storage_class, -1,
  /* DiagArray811 */ diag::warn_reinterpret_different_from_static, -1,
  /* DiagArray812 */ diag::remark_fe_backend_plugin, -1,
  /* DiagArray814 */ diag::warn_initializer_out_of_order, diag::warn_some_initializers_out_of_order, -1,
  /* DiagArray815 */ diag::ext_designated_init_reordered, -1,
  /* DiagArray816 */ diag::warn_objc_requires_super_protocol, -1,
  /* DiagArray817 */ diag::warn_pp_macro_is_reserved_attribute_id, -1,
  /* DiagArray819 */ diag::warn_reserved_extern_symbol, -1,
  /* DiagArray820 */ diag::warn_pp_macro_is_reserved_id, -1,
  /* DiagArray821 */ diag::warn_reserved_module_name, -1,
  /* DiagArray822 */ diag::ext_ms_reserved_user_defined_literal, diag::ext_reserved_user_defined_literal, -1,
  /* DiagArray823 */ diag::warn_pragma_restrict_expansion_macro_use, -1,
  /* DiagArray824 */ diag::ext_retained_language_linkage, -1,
  /* DiagArray826 */ diag::ext_return_has_expr, diag::ext_return_missing_expr, diag::warn_return_missing_expr, -1,
  /* DiagArray827 */ diag::warn_musttail_local_temp_addr_ref, diag::warn_ret_addr_label, diag::warn_ret_local_temp_addr_ref, diag::warn_ret_stack_addr_ref, -1,
  /* DiagArray829 */ diag::warn_falloff_nonvoid_coroutine, diag::warn_falloff_nonvoid_function, diag::warn_falloff_nonvoid_lambda, diag::warn_maybe_falloff_nonvoid_coroutine, diag::warn_maybe_falloff_nonvoid_function, diag::warn_maybe_falloff_nonvoid_lambda, -1,
  /* DiagArray830 */ diag::warn_return_value_udt, diag::warn_return_value_udt_incomplete, -1,
  /* DiagArray831 */ diag::ext_ovl_rewrite_equalequal_not_bool, -1,
  /* DiagArray832 */ diag::remark_cc1_round_trip_generated, -1,
  /* DiagArray833 */ diag::warn_no_dynamic_cast_with_rtti_disabled, diag::warn_no_typeid_with_rtti_disabled, -1,
  /* DiagArray834 */ diag::remark_sanitize_address_insert_extra_padding_accepted, diag::remark_sanitize_address_insert_extra_padding_rejected, -1,
  /* DiagArray835 */ diag::warn_drv_sarif_format_unstable, -1,
  /* DiagArray836 */ diag::remark_pp_search_path_usage, -1,
  /* DiagArray837 */ diag::warn_attribute_section_on_redeclaration, diag::warn_duplicate_codeseg_attribute, diag::warn_mismatched_section, -1,
  /* DiagArray838 */ diag::warn_unimplemented_selector, -1,
  /* DiagArray839 */ diag::warn_multiple_selectors, -1,
  /* DiagArray840 */ diag::warn_self_assignment_builtin, -1,
  /* DiagArray841 */ diag::warn_identity_field_assign, -1,
  /* DiagArray842 */ diag::warn_self_assignment_overloaded, -1,
  /* DiagArray843 */ diag::warn_self_move, -1,
  /* DiagArray844 */ diag::warn_semicolon_before_method_body, -1,
  /* DiagArray845 */ diag::warn_missing_sentinel, diag::warn_not_enough_argument, -1,
  /* DiagArray847 */ diag::warn_fe_serialized_diag_failure, diag::warn_fe_serialized_diag_failure_during_finalization, diag::warn_fe_serialized_diag_merge_failure, -1,
  /* DiagArray848 */ diag::warn_decl_shadow, -1,
  /* DiagArray850 */ diag::warn_shadow_field, -1,
  /* DiagArray851 */ diag::warn_ctor_parm_shadows_field, -1,
  /* DiagArray852 */ diag::warn_modifying_shadowing_decl, -1,
  /* DiagArray853 */ diag::warn_ivar_use_hidden, -1,
  /* DiagArray854 */ diag::warn_decl_shadow_uncaptured_local, -1,
  /* DiagArray855 */ diag::warn_shift_negative, -1,
  /* DiagArray856 */ diag::warn_shift_gt_typewidth, -1,
  /* DiagArray857 */ diag::warn_shift_lhs_negative, -1,
  /* DiagArray858 */ diag::warn_addition_in_bitshift, -1,
  /* DiagArray859 */ diag::warn_shift_result_gt_typewidth, -1,
  /* DiagArray860 */ diag::warn_shift_result_sets_sign_bit, -1,
  /* DiagArray861 */ diag::warn_impcast_integer_64_32, -1,
  /* DiagArray862 */ diag::warn_mixed_sign_comparison, -1,
  /* DiagArray863 */ diag::warn_impcast_integer_sign, diag::warn_impcast_integer_sign_conditional, diag::warn_impcast_nonnegative_result, -1,
  /* DiagArray865 */ diag::warn_no_underlying_type_specified_for_enum_bitfield, -1,
  /* DiagArray866 */ diag::ext_wchar_t_sign_spec, -1,
  /* DiagArray867 */ diag::warn_impcast_single_bit_bitield_precision_constant, -1,
  /* DiagArray868 */ diag::warn_sizeof_array_param, -1,
  /* DiagArray869 */ diag::warn_sizeof_array_decay, -1,
  /* DiagArray870 */ diag::warn_division_sizeof_array, -1,
  /* DiagArray871 */ diag::warn_division_sizeof_ptr, -1,
  /* DiagArray872 */ diag::warn_sizeof_pointer_expr_memaccess, diag::warn_sizeof_pointer_type_memaccess, -1,
  /* DiagArray873 */ diag::warn_slash_u_filename, -1,
  /* DiagArray874 */ diag::warn_slh_does_not_support_asm_goto, -1,
  /* DiagArray875 */ diag::remark_sloc_usage, -1,
  /* DiagArray876 */ diag::warn_sometimes_uninit_var, -1,
  /* DiagArray877 */ diag::warn_fe_source_mgr, -1,
  /* DiagArray878 */ diag::warn_acc_clause_unimplemented, diag::warn_acc_construct_unimplemented, diag::warn_pragma_acc_ignored, -1,
  /* DiagArray879 */ diag::warn_omp_declare_variant_after_emitted, diag::warn_omp_declare_variant_after_used, diag::warn_omp_declare_variant_marked_as_declare_variant, diag::warn_omp_declare_variant_score_not_constant, diag::warn_omp_nesting_simd, diag::warn_omp_unterminated_declare_target, diag::warn_pragma_omp_ignored, diag::warn_unknown_declare_variant_isa_trait, -1,
  /* DiagArray880 */ diag::warn_sampler_initializer_invalid_bits, -1,
  /* DiagArray882 */ diag::warn_stack_exhausted, -1,
  /* DiagArray883 */ diag::warn_stack_clash_protection_inline_asm, -1,
  /* DiagArray884 */ diag::ext_in_class_initializer_float_type_cxx11, -1,
  /* DiagArray885 */ diag::ext_internal_in_extern_inline, diag::ext_internal_in_extern_inline_quiet, -1,
  /* DiagArray886 */ diag::warn_static_inline_explicit_inst_ignored, -1,
  /* DiagArray887 */ diag::warn_static_local_in_extern_inline, -1,
  /* DiagArray888 */ diag::warn_static_self_reference_in_init, -1,
  /* DiagArray889 */ diag::warn_drv_libstdcxx_not_found, -1,
  /* DiagArray901 */ diag::warn_strict_potentially_direct_selector_expression, -1,
  /* DiagArray902 */ diag::ext_compat_template_param_shadow, -1,
  /* DiagArray903 */ diag::warn_strict_prototypes, -1,
  /* DiagArray904 */ diag::warn_strict_multiple_method_decl, -1,
  /* DiagArray905 */ diag::warn_stringcompare, -1,
  /* DiagArray906 */ diag::warn_concatenated_literal_array_init, -1,
  /* DiagArray907 */ diag::warn_impcast_string_literal_to_bool, -1,
  /* DiagArray908 */ diag::warn_string_plus_char, -1,
  /* DiagArray909 */ diag::warn_string_plus_int, -1,
  /* DiagArray910 */ diag::warn_strlcpycat_wrong_size, -1,
  /* DiagArray911 */ diag::warn_strncat_large_size, diag::warn_strncat_src_size, diag::warn_strncat_wrong_size, -1,
  /* DiagArray912 */ diag::warn_suggest_destructor_marked_not_override_overriding, -1,
  /* DiagArray913 */ diag::warn_suggest_function_marked_not_override_overriding, -1,
  /* DiagArray914 */ diag::ext_typecheck_base_super, -1,
  /* DiagArray915 */ diag::warn_suspicious_bzero_size, -1,
  /* DiagArray917 */ diag::warn_attr_swift_name_decl_kind, diag::warn_attr_swift_name_decl_missing_params, diag::warn_attr_swift_name_function, diag::warn_attr_swift_name_getter_parameters, diag::warn_attr_swift_name_invalid_identifier, diag::warn_attr_swift_name_missing_parameters, diag::warn_attr_swift_name_multiple_selfs, diag::warn_attr_swift_name_num_params, diag::warn_attr_swift_name_setter_parameters, diag::warn_attr_swift_name_subscript_getter_newValue, diag::warn_attr_swift_name_subscript_invalid_parameter, diag::warn_attr_swift_name_subscript_setter_multiple_newValues, diag::warn_attr_swift_name_subscript_setter_no_newValue, -1,
  /* DiagArray918 */ diag::warn_case_value_overflow, diag::warn_missing_case, diag::warn_not_in_enum, -1,
  /* DiagArray919 */ diag::warn_bool_switch_condition, -1,
  /* DiagArray920 */ diag::warn_switch_default, -1,
  /* DiagArray921 */ diag::warn_def_missing_case, -1,
  /* DiagArray922 */ diag::warn_sync_op_misaligned, -1,
  /* DiagArray923 */ diag::warn_sync_fetch_and_nand_semantics_change, -1,
  /* DiagArray925 */ diag::warn_target_clone_mixed_values, -1,
  /* DiagArray926 */ diag::warn_comparison_bitwise_always, diag::warn_comparison_bitwise_or, -1,
  /* DiagArray927 */ diag::warn_alignment_builtin_useless, diag::warn_comparison_always, -1,
  /* DiagArray928 */ diag::warn_integer_constants_in_conditional_always_true, diag::warn_left_shift_always, diag::warn_tautological_bool_compare, -1,
  /* DiagArray930 */ diag::warn_out_of_range_compare, -1,
  /* DiagArray931 */ diag::warn_tautological_negation_and_compare, diag::warn_tautological_negation_or_compare, -1,
  /* DiagArray932 */ diag::warn_tautological_compare_objc_bool, -1,
  /* DiagArray933 */ diag::warn_tautological_overlap_comparison, -1,
  /* DiagArray934 */ diag::warn_nonnull_expr_compare, diag::warn_null_pointer_compare, -1,
  /* DiagArray935 */ diag::warn_tautological_constant_compare, -1,
  /* DiagArray936 */ diag::warn_address_of_reference_null_compare, diag::warn_this_null_compare, -1,
  /* DiagArray937 */ diag::warn_unsigned_char_always_true_comparison, -1,
  /* DiagArray938 */ diag::warn_unsigned_enum_always_true_comparison, -1,
  /* DiagArray939 */ diag::warn_unsigned_always_true_comparison, -1,
  /* DiagArray940 */ diag::warn_tautological_compare_value_range, -1,
  /* DiagArray941 */ diag::warn_tcb_enforcement_violation, -1,
  /* DiagArray942 */ diag::ext_template_after_declarative_nns, -1,
  /* DiagArray943 */ diag::warn_tentative_incomplete_array, -1,
  /* DiagArray944 */ diag::ext_typecheck_decl_incomplete_type, -1,
  /* DiagArray946 */ diag::warn_acquired_before, diag::warn_acquired_before_after_cycle, diag::warn_cannot_resolve_lock, diag::warn_double_lock, diag::warn_expect_fewer_underlying_mutexes, diag::warn_expect_more_underlying_mutexes, diag::warn_expecting_lock_held_on_loop, diag::warn_expecting_locked, diag::warn_fun_excludes_mutex, diag::warn_fun_requires_lock, diag::warn_fun_requires_negative_cap, diag::warn_lock_exclusive_and_shared, diag::warn_lock_some_predecessors, diag::warn_no_unlock, diag::warn_unlock_but_no_lock, diag::warn_unlock_kind_mismatch, diag::warn_unmatched_underlying_mutexes, diag::warn_var_deref_requires_any_lock, diag::warn_var_deref_requires_lock, diag::warn_variable_requires_any_lock, diag::warn_variable_requires_lock, -1,
  /* DiagArray947 */ diag::warn_thread_attribute_argument_not_lockable, diag::warn_thread_attribute_decl_not_lockable, diag::warn_thread_attribute_decl_not_pointer, diag::warn_thread_attribute_ignored, diag::warn_thread_attribute_not_on_capability_member, diag::warn_thread_attribute_not_on_non_static_member, diag::warn_thread_attribute_not_on_scoped_lockable_param, -1,
  /* DiagArray948 */ diag::warn_thread_safety_beta, -1,
  /* DiagArray949 */ diag::warn_acquire_requires_negative_cap, -1,
  /* DiagArray950 */ diag::warn_fun_requires_lock_precise, diag::warn_var_deref_requires_lock_precise, diag::warn_variable_requires_lock_precise, -1,
  /* DiagArray951 */ diag::warn_guarded_pass_by_reference, diag::warn_pt_guarded_pass_by_reference, -1,
  /* DiagArray952 */ diag::warn_guarded_return_by_reference, diag::warn_pt_guarded_return_by_reference, -1,
  /* DiagArray953 */ diag::warn_thread_safety_verbose, -1,
  /* DiagArray954 */ diag::trigraph_converted, diag::trigraph_ends_block_comment, diag::trigraph_ignored, diag::trigraph_ignored_block_comment, -1,
  /* DiagArray956 */ diag::warn_type_safety_null_pointer_required, diag::warn_type_safety_type_mismatch, diag::warn_type_tag_for_datatype_wrong_kind, -1,
  /* DiagArray957 */ diag::ext_redefinition_of_typedef, -1,
  /* DiagArray958 */ diag::ext_typename_missing, -1,
  /* DiagArray959 */ diag::warn_fe_unable_to_open_stats_file, -1,
  /* DiagArray960 */ diag::warn_unaligned_access, -1,
  /* DiagArray961 */ diag::warn_imp_cast_drops_unaligned, -1,
  /* DiagArray962 */ diag::warn_unavailable_fwdclass_message, -1,
  /* DiagArray963 */ diag::warn_undeclared_selector, diag::warn_undeclared_selector_with_typo, -1,
  /* DiagArray964 */ diag::warn_pp_undef_identifier, -1,
  /* DiagArray965 */ diag::warn_pp_undef_prefix, -1,
  /* DiagArray966 */ diag::warn_attribute_arm_za_builtin_no_za_state, -1,
  /* DiagArray967 */ diag::warn_attribute_arm_zt0_builtin_no_zt0_state, -1,
  /* DiagArray968 */ diag::warn_address_of_reference_bool_conversion, diag::warn_this_bool_conversion, -1,
  /* DiagArray969 */ diag::warn_func_template_missing, -1,
  /* DiagArray970 */ diag::warn_undefined_inline, -1,
  /* DiagArray971 */ diag::warn_undefined_internal, -1,
  /* DiagArray972 */ diag::ext_undefined_internal_type, -1,
  /* DiagArray973 */ diag::warn_pointer_indirection_from_incompatible_type, diag::warn_undefined_reinterpret_cast, -1,
  /* DiagArray974 */ diag::warn_var_template_missing, -1,
  /* DiagArray975 */ diag::warn_throw_underaligned_obj, -1,
  /* DiagArray976 */ diag::warn_side_effects_unevaluated_context, -1,
  /* DiagArray977 */ diag::warn_unguarded_availability, diag::warn_unguarded_availability_unavailable, -1,
  /* DiagArray978 */ diag::warn_unguarded_availability_new, diag::warn_unguarded_availability_unavailable_new, -1,
  /* DiagArray979 */ diag::warn_delimited_ucn_empty, diag::warn_delimited_ucn_incomplete, diag::warn_ucn_escape_incomplete, diag::warn_ucn_escape_no_digits, diag::warn_ucn_escape_surrogate, diag::warn_ucn_not_valid_in_c89, diag::warn_ucn_not_valid_in_c89_literal, -1,
  /* DiagArray980 */ diag::warn_utf8_symbol_homoglyph, -1,
  /* DiagArray981 */ diag::ext_unicode_whitespace, -1,
  /* DiagArray982 */ diag::warn_utf8_symbol_zero_width, -1,
  /* DiagArray983 */ diag::warn_base_class_is_uninit, diag::warn_field_is_uninit, diag::warn_reference_field_is_uninit, diag::warn_uninit_byref_blockvar_captured_by_block, diag::warn_uninit_self_reference_in_init, diag::warn_uninit_self_reference_in_reference_init, diag::warn_uninit_var, -1,
  /* DiagArray984 */ diag::warn_uninit_const_reference, -1,
  /* DiagArray985 */ diag::warn_field_requires_explicit_init, -1,
  /* DiagArray986 */ diag::warn_drv_potentially_misspelled_joined_argument, diag::warn_drv_unknown_argument_clang_cl, diag::warn_drv_unknown_argument_clang_cl_with_suggestion, -1,
  /* DiagArray987 */ diag::warn_omp_assume_attribute_string_unknown, -1,
  /* DiagArray988 */ diag::warn_unknown_attribute_ignored, -1,
  /* DiagArray989 */ diag::warn_drv_new_cuda_version, diag::warn_drv_partially_supported_cuda_version, -1,
  /* DiagArray990 */ diag::warn_pp_invalid_directive, -1,
  /* DiagArray991 */ diag::ext_unknown_escape, -1,
  /* DiagArray992 */ diag::ext_on_off_switch_syntax, diag::ext_pragma_syntax_eod, diag::ext_stdc_pragma_ignored, diag::warn_pragma_diagnostic_cannot_pop, diag::warn_pragma_diagnostic_invalid, diag::warn_pragma_diagnostic_invalid_option, diag::warn_pragma_diagnostic_invalid_token, diag::warn_pragma_exec_charset_expected, diag::warn_pragma_exec_charset_push_invalid, diag::warn_pragma_exec_charset_spec_invalid, diag::warn_pragma_ignored, diag::warn_pragma_include_alias_expected, diag::warn_pragma_include_alias_expected_filename, diag::warn_pragma_include_alias_mismatch_angle, diag::warn_pragma_include_alias_mismatch_quote, diag::warn_pragma_warning_expected, diag::warn_pragma_warning_expected_number, diag::warn_pragma_warning_push_level, diag::warn_pragma_warning_spec_invalid, diag::warn_stdc_fenv_round_not_supported, -1,
  /* DiagArray993 */ diag::warn_unknown_sanitizer_ignored, -1,
  /* DiagArray994 */ diag::warn_pragma_diagnostic_unknown_warning, diag::warn_unknown_diag_option, diag::warn_unknown_warning_specifier, -1,
  /* DiagArray995 */ diag::ext_template_arg_unnamed_type, -1,
  /* DiagArray996 */ diag::warn_unneeded_internal_decl, diag::warn_unneeded_static_internal_decl, -1,
  /* DiagArray997 */ diag::warn_unneeded_member_function, -1,
  /* DiagArray998 */ diag::warn_unqualified_call_to_std_cast_function, -1,
  /* DiagArray999 */ diag::warn_unreachable, -1,
  /* DiagArray1001 */ diag::warn_unreachable_break, -1,
  /* DiagArray1002 */ diag::warn_unreachable_fallthrough_attr, -1,
  /* DiagArray1003 */ diag::warn_unreachable_association, -1,
  /* DiagArray1004 */ diag::warn_unreachable_loop_increment, -1,
  /* DiagArray1005 */ diag::warn_unreachable_return, -1,
  /* DiagArray1006 */ diag::warn_unsafe_buffer_operation, diag::warn_unsafe_buffer_variable, -1,
  /* DiagArray1007 */ diag::warn_unsafe_buffer_usage_in_container, -1,
  /* DiagArray1008 */ diag::warn_unsafe_buffer_libc_call, -1,
  /* DiagArray1009 */ diag::warn_unsequenced_mod_mod, diag::warn_unsequenced_mod_use, -1,
  /* DiagArray1010 */ diag::warn_drv_no_floating_point_registers, diag::warn_drv_unsupported_float_abi_by_lib, -1,
  /* DiagArray1011 */ diag::warn_target_unsupported_abs2008, diag::warn_target_unsupported_abslegacy, -1,
  /* DiagArray1012 */ diag::warn_at_available_unchecked_use, -1,
  /* DiagArray1013 */ diag::warn_target_unsupported_compact_branches, -1,
  /* DiagArray1014 */ diag::warn_attribute_dll_instantiated_base_class, -1,
  /* DiagArray1015 */ diag::warn_fe_backend_unsupported_fp_exceptions, diag::warn_fe_backend_unsupported_fp_rounding, -1,
  /* DiagArray1016 */ diag::warn_template_qualified_friend_ignored, diag::warn_template_qualified_friend_unsupported, -1,
  /* DiagArray1017 */ diag::warn_drv_unsupported_gpopt, -1,
  /* DiagArray1018 */ diag::warn_target_unsupported_nan2008, diag::warn_target_unsupported_nanlegacy, -1,
  /* DiagArray1019 */ diag::warn_drv_dwarf_version_limited_by_target, diag::warn_drv_unsupported_debug_info_opt_for_target, -1,
  /* DiagArray1020 */ diag::warn_attribute_protected_visibility, -1,
  /* DiagArray1021 */ diag::ext_partial_specs_not_deducible, -1,
  /* DiagArray1024 */ diag::warn_unused_but_set_parameter, -1,
  /* DiagArray1025 */ diag::warn_unused_but_set_variable, -1,
  /* DiagArray1026 */ diag::warn_drv_diagnostics_hotness_requires_pgo, diag::warn_drv_diagnostics_misexpect_requires_pgo, diag::warn_drv_empty_joined_argument, diag::warn_drv_fraw_string_literals_in_cxx11, diag::warn_drv_input_file_unused, diag::warn_drv_input_file_unused_by_cpp, diag::warn_drv_invalid_arch_name_with_suggestion, diag::warn_drv_large_data_threshold_invalid_code_model, diag::warn_drv_preprocessed_input_file_unused, diag::warn_drv_unused_argument, diag::warn_drv_unused_x, diag::warn_ignored_clang_option, diag::warn_ignoring_fdiscard_for_bitcode, diag::warn_ignoring_verify_debuginfo_preserve_export, -1,
  /* DiagArray1027 */ diag::warn_unused_comparison, -1,
  /* DiagArray1028 */ diag::warn_unused_const_variable, -1,
  /* DiagArray1029 */ diag::warn_unused_exception_param, -1,
  /* DiagArray1030 */ diag::warn_unused_function, -1,
  /* DiagArray1031 */ diag::warn_unused_property_expr, -1,
  /* DiagArray1032 */ diag::warn_unused_label, -1,
  /* DiagArray1033 */ diag::warn_unused_lambda_capture, -1,
  /* DiagArray1034 */ diag::warn_unused_local_typedef, -1,
  /* DiagArray1036 */ diag::pp_macro_not_used, -1,
  /* DiagArray1037 */ diag::warn_unused_member_function, -1,
  /* DiagArray1038 */ diag::warn_unused_parameter, -1,
  /* DiagArray1039 */ diag::warn_unused_private_field, -1,
  /* DiagArray1040 */ diag::warn_unused_property_backing_ivar, -1,
  /* DiagArray1041 */ diag::warn_unused_result, -1,
  /* DiagArray1042 */ diag::warn_unused_template, -1,
  /* DiagArray1043 */ diag::warn_unused_call, diag::warn_unused_comma_left_operand, diag::warn_unused_constructor, diag::warn_unused_container_subscript_expr, diag::warn_unused_expr, diag::warn_unused_return_type, diag::warn_unused_voidptr, -1,
  /* DiagArray1044 */ diag::warn_unused_variable, -1,
  /* DiagArray1045 */ diag::warn_unused_volatile, -1,
  /* DiagArray1046 */ diag::warn_used_but_marked_unused, -1,
  /* DiagArray1047 */ diag::warn_user_literal_reserved, -1,
  /* DiagArray1048 */ diag::warn_diagnose_if_succeeded, -1,
  /* DiagArray1049 */ diag::warn_second_arg_of_va_start_not_last_named_param, diag::warn_second_parameter_to_va_arg_array, diag::warn_second_parameter_to_va_arg_never_compatible, diag::warn_va_start_type_is_undefined, -1,
  /* DiagArray1050 */ diag::ext_c_missing_varargs_arg, diag::ext_cxx_missing_varargs_arg, diag::warn_c17_compat_missing_varargs_arg, -1,
  /* DiagArray1051 */ diag::ext_named_variadic_macro, diag::ext_pp_bad_vaopt_use, diag::ext_variadic_macro, -1,
  /* DiagArray1052 */ diag::warn_typecheck_vector_element_sizes_not_equal, -1,
  /* DiagArray1053 */ diag::warn_incompatible_vectors, -1,
  /* DiagArray1055 */ diag::warn_empty_parens_are_function_decl, diag::warn_parens_disambiguated_as_function_declaration, diag::warn_parens_disambiguated_as_variable_declaration, -1,
  /* DiagArray1056 */ diag::warn_decl_in_param_list, diag::warn_redefinition_in_param_list, -1,
  /* DiagArray1057 */ diag::warn_vla_used, -1,
  /* DiagArray1058 */ diag::ext_vla_cxx, diag::ext_vla_cxx_in_gnu_mode, -1,
  /* DiagArray1059 */ diag::ext_vla, -1,
  /* DiagArray1060 */ diag::ext_vla_cxx_in_gnu_mode_static_assert, diag::ext_vla_cxx_static_assert, -1,
  /* DiagArray1061 */ diag::warn_void_pointer_to_enum_cast, -1,
  /* DiagArray1062 */ diag::warn_void_pointer_to_int_cast, -1,
  /* DiagArray1063 */ diag::ext_typecheck_indirection_through_void_pointer, -1,
  /* DiagArray1065 */ diag::warn_wasm_dynamic_exception_spec_ignored, -1,
  /* DiagArray1066 */ diag::warn_weak_template_vtable, -1,
  /* DiagArray1067 */ diag::warn_weak_vtable, -1,
  /* DiagArray1068 */ diag::ext_deprecated_string_literal_conversion, -1,
  /* DiagArray1070 */ diag::warn_xor_used_as_pow, diag::warn_xor_used_as_pow_base, diag::warn_xor_used_as_pow_base_extra, -1,
  /* DiagArray1071 */ diag::warn_zero_as_null_pointer_constant, -1,
  /* DiagArray1072 */ diag::ext_typecheck_zero_array_size, -1,
};

static const int16_t DiagSubGroups[] = {
  /* Empty */ -1,
  /* DiagSubGroup0 */ 328, -1,
  /* DiagSubGroup4 */ 19, 328, -1,
  /* DiagSubGroup12 */ 745, 905, 934, -1,
  /* DiagSubGroup19 */ 609, 730, 918, 919, 574, 727, 1058, 742, -1,
  /* DiagSubGroup30 */ 235, -1,
  /* DiagSubGroup33 */ 40, 39, 36, -1,
  /* DiagSubGroup38 */ 35, -1,
  /* DiagSubGroup49 */ 50, -1,
  /* DiagSubGroup59 */ 433, 208, -1,
  /* DiagSubGroup64 */ 988, 423, -1,
  /* DiagSubGroup75 */ 112, 146, -1,
  /* DiagSubGroup76 */ 149, -1,
  /* DiagSubGroup79 */ 867, -1,
  /* DiagSubGroup86 */ 745, 968, -1,
  /* DiagSubGroup87 */ 86, -1,
  /* DiagSubGroup88 */ 83, -1,
  /* DiagSubGroup98 */ 101, -1,
  /* DiagSubGroup99 */ 105, -1,
  /* DiagSubGroup100 */ 109, -1,
  /* DiagSubGroup101 */ 109, 104, 102, 761, 763, 765, 767, 769, -1,
  /* DiagSubGroup103 */ 101, 762, 764, 766, 768, 770, -1,
  /* DiagSubGroup105 */ 106, 107, 108, -1,
  /* DiagSubGroup109 */ 110, -1,
  /* DiagSubGroup113 */ 763, 765, 767, 769, -1,
  /* DiagSubGroup114 */ 113, 764, 766, 768, 770, -1,
  /* DiagSubGroup115 */ 112, 111, -1,
  /* DiagSubGroup117 */ 265, 253, 118, 765, 767, 769, -1,
  /* DiagSubGroup119 */ 117, 766, 768, 770, -1,
  /* DiagSubGroup120 */ 116, -1,
  /* DiagSubGroup121 */ 115, -1,
  /* DiagSubGroup122 */ 117, -1,
  /* DiagSubGroup123 */ 118, -1,
  /* DiagSubGroup124 */ 120, -1,
  /* DiagSubGroup126 */ 767, 769, -1,
  /* DiagSubGroup127 */ 126, 768, 770, -1,
  /* DiagSubGroup129 */ 128, 125, 1050, -1,
  /* DiagSubGroup131 */ 769, -1,
  /* DiagSubGroup132 */ 133, -1,
  /* DiagSubGroup135 */ 126, -1,
  /* DiagSubGroup136 */ 127, -1,
  /* DiagSubGroup137 */ 129, -1,
  /* DiagSubGroup138 */ 132, -1,
  /* DiagSubGroup139 */ 228, -1,
  /* DiagSubGroup140 */ 134, -1,
  /* DiagSubGroup141 */ 765, -1,
  /* DiagSubGroup142 */ 766, -1,
  /* DiagSubGroup143 */ 763, -1,
  /* DiagSubGroup144 */ 764, -1,
  /* DiagSubGroup145 */ 761, -1,
  /* DiagSubGroup147 */ 762, -1,
  /* DiagSubGroup148 */ 151, 153, 761, 763, 765, 767, 769, -1,
  /* DiagSubGroup152 */ 148, 149, 150, 762, 764, 766, 768, 770, -1,
  /* DiagSubGroup156 */ 1050, -1,
  /* DiagSubGroup157 */ 155, -1,
  /* DiagSubGroup158 */ 156, -1,
  /* DiagSubGroup161 */ 128, -1,
  /* DiagSubGroup162 */ 161, -1,
  /* DiagSubGroup164 */ 183, -1,
  /* DiagSubGroup167 */ 169, 168, -1,
  /* DiagSubGroup177 */ 629, -1,
  /* DiagSubGroup181 */ 180, -1,
  /* DiagSubGroup185 */ 186, 187, -1,
  /* DiagSubGroup191 */ 79, 659, -1,
  /* DiagSubGroup196 */ 86, 191, 311, 80, 340, 483, 443, 440, 518, 626, 646, 670, 863, 907, -1,
  /* DiagSubGroup197 */ 646, -1,
  /* DiagSubGroup199 */ 200, 245, 22, 198, -1,
  /* DiagSubGroup202 */ 2, -1,
  /* DiagSubGroup210 */ 211, 212, 213, 215, 217, 216, 827, -1,
  /* DiagSubGroup230 */ 229, 227, -1,
  /* DiagSubGroup233 */ 235, 236, 237, 239, 240, 242, 246, 247, 248, 249, 250, 251, 238, 253, 254, 263, 265, 257, 262, 266, 267, 268, 269, 264, 255, -1,
  /* DiagSubGroup240 */ 243, -1,
  /* DiagSubGroup241 */ 242, -1,
  /* DiagSubGroup242 */ 244, -1,
  /* DiagSubGroup260 */ 261, -1,
  /* DiagSubGroup269 */ 102, -1,
  /* DiagSubGroup275 */ 276, -1,
  /* DiagSubGroup280 */ 282, 281, -1,
  /* DiagSubGroup283 */ 284, -1,
  /* DiagSubGroup296 */ 247, -1,
  /* DiagSubGroup298 */ 631, -1,
  /* DiagSubGroup307 */ 332, -1,
  /* DiagSubGroup308 */ 310, 248, -1,
  /* DiagSubGroup309 */ 249, -1,
  /* DiagSubGroup311 */ 312, 313, 309, -1,
  /* DiagSubGroup312 */ 250, -1,
  /* DiagSubGroup313 */ 251, -1,
  /* DiagSubGroup328 */ 240, 584, 430, 475, 844, 587, 862, 1038, 1024, 648, 649, 304, 906, 373, 168, -1,
  /* DiagSubGroup330 */ 150, 106, -1,
  /* DiagSubGroup331 */ 304, -1,
  /* DiagSubGroup340 */ 342, 343, -1,
  /* DiagSubGroup345 */ 346, 360, 632, 354, 359, 348, 347, 351, 356, -1,
  /* DiagSubGroup351 */ 352, -1,
  /* DiagSubGroup356 */ 357, -1,
  /* DiagSubGroup361 */ 350, 354, 359, -1,
  /* DiagSubGroup362 */ 351, 356, -1,
  /* DiagSubGroup366 */ 365, -1,
  /* DiagSubGroup372 */ 925, -1,
  /* DiagSubGroup374 */ 111, 116, 125, 130, -1,
  /* DiagSubGroup379 */ 380, 381, 383, 384, 385, 386, 387, 388, 389, 391, 1059, 392, 393, 394, 395, 396, 398, 399, 400, 401, 402, 804, 403, 404, 406, 407, 408, 409, 1072, 410, 411, -1,
  /* DiagSubGroup404 */ 405, -1,
  /* DiagSubGroup411 */ 1050, -1,
  /* DiagSubGroup421 */ 418, -1,
  /* DiagSubGroup429 */ 427, 428, -1,
  /* DiagSubGroup430 */ 431, -1,
  /* DiagSubGroup432 */ 441, 442, -1,
  /* DiagSubGroup437 */ 438, -1,
  /* DiagSubGroup440 */ 444, 693, -1,
  /* DiagSubGroup443 */ 861, 694, -1,
  /* DiagSubGroup444 */ 434, -1,
  /* DiagSubGroup460 */ 461, 455, -1,
  /* DiagSubGroup466 */ 468, 628, -1,
  /* DiagSubGroup472 */ 253, -1,
  /* DiagSubGroup484 */ 483, -1,
  /* DiagSubGroup486 */ 487, -1,
  /* DiagSubGroup489 */ 426, -1,
  /* DiagSubGroup520 */ 151, -1,
  /* DiagSubGroup523 */ 108, -1,
  /* DiagSubGroup524 */ 344, 797, -1,
  /* DiagSubGroup537 */ 541, 546, 558, 544, 554, 564, 538, 570, 550, 571, 561, 562, 569, 552, 549, 545, 567, 563, 548, 556, 555, 553, 540, 543, 572, 539, 542, 547, 565, 559, 566, 469, 560, -1,
  /* DiagSubGroup567 */ 568, -1,
  /* DiagSubGroup584 */ 582, -1,
  /* DiagSubGroup609 */ 48, 88, 174, 180, 230, 345, 344, 364, 432, 473, 485, 578, 579, 610, 614, 799, 813, 829, 840, 843, 868, 869, 909, 927, 954, 983, 992, 1022, 1064, 675, 663, 666, 721, 783, 170, 326, 1048, -1,
  /* DiagSubGroup610 */ 743, 808, 828, 843, -1,
  /* DiagSubGroup611 */ 558, -1,
  /* DiagSubGroup617 */ 109, -1,
  /* DiagSubGroup623 */ 118, -1,
  /* DiagSubGroup625 */ 862, 196, 519, -1,
  /* DiagSubGroup628 */ 627, -1,
  /* DiagSubGroup640 */ 641, -1,
  /* DiagSubGroup648 */ 400, -1,
  /* DiagSubGroup651 */ 652, -1,
  /* DiagSubGroup662 */ 689, -1,
  /* DiagSubGroup669 */ 695, -1,
  /* DiagSubGroup689 */ 690, -1,
  /* DiagSubGroup692 */ 694, 693, 659, 932, -1,
  /* DiagSubGroup701 */ 878, -1,
  /* DiagSubGroup705 */ 879, 707, 709, 711, 710, 706, 708, 712, -1,
  /* DiagSubGroup711 */ 710, -1,
  /* DiagSubGroup722 */ 475, -1,
  /* DiagSubGroup726 */ 727, -1,
  /* DiagSubGroup728 */ 729, -1,
  /* DiagSubGroup730 */ 522, 521, 82, 84, 858, 720, 731, 214, -1,
  /* DiagSubGroup732 */ 977, -1,
  /* DiagSubGroup739 */ 78, 159, 154, 106, 108, 111, 112, 116, 125, 128, 339, 374, 381, 383, 385, 386, 387, 388, 391, 392, 393, 395, 396, 398, 399, 400, 402, 403, 404, 408, 410, 511, 523, 541, 542, 544, 547, 549, 554, 555, 563, 719, 1072, 285, 513, 508, 231, 449, 301, 305, 654, 65, 972, 824, 184, 618, -1,
  /* DiagSubGroup741 */ 263, 525, 94, 823, 336, -1,
  /* DiagSubGroup744 */ 402, -1,
  /* DiagSubGroup749 */ 1061, -1,
  /* DiagSubGroup750 */ 749, 1062, -1,
  /* DiagSubGroup757 */ 758, -1,
  /* DiagSubGroup760 */ 992, 429, 755, 757, -1,
  /* DiagSubGroup762 */ 761, 146, -1,
  /* DiagSubGroup764 */ 763, -1,
  /* DiagSubGroup766 */ 765, -1,
  /* DiagSubGroup768 */ 767, -1,
  /* DiagSubGroup770 */ 769, -1,
  /* DiagSubGroup771 */ 769, -1,
  /* DiagSubGroup772 */ 770, -1,
  /* DiagSubGroup774 */ 773, -1,
  /* DiagSubGroup775 */ 1050, -1,
  /* DiagSubGroup776 */ 775, -1,
  /* DiagSubGroup777 */ 775, -1,
  /* DiagSubGroup778 */ 776, -1,
  /* DiagSubGroup780 */ 779, -1,
  /* DiagSubGroup797 */ 799, 798, -1,
  /* DiagSubGroup810 */ 265, -1,
  /* DiagSubGroup813 */ 814, 815, -1,
  /* DiagSubGroup818 */ 820, -1,
  /* DiagSubGroup819 */ 820, 821, 1047, 817, -1,
  /* DiagSubGroup822 */ 104, -1,
  /* DiagSubGroup825 */ 827, -1,
  /* DiagSubGroup829 */ 830, 826, -1,
  /* DiagSubGroup838 */ 839, -1,
  /* DiagSubGroup840 */ 842, 841, -1,
  /* DiagSubGroup846 */ 1009, -1,
  /* DiagSubGroup848 */ 852, 853, -1,
  /* DiagSubGroup849 */ 848, 851, 854, 850, -1,
  /* DiagSubGroup851 */ 852, -1,
  /* DiagSubGroup881 */ 880, -1,
  /* DiagSubGroup884 */ 406, -1,
  /* DiagSubGroup901 */ 753, -1,
  /* DiagSubGroup903 */ 258, -1,
  /* DiagSubGroup916 */ 872, 295, 640, 534, 915, -1,
  /* DiagSubGroup927 */ 928, 934, 933, 926, 936, 932, 931, -1,
  /* DiagSubGroup928 */ 930, -1,
  /* DiagSubGroup929 */ 955, 940, -1,
  /* DiagSubGroup945 */ 947, 946, 950, 951, -1,
  /* DiagSubGroup951 */ 952, -1,
  /* DiagSubGroup955 */ 935, 939, 937, 938, -1,
  /* DiagSubGroup976 */ 754, -1,
  /* DiagSubGroup977 */ 978, -1,
  /* DiagSubGroup983 */ 876, 888, 984, -1,
  /* DiagSubGroup995 */ 153, -1,
  /* DiagSubGroup999 */ 1004, 1002, 1003, -1,
  /* DiagSubGroup1000 */ 999, 1001, 1005, -1,
  /* DiagSubGroup1006 */ 1007, 1008, -1,
  /* DiagSubGroup1022 */ 1023, 1030, 1032, 1039, 1033, 1034, 1043, 1044, 1025, 1040, -1,
  /* DiagSubGroup1030 */ 996, -1,
  /* DiagSubGroup1035 */ 1034, -1,
  /* DiagSubGroup1037 */ 997, -1,
  /* DiagSubGroup1042 */ 996, -1,
  /* DiagSubGroup1043 */ 1027, 1041, 976, -1,
  /* DiagSubGroup1044 */ 1028, -1,
  /* DiagSubGroup1054 */ 1053, -1,
  /* DiagSubGroup1057 */ 1059, -1,
  /* DiagSubGroup1058 */ 1060, -1,
  /* DiagSubGroup1059 */ 1058, -1,
  /* DiagSubGroup1062 */ 1061, -1,
  /* DiagSubGroup1068 */ 269, -1,
  /* DiagSubGroup1069 */ 1068, -1,
};


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverlength-strings"
#endif
static constexpr char DiagGroupNamesStorage[] = 
  "\0"
  "#pragma-messages\0"
  "#warnings\0"
  "CFString-literal\0"
  "CL4\0"
  "IndependentClass-attribute\0"
  "NSObject-attribute\0"
  "aarch64-sme-attributes\0"
  "abi\0"
  "absolute-value\0"
  "abstract-final-class\0"
  "abstract-vbase-init\0"
  "address\0"
  "address-of-packed-member\0"
  "address-of-temporary\0"
  "aggregate-return\0"
  "aix-compat\0"
  "alias-template-in-declaration-name\0"
  "align-mismatch\0"
  "all\0"
  "alloca\0"
  "alloca-with-align-alignof\0"
  "always-inline-coroutine\0"
  "ambiguous-delete\0"
  "ambiguous-ellipsis\0"
  "ambiguous-macro\0"
  "ambiguous-member-template\0"
  "ambiguous-reversed-operator\0"
  "analyzer-incompatible-plugin\0"
  "android-unversioned-fallback\0"
  "anon-enum-enum-conversion\0"
  "anonymous-pack-parens\0"
  "apinotes\0"
  "arc\0"
  "arc-bridge-casts-disallowed-in-nonarc\0"
  "arc-maybe-repeated-use-of-weak\0"
  "arc-non-pod-memaccess\0"
  "arc-performSelector-leaks\0"
  "arc-repeated-use-of-weak\0"
  "arc-retain-cycles\0"
  "arc-unsafe-retained-assign\0"
  "argument-outside-range\0"
  "argument-undefined-behaviour\0"
  "arm-interrupt-vfp-clobber\0"
  "array-bounds\0"
  "array-bounds-pointer-arithmetic\0"
  "array-compare\0"
  "array-compare-cxx26\0"
  "array-parameter\0"
  "asm\0"
  "asm-operand-widths\0"
  "assign-enum\0"
  "assume\0"
  "at-protocol\0"
  "atimport-in-framework-header\0"
  "atomic-access\0"
  "atomic-alignment\0"
  "atomic-implicit-seq-cst\0"
  "atomic-memory-ordering\0"
  "atomic-properties\0"
  "atomic-property-with-user-defined-accessor\0"
  "attribute-packed-for-bitfield\0"
  "attribute-statement\0"
  "attribute-warning\0"
  "attributes\0"
  "auto-decl-extensions\0"
  "auto-disable-vptr-sanitizer\0"
  "auto-import\0"
  "auto-storage-class\0"
  "auto-var-id\0"
  "availability\0"
  "avr-rtlib-linking-quirks\0"
  "backend-plugin\0"
  "backslash-newline-escape\0"
  "bad-function-cast\0"
  "binary-literal\0"
  "bind-to-temporary-copy\0"
  "binding-in-condition\0"
  "bit-int-extension\0"
  "bitfield-constant-conversion\0"
  "bitfield-enum-conversion\0"
  "bitfield-width\0"
  "bitwise-conditional-parentheses\0"
  "bitwise-instead-of-logical\0"
  "bitwise-op-parentheses\0"
  "block-capture-autoreleasing\0"
  "bool-conversion\0"
  "bool-conversions\0"
  "bool-operation\0"
  "bounds-safety-counted-by-elt-type-unknown-size\0"
  "braced-scalar-init\0"
  "branch-protection\0"
  "bridge-cast\0"
  "builtin-assume-aligned-alignment\0"
  "builtin-macro-redefined\0"
  "builtin-memcpy-chk-size\0"
  "builtin-requires-header\0"
  "c++-compat\0"
  "c++0x-compat\0"
  "c++0x-extensions\0"
  "c++0x-narrowing\0"
  "c++11-compat\0"
  "c++11-compat-deprecated-writable-strings\0"
  "c++11-compat-pedantic\0"
  "c++11-compat-reserved-user-defined-literal\0"
  "c++11-extensions\0"
  "c++11-extra-semi\0"
  "c++11-inline-namespace\0"
  "c++11-long-long\0"
  "c++11-narrowing\0"
  "c++11-narrowing-const-reference\0"
  "c++14-attribute-extensions\0"
  "c++14-binary-literal\0"
  "c++14-compat\0"
  "c++14-compat-pedantic\0"
  "c++14-extensions\0"
  "c++17-attribute-extensions\0"
  "c++17-compat\0"
  "c++17-compat-mangling\0"
  "c++17-compat-pedantic\0"
  "c++17-extensions\0"
  "c++1y-extensions\0"
  "c++1z-compat\0"
  "c++1z-compat-mangling\0"
  "c++1z-extensions\0"
  "c++20-attribute-extensions\0"
  "c++20-compat\0"
  "c++20-compat-pedantic\0"
  "c++20-designator\0"
  "c++20-extensions\0"
  "c++23-attribute-extensions\0"
  "c++23-compat\0"
  "c++23-extensions\0"
  "c++23-lambda-attributes\0"
  "c++26-extensions\0"
  "c++2a-compat\0"
  "c++2a-compat-pedantic\0"
  "c++2a-extensions\0"
  "c++2b-extensions\0"
  "c++2c-compat\0"
  "c++2c-extensions\0"
  "c++98-c++11-c++14-c++17-compat\0"
  "c++98-c++11-c++14-c++17-compat-pedantic\0"
  "c++98-c++11-c++14-compat\0"
  "c++98-c++11-c++14-compat-pedantic\0"
  "c++98-c++11-compat\0"
  "c++98-c++11-compat-binary-literal\0"
  "c++98-c++11-compat-pedantic\0"
  "c++98-compat\0"
  "c++98-compat-bind-to-temporary-copy\0"
  "c++98-compat-extra-semi\0"
  "c++98-compat-local-type-template-args\0"
  "c++98-compat-pedantic\0"
  "c++98-compat-unnamed-type-template-args\0"
  "c11-extensions\0"
  "c23-compat\0"
  "c23-extensions\0"
  "c2x-compat\0"
  "c2x-extensions\0"
  "c2y-extensions\0"
  "c99-compat\0"
  "c99-designator\0"
  "c99-extensions\0"
  "call-to-pure-virtual-from-ctor-dtor\0"
  "called-once-parameter\0"
  "cast-align\0"
  "cast-calling-convention\0"
  "cast-function-type\0"
  "cast-function-type-mismatch\0"
  "cast-function-type-strict\0"
  "cast-of-sel-type\0"
  "cast-qual\0"
  "cast-qual-unrelated\0"
  "char-align\0"
  "char-subscripts\0"
  "clang-cl-pch\0"
  "class-conversion\0"
  "class-varargs\0"
  "cmse-union-leak\0"
  "comma\0"
  "comment\0"
  "comments\0"
  "compare-distinct-pointer-types\0"
  "completion-handler\0"
  "complex-component-init\0"
  "compound-token-split\0"
  "compound-token-split-by-macro\0"
  "compound-token-split-by-space\0"
  "conditional-type-mismatch\0"
  "conditional-uninitialized\0"
  "config-macros\0"
  "constant-conversion\0"
  "constant-evaluated\0"
  "constant-logical-operand\0"
  "constexpr-not-const\0"
  "consumed\0"
  "conversion\0"
  "conversion-null\0"
  "coro-non-aligned-allocation-function\0"
  "coroutine\0"
  "coroutine-missing-unhandled-exception\0"
  "covered-switch-default\0"
  "cpp\0"
  "cstring-format-directive\0"
  "ctad-maybe-unsupported\0"
  "ctor-dtor-privacy\0"
  "ctu\0"
  "cuda-compat\0"
  "custom-atomic-properties\0"
  "cxx-attribute-extension\0"
  "dangling\0"
  "dangling-assignment\0"
  "dangling-assignment-gsl\0"
  "dangling-capture\0"
  "dangling-else\0"
  "dangling-field\0"
  "dangling-gsl\0"
  "dangling-initializer-list\0"
  "darwin-sdk-settings\0"
  "date-time\0"
  "dealloc-in-category\0"
  "debug-compression-unavailable\0"
  "declaration-after-statement\0"
  "decls-in-multiple-modules\0"
  "defaulted-function-deleted\0"
  "delayed-template-parsing-in-cxx20\0"
  "delegating-ctor-cycles\0"
  "delete-abstract-non-virtual-dtor\0"
  "delete-incomplete\0"
  "delete-non-abstract-non-virtual-dtor\0"
  "delete-non-virtual-dtor\0"
  "delimited-escape-sequence-extension\0"
  "deprecate-lax-vec-conv-all\0"
  "deprecated\0"
  "deprecated-altivec-src-compat\0"
  "deprecated-anon-enum-enum-conversion\0"
  "deprecated-array-compare\0"
  "deprecated-attributes\0"
  "deprecated-builtins\0"
  "deprecated-comma-subscript\0"
  "deprecated-copy\0"
  "deprecated-copy-dtor\0"
  "deprecated-copy-with-dtor\0"
  "deprecated-copy-with-user-provided-copy\0"
  "deprecated-copy-with-user-provided-dtor\0"
  "deprecated-coroutine\0"
  "deprecated-declarations\0"
  "deprecated-dynamic-exception-spec\0"
  "deprecated-enum-compare\0"
  "deprecated-enum-compare-conditional\0"
  "deprecated-enum-enum-conversion\0"
  "deprecated-enum-float-conversion\0"
  "deprecated-implementations\0"
  "deprecated-increment-bool\0"
  "deprecated-literal-operator\0"
  "deprecated-missing-comma-variadic-parameter\0"
  "deprecated-module-dot-map\0"
  "deprecated-no-relaxed-template-template-args\0"
  "deprecated-non-prototype\0"
  "deprecated-objc-isa-usage\0"
  "deprecated-objc-pointer-introspection\0"
  "deprecated-objc-pointer-introspection-performSelector\0"
  "deprecated-ofast\0"
  "deprecated-pragma\0"
  "deprecated-redundant-constexpr-static-def\0"
  "deprecated-register\0"
  "deprecated-this-capture\0"
  "deprecated-type\0"
  "deprecated-volatile\0"
  "deprecated-writable-strings\0"
  "direct-ivar-access\0"
  "disabled-macro-expansion\0"
  "disabled-optimization\0"
  "discard-qual\0"
  "distributed-object-modifiers\0"
  "div-by-zero\0"
  "division-by-zero\0"
  "dll-attribute-on-redeclaration\0"
  "dllexport-explicit-instantiation-decl\0"
  "dllimport-static-field-def\0"
  "documentation\0"
  "documentation-deprecated-sync\0"
  "documentation-html\0"
  "documentation-pedantic\0"
  "documentation-unknown-command\0"
  "dollar-in-identifier-extension\0"
  "double-promotion\0"
  "dtor-name\0"
  "dtor-typedef\0"
  "duplicate-decl-specifier\0"
  "duplicate-enum\0"
  "duplicate-method-arg\0"
  "duplicate-method-match\0"
  "duplicate-protocol\0"
  "dxil-validation\0"
  "dynamic-class-memaccess\0"
  "dynamic-exception-spec\0"
  "eager-load-cxx-named-modules\0"
  "effc++\0"
  "elaborated-enum-base\0"
  "elaborated-enum-class\0"
  "embedded-directive\0"
  "empty-body\0"
  "empty-decomposition\0"
  "empty-init-stmt\0"
  "empty-translation-unit\0"
  "encode-type\0"
  "endif-labels\0"
  "enum-compare\0"
  "enum-compare-conditional\0"
  "enum-compare-switch\0"
  "enum-conversion\0"
  "enum-enum-conversion\0"
  "enum-float-conversion\0"
  "enum-too-large\0"
  "exceptions\0"
  "excess-initializers\0"
  "excessive-regsave\0"
  "exit-time-destructors\0"
  "expansion-to-defined\0"
  "experimental-header-units\0"
  "experimental-option\0"
  "explicit-initialize-call\0"
  "explicit-ownership-type\0"
  "explicit-specialization-storage-class\0"
  "export-unnamed\0"
  "extern-c-compat\0"
  "extern-initializer\0"
  "extra\0"
  "extra-qualification\0"
  "extra-semi\0"
  "extra-semi-stmt\0"
  "extra-tokens\0"
  "extractapi-misuse\0"
  "extraneous-template-head\0"
  "final-dtor-non-final-class\0"
  "final-macro\0"
  "fixed-point-overflow\0"
  "flag-enum\0"
  "flexible-array-extensions\0"
  "float-conversion\0"
  "float-equal\0"
  "float-overflow-conversion\0"
  "float-zero-conversion\0"
  "for-loop-analysis\0"
  "format\0"
  "format-extra-args\0"
  "format-insufficient-args\0"
  "format-invalid-specifier\0"
  "format-non-iso\0"
  "format-nonliteral\0"
  "format-overflow\0"
  "format-overflow-non-kprintf\0"
  "format-pedantic\0"
  "format-security\0"
  "format-signedness\0"
  "format-truncation\0"
  "format-truncation-non-kprintf\0"
  "format-type-confusion\0"
  "format-y2k\0"
  "format-zero-length\0"
  "format=2\0"
  "fortify-source\0"
  "four-char-constants\0"
  "frame-address\0"
  "frame-larger-than\0"
  "frame-larger-than=\0"
  "framework-include-private-from-public\0"
  "free-nonheap-object\0"
  "friend-enum\0"
  "function-def-in-objc-container\0"
  "function-effects\0"
  "function-multiversion\0"
  "fuse-ld-path\0"
  "future-attribute-extensions\0"
  "future-compat\0"
  "gcc-compat\0"
  "global-constructors\0"
  "global-isel\0"
  "gnu\0"
  "gnu-alignof-expression\0"
  "gnu-anonymous-struct\0"
  "gnu-array-member-paren-init\0"
  "gnu-auto-type\0"
  "gnu-binary-literal\0"
  "gnu-case-range\0"
  "gnu-complex-integer\0"
  "gnu-compound-literal-initializer\0"
  "gnu-conditional-omitted-operand\0"
  "gnu-designator\0"
  "gnu-empty-initializer\0"
  "gnu-empty-struct\0"
  "gnu-flexible-array-initializer\0"
  "gnu-flexible-array-union-member\0"
  "gnu-folding-constant\0"
  "gnu-imaginary-constant\0"
  "gnu-include-next\0"
  "gnu-inline-cpp-without-extern\0"
  "gnu-label-as-value\0"
  "gnu-line-marker\0"
  "gnu-null-pointer-arithmetic\0"
  "gnu-offsetof-extensions\0"
  "gnu-pointer-arith\0"
  "gnu-redeclared-enum\0"
  "gnu-statement-expression\0"
  "gnu-statement-expression-from-macro-expansion\0"
  "gnu-static-float-init\0"
  "gnu-string-literal-operator-template\0"
  "gnu-union-cast\0"
  "gnu-variable-sized-type-not-at-end\0"
  "gnu-zero-line-directive\0"
  "gnu-zero-variadic-macro-arguments\0"
  "gpu-maybe-wrong-side\0"
  "header-guard\0"
  "header-hygiene\0"
  "higher-precision-for-complex-division\0"
  "hip-omp-target-directives\0"
  "hip-only\0"
  "hlsl-202y-extensions\0"
  "hlsl-availability\0"
  "hlsl-dxc-compatability\0"
  "hlsl-extensions\0"
  "idiomatic-parentheses\0"
  "ignored-attributes\0"
  "ignored-availability-without-sdk-settings\0"
  "ignored-gch\0"
  "ignored-optimization-argument\0"
  "ignored-pragma-intrinsic\0"
  "ignored-pragma-optimize\0"
  "ignored-pragmas\0"
  "ignored-qualifiers\0"
  "ignored-reference-qualifiers\0"
  "implicit\0"
  "implicit-atomic-properties\0"
  "implicit-const-int-float-conversion\0"
  "implicit-conversion-floating-point-to-bool\0"
  "implicit-exception-spec-mismatch\0"
  "implicit-fallthrough\0"
  "implicit-fallthrough-per-function\0"
  "implicit-fixed-point-conversion\0"
  "implicit-float-conversion\0"
  "implicit-function-declaration\0"
  "implicit-int\0"
  "implicit-int-conversion\0"
  "implicit-int-float-conversion\0"
  "implicit-retain-self\0"
  "implicitly-unsigned-literal\0"
  "import\0"
  "import-implementation-partition-unit-in-interface-unit\0"
  "import-preprocessor-directive-pedantic\0"
  "inaccessible-base\0"
  "include-angled-in-module-purview\0"
  "include-next-absolute-path\0"
  "include-next-outside-header\0"
  "incompatible-exception-spec\0"
  "incompatible-function-pointer-types\0"
  "incompatible-function-pointer-types-strict\0"
  "incompatible-library-redeclaration\0"
  "incompatible-ms-pragma-section\0"
  "incompatible-ms-struct\0"
  "incompatible-pointer-types\0"
  "incompatible-pointer-types-discards-qualifiers\0"
  "incompatible-property-type\0"
  "incompatible-sysroot\0"
  "incomplete-framework-module-declaration\0"
  "incomplete-implementation\0"
  "incomplete-module\0"
  "incomplete-setjmp-declaration\0"
  "incomplete-umbrella\0"
  "inconsistent-dllimport\0"
  "inconsistent-missing-destructor-override\0"
  "inconsistent-missing-override\0"
  "increment-bool\0"
  "infinite-recursion\0"
  "init-self\0"
  "initializer-overrides\0"
  "injected-class-name\0"
  "inline\0"
  "inline-asm\0"
  "inline-namespace-reopened-noninline\0"
  "inline-new-delete\0"
  "installapi-violation\0"
  "instantiation-after-specialization\0"
  "int-conversion\0"
  "int-conversions\0"
  "int-in-bool-context\0"
  "int-to-pointer-cast\0"
  "int-to-void-pointer-cast\0"
  "integer-overflow\0"
  "invalid-command-line-argument\0"
  "invalid-constexpr\0"
  "invalid-feature-combination\0"
  "invalid-gnu-asm-cast\0"
  "invalid-iboutlet\0"
  "invalid-initializer-from-system-header\0"
  "invalid-ios-deployment-target\0"
  "invalid-no-builtin-names\0"
  "invalid-noreturn\0"
  "invalid-offsetof\0"
  "invalid-or-nonexistent-directory\0"
  "invalid-partial-specialization\0"
  "invalid-pch\0"
  "invalid-pp-token\0"
  "invalid-source-encoding\0"
  "invalid-specialization\0"
  "invalid-static-assert-message\0"
  "invalid-token-paste\0"
  "invalid-unevaluated-string\0"
  "invalid-utf8\0"
  "jump-seh-finally\0"
  "keyword-compat\0"
  "keyword-macro\0"
  "knr-promoted-parameter\0"
  "language-extension-token\0"
  "large-by-value-copy\0"
  "legacy-constant-register-binding\0"
  "liblto\0"
  "linker-warnings\0"
  "literal-conversion\0"
  "literal-range\0"
  "local-type-template-args\0"
  "logical-not-parentheses\0"
  "logical-op-parentheses\0"
  "long-long\0"
  "loop-analysis\0"
  "macro-redefined\0"
  "main\0"
  "main-return-type\0"
  "malformed-warning-check\0"
  "many-braces-around-scalar-init\0"
  "math-errno-enabled-with-veclib\0"
  "mathematical-notation-identifier-extension\0"
  "max-tokens\0"
  "max-unsigned-zero\0"
  "memset-transposed-args\0"
  "memsize-comparison\0"
  "method-signatures\0"
  "microsoft\0"
  "microsoft-abstract\0"
  "microsoft-anon-tag\0"
  "microsoft-cast\0"
  "microsoft-charize\0"
  "microsoft-comment-paste\0"
  "microsoft-const-init\0"
  "microsoft-cpp-macro\0"
  "microsoft-default-arg-redefinition\0"
  "microsoft-drectve-section\0"
  "microsoft-end-of-file\0"
  "microsoft-enum-forward-reference\0"
  "microsoft-enum-value\0"
  "microsoft-exception-spec\0"
  "microsoft-exists\0"
  "microsoft-explicit-constructor-call\0"
  "microsoft-extra-qualification\0"
  "microsoft-fixed-enum\0"
  "microsoft-flexible-array\0"
  "microsoft-goto\0"
  "microsoft-inaccessible-base\0"
  "microsoft-include\0"
  "microsoft-init-from-predefined\0"
  "microsoft-inline-on-non-function\0"
  "microsoft-mutable-reference\0"
  "microsoft-pure-definition\0"
  "microsoft-redeclare-static\0"
  "microsoft-sealed\0"
  "microsoft-static-assert\0"
  "microsoft-string-literal-from-predefined\0"
  "microsoft-template\0"
  "microsoft-template-shadow\0"
  "microsoft-union-member-reference\0"
  "microsoft-unqualified-friend\0"
  "microsoft-using-decl\0"
  "microsoft-void-pseudo-dtor\0"
  "misexpect\0"
  "misleading-indentation\0"
  "mismatched-new-delete\0"
  "mismatched-parameter-types\0"
  "mismatched-return-types\0"
  "mismatched-tags\0"
  "missing-braces\0"
  "missing-constinit\0"
  "missing-declarations\0"
  "missing-designated-field-initializers\0"
  "missing-exception-spec\0"
  "missing-field-initializers\0"
  "missing-format-attribute\0"
  "missing-include-dirs\0"
  "missing-method-return-type\0"
  "missing-multilib\0"
  "missing-noescape\0"
  "missing-noreturn\0"
  "missing-prototype-for-cc\0"
  "missing-prototypes\0"
  "missing-selector-name\0"
  "missing-sysroot\0"
  "missing-template-arg-list-after-template-kw\0"
  "missing-variable-declarations\0"
  "misspelled-assumption\0"
  "mix-packoffset\0"
  "module-build\0"
  "module-conflict\0"
  "module-file-config-mismatch\0"
  "module-file-extension\0"
  "module-import\0"
  "module-import-in-extern-c\0"
  "module-include-translation\0"
  "module-lock\0"
  "modules-ambiguous-internal-linkage\0"
  "modules-import-nested-redundant\0"
  "most\0"
  "move\0"
  "msvc-include\0"
  "msvc-not-found\0"
  "multi-gpu\0"
  "multichar\0"
  "multiple-move-vbase\0"
  "nan-infinity-disabled\0"
  "narrowing\0"
  "nested-anon-types\0"
  "nested-externs\0"
  "new-returns-null\0"
  "newline-eof\0"
  "noderef\0"
  "noexcept-type\0"
  "non-c-typedef-for-linkage\0"
  "non-gcc\0"
  "non-literal-null-conversion\0"
  "non-modular-include-in-framework-module\0"
  "non-modular-include-in-module\0"
  "non-pod-varargs\0"
  "non-power-of-two-alignment\0"
  "non-virtual-dtor\0"
  "nonnull\0"
  "nonportable-cfstrings\0"
  "nonportable-include-path\0"
  "nonportable-private-apinotes-path\0"
  "nonportable-private-system-apinotes-path\0"
  "nonportable-sycl\0"
  "nonportable-system-include-path\0"
  "nonportable-vector-initialization\0"
  "nontrivial-memaccess\0"
  "nontrivial-memcall\0"
  "nsconsumed-mismatch\0"
  "nsreturns-mismatch\0"
  "null-arithmetic\0"
  "null-character\0"
  "null-conversion\0"
  "null-dereference\0"
  "null-pointer-arithmetic\0"
  "null-pointer-subtraction\0"
  "nullability\0"
  "nullability-completeness\0"
  "nullability-completeness-on-arrays\0"
  "nullability-declspec\0"
  "nullability-extension\0"
  "nullability-inferred-on-nested-type\0"
  "nullable-to-nonnull-conversion\0"
  "nvcc-compat\0"
  "objc-autosynthesis-property-ivar-name-match\0"
  "objc-bool-constant-conversion\0"
  "objc-boxing\0"
  "objc-circular-container\0"
  "objc-cocoa-api\0"
  "objc-designated-initializers\0"
  "objc-dictionary-duplicate-keys\0"
  "objc-duplicate-category-definition\0"
  "objc-flexible-array\0"
  "objc-forward-class-redefinition\0"
  "objc-interface-ivars\0"
  "objc-literal-compare\0"
  "objc-literal-conversion\0"
  "objc-macro-redefinition\0"
  "objc-messaging-id\0"
  "objc-method-access\0"
  "objc-missing-property-synthesis\0"
  "objc-missing-super-calls\0"
  "objc-multiple-method-names\0"
  "objc-noncopy-retain-block-property\0"
  "objc-nonunified-exceptions\0"
  "objc-property-assign-on-object-type\0"
  "objc-property-implementation\0"
  "objc-property-implicit-mismatch\0"
  "objc-property-matches-cocoa-ownership-rule\0"
  "objc-property-no-attribute\0"
  "objc-property-synthesis\0"
  "objc-protocol-method-implementation\0"
  "objc-protocol-property-synthesis\0"
  "objc-protocol-qualifiers\0"
  "objc-readonly-with-setter-property\0"
  "objc-redundant-api-use\0"
  "objc-redundant-literal-use\0"
  "objc-root-class\0"
  "objc-signed-char-bool\0"
  "objc-signed-char-bool-implicit-float-conversion\0"
  "objc-signed-char-bool-implicit-int-conversion\0"
  "objc-string-compare\0"
  "objc-string-concatenation\0"
  "objc-unsafe-perform-selector\0"
  "odr\0"
  "old-style-cast\0"
  "old-style-definition\0"
  "openacc\0"
  "openacc-deprecated-clause-alias\0"
  "openacc-self-if-potential-conflict\0"
  "opencl-unsupported-rgba\0"
  "openmp\0"
  "openmp-51-extensions\0"
  "openmp-clauses\0"
  "openmp-extensions\0"
  "openmp-loop-form\0"
  "openmp-mapping\0"
  "openmp-target\0"
  "openmp-target-exception\0"
  "option-ignored\0"
  "ordered-compare-function-pointers\0"
  "out-of-line-declaration\0"
  "out-of-scope-function\0"
  "over-aligned\0"
  "overflow\0"
  "overlength-strings\0"
  "overloaded-shift-op-parentheses\0"
  "overloaded-virtual\0"
  "override-init\0"
  "override-module\0"
  "overriding-method-mismatch\0"
  "overriding-option\0"
  "packed\0"
  "packed-non-pod\0"
  "padded\0"
  "padded-bitfield\0"
  "parentheses\0"
  "parentheses-equality\0"
  "partial-availability\0"
  "pass\0"
  "pass-analysis\0"
  "pass-failed\0"
  "pass-missed\0"
  "pch-date-time\0"
  "pch-vfs-diff\0"
  "pedantic\0"
  "pedantic-core-features\0"
  "pedantic-macros\0"
  "perf-constraint-implies-noexcept\0"
  "pessimizing-move\0"
  "pointer-arith\0"
  "pointer-bool-conversion\0"
  "pointer-compare\0"
  "pointer-integer-compare\0"
  "pointer-sign\0"
  "pointer-to-enum-cast\0"
  "pointer-to-int-cast\0"
  "pointer-type-mismatch\0"
  "poison-system-directories\0"
  "potentially-direct-selector\0"
  "potentially-evaluated-expression\0"
  "pragma-clang-attribute\0"
  "pragma-once-outside-header\0"
  "pragma-pack\0"
  "pragma-pack-suspicious-include\0"
  "pragma-system-header-outside-header\0"
  "pragmas\0"
  "pre-c++14-compat\0"
  "pre-c++14-compat-pedantic\0"
  "pre-c++17-compat\0"
  "pre-c++17-compat-pedantic\0"
  "pre-c++20-compat\0"
  "pre-c++20-compat-pedantic\0"
  "pre-c++23-compat\0"
  "pre-c++23-compat-pedantic\0"
  "pre-c++26-compat\0"
  "pre-c++26-compat-pedantic\0"
  "pre-c++2c-compat\0"
  "pre-c++2c-compat-pedantic\0"
  "pre-c11-compat\0"
  "pre-c11-compat-pedantic\0"
  "pre-c23-compat\0"
  "pre-c23-compat-pedantic\0"
  "pre-c2x-compat\0"
  "pre-c2x-compat-pedantic\0"
  "pre-c2y-compat\0"
  "pre-c2y-compat-pedantic\0"
  "pre-openmp-51-compat\0"
  "predefined-identifier-outside-function\0"
  "private-extern\0"
  "private-header\0"
  "private-module\0"
  "profile-instr-missing\0"
  "profile-instr-out-of-date\0"
  "profile-instr-unprofiled\0"
  "property-access-dot-syntax\0"
  "property-attribute-mismatch\0"
  "protocol\0"
  "protocol-property-synthesis-ambiguity\0"
  "psabi\0"
  "ptrauth-null-pointers\0"
  "qualified-void-return-type\0"
  "quoted-include-in-framework-header\0"
  "range-loop-analysis\0"
  "range-loop-bind-reference\0"
  "range-loop-construct\0"
  "read-only-types\0"
  "readonly-iboutlet-property\0"
  "receiver-expr\0"
  "receiver-forward-class\0"
  "redeclared-class-member\0"
  "redundant-attribute\0"
  "redundant-consteval-if\0"
  "redundant-decls\0"
  "redundant-move\0"
  "redundant-parens\0"
  "register\0"
  "reinterpret-base-class\0"
  "remark-backend-plugin\0"
  "reorder\0"
  "reorder-ctor\0"
  "reorder-init-list\0"
  "requires-super-attribute\0"
  "reserved-attribute-identifier\0"
  "reserved-id-macro\0"
  "reserved-identifier\0"
  "reserved-macro-identifier\0"
  "reserved-module-identifier\0"
  "reserved-user-defined-literal\0"
  "restrict-expansion\0"
  "retained-language-linkage\0"
  "return-local-addr\0"
  "return-mismatch\0"
  "return-stack-address\0"
  "return-std-move\0"
  "return-type\0"
  "return-type-c-linkage\0"
  "rewrite-not-bool\0"
  "round-trip-cc1-args\0"
  "rtti\0"
  "sanitize-address\0"
  "sarif-format-unstable\0"
  "search-path-usage\0"
  "section\0"
  "selector\0"
  "selector-type-mismatch\0"
  "self-assign\0"
  "self-assign-field\0"
  "self-assign-overloaded\0"
  "self-move\0"
  "semicolon-before-method-body\0"
  "sentinel\0"
  "sequence-point\0"
  "serialized-diagnostics\0"
  "shadow\0"
  "shadow-all\0"
  "shadow-field\0"
  "shadow-field-in-constructor\0"
  "shadow-field-in-constructor-modified\0"
  "shadow-ivar\0"
  "shadow-uncaptured-local\0"
  "shift-count-negative\0"
  "shift-count-overflow\0"
  "shift-negative-value\0"
  "shift-op-parentheses\0"
  "shift-overflow\0"
  "shift-sign-overflow\0"
  "shorten-64-to-32\0"
  "sign-compare\0"
  "sign-conversion\0"
  "sign-promo\0"
  "signed-enum-bitfield\0"
  "signed-unsigned-wchar\0"
  "single-bit-bitfield-constant-conversion\0"
  "sizeof-array-argument\0"
  "sizeof-array-decay\0"
  "sizeof-array-div\0"
  "sizeof-pointer-div\0"
  "sizeof-pointer-memaccess\0"
  "slash-u-filename\0"
  "slh-asm-goto\0"
  "sloc-usage\0"
  "sometimes-uninitialized\0"
  "source-mgr\0"
  "source-uses-openacc\0"
  "source-uses-openmp\0"
  "spir-compat\0"
  "spirv-compat\0"
  "stack-exhausted\0"
  "stack-protector\0"
  "static-float-init\0"
  "static-in-inline\0"
  "static-inline-explicit-instantiation\0"
  "static-local-in-inline\0"
  "static-self-init\0"
  "stdlibcxx-not-found\0"
  "strict-aliasing\0"
  "strict-aliasing=0\0"
  "strict-aliasing=1\0"
  "strict-aliasing=2\0"
  "strict-overflow\0"
  "strict-overflow=0\0"
  "strict-overflow=1\0"
  "strict-overflow=2\0"
  "strict-overflow=3\0"
  "strict-overflow=4\0"
  "strict-overflow=5\0"
  "strict-potentially-direct-selector\0"
  "strict-primary-template-shadow\0"
  "strict-prototypes\0"
  "strict-selector-match\0"
  "string-compare\0"
  "string-concatenation\0"
  "string-conversion\0"
  "string-plus-char\0"
  "string-plus-int\0"
  "strlcpy-strlcat-size\0"
  "strncat-size\0"
  "suggest-destructor-override\0"
  "suggest-override\0"
  "super-class-method-mismatch\0"
  "suspicious-bzero\0"
  "suspicious-memaccess\0"
  "swift-name-attribute\0"
  "switch\0"
  "switch-bool\0"
  "switch-default\0"
  "switch-enum\0"
  "sync-alignment\0"
  "sync-fetch-and-nand-semantics-changed\0"
  "synth\0"
  "target-clones-mixed-specifiers\0"
  "tautological-bitwise-compare\0"
  "tautological-compare\0"
  "tautological-constant-compare\0"
  "tautological-constant-in-range-compare\0"
  "tautological-constant-out-of-range-compare\0"
  "tautological-negation-compare\0"
  "tautological-objc-bool-compare\0"
  "tautological-overlap-compare\0"
  "tautological-pointer-compare\0"
  "tautological-type-limit-compare\0"
  "tautological-undefined-compare\0"
  "tautological-unsigned-char-zero-compare\0"
  "tautological-unsigned-enum-zero-compare\0"
  "tautological-unsigned-zero-compare\0"
  "tautological-value-range-compare\0"
  "tcb-enforcement\0"
  "template-in-declaration-name\0"
  "tentative-definition-array\0"
  "tentative-definition-incomplete-type\0"
  "thread-safety\0"
  "thread-safety-analysis\0"
  "thread-safety-attributes\0"
  "thread-safety-beta\0"
  "thread-safety-negative\0"
  "thread-safety-precise\0"
  "thread-safety-reference\0"
  "thread-safety-reference-return\0"
  "thread-safety-verbose\0"
  "trigraphs\0"
  "type-limits\0"
  "type-safety\0"
  "typedef-redefinition\0"
  "typename-missing\0"
  "unable-to-open-stats-file\0"
  "unaligned-access\0"
  "unaligned-qualifier-implicit-cast\0"
  "unavailable-declarations\0"
  "undeclared-selector\0"
  "undef\0"
  "undef-prefix\0"
  "undefined-arm-za\0"
  "undefined-arm-zt0\0"
  "undefined-bool-conversion\0"
  "undefined-func-template\0"
  "undefined-inline\0"
  "undefined-internal\0"
  "undefined-internal-type\0"
  "undefined-reinterpret-cast\0"
  "undefined-var-template\0"
  "underaligned-exception-object\0"
  "unevaluated-expression\0"
  "unguarded-availability\0"
  "unguarded-availability-new\0"
  "unicode\0"
  "unicode-homoglyph\0"
  "unicode-whitespace\0"
  "unicode-zero-width\0"
  "uninitialized\0"
  "uninitialized-const-reference\0"
  "uninitialized-explicit-init\0"
  "unknown-argument\0"
  "unknown-assumption\0"
  "unknown-attributes\0"
  "unknown-cuda-version\0"
  "unknown-directives\0"
  "unknown-escape-sequence\0"
  "unknown-pragmas\0"
  "unknown-sanitizers\0"
  "unknown-warning-option\0"
  "unnamed-type-template-args\0"
  "unneeded-internal-declaration\0"
  "unneeded-member-function\0"
  "unqualified-std-cast-call\0"
  "unreachable-code\0"
  "unreachable-code-aggressive\0"
  "unreachable-code-break\0"
  "unreachable-code-fallthrough\0"
  "unreachable-code-generic-assoc\0"
  "unreachable-code-loop-increment\0"
  "unreachable-code-return\0"
  "unsafe-buffer-usage\0"
  "unsafe-buffer-usage-in-container\0"
  "unsafe-buffer-usage-in-libc-call\0"
  "unsequenced\0"
  "unsupported-abi\0"
  "unsupported-abs\0"
  "unsupported-availability-guard\0"
  "unsupported-cb\0"
  "unsupported-dll-base-class-template\0"
  "unsupported-floating-point-opt\0"
  "unsupported-friend\0"
  "unsupported-gpopt\0"
  "unsupported-nan\0"
  "unsupported-target-opt\0"
  "unsupported-visibility\0"
  "unusable-partial-specialization\0"
  "unused\0"
  "unused-argument\0"
  "unused-but-set-parameter\0"
  "unused-but-set-variable\0"
  "unused-command-line-argument\0"
  "unused-comparison\0"
  "unused-const-variable\0"
  "unused-exception-parameter\0"
  "unused-function\0"
  "unused-getter-return-value\0"
  "unused-label\0"
  "unused-lambda-capture\0"
  "unused-local-typedef\0"
  "unused-local-typedefs\0"
  "unused-macros\0"
  "unused-member-function\0"
  "unused-parameter\0"
  "unused-private-field\0"
  "unused-property-ivar\0"
  "unused-result\0"
  "unused-template\0"
  "unused-value\0"
  "unused-variable\0"
  "unused-volatile-lvalue\0"
  "used-but-marked-unused\0"
  "user-defined-literals\0"
  "user-defined-warnings\0"
  "varargs\0"
  "variadic-macro-arguments-omitted\0"
  "variadic-macros\0"
  "vec-elem-size\0"
  "vector-conversion\0"
  "vector-conversions\0"
  "vexing-parse\0"
  "visibility\0"
  "vla\0"
  "vla-cxx-extension\0"
  "vla-extension\0"
  "vla-extension-static-assert\0"
  "void-pointer-to-enum-cast\0"
  "void-pointer-to-int-cast\0"
  "void-ptr-dereference\0"
  "volatile-register-var\0"
  "wasm-exception-spec\0"
  "weak-template-vtables\0"
  "weak-vtables\0"
  "writable-strings\0"
  "write-strings\0"
  "xor-used-as-pow\0"
  "zero-as-null-pointer-constant\0"
  "zero-length-array\0"
  ;
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

static constexpr llvm::StringTable DiagGroupNames =
    DiagGroupNamesStorage;

#endif // GET_DIAG_ARRAYS


#ifdef DIAG_ENTRY
DIAG_ENTRY(anonymous_59 /*  */, 0, 0, /* DiagSubGroup0 */ 1, R"()")
DIAG_ENTRY(PoundPragmaMessage /* #pragma-messages */, 1, /* DiagArray1 */ 1, 0, R"()")
DIAG_ENTRY(PoundWarning /* #warnings */, 18, /* DiagArray2 */ 3, 0, R"()")
DIAG_ENTRY(anonymous_293 /* CFString-literal */, 28, /* DiagArray3 */ 5, 0, R"()")
DIAG_ENTRY(anonymous_58 /* CL4 */, 45, 0, /* DiagSubGroup4 */ 3, R"()")
DIAG_ENTRY(IndependentClassAttribute /* IndependentClass-attribute */, 49, /* DiagArray5 */ 7, 0, R"()")
DIAG_ENTRY(NSobjectAttribute /* NSObject-attribute */, 76, /* DiagArray6 */ 10, 0, R"()")
DIAG_ENTRY(AArch64SMEAttributes /* aarch64-sme-attributes */, 95, /* DiagArray7 */ 12, 0, R"()")
DIAG_ENTRY(anonymous_0 /* abi */, 118, 0, 0, R"()")
DIAG_ENTRY(AbsoluteValue /* absolute-value */, 122, /* DiagArray9 */ 16, 0, R"()")
DIAG_ENTRY(AbstractFinalClass /* abstract-final-class */, 137, /* DiagArray10 */ 21, 0, R"()")
DIAG_ENTRY(anonymous_285 /* abstract-vbase-init */, 158, /* DiagArray11 */ 23, 0, R"()")
DIAG_ENTRY(anonymous_57 /* address */, 178, 0, /* DiagSubGroup12 */ 6, R"()")
DIAG_ENTRY(anonymous_259 /* address-of-packed-member */, 186, /* DiagArray13 */ 25, 0, R"()")
DIAG_ENTRY(AddressOfTemporary /* address-of-temporary */, 211, /* DiagArray14 */ 27, 0, R"()")
DIAG_ENTRY(anonymous_2 /* aggregate-return */, 232, 0, 0, R"()")
DIAG_ENTRY(AIXCompat /* aix-compat */, 249, /* DiagArray16 */ 29, 0, R"()")
DIAG_ENTRY(anonymous_271 /* alias-template-in-declaration-name */, 260, /* DiagArray17 */ 32, 0, R"()")
DIAG_ENTRY(anonymous_260 /* align-mismatch */, 295, /* DiagArray18 */ 34, 0, R"()")
DIAG_ENTRY(All /* all */, 310, 0, /* DiagSubGroup19 */ 10, R"()")
DIAG_ENTRY(anonymous_200 /* alloca */, 314, /* DiagArray20 */ 36, 0, R"()")
DIAG_ENTRY(anonymous_201 /* alloca-with-align-alignof */, 321, /* DiagArray21 */ 38, 0, R"()")
DIAG_ENTRY(AlwaysInlineCoroutine /* always-inline-coroutine */, 347, /* DiagArray22 */ 40, 0, R"()")
DIAG_ENTRY(anonymous_266 /* ambiguous-delete */, 371, /* DiagArray23 */ 42, 0, R"()")
DIAG_ENTRY(anonymous_149 /* ambiguous-ellipsis */, 388, /* DiagArray24 */ 44, 0, R"()")
DIAG_ENTRY(AmbiguousMacro /* ambiguous-macro */, 407, /* DiagArray25 */ 46, 0, R"()")
DIAG_ENTRY(AmbigMemberTemplate /* ambiguous-member-template */, 423, /* DiagArray26 */ 48, 0, R"()")
DIAG_ENTRY(anonymous_221 /* ambiguous-reversed-operator */, 449, /* DiagArray27 */ 50, 0, R"()")
DIAG_ENTRY(anonymous_106 /* analyzer-incompatible-plugin */, 477, /* DiagArray28 */ 52, 0, R"()")
DIAG_ENTRY(anonymous_103 /* android-unversioned-fallback */, 506, /* DiagArray29 */ 54, 0, R"()")
DIAG_ENTRY(AnonEnumEnumConversion /* anon-enum-enum-conversion */, 535, /* DiagArray30 */ 56, /* DiagSubGroup30 */ 19, R"()")
DIAG_ENTRY(anonymous_150 /* anonymous-pack-parens */, 561, /* DiagArray31 */ 58, 0, R"()")
DIAG_ENTRY(anonymous_88 /* apinotes */, 583, /* DiagArray32 */ 60, 0, R"()")
DIAG_ENTRY(AutomaticReferenceCounting /* arc */, 592, 0, /* DiagSubGroup33 */ 21, R"()")
DIAG_ENTRY(anonymous_148 /* arc-bridge-casts-disallowed-in-nonarc */, 596, /* DiagArray34 */ 62, 0, R"()")
DIAG_ENTRY(ARCRepeatedUseOfWeakMaybe /* arc-maybe-repeated-use-of-weak */, 634, /* DiagArray35 */ 64, 0, R"()")
DIAG_ENTRY(ARCNonPodMemAccess /* arc-non-pod-memaccess */, 665, /* DiagArray36 */ 66, 0, R"()")
DIAG_ENTRY(anonymous_181 /* arc-performSelector-leaks */, 687, /* DiagArray37 */ 68, 0, R"()")
DIAG_ENTRY(ARCRepeatedUseOfWeak /* arc-repeated-use-of-weak */, 713, /* DiagArray38 */ 70, /* DiagSubGroup38 */ 25, R"()")
DIAG_ENTRY(ARCRetainCycles /* arc-retain-cycles */, 738, /* DiagArray39 */ 72, 0, R"()")
DIAG_ENTRY(ARCUnsafeRetainedAssign /* arc-unsafe-retained-assign */, 756, /* DiagArray40 */ 74, 0, R"()")
DIAG_ENTRY(anonymous_295 /* argument-outside-range */, 783, /* DiagArray41 */ 78, 0, R"()")
DIAG_ENTRY(anonymous_296 /* argument-undefined-behaviour */, 806, /* DiagArray42 */ 80, 0, R"()")
DIAG_ENTRY(anonymous_159 /* arm-interrupt-vfp-clobber */, 835, /* DiagArray43 */ 82, 0, R"()")
DIAG_ENTRY(ArrayBounds /* array-bounds */, 861, /* DiagArray44 */ 84, 0, R"()")
DIAG_ENTRY(ArrayBoundsPointerArithmetic /* array-bounds-pointer-arithmetic */, 874, /* DiagArray45 */ 90, 0, R"()")
DIAG_ENTRY(anonymous_291 /* array-compare */, 906, /* DiagArray46 */ 93, 0, R"()")
DIAG_ENTRY(anonymous_292 /* array-compare-cxx26 */, 920, /* DiagArray47 */ 95, 0, R"()")
DIAG_ENTRY(ArrayParameter /* array-parameter */, 940, /* DiagArray48 */ 97, 0, R"()")
DIAG_ENTRY(ASM /* asm */, 956, 0, /* DiagSubGroup49 */ 27, R"()")
DIAG_ENTRY(ASMOperandWidths /* asm-operand-widths */, 960, /* DiagArray50 */ 99, 0, R"()")
DIAG_ENTRY(anonymous_294 /* assign-enum */, 979, /* DiagArray51 */ 101, 0, R"()")
DIAG_ENTRY(anonymous_168 /* assume */, 991, /* DiagArray52 */ 103, 0, R"()")
DIAG_ENTRY(anonymous_56 /* at-protocol */, 998, 0, 0, R"()")
DIAG_ENTRY(FrameworkHdrAtImport /* atimport-in-framework-header */, 1010, /* DiagArray54 */ 105, 0, R"()")
DIAG_ENTRY(anonymous_247 /* atomic-access */, 1039, /* DiagArray55 */ 107, 0, R"()")
DIAG_ENTRY(AtomicAlignment /* atomic-alignment */, 1053, /* DiagArray56 */ 109, 0, R"()")
DIAG_ENTRY(anonymous_276 /* atomic-implicit-seq-cst */, 1070, /* DiagArray57 */ 112, 0, R"()")
DIAG_ENTRY(anonymous_275 /* atomic-memory-ordering */, 1094, /* DiagArray58 */ 114, 0, R"()")
DIAG_ENTRY(AtomicProperties /* atomic-properties */, 1117, 0, /* DiagSubGroup59 */ 29, R"()")
DIAG_ENTRY(anonymous_176 /* atomic-property-with-user-defined-accessor */, 1135, /* DiagArray60 */ 116, 0, R"()")
DIAG_ENTRY(anonymous_217 /* attribute-packed-for-bitfield */, 1178, /* DiagArray61 */ 118, 0, R"()")
DIAG_ENTRY(HLSLAttributeStatement /* attribute-statement */, 1208, /* DiagArray62 */ 120, 0, R"()")
DIAG_ENTRY(BackendWarningAttributes /* attribute-warning */, 1228, /* DiagArray63 */ 122, 0, R"()")
DIAG_ENTRY(Attributes /* attributes */, 1246, 0, /* DiagSubGroup64 */ 32, R"()")
DIAG_ENTRY(anonymous_157 /* auto-decl-extensions */, 1257, /* DiagArray65 */ 125, 0, R"()")
DIAG_ENTRY(AutoDisableVptrSanitizer /* auto-disable-vptr-sanitizer */, 1278, /* DiagArray66 */ 127, 0, R"()")
DIAG_ENTRY(anonymous_3 /* auto-import */, 1306, 0, 0, R"()")
DIAG_ENTRY(anonymous_146 /* auto-storage-class */, 1318, /* DiagArray68 */ 129, 0, R"()")
DIAG_ENTRY(anonymous_195 /* auto-var-id */, 1337, /* DiagArray69 */ 131, 0, R"()")
DIAG_ENTRY(Availability /* availability */, 1349, /* DiagArray70 */ 133, 0, R"()")
DIAG_ENTRY(AVRRtlibLinkingQuirks /* avr-rtlib-linking-quirks */, 1362, /* DiagArray71 */ 145, 0, R"()")
DIAG_ENTRY(BackendPlugin /* backend-plugin */, 1387, /* DiagArray72 */ 151, 0, R"()")
DIAG_ENTRY(anonymous_110 /* backslash-newline-escape */, 1402, /* DiagArray73 */ 154, 0, R"()")
DIAG_ENTRY(BadFunctionCast /* bad-function-cast */, 1427, /* DiagArray74 */ 156, 0, R"()")
DIAG_ENTRY(BinaryLiteral /* binary-literal */, 1445, 0, /* DiagSubGroup75 */ 35, R"()")
DIAG_ENTRY(BindToTemporaryCopy /* bind-to-temporary-copy */, 1460, /* DiagArray76 */ 158, /* DiagSubGroup76 */ 38, R"()")
DIAG_ENTRY(anonymous_163 /* binding-in-condition */, 1483, /* DiagArray77 */ 161, 0, R"()")
DIAG_ENTRY(BitIntExtension /* bit-int-extension */, 1504, /* DiagArray78 */ 163, 0, R"()")
DIAG_ENTRY(BitFieldConstantConversion /* bitfield-constant-conversion */, 1522, /* DiagArray79 */ 166, /* DiagSubGroup79 */ 40, R"()")
DIAG_ENTRY(BitFieldEnumConversion /* bitfield-enum-conversion */, 1551, /* DiagArray80 */ 168, 0, R"()")
DIAG_ENTRY(BitFieldWidth /* bitfield-width */, 1576, /* DiagArray81 */ 172, 0, R"()")
DIAG_ENTRY(BitwiseConditionalParentheses /* bitwise-conditional-parentheses */, 1591, /* DiagArray82 */ 174, 0, R"()")
DIAG_ENTRY(BitwiseInsteadOfLogical /* bitwise-instead-of-logical */, 1623, /* DiagArray83 */ 176, 0, R"()")
DIAG_ENTRY(BitwiseOpParentheses /* bitwise-op-parentheses */, 1650, /* DiagArray84 */ 178, 0, R"()")
DIAG_ENTRY(BlockCaptureAutoReleasing /* block-capture-autoreleasing */, 1673, /* DiagArray85 */ 180, 0, R"()")
DIAG_ENTRY(BoolConversion /* bool-conversion */, 1701, /* DiagArray86 */ 182, /* DiagSubGroup86 */ 42, R"()")
DIAG_ENTRY(anonymous_64 /* bool-conversions */, 1717, 0, /* DiagSubGroup87 */ 45, R"()")
DIAG_ENTRY(BoolOperation /* bool-operation */, 1734, /* DiagArray88 */ 184, /* DiagSubGroup88 */ 47, R"()")
DIAG_ENTRY(BoundsSafetyCountedByEltTyUnknownSize /* bounds-safety-counted-by-elt-type-unknown-size */, 1749, /* DiagArray89 */ 186, 0, R"()")
DIAG_ENTRY(anonymous_241 /* braced-scalar-init */, 1796, /* DiagArray90 */ 188, 0, R"()")
DIAG_ENTRY(BranchProtection /* branch-protection */, 1815, /* DiagArray91 */ 190, 0, R"()")
DIAG_ENTRY(ObjCBridge /* bridge-cast */, 1833, /* DiagArray92 */ 195, 0, R"()")
DIAG_ENTRY(anonymous_203 /* builtin-assume-aligned-alignment */, 1845, /* DiagArray93 */ 198, 0, R"()")
DIAG_ENTRY(BuiltinMacroRedefined /* builtin-macro-redefined */, 1878, /* DiagArray94 */ 200, 0, R"()")
DIAG_ENTRY(anonymous_169 /* builtin-memcpy-chk-size */, 1902, /* DiagArray95 */ 203, 0, R"()")
DIAG_ENTRY(BuiltinRequiresHeader /* builtin-requires-header */, 1926, /* DiagArray96 */ 205, 0, R"()")
DIAG_ENTRY(CXXCompat /* c++-compat */, 1950, /* DiagArray97 */ 207, 0, R"()")
DIAG_ENTRY(anonymous_25 /* c++0x-compat */, 1961, 0, /* DiagSubGroup98 */ 49, R"()")
DIAG_ENTRY(anonymous_68 /* c++0x-extensions */, 1974, 0, /* DiagSubGroup99 */ 51, R"()")
DIAG_ENTRY(anonymous_23 /* c++0x-narrowing */, 1991, 0, /* DiagSubGroup100 */ 53, R"()")
DIAG_ENTRY(CXX11Compat /* c++11-compat */, 2007, /* DiagArray101 */ 209, /* DiagSubGroup101 */ 55, R"()")
DIAG_ENTRY(CXX11CompatDeprecatedWritableStr /* c++11-compat-deprecated-writable-strings */, 2020, /* DiagArray102 */ 220, 0, R"()")
DIAG_ENTRY(CXX11CompatPedantic /* c++11-compat-pedantic */, 2061, 0, /* DiagSubGroup103 */ 64, R"()")
DIAG_ENTRY(CXX11CompatReservedUserDefinedLiteral /* c++11-compat-reserved-user-defined-literal */, 2083, /* DiagArray104 */ 222, 0, R"()")
DIAG_ENTRY(CXX11 /* c++11-extensions */, 2126, /* DiagArray105 */ 224, /* DiagSubGroup105 */ 71, R"()")
DIAG_ENTRY(CXX11ExtraSemi /* c++11-extra-semi */, 2143, /* DiagArray106 */ 251, 0, R"()")
DIAG_ENTRY(CXX11InlineNamespace /* c++11-inline-namespace */, 2160, /* DiagArray107 */ 253, 0, R"()")
DIAG_ENTRY(CXX11LongLong /* c++11-long-long */, 2183, /* DiagArray108 */ 255, 0, R"()")
DIAG_ENTRY(CXX11Narrowing /* c++11-narrowing */, 2199, /* DiagArray109 */ 257, /* DiagSubGroup109 */ 75, R"()")
DIAG_ENTRY(CXX11NarrowingConstReference /* c++11-narrowing-const-reference */, 2215, /* DiagArray110 */ 265, 0, R"()")
DIAG_ENTRY(CXX14Attrs /* c++14-attribute-extensions */, 2247, /* DiagArray111 */ 269, 0, R"()")
DIAG_ENTRY(CXX14BinaryLiteral /* c++14-binary-literal */, 2274, /* DiagArray112 */ 271, 0, R"()")
DIAG_ENTRY(CXX14Compat /* c++14-compat */, 2295, 0, /* DiagSubGroup113 */ 77, R"()")
DIAG_ENTRY(CXX14CompatPedantic /* c++14-compat-pedantic */, 2308, 0, /* DiagSubGroup114 */ 82, R"()")
DIAG_ENTRY(CXX14 /* c++14-extensions */, 2330, /* DiagArray115 */ 273, /* DiagSubGroup115 */ 88, R"()")
DIAG_ENTRY(CXX17Attrs /* c++17-attribute-extensions */, 2347, /* DiagArray116 */ 281, 0, R"()")
DIAG_ENTRY(CXX17Compat /* c++17-compat */, 2374, 0, /* DiagSubGroup117 */ 91, R"()")
DIAG_ENTRY(CXX17CompatMangling /* c++17-compat-mangling */, 2387, /* DiagArray118 */ 283, 0, R"()")
DIAG_ENTRY(CXX17CompatPedantic /* c++17-compat-pedantic */, 2409, 0, /* DiagSubGroup119 */ 98, R"()")
DIAG_ENTRY(CXX17 /* c++17-extensions */, 2431, /* DiagArray120 */ 285, /* DiagSubGroup120 */ 103, R"()")
DIAG_ENTRY(anonymous_69 /* c++1y-extensions */, 2448, 0, /* DiagSubGroup121 */ 105, R"()")
DIAG_ENTRY(anonymous_26 /* c++1z-compat */, 2465, 0, /* DiagSubGroup122 */ 107, R"()")
DIAG_ENTRY(anonymous_12 /* c++1z-compat-mangling */, 2478, 0, /* DiagSubGroup123 */ 109, R"()")
DIAG_ENTRY(anonymous_70 /* c++1z-extensions */, 2500, 0, /* DiagSubGroup124 */ 111, R"()")
DIAG_ENTRY(CXX20Attrs /* c++20-attribute-extensions */, 2517, /* DiagArray125 */ 303, 0, R"()")
DIAG_ENTRY(CXX20Compat /* c++20-compat */, 2544, /* DiagArray126 */ 305, /* DiagSubGroup126 */ 113, R"()")
DIAG_ENTRY(CXX20CompatPedantic /* c++20-compat-pedantic */, 2557, 0, /* DiagSubGroup127 */ 116, R"()")
DIAG_ENTRY(CXX20Designator /* c++20-designator */, 2579, /* DiagArray128 */ 316, 0, R"()")
DIAG_ENTRY(CXX20 /* c++20-extensions */, 2596, /* DiagArray129 */ 318, /* DiagSubGroup129 */ 120, R"()")
DIAG_ENTRY(CXX23Attrs /* c++23-attribute-extensions */, 2613, /* DiagArray130 */ 340, 0, R"()")
DIAG_ENTRY(CXX23Compat /* c++23-compat */, 2640, 0, /* DiagSubGroup131 */ 124, R"()")
DIAG_ENTRY(CXX23 /* c++23-extensions */, 2653, /* DiagArray132 */ 342, /* DiagSubGroup132 */ 126, R"()")
DIAG_ENTRY(CXX23AttrsOnLambda /* c++23-lambda-attributes */, 2670, /* DiagArray133 */ 353, 0, R"()")
DIAG_ENTRY(CXX26 /* c++26-extensions */, 2694, /* DiagArray134 */ 355, 0, R"()")
DIAG_ENTRY(anonymous_27 /* c++2a-compat */, 2711, 0, /* DiagSubGroup135 */ 128, R"()")
DIAG_ENTRY(anonymous_28 /* c++2a-compat-pedantic */, 2724, 0, /* DiagSubGroup136 */ 130, R"()")
DIAG_ENTRY(anonymous_71 /* c++2a-extensions */, 2746, 0, /* DiagSubGroup137 */ 132, R"()")
DIAG_ENTRY(anonymous_72 /* c++2b-extensions */, 2763, 0, /* DiagSubGroup138 */ 134, R"()")
DIAG_ENTRY(CXX26Compat /* c++2c-compat */, 2780, 0, /* DiagSubGroup139 */ 136, R"()")
DIAG_ENTRY(anonymous_73 /* c++2c-extensions */, 2793, 0, /* DiagSubGroup140 */ 138, R"()")
DIAG_ENTRY(anonymous_19 /* c++98-c++11-c++14-c++17-compat */, 2810, 0, /* DiagSubGroup141 */ 140, R"()")
DIAG_ENTRY(anonymous_20 /* c++98-c++11-c++14-c++17-compat-pedantic */, 2841, 0, /* DiagSubGroup142 */ 142, R"()")
DIAG_ENTRY(anonymous_17 /* c++98-c++11-c++14-compat */, 2881, 0, /* DiagSubGroup143 */ 144, R"()")
DIAG_ENTRY(anonymous_18 /* c++98-c++11-c++14-compat-pedantic */, 2906, 0, /* DiagSubGroup144 */ 146, R"()")
DIAG_ENTRY(anonymous_15 /* c++98-c++11-compat */, 2940, 0, /* DiagSubGroup145 */ 148, R"()")
DIAG_ENTRY(CXXPre14CompatBinaryLiteral /* c++98-c++11-compat-binary-literal */, 2959, /* DiagArray146 */ 363, 0, R"()")
DIAG_ENTRY(anonymous_16 /* c++98-c++11-compat-pedantic */, 2993, 0, /* DiagSubGroup147 */ 150, R"()")
DIAG_ENTRY(CXX98Compat /* c++98-compat */, 3021, /* DiagArray148 */ 365, /* DiagSubGroup148 */ 152, R"()")
DIAG_ENTRY(CXX98CompatBindToTemporaryCopy /* c++98-compat-bind-to-temporary-copy */, 3034, /* DiagArray149 */ 424, 0, R"()")
DIAG_ENTRY(CXX98CompatExtraSemi /* c++98-compat-extra-semi */, 3070, /* DiagArray150 */ 426, 0, R"()")
DIAG_ENTRY(CXX98CompatLocalTypeTemplateArgs /* c++98-compat-local-type-template-args */, 3094, /* DiagArray151 */ 428, 0, R"()")
DIAG_ENTRY(CXX98CompatPedantic /* c++98-compat-pedantic */, 3132, /* DiagArray152 */ 430, /* DiagSubGroup152 */ 160, R"()")
DIAG_ENTRY(CXX98CompatUnnamedTypeTemplateArgs /* c++98-compat-unnamed-type-template-args */, 3154, /* DiagArray153 */ 440, 0, R"()")
DIAG_ENTRY(C11 /* c11-extensions */, 3194, /* DiagArray154 */ 442, 0, R"()")
DIAG_ENTRY(C23Compat /* c23-compat */, 3209, /* DiagArray155 */ 447, 0, R"()")
DIAG_ENTRY(C23 /* c23-extensions */, 3220, /* DiagArray156 */ 450, /* DiagSubGroup156 */ 169, R"()")
DIAG_ENTRY(anonymous_4 /* c2x-compat */, 3235, 0, /* DiagSubGroup157 */ 171, R"()")
DIAG_ENTRY(anonymous_74 /* c2x-extensions */, 3246, 0, /* DiagSubGroup158 */ 173, R"()")
DIAG_ENTRY(C2y /* c2y-extensions */, 3261, /* DiagArray159 */ 465, 0, R"()")
DIAG_ENTRY(C99Compat /* c99-compat */, 3276, /* DiagArray160 */ 471, 0, R"()")
DIAG_ENTRY(C99Designator /* c99-designator */, 3287, /* DiagArray161 */ 476, /* DiagSubGroup161 */ 175, R"()")
DIAG_ENTRY(C99 /* c99-extensions */, 3302, /* DiagArray162 */ 482, /* DiagSubGroup162 */ 177, R"()")
DIAG_ENTRY(PureVirtualCallFromCtorDtor /* call-to-pure-virtual-from-ctor-dtor */, 3317, /* DiagArray163 */ 493, 0, R"()")
DIAG_ENTRY(CalledOnceParameter /* called-once-parameter */, 3353, /* DiagArray164 */ 495, /* DiagSubGroup164 */ 179, R"()")
DIAG_ENTRY(CastAlign /* cast-align */, 3375, /* DiagArray165 */ 499, 0, R"()")
DIAG_ENTRY(anonymous_279 /* cast-calling-convention */, 3386, /* DiagArray166 */ 501, 0, R"()")
DIAG_ENTRY(CastFunctionType /* cast-function-type */, 3410, 0, /* DiagSubGroup167 */ 181, R"()")
DIAG_ENTRY(CastFunctionTypeMismatch /* cast-function-type-mismatch */, 3429, /* DiagArray168 */ 503, 0, R"()")
DIAG_ENTRY(CastFunctionTypeStrict /* cast-function-type-strict */, 3457, /* DiagArray169 */ 505, 0, R"()")
DIAG_ENTRY(SelTypeCast /* cast-of-sel-type */, 3483, /* DiagArray170 */ 507, 0, R"()")
DIAG_ENTRY(CastQual /* cast-qual */, 3500, /* DiagArray171 */ 509, 0, R"()")
DIAG_ENTRY(anonymous_264 /* cast-qual-unrelated */, 3510, /* DiagArray172 */ 512, 0, R"()")
DIAG_ENTRY(anonymous_5 /* char-align */, 3530, 0, 0, R"()")
DIAG_ENTRY(CharSubscript /* char-subscripts */, 3541, /* DiagArray174 */ 514, 0, R"()")
DIAG_ENTRY(ClangClPch /* clang-cl-pch */, 3557, /* DiagArray175 */ 517, 0, R"()")
DIAG_ENTRY(ClassConversion /* class-conversion */, 3570, /* DiagArray176 */ 522, 0, R"()")
DIAG_ENTRY(ClassVarargs /* class-varargs */, 3587, /* DiagArray177 */ 526, /* DiagSubGroup177 */ 184, R"()")
DIAG_ENTRY(anonymous_204 /* cmse-union-leak */, 3601, /* DiagArray178 */ 528, 0, R"()")
DIAG_ENTRY(anonymous_155 /* comma */, 3617, /* DiagArray179 */ 530, 0, R"()")
DIAG_ENTRY(Comment /* comment */, 3623, /* DiagArray180 */ 532, 0, R"()")
DIAG_ENTRY(anonymous_62 /* comments */, 3631, 0, /* DiagSubGroup181 */ 186, R"()")
DIAG_ENTRY(CompareDistinctPointerType /* compare-distinct-pointer-types */, 3640, /* DiagArray182 */ 537, 0, R"()")
DIAG_ENTRY(CompletionHandler /* completion-handler */, 3671, /* DiagArray183 */ 539, 0, R"()")
DIAG_ENTRY(anonymous_243 /* complex-component-init */, 3690, /* DiagArray184 */ 543, 0, R"()")
DIAG_ENTRY(CompoundTokenSplit /* compound-token-split */, 3713, 0, /* DiagSubGroup185 */ 188, R"()")
DIAG_ENTRY(CompoundTokenSplitByMacro /* compound-token-split-by-macro */, 3734, /* DiagArray186 */ 545, 0, R"()")
DIAG_ENTRY(CompoundTokenSplitBySpace /* compound-token-split-by-space */, 3764, /* DiagArray187 */ 547, 0, R"()")
DIAG_ENTRY(anonymous_281 /* conditional-type-mismatch */, 3794, /* DiagArray188 */ 549, 0, R"()")
DIAG_ENTRY(UninitializedMaybe /* conditional-uninitialized */, 3820, /* DiagArray189 */ 551, 0, R"()")
DIAG_ENTRY(ConfigMacros /* config-macros */, 3846, /* DiagArray190 */ 553, 0, R"()")
DIAG_ENTRY(ConstantConversion /* constant-conversion */, 3860, /* DiagArray191 */ 555, /* DiagSubGroup191 */ 191, R"()")
DIAG_ENTRY(anonymous_80 /* constant-evaluated */, 3880, /* DiagArray192 */ 557, 0, R"()")
DIAG_ENTRY(anonymous_254 /* constant-logical-operand */, 3899, /* DiagArray193 */ 559, 0, R"()")
DIAG_ENTRY(anonymous_193 /* constexpr-not-const */, 3924, /* DiagArray194 */ 561, 0, R"()")
DIAG_ENTRY(Consumed /* consumed */, 3944, /* DiagArray195 */ 563, 0, R"()")
DIAG_ENTRY(Conversion /* conversion */, 3953, /* DiagArray196 */ 572, /* DiagSubGroup196 */ 194, R"()")
DIAG_ENTRY(anonymous_63 /* conversion-null */, 3964, 0, /* DiagSubGroup197 */ 209, R"()")
DIAG_ENTRY(CoroNonAlignedAllocationFunction /* coro-non-aligned-allocation-function */, 3980, /* DiagArray198 */ 579, 0, R"()")
DIAG_ENTRY(Coroutine /* coroutine */, 4017, /* DiagArray199 */ 581, /* DiagSubGroup199 */ 211, R"()")
DIAG_ENTRY(CoroutineMissingUnhandledException /* coroutine-missing-unhandled-exception */, 4027, /* DiagArray200 */ 583, 0, R"()")
DIAG_ENTRY(CoveredSwitchDefault /* covered-switch-default */, 4065, /* DiagArray201 */ 585, 0, R"()")
DIAG_ENTRY(anonymous_61 /* cpp */, 4088, 0, /* DiagSubGroup202 */ 216, R"()")
DIAG_ENTRY(ObjCCStringFormat /* cstring-format-directive */, 4092, /* DiagArray203 */ 587, 0, R"()")
DIAG_ENTRY(CTADMaybeUnsupported /* ctad-maybe-unsupported */, 4117, /* DiagArray204 */ 589, 0, R"()")
DIAG_ENTRY(anonymous_6 /* ctor-dtor-privacy */, 4140, 0, 0, R"()")
DIAG_ENTRY(CrossTU /* ctu */, 4158, /* DiagArray206 */ 591, 0, R"()")
DIAG_ENTRY(CudaCompat /* cuda-compat */, 4162, /* DiagArray207 */ 593, 0, R"()")
DIAG_ENTRY(CustomAtomic /* custom-atomic-properties */, 4174, /* DiagArray208 */ 599, 0, R"()")
DIAG_ENTRY(anonymous_151 /* cxx-attribute-extension */, 4199, /* DiagArray209 */ 601, 0, R"()")
DIAG_ENTRY(Dangling /* dangling */, 4223, /* DiagArray210 */ 603, /* DiagSubGroup210 */ 218, R"()")
DIAG_ENTRY(DanglingAssignment /* dangling-assignment */, 4232, /* DiagArray211 */ 605, 0, R"()")
DIAG_ENTRY(DanglingAssignmentGsl /* dangling-assignment-gsl */, 4252, /* DiagArray212 */ 607, 0, R"()")
DIAG_ENTRY(DanglingCapture /* dangling-capture */, 4276, /* DiagArray213 */ 609, 0, R"()")
DIAG_ENTRY(DanglingElse /* dangling-else */, 4293, /* DiagArray214 */ 612, 0, R"()")
DIAG_ENTRY(DanglingField /* dangling-field */, 4307, /* DiagArray215 */ 614, 0, R"()")
DIAG_ENTRY(DanglingGsl /* dangling-gsl */, 4322, /* DiagArray216 */ 619, 0, R"()")
DIAG_ENTRY(DanglingInitializerList /* dangling-initializer-list */, 4335, /* DiagArray217 */ 622, 0, R"()")
DIAG_ENTRY(anonymous_99 /* darwin-sdk-settings */, 4361, /* DiagArray218 */ 624, 0, R"()")
DIAG_ENTRY(anonymous_136 /* date-time */, 4381, /* DiagArray219 */ 626, 0, R"()")
DIAG_ENTRY(DeallocInCategory /* dealloc-in-category */, 4391, /* DiagArray220 */ 628, 0, R"()")
DIAG_ENTRY(anonymous_95 /* debug-compression-unavailable */, 4411, /* DiagArray221 */ 630, 0, R"()")
DIAG_ENTRY(DeclarationAfterStatement /* declaration-after-statement */, 4441, /* DiagArray222 */ 632, 0, R"()")
DIAG_ENTRY(anonymous_312 /* decls-in-multiple-modules */, 4469, /* DiagArray223 */ 635, 0, R"()")
DIAG_ENTRY(DefaultedFunctionDeleted /* defaulted-function-deleted */, 4495, /* DiagArray224 */ 637, 0, R"()")
DIAG_ENTRY(anonymous_96 /* delayed-template-parsing-in-cxx20 */, 4522, /* DiagArray225 */ 640, 0, R"()")
DIAG_ENTRY(DelegatingCtorCycles /* delegating-ctor-cycles */, 4556, /* DiagArray226 */ 642, 0, R"()")
DIAG_ENTRY(DeleteAbstractNonVirtualDtor /* delete-abstract-non-virtual-dtor */, 4579, /* DiagArray227 */ 644, 0, R"()")
DIAG_ENTRY(DeleteIncomplete /* delete-incomplete */, 4612, /* DiagArray228 */ 646, 0, R"()")
DIAG_ENTRY(DeleteNonAbstractNonVirtualDtor /* delete-non-abstract-non-virtual-dtor */, 4630, /* DiagArray229 */ 649, 0, R"()")
DIAG_ENTRY(DeleteNonVirtualDtor /* delete-non-virtual-dtor */, 4667, 0, /* DiagSubGroup230 */ 226, R"()")
DIAG_ENTRY(anonymous_118 /* delimited-escape-sequence-extension */, 4691, /* DiagArray231 */ 651, 0, R"()")
DIAG_ENTRY(anonymous_268 /* deprecate-lax-vec-conv-all */, 4727, /* DiagArray232 */ 653, 0, R"()")
DIAG_ENTRY(Deprecated /* deprecated */, 4754, /* DiagArray233 */ 655, /* DiagSubGroup233 */ 229, R"()")
DIAG_ENTRY(anonymous_267 /* deprecated-altivec-src-compat */, 4765, /* DiagArray234 */ 665, 0, R"()")
DIAG_ENTRY(DeprecatedAnonEnumEnumConversion /* deprecated-anon-enum-enum-conversion */, 4795, /* DiagArray235 */ 667, 0, R"()")
DIAG_ENTRY(DeprecatedArrayCompare /* deprecated-array-compare */, 4832, /* DiagArray236 */ 669, 0, R"()")
DIAG_ENTRY(DeprecatedAttributes /* deprecated-attributes */, 4857, /* DiagArray237 */ 671, 0, R"()")
DIAG_ENTRY(DeprecatedBuiltins /* deprecated-builtins */, 4879, /* DiagArray238 */ 675, 0, R"()")
DIAG_ENTRY(DeprecatedCommaSubscript /* deprecated-comma-subscript */, 4899, /* DiagArray239 */ 677, 0, R"()")
DIAG_ENTRY(DeprecatedCopy /* deprecated-copy */, 4926, /* DiagArray240 */ 679, /* DiagSubGroup240 */ 255, R"()")
DIAG_ENTRY(anonymous_7 /* deprecated-copy-dtor */, 4942, 0, /* DiagSubGroup241 */ 257, R"()")
DIAG_ENTRY(DeprecatedCopyWithDtor /* deprecated-copy-with-dtor */, 4963, /* DiagArray242 */ 681, /* DiagSubGroup242 */ 259, R"()")
DIAG_ENTRY(DeprecatedCopyWithUserProvidedCopy /* deprecated-copy-with-user-provided-copy */, 4989, /* DiagArray243 */ 683, 0, R"()")
DIAG_ENTRY(DeprecatedCopyWithUserProvidedDtor /* deprecated-copy-with-user-provided-dtor */, 5029, /* DiagArray244 */ 685, 0, R"()")
DIAG_ENTRY(DeprecatedCoroutine /* deprecated-coroutine */, 5069, /* DiagArray245 */ 687, 0, R"()")
DIAG_ENTRY(DeprecatedDeclarations /* deprecated-declarations */, 5090, /* DiagArray246 */ 689, 0, R"()")
DIAG_ENTRY(DeprecatedDynamicExceptionSpec /* deprecated-dynamic-exception-spec */, 5114, /* DiagArray247 */ 696, 0, R"()")
DIAG_ENTRY(DeprecatedEnumCompare /* deprecated-enum-compare */, 5148, /* DiagArray248 */ 698, 0, R"()")
DIAG_ENTRY(DeprecatedEnumCompareConditional /* deprecated-enum-compare-conditional */, 5172, /* DiagArray249 */ 700, 0, R"()")
DIAG_ENTRY(DeprecatedEnumEnumConversion /* deprecated-enum-enum-conversion */, 5208, /* DiagArray250 */ 702, 0, R"()")
DIAG_ENTRY(DeprecatedEnumFloatConversion /* deprecated-enum-float-conversion */, 5240, /* DiagArray251 */ 704, 0, R"()")
DIAG_ENTRY(DeprecatedImplementations /* deprecated-implementations */, 5273, /* DiagArray252 */ 706, 0, R"()")
DIAG_ENTRY(DeprecatedIncrementBool /* deprecated-increment-bool */, 5300, /* DiagArray253 */ 709, 0, R"()")
DIAG_ENTRY(DeprecatedLiteralOperator /* deprecated-literal-operator */, 5326, /* DiagArray254 */ 711, 0, R"()")
DIAG_ENTRY(DeprecatedMissingCommaVariadicParam /* deprecated-missing-comma-variadic-parameter */, 5354, /* DiagArray255 */ 713, 0, R"()")
DIAG_ENTRY(DeprecatedModuleDotMap /* deprecated-module-dot-map */, 5398, /* DiagArray256 */ 715, 0, R"()")
DIAG_ENTRY(DeprecatedNoRelaxedTemplateTemplateArgs /* deprecated-no-relaxed-template-template-args */, 5424, /* DiagArray257 */ 717, 0, R"()")
DIAG_ENTRY(DeprecatedNonPrototype /* deprecated-non-prototype */, 5469, /* DiagArray258 */ 719, 0, R"()")
DIAG_ENTRY(DeprecatedObjCIsaUsage /* deprecated-objc-isa-usage */, 5494, /* DiagArray259 */ 722, 0, R"()")
DIAG_ENTRY(ObjCPointerIntrospect /* deprecated-objc-pointer-introspection */, 5520, /* DiagArray260 */ 725, /* DiagSubGroup260 */ 261, R"()")
DIAG_ENTRY(ObjCPointerIntrospectPerformSelector /* deprecated-objc-pointer-introspection-performSelector */, 5558, /* DiagArray261 */ 727, 0, R"()")
DIAG_ENTRY(DeprecatedOFast /* deprecated-ofast */, 5612, /* DiagArray262 */ 729, 0, R"()")
DIAG_ENTRY(DeprecatedPragma /* deprecated-pragma */, 5629, /* DiagArray263 */ 732, 0, R"()")
DIAG_ENTRY(DeprecatedRedundantConstexprStaticDef /* deprecated-redundant-constexpr-static-def */, 5647, /* DiagArray264 */ 734, 0, R"()")
DIAG_ENTRY(DeprecatedRegister /* deprecated-register */, 5689, /* DiagArray265 */ 736, 0, R"()")
DIAG_ENTRY(DeprecatedThisCapture /* deprecated-this-capture */, 5709, /* DiagArray266 */ 738, 0, R"()")
DIAG_ENTRY(DeprecatedType /* deprecated-type */, 5733, /* DiagArray267 */ 740, 0, R"()")
DIAG_ENTRY(DeprecatedVolatile /* deprecated-volatile */, 5749, /* DiagArray268 */ 742, 0, R"()")
DIAG_ENTRY(DeprecatedWritableStr /* deprecated-writable-strings */, 5769, 0, /* DiagSubGroup269 */ 263, R"()")
DIAG_ENTRY(anonymous_300 /* direct-ivar-access */, 5797, /* DiagArray270 */ 748, 0, R"()")
DIAG_ENTRY(anonymous_127 /* disabled-macro-expansion */, 5816, /* DiagArray271 */ 750, 0, R"()")
DIAG_ENTRY(anonymous_9 /* disabled-optimization */, 5841, 0, 0, R"()")
DIAG_ENTRY(anonymous_10 /* discard-qual */, 5863, 0, 0, R"()")
DIAG_ENTRY(DistributedObjectModifiers /* distributed-object-modifiers */, 5876, /* DiagArray274 */ 752, 0, R"()")
DIAG_ENTRY(anonymous_11 /* div-by-zero */, 5905, 0, /* DiagSubGroup275 */ 265, R"()")
DIAG_ENTRY(DivZero /* division-by-zero */, 5917, /* DiagArray276 */ 755, 0, R"()")
DIAG_ENTRY(anonymous_205 /* dll-attribute-on-redeclaration */, 5934, /* DiagArray277 */ 757, 0, R"()")
DIAG_ENTRY(DllexportExplicitInstantiationDecl /* dllexport-explicit-instantiation-decl */, 5965, /* DiagArray278 */ 759, 0, R"()")
DIAG_ENTRY(anonymous_206 /* dllimport-static-field-def */, 6003, /* DiagArray279 */ 761, 0, R"()")
DIAG_ENTRY(Documentation /* documentation */, 6030, /* DiagArray280 */ 763, /* DiagSubGroup280 */ 267, R"()")
DIAG_ENTRY(DocumentationDeprecatedSync /* documentation-deprecated-sync */, 6044, /* DiagArray281 */ 784, 0, R"()")
DIAG_ENTRY(DocumentationHTML /* documentation-html */, 6074, /* DiagArray282 */ 786, 0, R"()")
DIAG_ENTRY(DocumentationPedantic /* documentation-pedantic */, 6093, /* DiagArray283 */ 791, /* DiagSubGroup283 */ 270, R"()")
DIAG_ENTRY(DocumentationUnknownCommand /* documentation-unknown-command */, 6116, /* DiagArray284 */ 793, 0, R"()")
DIAG_ENTRY(anonymous_111 /* dollar-in-identifier-extension */, 6146, /* DiagArray285 */ 796, 0, R"()")
DIAG_ENTRY(DoublePromotion /* double-promotion */, 6177, /* DiagArray286 */ 798, 0, R"()")
DIAG_ENTRY(DtorName /* dtor-name */, 6194, /* DiagArray287 */ 800, 0, R"()")
DIAG_ENTRY(anonymous_192 /* dtor-typedef */, 6204, /* DiagArray288 */ 804, 0, R"()")
DIAG_ENTRY(DuplicateDeclSpecifier /* duplicate-decl-specifier */, 6217, /* DiagArray289 */ 806, 0, R"()")
DIAG_ENTRY(anonymous_154 /* duplicate-enum */, 6242, /* DiagArray290 */ 811, 0, R"()")
DIAG_ENTRY(DuplicateArgDecl /* duplicate-method-arg */, 6257, /* DiagArray291 */ 813, 0, R"()")
DIAG_ENTRY(MethodDuplicate /* duplicate-method-match */, 6278, /* DiagArray292 */ 815, 0, R"()")
DIAG_ENTRY(anonymous_171 /* duplicate-protocol */, 6301, /* DiagArray293 */ 817, 0, R"()")
DIAG_ENTRY(DXILValidation /* dxil-validation */, 6320, /* DiagArray294 */ 819, 0, R"()")
DIAG_ENTRY(DynamicClassMemaccess /* dynamic-class-memaccess */, 6336, /* DiagArray295 */ 821, 0, R"()")
DIAG_ENTRY(DynamicExceptionSpec /* dynamic-exception-spec */, 6360, /* DiagArray296 */ 823, /* DiagSubGroup296 */ 272, R"()")
DIAG_ENTRY(anonymous_108 /* eager-load-cxx-named-modules */, 6383, /* DiagArray297 */ 825, 0, R"()")
DIAG_ENTRY(anonymous_38 /* effc++ */, 6412, 0, /* DiagSubGroup298 */ 274, R"()")
DIAG_ENTRY(anonymous_142 /* elaborated-enum-base */, 6419, /* DiagArray299 */ 827, 0, R"()")
DIAG_ENTRY(anonymous_143 /* elaborated-enum-class */, 6440, /* DiagArray300 */ 829, 0, R"()")
DIAG_ENTRY(anonymous_133 /* embedded-directive */, 6462, /* DiagArray301 */ 831, 0, R"()")
DIAG_ENTRY(EmptyBody /* empty-body */, 6481, /* DiagArray302 */ 833, 0, R"()")
DIAG_ENTRY(anonymous_147 /* empty-decomposition */, 6492, /* DiagArray303 */ 839, 0, R"()")
DIAG_ENTRY(EmptyInitStatement /* empty-init-stmt */, 6512, /* DiagArray304 */ 841, 0, R"()")
DIAG_ENTRY(anonymous_140 /* empty-translation-unit */, 6528, /* DiagArray305 */ 843, 0, R"()")
DIAG_ENTRY(anonymous_209 /* encode-type */, 6551, /* DiagArray306 */ 845, 0, R"()")
DIAG_ENTRY(anonymous_60 /* endif-labels */, 6563, 0, /* DiagSubGroup307 */ 276, R"()")
DIAG_ENTRY(EnumCompare /* enum-compare */, 6576, /* DiagArray308 */ 847, /* DiagSubGroup308 */ 278, R"()")
DIAG_ENTRY(EnumCompareConditional /* enum-compare-conditional */, 6589, /* DiagArray309 */ 849, /* DiagSubGroup309 */ 281, R"()")
DIAG_ENTRY(EnumCompareSwitch /* enum-compare-switch */, 6614, /* DiagArray310 */ 851, 0, R"()")
DIAG_ENTRY(EnumConversion /* enum-conversion */, 6634, /* DiagArray311 */ 853, /* DiagSubGroup311 */ 283, R"()")
DIAG_ENTRY(EnumEnumConversion /* enum-enum-conversion */, 6650, /* DiagArray312 */ 855, /* DiagSubGroup312 */ 287, R"()")
DIAG_ENTRY(EnumFloatConversion /* enum-float-conversion */, 6671, /* DiagArray313 */ 858, /* DiagSubGroup313 */ 289, R"()")
DIAG_ENTRY(EnumTooLarge /* enum-too-large */, 6693, /* DiagArray314 */ 860, 0, R"()")
DIAG_ENTRY(Exceptions /* exceptions */, 6708, /* DiagArray315 */ 863, 0, R"()")
DIAG_ENTRY(ExcessInitializers /* excess-initializers */, 6719, /* DiagArray316 */ 867, 0, R"()")
DIAG_ENTRY(anonymous_158 /* excessive-regsave */, 6739, /* DiagArray317 */ 872, 0, R"()")
DIAG_ENTRY(ExitTimeDestructors /* exit-time-destructors */, 6757, /* DiagArray318 */ 874, 0, R"()")
DIAG_ENTRY(ExpansionToDefined /* expansion-to-defined */, 6779, /* DiagArray319 */ 876, 0, R"()")
DIAG_ENTRY(anonymous_304 /* experimental-header-units */, 6800, /* DiagArray320 */ 879, 0, R"()")
DIAG_ENTRY(ExperimentalOption /* experimental-option */, 6826, /* DiagArray321 */ 881, 0, R"()")
DIAG_ENTRY(ExplicitInitializeCall /* explicit-initialize-call */, 6846, /* DiagArray322 */ 883, 0, R"()")
DIAG_ENTRY(anonymous_245 /* explicit-ownership-type */, 6871, /* DiagArray323 */ 886, 0, R"()")
DIAG_ENTRY(ExplicitSpecializationStorageClass /* explicit-specialization-storage-class */, 6895, /* DiagArray324 */ 888, 0, R"()")
DIAG_ENTRY(ExportUnnamed /* export-unnamed */, 6933, 0, 0, R"()")
DIAG_ENTRY(ExternCCompat /* extern-c-compat */, 6948, /* DiagArray326 */ 890, 0, R"()")
DIAG_ENTRY(anonymous_240 /* extern-initializer */, 6964, /* DiagArray327 */ 892, 0, R"()")
DIAG_ENTRY(Extra /* extra */, 6983, 0, /* DiagSubGroup328 */ 291, R"()")
DIAG_ENTRY(anonymous_255 /* extra-qualification */, 6989, /* DiagArray329 */ 894, 0, R"()")
DIAG_ENTRY(ExtraSemi /* extra-semi */, 7009, /* DiagArray330 */ 896, /* DiagSubGroup330 */ 307, R"()")
DIAG_ENTRY(ExtraSemiStmt /* extra-semi-stmt */, 7020, /* DiagArray331 */ 899, /* DiagSubGroup331 */ 310, R"()")
DIAG_ENTRY(ExtraTokens /* extra-tokens */, 7036, /* DiagArray332 */ 901, 0, R"()")
DIAG_ENTRY(ExtractAPIMisuse /* extractapi-misuse */, 7049, /* DiagArray333 */ 904, 0, R"()")
DIAG_ENTRY(anonymous_226 /* extraneous-template-head */, 7067, /* DiagArray334 */ 906, 0, R"()")
DIAG_ENTRY(FinalDtorNonFinalClass /* final-dtor-non-final-class */, 7092, /* DiagArray335 */ 908, 0, R"()")
DIAG_ENTRY(FinalMacro /* final-macro */, 7119, /* DiagArray336 */ 910, 0, R"()")
DIAG_ENTRY(anonymous_79 /* fixed-point-overflow */, 7131, /* DiagArray337 */ 912, 0, R"()")
DIAG_ENTRY(FlagEnum /* flag-enum */, 7152, /* DiagArray338 */ 914, 0, R"()")
DIAG_ENTRY(FlexibleArrayExtensions /* flexible-array-extensions */, 7162, /* DiagArray339 */ 916, 0, R"()")
DIAG_ENTRY(FloatConversion /* float-conversion */, 7188, /* DiagArray340 */ 919, /* DiagSubGroup340 */ 312, R"()")
DIAG_ENTRY(anonymous_248 /* float-equal */, 7205, /* DiagArray341 */ 921, 0, R"()")
DIAG_ENTRY(FloatOverflowConversion /* float-overflow-conversion */, 7217, /* DiagArray342 */ 923, 0, R"()")
DIAG_ENTRY(FloatZeroConversion /* float-zero-conversion */, 7243, /* DiagArray343 */ 926, 0, R"()")
DIAG_ENTRY(ForLoopAnalysis /* for-loop-analysis */, 7265, /* DiagArray344 */ 928, 0, R"()")
DIAG_ENTRY(Format /* format */, 7283, /* DiagArray345 */ 931, /* DiagSubGroup345 */ 315, R"()")
DIAG_ENTRY(FormatExtraArgs /* format-extra-args */, 7290, /* DiagArray346 */ 959, 0, R"()")
DIAG_ENTRY(FormatInsufficientArgs /* format-insufficient-args */, 7308, /* DiagArray347 */ 961, 0, R"()")
DIAG_ENTRY(FormatInvalidSpecifier /* format-invalid-specifier */, 7333, /* DiagArray348 */ 963, 0, R"()")
DIAG_ENTRY(FormatNonStandard /* format-non-iso */, 7358, /* DiagArray349 */ 965, 0, R"()")
DIAG_ENTRY(FormatNonLiteral /* format-nonliteral */, 7373, /* DiagArray350 */ 969, 0, R"()")
DIAG_ENTRY(FormatOverflow /* format-overflow */, 7391, /* DiagArray351 */ 971, /* DiagSubGroup351 */ 325, R"()")
DIAG_ENTRY(FormatOverflowNonKprintf /* format-overflow-non-kprintf */, 7407, /* DiagArray352 */ 973, 0, R"()")
DIAG_ENTRY(FormatPedantic /* format-pedantic */, 7435, /* DiagArray353 */ 975, 0, R"()")
DIAG_ENTRY(FormatSecurity /* format-security */, 7451, /* DiagArray354 */ 978, 0, R"()")
DIAG_ENTRY(FormatSignedness /* format-signedness */, 7467, /* DiagArray355 */ 980, 0, R"()")
DIAG_ENTRY(FormatTruncation /* format-truncation */, 7485, /* DiagArray356 */ 982, /* DiagSubGroup356 */ 327, R"()")
DIAG_ENTRY(FormatTruncationNonKprintf /* format-truncation-non-kprintf */, 7503, /* DiagArray357 */ 984, 0, R"()")
DIAG_ENTRY(FormatTypeConfusion /* format-type-confusion */, 7533, /* DiagArray358 */ 986, 0, R"()")
DIAG_ENTRY(FormatY2K /* format-y2k */, 7555, 0, 0, R"()")
DIAG_ENTRY(FormatZeroLength /* format-zero-length */, 7566, /* DiagArray360 */ 988, 0, R"()")
DIAG_ENTRY(Format2 /* format=2 */, 7585, 0, /* DiagSubGroup361 */ 329, R"()")
DIAG_ENTRY(FortifySource /* fortify-source */, 7594, /* DiagArray362 */ 990, /* DiagSubGroup362 */ 333, R"()")
DIAG_ENTRY(FourByteMultiChar /* four-char-constants */, 7609, /* DiagArray363 */ 995, 0, R"()")
DIAG_ENTRY(FrameAddress /* frame-address */, 7629, /* DiagArray364 */ 997, 0, R"()")
DIAG_ENTRY(BackendFrameLargerThan /* frame-larger-than */, 7643, /* DiagArray365 */ 999, 0, R"(More fine grained information about the stack layout is available by adding the
`-Rpass-analysis=stack-frame-layout` command-line flag to the compiler
invocation.

The diagnostic information can be saved to a file in a machine readable format,
like YAML by adding the `-foptimization-record-file=<file>` command-line flag.

Results can be filtered by function name by passing
`-mllvm -filter-print-funcs=foo`, where `foo` is the target function's name.

   .. code-block:: console

      clang -c a.cpp -Rpass-analysis=stack-frame-layout -mllvm -filter-print-funcs=foo

   .. code-block:: console

      clang -c a.cpp -Rpass-analysis=stack-frame-layout -foptimization-record-file=<file>)")
DIAG_ENTRY(anonymous_76 /* frame-larger-than= */, 7661, 0, /* DiagSubGroup366 */ 336, R"()")
DIAG_ENTRY(FrameworkIncludePrivateFromPublic /* framework-include-private-from-public */, 7680, /* DiagArray367 */ 1002, 0, R"()")
DIAG_ENTRY(FreeNonHeapObject /* free-nonheap-object */, 7718, /* DiagArray368 */ 1004, 0, R"()")
DIAG_ENTRY(anonymous_187 /* friend-enum */, 7738, /* DiagArray369 */ 1006, 0, R"()")
DIAG_ENTRY(FunctionDefInObjCContainer /* function-def-in-objc-container */, 7750, /* DiagArray370 */ 1008, 0, R"()")
DIAG_ENTRY(FunctionEffects /* function-effects */, 7781, /* DiagArray371 */ 1010, 0, R"()")
DIAG_ENTRY(FunctionMultiVersioning /* function-multiversion */, 7798, /* DiagArray372 */ 1018, /* DiagSubGroup372 */ 338, R"()")
DIAG_ENTRY(FUseLdPath /* fuse-ld-path */, 7820, /* DiagArray373 */ 1023, 0, R"()")
DIAG_ENTRY(FutureAttrs /* future-attribute-extensions */, 7833, 0, /* DiagSubGroup374 */ 340, R"()")
DIAG_ENTRY(FutureCompat /* future-compat */, 7861, 0, 0, R"()")
DIAG_ENTRY(GccCompat /* gcc-compat */, 7875, /* DiagArray376 */ 1025, 0, R"()")
DIAG_ENTRY(GlobalConstructors /* global-constructors */, 7886, /* DiagArray377 */ 1037, 0, R"(Emit a warning for each variable declaration that generates code run at startup.)")
DIAG_ENTRY(GlobalISel /* global-isel */, 7906, /* DiagArray378 */ 1040, 0, R"()")
DIAG_ENTRY(GNU /* gnu */, 7918, 0, /* DiagSubGroup379 */ 345, R"()")
DIAG_ENTRY(GNUAlignofExpression /* gnu-alignof-expression */, 7922, /* DiagArray380 */ 1043, 0, R"()")
DIAG_ENTRY(GNUAnonymousStruct /* gnu-anonymous-struct */, 7945, /* DiagArray381 */ 1045, 0, R"()")
DIAG_ENTRY(anonymous_258 /* gnu-array-member-paren-init */, 7966, /* DiagArray382 */ 1047, 0, R"()")
DIAG_ENTRY(GNUAutoType /* gnu-auto-type */, 7994, /* DiagArray383 */ 1049, 0, R"()")
DIAG_ENTRY(GNUBinaryLiteral /* gnu-binary-literal */, 8008, 0, 0, R"()")
DIAG_ENTRY(GNUCaseRange /* gnu-case-range */, 8027, /* DiagArray385 */ 1051, 0, R"()")
DIAG_ENTRY(GNUComplexInteger /* gnu-complex-integer */, 8042, /* DiagArray386 */ 1053, 0, R"()")
DIAG_ENTRY(GNUCompoundLiteralInitializer /* gnu-compound-literal-initializer */, 8062, /* DiagArray387 */ 1055, 0, R"()")
DIAG_ENTRY(GNUConditionalOmittedOperand /* gnu-conditional-omitted-operand */, 8095, /* DiagArray388 */ 1057, 0, R"()")
DIAG_ENTRY(GNUDesignator /* gnu-designator */, 8127, /* DiagArray389 */ 1059, 0, R"()")
DIAG_ENTRY(anonymous_1 /* gnu-empty-initializer */, 8142, 0, 0, R"()")
DIAG_ENTRY(GNUEmptyStruct /* gnu-empty-struct */, 8164, /* DiagArray391 */ 1063, 0, R"()")
DIAG_ENTRY(GNUFlexibleArrayInitializer /* gnu-flexible-array-initializer */, 8181, /* DiagArray392 */ 1067, 0, R"()")
DIAG_ENTRY(GNUFlexibleArrayUnionMember /* gnu-flexible-array-union-member */, 8212, /* DiagArray393 */ 1069, 0, R"()")
DIAG_ENTRY(GNUFoldingConstant /* gnu-folding-constant */, 8244, /* DiagArray394 */ 1071, 0, R"()")
DIAG_ENTRY(GNUImaginaryConstant /* gnu-imaginary-constant */, 8265, /* DiagArray395 */ 1075, 0, R"()")
DIAG_ENTRY(GNUIncludeNext /* gnu-include-next */, 8288, /* DiagArray396 */ 1077, 0, R"()")
DIAG_ENTRY(anonymous_210 /* gnu-inline-cpp-without-extern */, 8305, /* DiagArray397 */ 1079, 0, R"()")
DIAG_ENTRY(GNULabelsAsValue /* gnu-label-as-value */, 8335, /* DiagArray398 */ 1081, 0, R"()")
DIAG_ENTRY(GNULineMarker /* gnu-line-marker */, 8354, /* DiagArray399 */ 1084, 0, R"()")
DIAG_ENTRY(GNUNullPointerArithmetic /* gnu-null-pointer-arithmetic */, 8370, /* DiagArray400 */ 1086, 0, R"()")
DIAG_ENTRY(GNUOffsetofExtensions /* gnu-offsetof-extensions */, 8398, 0, 0, R"()")
DIAG_ENTRY(GNUPointerArith /* gnu-pointer-arith */, 8422, /* DiagArray402 */ 1088, 0, R"()")
DIAG_ENTRY(GNURedeclaredEnum /* gnu-redeclared-enum */, 8440, /* DiagArray403 */ 1092, 0, R"()")
DIAG_ENTRY(GNUStatementExpression /* gnu-statement-expression */, 8460, /* DiagArray404 */ 1094, /* DiagSubGroup404 */ 377, R"()")
DIAG_ENTRY(GNUStatementExpressionFromMacroExpansion /* gnu-statement-expression-from-macro-expansion */, 8485, /* DiagArray405 */ 1096, 0, R"()")
DIAG_ENTRY(GNUStaticFloatInit /* gnu-static-float-init */, 8531, /* DiagArray406 */ 1098, 0, R"()")
DIAG_ENTRY(GNUStringLiteralOperatorTemplate /* gnu-string-literal-operator-template */, 8553, /* DiagArray407 */ 1100, 0, R"()")
DIAG_ENTRY(GNUUnionCast /* gnu-union-cast */, 8590, /* DiagArray408 */ 1102, 0, R"()")
DIAG_ENTRY(GNUVariableSizedTypeNotAtEnd /* gnu-variable-sized-type-not-at-end */, 8605, /* DiagArray409 */ 1104, 0, R"()")
DIAG_ENTRY(GNUZeroLineDirective /* gnu-zero-line-directive */, 8640, /* DiagArray410 */ 1106, 0, R"()")
DIAG_ENTRY(GNUZeroVariadicMacroArguments /* gnu-zero-variadic-macro-arguments */, 8664, /* DiagArray411 */ 1108, /* DiagSubGroup411 */ 379, R"()")
DIAG_ENTRY(anonymous_277 /* gpu-maybe-wrong-side */, 8698, /* DiagArray412 */ 1110, 0, R"()")
DIAG_ENTRY(anonymous_139 /* header-guard */, 8719, /* DiagArray413 */ 1112, 0, R"()")
DIAG_ENTRY(HeaderHygiene /* header-hygiene */, 8732, /* DiagArray414 */ 1114, 0, R"()")
DIAG_ENTRY(anonymous_246 /* higher-precision-for-complex-division */, 8747, /* DiagArray415 */ 1116, 0, R"()")
DIAG_ENTRY(HIPOpenMPOffloading /* hip-omp-target-directives */, 8785, /* DiagArray416 */ 1118, 0, R"()")
DIAG_ENTRY(HIPOnly /* hip-only */, 8811, /* DiagArray417 */ 1120, 0, R"()")
DIAG_ENTRY(HLSL202y /* hlsl-202y-extensions */, 8820, /* DiagArray418 */ 1122, 0, R"()")
DIAG_ENTRY(HLSLAvailability /* hlsl-availability */, 8841, /* DiagArray419 */ 1124, 0, R"()")
DIAG_ENTRY(HLSLDXCCompat /* hlsl-dxc-compatability */, 8859, /* DiagArray420 */ 1127, 0, R"()")
DIAG_ENTRY(HLSLExtension /* hlsl-extensions */, 8882, /* DiagArray421 */ 1129, /* DiagSubGroup421 */ 381, R"()")
DIAG_ENTRY(anonymous_272 /* idiomatic-parentheses */, 8898, /* DiagArray422 */ 1132, 0, R"()")
DIAG_ENTRY(IgnoredAttributes /* ignored-attributes */, 8920, /* DiagArray423 */ 1134, 0, R"()")
DIAG_ENTRY(anonymous_213 /* ignored-availability-without-sdk-settings */, 8939, /* DiagArray424 */ 1225, 0, R"()")
DIAG_ENTRY(IgnoredGCH /* ignored-gch */, 8981, /* DiagArray425 */ 1227, 0, R"()")
DIAG_ENTRY(IgnoredOptimizationArgument /* ignored-optimization-argument */, 8993, /* DiagArray426 */ 1230, 0, R"()")
DIAG_ENTRY(IgnoredPragmaIntrinsic /* ignored-pragma-intrinsic */, 9023, /* DiagArray427 */ 1233, 0, R"()")
DIAG_ENTRY(IgnoredPragmaOptimize /* ignored-pragma-optimize */, 9048, 0, 0, R"()")
DIAG_ENTRY(IgnoredPragmas /* ignored-pragmas */, 9072, /* DiagArray429 */ 1235, /* DiagSubGroup429 */ 383, R"()")
DIAG_ENTRY(IgnoredQualifiers /* ignored-qualifiers */, 9088, /* DiagArray430 */ 1285, /* DiagSubGroup430 */ 386, R"()")
DIAG_ENTRY(IgnoredReferenceQualifiers /* ignored-reference-qualifiers */, 9107, /* DiagArray431 */ 1292, 0, R"()")
DIAG_ENTRY(Implicit /* implicit */, 9136, 0, /* DiagSubGroup432 */ 388, R"()")
DIAG_ENTRY(ImplicitAtomic /* implicit-atomic-properties */, 9145, /* DiagArray433 */ 1294, 0, R"()")
DIAG_ENTRY(ImplicitConstIntFloatConversion /* implicit-const-int-float-conversion */, 9172, /* DiagArray434 */ 1297, 0, R"()")
DIAG_ENTRY(ImplicitConversionFloatingPointToBool /* implicit-conversion-floating-point-to-bool */, 9208, /* DiagArray435 */ 1299, 0, R"()")
DIAG_ENTRY(anonymous_290 /* implicit-exception-spec-mismatch */, 9251, /* DiagArray436 */ 1301, 0, R"()")
DIAG_ENTRY(ImplicitFallthrough /* implicit-fallthrough */, 9284, /* DiagArray437 */ 1303, /* DiagSubGroup437 */ 391, R"()")
DIAG_ENTRY(ImplicitFallthroughPerFunction /* implicit-fallthrough-per-function */, 9305, /* DiagArray438 */ 1305, 0, R"()")
DIAG_ENTRY(ImplicitFixedPointConversion /* implicit-fixed-point-conversion */, 9339, /* DiagArray439 */ 1307, 0, R"()")
DIAG_ENTRY(ImplicitFloatConversion /* implicit-float-conversion */, 9371, /* DiagArray440 */ 1309, /* DiagSubGroup440 */ 393, R"()")
DIAG_ENTRY(ImplicitFunctionDeclare /* implicit-function-declaration */, 9397, /* DiagArray441 */ 1312, 0, R"()")
DIAG_ENTRY(ImplicitInt /* implicit-int */, 9427, /* DiagArray442 */ 1318, 0, R"()")
DIAG_ENTRY(ImplicitIntConversion /* implicit-int-conversion */, 9440, /* DiagArray443 */ 1322, /* DiagSubGroup443 */ 396, R"()")
DIAG_ENTRY(ImplicitIntFloatConversion /* implicit-int-float-conversion */, 9464, /* DiagArray444 */ 1325, /* DiagSubGroup444 */ 399, R"()")
DIAG_ENTRY(anonymous_182 /* implicit-retain-self */, 9494, /* DiagArray445 */ 1327, 0, R"()")
DIAG_ENTRY(ImplicitlyUnsignedLiteral /* implicitly-unsigned-literal */, 9515, /* DiagArray446 */ 1329, 0, R"()")
DIAG_ENTRY(anonymous_30 /* import */, 9543, 0, 0, R"()")
DIAG_ENTRY(anonymous_161 /* import-implementation-partition-unit-in-interface-unit */, 9550, /* DiagArray448 */ 1331, 0, R"()")
DIAG_ENTRY(anonymous_132 /* import-preprocessor-directive-pedantic */, 9605, /* DiagArray449 */ 1333, 0, R"()")
DIAG_ENTRY(anonymous_287 /* inaccessible-base */, 9644, /* DiagArray450 */ 1335, 0, R"()")
DIAG_ENTRY(anonymous_138 /* include-angled-in-module-purview */, 9662, /* DiagArray451 */ 1337, 0, R"()")
DIAG_ENTRY(anonymous_122 /* include-next-absolute-path */, 9695, /* DiagArray452 */ 1339, 0, R"()")
DIAG_ENTRY(anonymous_121 /* include-next-outside-header */, 9722, /* DiagArray453 */ 1341, 0, R"()")
DIAG_ENTRY(IncompatibleExceptionSpec /* incompatible-exception-spec */, 9750, /* DiagArray454 */ 1343, 0, R"()")
DIAG_ENTRY(IncompatibleFunctionPointerTypes /* incompatible-function-pointer-types */, 9778, /* DiagArray455 */ 1346, 0, R"()")
DIAG_ENTRY(anonymous_274 /* incompatible-function-pointer-types-strict */, 9814, /* DiagArray456 */ 1348, 0, R"()")
DIAG_ENTRY(anonymous_165 /* incompatible-library-redeclaration */, 9857, /* DiagArray457 */ 1350, 0, R"()")
DIAG_ENTRY(IncompatibleMSPragmaSection /* incompatible-ms-pragma-section */, 9892, /* DiagArray458 */ 1352, 0, R"()")
DIAG_ENTRY(IncompatibleMSStruct /* incompatible-ms-struct */, 9923, /* DiagArray459 */ 1354, 0, R"()")
DIAG_ENTRY(IncompatiblePointerTypes /* incompatible-pointer-types */, 9946, /* DiagArray460 */ 1357, /* DiagSubGroup460 */ 401, R"()")
DIAG_ENTRY(IncompatiblePointerTypesDiscardsQualifiers /* incompatible-pointer-types-discards-qualifiers */, 9973, /* DiagArray461 */ 1359, 0, R"()")
DIAG_ENTRY(anonymous_172 /* incompatible-property-type */, 10020, /* DiagArray462 */ 1363, 0, R"()")
DIAG_ENTRY(anonymous_94 /* incompatible-sysroot */, 10047, /* DiagArray463 */ 1365, 0, R"()")
DIAG_ENTRY(IncompleteFrameworkModuleDeclaration /* incomplete-framework-module-declaration */, 10068, /* DiagArray464 */ 1367, 0, R"()")
DIAG_ENTRY(anonymous_174 /* incomplete-implementation */, 10108, /* DiagArray465 */ 1369, 0, R"()")
DIAG_ENTRY(IncompleteModule /* incomplete-module */, 10134, 0, /* DiagSubGroup466 */ 404, R"()")
DIAG_ENTRY(anonymous_164 /* incomplete-setjmp-declaration */, 10152, /* DiagArray467 */ 1371, 0, R"()")
DIAG_ENTRY(IncompleteUmbrella /* incomplete-umbrella */, 10182, /* DiagArray468 */ 1373, 0, R"()")
DIAG_ENTRY(MicrosoftInconsistentDllImport /* inconsistent-dllimport */, 10202, /* DiagArray469 */ 1377, 0, R"()")
DIAG_ENTRY(CXX11WarnInconsistentOverrideDestructor /* inconsistent-missing-destructor-override */, 10225, /* DiagArray470 */ 1380, 0, R"()")
DIAG_ENTRY(CXX11WarnInconsistentOverrideMethod /* inconsistent-missing-override */, 10266, /* DiagArray471 */ 1382, 0, R"()")
DIAG_ENTRY(IncrementBool /* increment-bool */, 10296, /* DiagArray472 */ 1384, /* DiagSubGroup472 */ 407, R"()")
DIAG_ENTRY(InfiniteRecursion /* infinite-recursion */, 10311, /* DiagArray473 */ 1386, 0, R"()")
DIAG_ENTRY(anonymous_31 /* init-self */, 10330, 0, 0, R"()")
DIAG_ENTRY(InitializerOverrides /* initializer-overrides */, 10340, /* DiagArray475 */ 1388, 0, R"()")
DIAG_ENTRY(anonymous_191 /* injected-class-name */, 10362, /* DiagArray476 */ 1392, 0, R"()")
DIAG_ENTRY(anonymous_32 /* inline */, 10382, 0, 0, R"()")
DIAG_ENTRY(BackendInlineAsm /* inline-asm */, 10389, /* DiagArray478 */ 1394, 0, R"()")
DIAG_ENTRY(InlineNamespaceReopenedNoninline /* inline-namespace-reopened-noninline */, 10400, /* DiagArray479 */ 1396, 0, R"()")
DIAG_ENTRY(anonymous_288 /* inline-new-delete */, 10436, /* DiagArray480 */ 1398, 0, R"()")
DIAG_ENTRY(InstallAPIViolation /* installapi-violation */, 10454, /* DiagArray481 */ 1400, 0, R"()")
DIAG_ENTRY(anonymous_229 /* instantiation-after-specialization */, 10475, /* DiagArray482 */ 1414, 0, R"()")
DIAG_ENTRY(IntConversion /* int-conversion */, 10510, /* DiagArray483 */ 1416, 0, R"()")
DIAG_ENTRY(anonymous_65 /* int-conversions */, 10525, 0, /* DiagSubGroup484 */ 409, R"()")
DIAG_ENTRY(IntInBoolContext /* int-in-bool-context */, 10541, /* DiagArray485 */ 1419, 0, R"()")
DIAG_ENTRY(IntToPointerCast /* int-to-pointer-cast */, 10561, /* DiagArray486 */ 1422, /* DiagSubGroup486 */ 411, R"()")
DIAG_ENTRY(IntToVoidPointerCast /* int-to-void-pointer-cast */, 10581, /* DiagArray487 */ 1424, 0, R"()")
DIAG_ENTRY(anonymous_78 /* integer-overflow */, 10606, /* DiagArray488 */ 1426, 0, R"()")
DIAG_ENTRY(InvalidCommandLineArgument /* invalid-command-line-argument */, 10623, /* DiagArray489 */ 1428, /* DiagSubGroup489 */ 413, R"()")
DIAG_ENTRY(anonymous_194 /* invalid-constexpr */, 10653, /* DiagArray490 */ 1440, 0, R"()")
DIAG_ENTRY(anonymous_86 /* invalid-feature-combination */, 10671, /* DiagArray491 */ 1442, 0, R"()")
DIAG_ENTRY(anonymous_284 /* invalid-gnu-asm-cast */, 10699, /* DiagArray492 */ 1444, 0, R"()")
DIAG_ENTRY(ObjCInvalidIBOutletProperty /* invalid-iboutlet */, 10720, /* DiagArray493 */ 1446, 0, R"()")
DIAG_ENTRY(anonymous_207 /* invalid-initializer-from-system-header */, 10737, /* DiagArray494 */ 1449, 0, R"()")
DIAG_ENTRY(InvalidIOSDeploymentTarget /* invalid-ios-deployment-target */, 10776, /* DiagArray495 */ 1451, 0, R"()")
DIAG_ENTRY(anonymous_219 /* invalid-no-builtin-names */, 10806, /* DiagArray496 */ 1453, 0, R"()")
DIAG_ENTRY(InvalidNoreturn /* invalid-noreturn */, 10831, /* DiagArray497 */ 1455, 0, R"()")
DIAG_ENTRY(InvalidOffsetof /* invalid-offsetof */, 10848, /* DiagArray498 */ 1458, 0, R"()")
DIAG_ENTRY(InvalidOrNonExistentDirectory /* invalid-or-nonexistent-directory */, 10865, /* DiagArray499 */ 1461, 0, R"()")
DIAG_ENTRY(anonymous_227 /* invalid-partial-specialization */, 10898, /* DiagArray500 */ 1463, 0, R"()")
DIAG_ENTRY(anonymous_33 /* invalid-pch */, 10929, 0, 0, R"()")
DIAG_ENTRY(InvalidPPToken /* invalid-pp-token */, 10941, /* DiagArray502 */ 1465, 0, R"()")
DIAG_ENTRY(InvalidSourceEncoding /* invalid-source-encoding */, 10958, /* DiagArray503 */ 1468, 0, R"()")
DIAG_ENTRY(anonymous_225 /* invalid-specialization */, 10982, /* DiagArray504 */ 1471, 0, R"()")
DIAG_ENTRY(anonymous_185 /* invalid-static-assert-message */, 11005, /* DiagArray505 */ 1473, 0, R"()")
DIAG_ENTRY(anonymous_135 /* invalid-token-paste */, 11035, /* DiagArray506 */ 1475, 0, R"()")
DIAG_ENTRY(anonymous_120 /* invalid-unevaluated-string */, 11055, /* DiagArray507 */ 1477, 0, R"()")
DIAG_ENTRY(anonymous_113 /* invalid-utf8 */, 11082, /* DiagArray508 */ 1479, 0, R"()")
DIAG_ENTRY(anonymous_269 /* jump-seh-finally */, 11095, /* DiagArray509 */ 1481, 0, R"()")
DIAG_ENTRY(KeywordCompat /* keyword-compat */, 11112, /* DiagArray510 */ 1483, 0, R"()")
DIAG_ENTRY(KeywordAsMacro /* keyword-macro */, 11127, /* DiagArray511 */ 1485, 0, R"()")
DIAG_ENTRY(KNRPromotedParameter /* knr-promoted-parameter */, 11141, /* DiagArray512 */ 1487, 0, R"()")
DIAG_ENTRY(anonymous_112 /* language-extension-token */, 11164, /* DiagArray513 */ 1489, 0, R"()")
DIAG_ENTRY(LargeByValueCopy /* large-by-value-copy */, 11189, /* DiagArray514 */ 1491, 0, R"()")
DIAG_ENTRY(LegacyConstantRegisterBinding /* legacy-constant-register-binding */, 11209, /* DiagArray515 */ 1494, 0, R"()")
DIAG_ENTRY(LibLTO /* liblto */, 11242, 0, 0, R"()")
DIAG_ENTRY(LinkerWarnings /* linker-warnings */, 11249, /* DiagArray517 */ 1499, 0, R"()")
DIAG_ENTRY(LiteralConversion /* literal-conversion */, 11265, /* DiagArray518 */ 1501, 0, R"()")
DIAG_ENTRY(LiteralRange /* literal-range */, 11284, /* DiagArray519 */ 1504, 0, R"()")
DIAG_ENTRY(LocalTypeTemplateArgs /* local-type-template-args */, 11298, /* DiagArray520 */ 1508, /* DiagSubGroup520 */ 415, R"()")
DIAG_ENTRY(LogicalNotParentheses /* logical-not-parentheses */, 11323, /* DiagArray521 */ 1510, 0, R"()")
DIAG_ENTRY(LogicalOpParentheses /* logical-op-parentheses */, 11347, /* DiagArray522 */ 1512, 0, R"()")
DIAG_ENTRY(LongLong /* long-long */, 11370, /* DiagArray523 */ 1514, /* DiagSubGroup523 */ 417, R"()")
DIAG_ENTRY(LoopAnalysis /* loop-analysis */, 11380, 0, /* DiagSubGroup524 */ 419, R"()")
DIAG_ENTRY(MacroRedefined /* macro-redefined */, 11394, /* DiagArray525 */ 1516, 0, R"()")
DIAG_ENTRY(Main /* main */, 11410, /* DiagArray526 */ 1518, 0, R"()")
DIAG_ENTRY(MainReturnType /* main-return-type */, 11415, /* DiagArray527 */ 1527, 0, R"()")
DIAG_ENTRY(MalformedWarningCheck /* malformed-warning-check */, 11432, /* DiagArray528 */ 1529, 0, R"()")
DIAG_ENTRY(anonymous_242 /* many-braces-around-scalar-init */, 11456, /* DiagArray529 */ 1531, 0, R"()")
DIAG_ENTRY(MathErrnoEnabledWithVecLib /* math-errno-enabled-with-veclib */, 11487, /* DiagArray530 */ 1533, 0, R"()")
DIAG_ENTRY(anonymous_117 /* mathematical-notation-identifier-extension */, 11518, /* DiagArray531 */ 1535, 0, R"()")
DIAG_ENTRY(MaxTokens /* max-tokens */, 11561, /* DiagArray532 */ 1537, 0, R"(The warning is issued if the number of pre-processor tokens exceeds
the token limit, which can be set in three ways:

1. As a limit at a specific point in a file, using the ``clang max_tokens_here``
   pragma:

   .. code-block:: c++

      #pragma clang max_tokens_here 1234

2. As a per-translation unit limit, using the ``-fmax-tokens=`` command-line
   flag:

   .. code-block:: console

      clang -c a.cpp -fmax-tokens=1234

3. As a per-translation unit limit using the ``clang max_tokens_total`` pragma,
   which works like and overrides the ``-fmax-tokens=`` flag:

   .. code-block:: c++

      #pragma clang max_tokens_total 1234

These limits can be helpful in limiting code growth through included files.

Setting a token limit of zero means no limit.

Note that the warning is disabled by default, so -Wmax-tokens must be used
in addition with the pragmas or -fmax-tokens flag to get any warnings.)")
DIAG_ENTRY(MaxUnsignedZero /* max-unsigned-zero */, 11572, /* DiagArray533 */ 1540, 0, R"()")
DIAG_ENTRY(MemsetTransposedArgs /* memset-transposed-args */, 11590, /* DiagArray534 */ 1542, 0, R"()")
DIAG_ENTRY(anonymous_167 /* memsize-comparison */, 11613, /* DiagArray535 */ 1544, 0, R"()")
DIAG_ENTRY(MethodSignatures /* method-signatures */, 11632, /* DiagArray536 */ 1546, 0, R"()")
DIAG_ENTRY(Microsoft /* microsoft */, 11650, 0, /* DiagSubGroup537 */ 422, R"()")
DIAG_ENTRY(MicrosoftAbstract /* microsoft-abstract */, 11660, /* DiagArray538 */ 1549, 0, R"()")
DIAG_ENTRY(MicrosoftAnonTag /* microsoft-anon-tag */, 11679, /* DiagArray539 */ 1551, 0, R"()")
DIAG_ENTRY(MicrosoftCast /* microsoft-cast */, 11698, /* DiagArray540 */ 1554, 0, R"()")
DIAG_ENTRY(MicrosoftCharize /* microsoft-charize */, 11713, /* DiagArray541 */ 1557, 0, R"()")
DIAG_ENTRY(MicrosoftCommentPaste /* microsoft-comment-paste */, 11731, /* DiagArray542 */ 1559, 0, R"()")
DIAG_ENTRY(MicrosoftConstInit /* microsoft-const-init */, 11755, /* DiagArray543 */ 1561, 0, R"()")
DIAG_ENTRY(MicrosoftCppMacro /* microsoft-cpp-macro */, 11776, /* DiagArray544 */ 1563, 0, R"()")
DIAG_ENTRY(MicrosoftDefaultArgRedefinition /* microsoft-default-arg-redefinition */, 11796, /* DiagArray545 */ 1565, 0, R"()")
DIAG_ENTRY(MicrosoftDrectveSection /* microsoft-drectve-section */, 11831, /* DiagArray546 */ 1567, 0, R"()")
DIAG_ENTRY(MicrosoftEndOfFile /* microsoft-end-of-file */, 11857, /* DiagArray547 */ 1569, 0, R"()")
DIAG_ENTRY(MicrosoftEnumForwardReference /* microsoft-enum-forward-reference */, 11879, /* DiagArray548 */ 1571, 0, R"()")
DIAG_ENTRY(MicrosoftEnumValue /* microsoft-enum-value */, 11912, /* DiagArray549 */ 1573, 0, R"()")
DIAG_ENTRY(MicrosoftExceptionSpec /* microsoft-exception-spec */, 11933, /* DiagArray550 */ 1575, 0, R"()")
DIAG_ENTRY(anonymous_144 /* microsoft-exists */, 11958, /* DiagArray551 */ 1581, 0, R"()")
DIAG_ENTRY(MicrosoftExplicitConstructorCall /* microsoft-explicit-constructor-call */, 11975, /* DiagArray552 */ 1583, 0, R"()")
DIAG_ENTRY(MicrosoftExtraQualification /* microsoft-extra-qualification */, 12011, /* DiagArray553 */ 1585, 0, R"()")
DIAG_ENTRY(MicrosoftFixedEnum /* microsoft-fixed-enum */, 12041, /* DiagArray554 */ 1587, 0, R"()")
DIAG_ENTRY(MicrosoftFlexibleArray /* microsoft-flexible-array */, 12062, /* DiagArray555 */ 1589, 0, R"()")
DIAG_ENTRY(MicrosoftGoto /* microsoft-goto */, 12087, /* DiagArray556 */ 1592, 0, R"()")
DIAG_ENTRY(MicrosoftInaccessibleBase /* microsoft-inaccessible-base */, 12102, /* DiagArray557 */ 1594, 0, R"()")
DIAG_ENTRY(MicrosoftInclude /* microsoft-include */, 12130, /* DiagArray558 */ 1596, 0, R"()")
DIAG_ENTRY(MicrosoftInitFromPredefined /* microsoft-init-from-predefined */, 12148, /* DiagArray559 */ 1598, 0, R"()")
DIAG_ENTRY(MicrosoftInlineOnNonFunction /* microsoft-inline-on-non-function */, 12179, /* DiagArray560 */ 1600, 0, R"()")
DIAG_ENTRY(MicrosoftMutableReference /* microsoft-mutable-reference */, 12212, /* DiagArray561 */ 1602, 0, R"()")
DIAG_ENTRY(MicrosoftPureDefinition /* microsoft-pure-definition */, 12240, /* DiagArray562 */ 1604, 0, R"()")
DIAG_ENTRY(MicrosoftRedeclareStatic /* microsoft-redeclare-static */, 12266, /* DiagArray563 */ 1606, 0, R"()")
DIAG_ENTRY(MicrosoftSealed /* microsoft-sealed */, 12293, /* DiagArray564 */ 1608, 0, R"()")
DIAG_ENTRY(MicrosoftStaticAssert /* microsoft-static-assert */, 12310, /* DiagArray565 */ 1610, 0, R"()")
DIAG_ENTRY(MicrosoftStringLiteralFromPredefined /* microsoft-string-literal-from-predefined */, 12334, /* DiagArray566 */ 1612, 0, R"()")
DIAG_ENTRY(MicrosoftTemplate /* microsoft-template */, 12375, /* DiagArray567 */ 1614, /* DiagSubGroup567 */ 456, R"()")
DIAG_ENTRY(MicrosoftTemplateShadow /* microsoft-template-shadow */, 12394, /* DiagArray568 */ 1625, 0, R"()")
DIAG_ENTRY(MicrosoftUnionMemberReference /* microsoft-union-member-reference */, 12420, /* DiagArray569 */ 1627, 0, R"()")
DIAG_ENTRY(MicrosoftUnqualifiedFriend /* microsoft-unqualified-friend */, 12453, /* DiagArray570 */ 1629, 0, R"()")
DIAG_ENTRY(MicrosoftUsingDecl /* microsoft-using-decl */, 12482, /* DiagArray571 */ 1631, 0, R"()")
DIAG_ENTRY(MicrosoftVoidPseudoDtor /* microsoft-void-pseudo-dtor */, 12503, /* DiagArray572 */ 1633, 0, R"()")
DIAG_ENTRY(MisExpect /* misexpect */, 12530, /* DiagArray573 */ 1635, 0, R"()")
DIAG_ENTRY(MisleadingIndentation /* misleading-indentation */, 12540, /* DiagArray574 */ 1637, 0, R"()")
DIAG_ENTRY(anonymous_265 /* mismatched-new-delete */, 12563, /* DiagArray575 */ 1639, 0, R"()")
DIAG_ENTRY(MismatchedParameterTypes /* mismatched-parameter-types */, 12585, /* DiagArray576 */ 1641, 0, R"()")
DIAG_ENTRY(MismatchedReturnTypes /* mismatched-return-types */, 12612, /* DiagArray577 */ 1643, 0, R"()")
DIAG_ENTRY(MismatchedTags /* mismatched-tags */, 12636, /* DiagArray578 */ 1645, 0, R"()")
DIAG_ENTRY(MissingBraces /* missing-braces */, 12652, /* DiagArray579 */ 1648, 0, R"()")
DIAG_ENTRY(anonymous_283 /* missing-constinit */, 12667, /* DiagArray580 */ 1650, 0, R"()")
DIAG_ENTRY(MissingDeclarations /* missing-declarations */, 12685, /* DiagArray581 */ 1652, 0, R"()")
DIAG_ENTRY(MissingDesignatedFieldInitializers /* missing-designated-field-initializers */, 12706, /* DiagArray582 */ 1657, 0, R"(Warn about designated initializers with some fields missing (only in C++).)")
DIAG_ENTRY(anonymous_190 /* missing-exception-spec */, 12744, /* DiagArray583 */ 1659, 0, R"()")
DIAG_ENTRY(MissingFieldInitializers /* missing-field-initializers */, 12767, /* DiagArray584 */ 1661, /* DiagSubGroup584 */ 458, R"()")
DIAG_ENTRY(anonymous_34 /* missing-format-attribute */, 12794, 0, 0, R"()")
DIAG_ENTRY(MissingIncludeDirs /* missing-include-dirs */, 12819, /* DiagArray586 */ 1663, 0, R"()")
DIAG_ENTRY(MissingMethodReturnType /* missing-method-return-type */, 12840, /* DiagArray587 */ 1665, 0, R"()")
DIAG_ENTRY(anonymous_102 /* missing-multilib */, 12867, /* DiagArray588 */ 1667, 0, R"()")
DIAG_ENTRY(MissingNoEscape /* missing-noescape */, 12884, /* DiagArray589 */ 1669, 0, R"()")
DIAG_ENTRY(MissingNoreturn /* missing-noreturn */, 12901, /* DiagArray590 */ 1671, 0, R"()")
DIAG_ENTRY(anonymous_211 /* missing-prototype-for-cc */, 12918, /* DiagArray591 */ 1674, 0, R"()")
DIAG_ENTRY(anonymous_231 /* missing-prototypes */, 12943, /* DiagArray592 */ 1676, 0, R"()")
DIAG_ENTRY(anonymous_145 /* missing-selector-name */, 12962, /* DiagArray593 */ 1678, 0, R"()")
DIAG_ENTRY(anonymous_93 /* missing-sysroot */, 12984, /* DiagArray594 */ 1680, 0, R"()")
DIAG_ENTRY(anonymous_152 /* missing-template-arg-list-after-template-kw */, 13000, /* DiagArray595 */ 1682, 0, R"()")
DIAG_ENTRY(anonymous_232 /* missing-variable-declarations */, 13044, /* DiagArray596 */ 1684, 0, R"()")
DIAG_ENTRY(MisspelledAssumption /* misspelled-assumption */, 13074, /* DiagArray597 */ 1686, 0, R"()")
DIAG_ENTRY(HLSLMixPackOffset /* mix-packoffset */, 13096, /* DiagArray598 */ 1688, 0, R"()")
DIAG_ENTRY(ModuleBuild /* module-build */, 13111, /* DiagArray599 */ 1690, 0, R"()")
DIAG_ENTRY(ModuleConflict /* module-conflict */, 13124, /* DiagArray600 */ 1695, 0, R"()")
DIAG_ENTRY(anonymous_107 /* module-file-config-mismatch */, 13140, /* DiagArray601 */ 1698, 0, R"()")
DIAG_ENTRY(ModuleFileExtension /* module-file-extension */, 13168, /* DiagArray602 */ 1700, 0, R"()")
DIAG_ENTRY(ModuleImport /* module-import */, 13190, /* DiagArray603 */ 1702, 0, R"()")
DIAG_ENTRY(anonymous_302 /* module-import-in-extern-c */, 13204, /* DiagArray604 */ 1704, 0, R"()")
DIAG_ENTRY(ModuleIncludeDirectiveTranslation /* module-include-translation */, 13230, /* DiagArray605 */ 1706, 0, R"()")
DIAG_ENTRY(ModuleLock /* module-lock */, 13257, /* DiagArray606 */ 1708, 0, R"()")
DIAG_ENTRY(anonymous_305 /* modules-ambiguous-internal-linkage */, 13269, /* DiagArray607 */ 1710, 0, R"()")
DIAG_ENTRY(anonymous_303 /* modules-import-nested-redundant */, 13304, /* DiagArray608 */ 1712, 0, R"()")
DIAG_ENTRY(Most /* most */, 13336, 0, /* DiagSubGroup609 */ 460, R"()")
DIAG_ENTRY(Move /* move */, 13341, 0, /* DiagSubGroup610 */ 498, R"()")
DIAG_ENTRY(anonymous_75 /* msvc-include */, 13346, 0, /* DiagSubGroup611 */ 503, R"()")
DIAG_ENTRY(anonymous_98 /* msvc-not-found */, 13359, /* DiagArray612 */ 1714, 0, R"()")
DIAG_ENTRY(MultiGPU /* multi-gpu */, 13374, /* DiagArray613 */ 1716, 0, R"()")
DIAG_ENTRY(MultiChar /* multichar */, 13384, /* DiagArray614 */ 1718, 0, R"()")
DIAG_ENTRY(anonymous_289 /* multiple-move-vbase */, 13394, /* DiagArray615 */ 1720, 0, R"()")
DIAG_ENTRY(anonymous_82 /* nan-infinity-disabled */, 13414, /* DiagArray616 */ 1722, 0, R"(This warning is enabled when source code using the macros ``INFINITY`` or ``NAN``
is compiled with floating-point options preventing these two values. This can
lead to undefined behavior. Check the order of command line arguments that modify
this behavior, such as ``-ffast-math``, ``-fhonor-infinities``, and
``-fhonor-nans`` (etc), as well as ``#pragma`` directives if this diagnostic is
generated unexpectedly.)")
DIAG_ENTRY(anonymous_24 /* narrowing */, 13436, 0, /* DiagSubGroup617 */ 505, R"()")
DIAG_ENTRY(anonymous_286 /* nested-anon-types */, 13446, /* DiagArray618 */ 1724, 0, R"()")
DIAG_ENTRY(anonymous_35 /* nested-externs */, 13464, 0, 0, R"()")
DIAG_ENTRY(OperatorNewReturnsNull /* new-returns-null */, 13479, /* DiagArray620 */ 1726, 0, R"()")
DIAG_ENTRY(NewlineEOF /* newline-eof */, 13496, /* DiagArray621 */ 1728, 0, R"()")
DIAG_ENTRY(NoDeref /* noderef */, 13508, /* DiagArray622 */ 1731, 0, R"()")
DIAG_ENTRY(NoexceptType /* noexcept-type */, 13516, 0, /* DiagSubGroup623 */ 507, R"()")
DIAG_ENTRY(anonymous_170 /* non-c-typedef-for-linkage */, 13530, /* DiagArray624 */ 1735, 0, R"()")
DIAG_ENTRY(NonGCC /* non-gcc */, 13556, 0, /* DiagSubGroup625 */ 509, R"()")
DIAG_ENTRY(NonLiteralNullConversion /* non-literal-null-conversion */, 13564, /* DiagArray626 */ 1737, 0, R"()")
DIAG_ENTRY(NonModularIncludeInFrameworkModule /* non-modular-include-in-framework-module */, 13592, /* DiagArray627 */ 1739, 0, R"()")
DIAG_ENTRY(NonModularIncludeInModule /* non-modular-include-in-module */, 13632, /* DiagArray628 */ 1741, /* DiagSubGroup628 */ 513, R"()")
DIAG_ENTRY(NonPODVarargs /* non-pod-varargs */, 13662, /* DiagArray629 */ 1743, 0, R"()")
DIAG_ENTRY(anonymous_202 /* non-power-of-two-alignment */, 13678, /* DiagArray630 */ 1748, 0, R"()")
DIAG_ENTRY(NonVirtualDtor /* non-virtual-dtor */, 13705, /* DiagArray631 */ 1750, 0, R"()")
DIAG_ENTRY(NonNull /* nonnull */, 13722, /* DiagArray632 */ 1752, 0, R"()")
DIAG_ENTRY(anonymous_37 /* nonportable-cfstrings */, 13730, 0, 0, R"()")
DIAG_ENTRY(anonymous_123 /* nonportable-include-path */, 13752, /* DiagArray634 */ 1755, 0, R"()")
DIAG_ENTRY(anonymous_89 /* nonportable-private-apinotes-path */, 13777, /* DiagArray635 */ 1757, 0, R"()")
DIAG_ENTRY(anonymous_90 /* nonportable-private-system-apinotes-path */, 13811, /* DiagArray636 */ 1759, 0, R"()")
DIAG_ENTRY(anonymous_307 /* nonportable-sycl */, 13852, /* DiagArray637 */ 1761, 0, R"()")
DIAG_ENTRY(anonymous_124 /* nonportable-system-include-path */, 13869, /* DiagArray638 */ 1763, 0, R"()")
DIAG_ENTRY(anonymous_297 /* nonportable-vector-initialization */, 13901, /* DiagArray639 */ 1765, 0, R"()")
DIAG_ENTRY(NonTrivialMemaccess /* nontrivial-memaccess */, 13935, /* DiagArray640 */ 1767, /* DiagSubGroup640 */ 515, R"()")
DIAG_ENTRY(NonTrivialMemcall /* nontrivial-memcall */, 13956, /* DiagArray641 */ 1769, 0, R"()")
DIAG_ENTRY(NSConsumedMismatch /* nsconsumed-mismatch */, 13975, /* DiagArray642 */ 1771, 0, R"()")
DIAG_ENTRY(NSReturnsMismatch /* nsreturns-mismatch */, 13995, /* DiagArray643 */ 1773, 0, R"()")
DIAG_ENTRY(NullArithmetic /* null-arithmetic */, 14014, /* DiagArray644 */ 1775, 0, R"()")
DIAG_ENTRY(NullCharacter /* null-character */, 14030, /* DiagArray645 */ 1778, 0, R"()")
DIAG_ENTRY(NullConversion /* null-conversion */, 14045, /* DiagArray646 */ 1781, 0, R"()")
DIAG_ENTRY(NullDereference /* null-dereference */, 14061, /* DiagArray647 */ 1783, 0, R"()")
DIAG_ENTRY(NullPointerArithmetic /* null-pointer-arithmetic */, 14078, /* DiagArray648 */ 1786, /* DiagSubGroup648 */ 517, R"()")
DIAG_ENTRY(NullPointerSubtraction /* null-pointer-subtraction */, 14102, /* DiagArray649 */ 1788, 0, R"()")
DIAG_ENTRY(Nullability /* nullability */, 14127, /* DiagArray650 */ 1790, 0, R"()")
DIAG_ENTRY(NullabilityCompleteness /* nullability-completeness */, 14139, /* DiagArray651 */ 1796, /* DiagSubGroup651 */ 519, R"()")
DIAG_ENTRY(NullabilityCompletenessOnArrays /* nullability-completeness-on-arrays */, 14164, /* DiagArray652 */ 1798, 0, R"()")
DIAG_ENTRY(NullabilityDeclSpec /* nullability-declspec */, 14199, /* DiagArray653 */ 1800, 0, R"()")
DIAG_ENTRY(anonymous_141 /* nullability-extension */, 14220, /* DiagArray654 */ 1802, 0, R"()")
DIAG_ENTRY(NullabilityInferredOnNestedType /* nullability-inferred-on-nested-type */, 14242, /* DiagArray655 */ 1804, 0, R"()")
DIAG_ENTRY(NullableToNonNullConversion /* nullable-to-nonnull-conversion */, 14278, /* DiagArray656 */ 1806, 0, R"()")
DIAG_ENTRY(anonymous_278 /* nvcc-compat */, 14309, /* DiagArray657 */ 1808, 0, R"()")
DIAG_ENTRY(anonymous_179 /* objc-autosynthesis-property-ivar-name-match */, 14321, /* DiagArray658 */ 1810, 0, R"()")
DIAG_ENTRY(ObjCBoolConstantConversion /* objc-bool-constant-conversion */, 14365, /* DiagArray659 */ 1812, 0, R"()")
DIAG_ENTRY(ObjCBoxing /* objc-boxing */, 14395, /* DiagArray660 */ 1814, 0, R"()")
DIAG_ENTRY(anonymous_262 /* objc-circular-container */, 14407, /* DiagArray661 */ 1816, 0, R"()")
DIAG_ENTRY(ObjCCocoaAPI /* objc-cocoa-api */, 14431, 0, /* DiagSubGroup662 */ 521, R"()")
DIAG_ENTRY(ObjCDesignatedInit /* objc-designated-initializers */, 14446, /* DiagArray663 */ 1818, 0, R"()")
DIAG_ENTRY(anonymous_199 /* objc-dictionary-duplicate-keys */, 14475, /* DiagArray664 */ 1825, 0, R"()")
DIAG_ENTRY(anonymous_85 /* objc-duplicate-category-definition */, 14506, /* DiagArray665 */ 1827, 0, R"()")
DIAG_ENTRY(ObjCFlexibleArray /* objc-flexible-array */, 14541, /* DiagArray666 */ 1829, 0, R"()")
DIAG_ENTRY(anonymous_238 /* objc-forward-class-redefinition */, 14561, /* DiagArray667 */ 1832, 0, R"()")
DIAG_ENTRY(anonymous_239 /* objc-interface-ivars */, 14593, /* DiagArray668 */ 1834, 0, R"()")
DIAG_ENTRY(ObjCLiteralComparison /* objc-literal-compare */, 14614, /* DiagArray669 */ 1836, /* DiagSubGroup669 */ 523, R"()")
DIAG_ENTRY(ObjCLiteralConversion /* objc-literal-conversion */, 14635, /* DiagArray670 */ 1838, 0, R"()")
DIAG_ENTRY(anonymous_131 /* objc-macro-redefinition */, 14659, /* DiagArray671 */ 1841, 0, R"()")
DIAG_ENTRY(anonymous_184 /* objc-messaging-id */, 14683, /* DiagArray672 */ 1843, 0, R"()")
DIAG_ENTRY(MethodAccess /* objc-method-access */, 14701, /* DiagArray673 */ 1845, 0, R"()")
DIAG_ENTRY(anonymous_180 /* objc-missing-property-synthesis */, 14720, /* DiagArray674 */ 1852, 0, R"()")
DIAG_ENTRY(ObjCMissingSuperCalls /* objc-missing-super-calls */, 14752, /* DiagArray675 */ 1854, 0, R"()")
DIAG_ENTRY(ObjCMultipleMethodNames /* objc-multiple-method-names */, 14777, /* DiagArray676 */ 1856, 0, R"()")
DIAG_ENTRY(ObjCRetainBlockProperty /* objc-noncopy-retain-block-property */, 14804, /* DiagArray677 */ 1858, 0, R"()")
DIAG_ENTRY(ObjCNonUnifiedException /* objc-nonunified-exceptions */, 14839, /* DiagArray678 */ 1860, 0, R"()")
DIAG_ENTRY(ObjCPropertyAssignOnObjectType /* objc-property-assign-on-object-type */, 14866, /* DiagArray679 */ 1862, 0, R"()")
DIAG_ENTRY(ObjCPropertyImpl /* objc-property-implementation */, 14902, /* DiagArray680 */ 1864, 0, R"()")
DIAG_ENTRY(anonymous_175 /* objc-property-implicit-mismatch */, 14931, /* DiagArray681 */ 1869, 0, R"()")
DIAG_ENTRY(anonymous_177 /* objc-property-matches-cocoa-ownership-rule */, 14963, /* DiagArray682 */ 1871, 0, R"()")
DIAG_ENTRY(ObjCPropertyNoAttribute /* objc-property-no-attribute */, 15006, /* DiagArray683 */ 1873, 0, R"()")
DIAG_ENTRY(ObjCNoPropertyAutoSynthesis /* objc-property-synthesis */, 15033, /* DiagArray684 */ 1876, 0, R"()")
DIAG_ENTRY(ObjCProtocolMethodImpl /* objc-protocol-method-implementation */, 15057, /* DiagArray685 */ 1880, 0, R"()")
DIAG_ENTRY(anonymous_178 /* objc-protocol-property-synthesis */, 15093, /* DiagArray686 */ 1882, 0, R"()")
DIAG_ENTRY(ObjCProtocolQualifiers /* objc-protocol-qualifiers */, 15126, /* DiagArray687 */ 1884, 0, R"()")
DIAG_ENTRY(ObjCReadonlyPropertyHasSetter /* objc-readonly-with-setter-property */, 15151, /* DiagArray688 */ 1886, 0, R"()")
DIAG_ENTRY(ObjCRedundantAPIUse /* objc-redundant-api-use */, 15186, 0, /* DiagSubGroup689 */ 525, R"()")
DIAG_ENTRY(ObjCRedundantLiteralUse /* objc-redundant-literal-use */, 15209, /* DiagArray690 */ 1888, 0, R"()")
DIAG_ENTRY(ObjCRootClass /* objc-root-class */, 15236, /* DiagArray691 */ 1890, 0, R"()")
DIAG_ENTRY(ObjCSignedCharBool /* objc-signed-char-bool */, 15252, 0, /* DiagSubGroup692 */ 527, R"()")
DIAG_ENTRY(ObjCSignedCharBoolImplicitFloatConversion /* objc-signed-char-bool-implicit-float-conversion */, 15274, /* DiagArray693 */ 1892, 0, R"()")
DIAG_ENTRY(ObjCSignedCharBoolImplicitIntConversion /* objc-signed-char-bool-implicit-int-conversion */, 15322, /* DiagArray694 */ 1894, 0, R"()")
DIAG_ENTRY(ObjCStringComparison /* objc-string-compare */, 15368, /* DiagArray695 */ 1896, 0, R"()")
DIAG_ENTRY(ObjCStringConcatenation /* objc-string-concatenation */, 15388, /* DiagArray696 */ 1898, 0, R"()")
DIAG_ENTRY(anonymous_263 /* objc-unsafe-perform-selector */, 15414, /* DiagArray697 */ 1900, 0, R"()")
DIAG_ENTRY(ODR /* odr */, 15443, /* DiagArray698 */ 1902, 0, R"()")
DIAG_ENTRY(OldStyleCast /* old-style-cast */, 15447, /* DiagArray699 */ 1921, 0, R"()")
DIAG_ENTRY(anonymous_39 /* old-style-definition */, 15462, 0, 0, R"()")
DIAG_ENTRY(OpenACC /* openacc */, 15483, 0, /* DiagSubGroup701 */ 532, R"()")
DIAG_ENTRY(anonymous_310 /* openacc-deprecated-clause-alias */, 15491, /* DiagArray702 */ 1923, 0, R"()")
DIAG_ENTRY(anonymous_309 /* openacc-self-if-potential-conflict */, 15523, /* DiagArray703 */ 1925, 0, R"()")
DIAG_ENTRY(OpenCLUnsupportedRGBA /* opencl-unsupported-rgba */, 15558, /* DiagArray704 */ 1927, 0, R"()")
DIAG_ENTRY(OpenMP /* openmp */, 15582, 0, /* DiagSubGroup705 */ 534, R"()")
DIAG_ENTRY(OpenMP51Ext /* openmp-51-extensions */, 15589, /* DiagArray706 */ 1929, 0, R"()")
DIAG_ENTRY(OpenMPClauses /* openmp-clauses */, 15610, /* DiagArray707 */ 1931, 0, R"()")
DIAG_ENTRY(OpenMPExtensions /* openmp-extensions */, 15625, /* DiagArray708 */ 1951, 0, R"()")
DIAG_ENTRY(OpenMPLoopForm /* openmp-loop-form */, 15643, /* DiagArray709 */ 1953, 0, R"()")
DIAG_ENTRY(OpenMPMapping /* openmp-mapping */, 15660, /* DiagArray710 */ 1956, 0, R"()")
DIAG_ENTRY(OpenMPTarget /* openmp-target */, 15675, /* DiagArray711 */ 1958, /* DiagSubGroup711 */ 543, R"()")
DIAG_ENTRY(OpenMPTargetException /* openmp-target-exception */, 15689, /* DiagArray712 */ 1962, 0, R"()")
DIAG_ENTRY(OptionIgnored /* option-ignored */, 15713, /* DiagArray713 */ 1965, 0, R"()")
DIAG_ENTRY(OrderedCompareFunctionPointers /* ordered-compare-function-pointers */, 15728, /* DiagArray714 */ 1983, 0, R"()")
DIAG_ENTRY(OutOfLineDeclaration /* out-of-line-declaration */, 15762, /* DiagArray715 */ 1986, 0, R"()")
DIAG_ENTRY(anonymous_162 /* out-of-scope-function */, 15786, /* DiagArray716 */ 1988, 0, R"()")
DIAG_ENTRY(OveralignedType /* over-aligned */, 15808, /* DiagArray717 */ 1990, 0, R"()")
DIAG_ENTRY(anonymous_40 /* overflow */, 15821, 0, 0, R"()")
DIAG_ENTRY(OverlengthStrings /* overlength-strings */, 15830, /* DiagArray719 */ 1992, 0, R"()")
DIAG_ENTRY(OverloadedShiftOpParentheses /* overloaded-shift-op-parentheses */, 15849, /* DiagArray720 */ 1994, 0, R"()")
DIAG_ENTRY(OverloadedVirtual /* overloaded-virtual */, 15881, /* DiagArray721 */ 1996, 0, R"()")
DIAG_ENTRY(anonymous_36 /* override-init */, 15900, 0, /* DiagSubGroup722 */ 545, R"()")
DIAG_ENTRY(anonymous_104 /* override-module */, 15914, /* DiagArray723 */ 1998, 0, R"()")
DIAG_ENTRY(OverridingMethodMismatch /* overriding-method-mismatch */, 15930, /* DiagArray724 */ 2000, 0, R"()")
DIAG_ENTRY(anonymous_92 /* overriding-option */, 15957, /* DiagArray725 */ 2008, 0, R"()")
DIAG_ENTRY(Packed /* packed */, 15975, /* DiagArray726 */ 2010, /* DiagSubGroup726 */ 547, R"()")
DIAG_ENTRY(PackedNonPod /* packed-non-pod */, 15982, /* DiagArray727 */ 2012, 0, R"()")
DIAG_ENTRY(Padded /* padded */, 15997, /* DiagArray728 */ 2014, /* DiagSubGroup728 */ 549, R"()")
DIAG_ENTRY(PaddedBitField /* padded-bitfield */, 16004, /* DiagArray729 */ 2018, 0, R"()")
DIAG_ENTRY(Parentheses /* parentheses */, 16020, /* DiagArray730 */ 2021, /* DiagSubGroup730 */ 551, R"()")
DIAG_ENTRY(ParenthesesOnEquality /* parentheses-equality */, 16032, /* DiagArray731 */ 2026, 0, R"()")
DIAG_ENTRY(anonymous_8 /* partial-availability */, 16053, 0, /* DiagSubGroup732 */ 560, R"()")
DIAG_ENTRY(BackendOptimizationRemark /* pass */, 16074, /* DiagArray733 */ 2028, 0, R"()")
DIAG_ENTRY(BackendOptimizationRemarkAnalysis /* pass-analysis */, 16079, /* DiagArray734 */ 2030, 0, R"()")
DIAG_ENTRY(BackendOptimizationFailure /* pass-failed */, 16093, /* DiagArray735 */ 2034, 0, R"()")
DIAG_ENTRY(BackendOptimizationRemarkMissed /* pass-missed */, 16105, /* DiagArray736 */ 2036, 0, R"()")
DIAG_ENTRY(anonymous_313 /* pch-date-time */, 16117, /* DiagArray737 */ 2038, 0, R"()")
DIAG_ENTRY(anonymous_311 /* pch-vfs-diff */, 16131, /* DiagArray738 */ 2040, 0, R"()")
DIAG_ENTRY(Pedantic /* pedantic */, 16144, /* DiagArray739 */ 2042, /* DiagSubGroup739 */ 562, R"()")
DIAG_ENTRY(OpenCLCoreFeaturesDiagGroup /* pedantic-core-features */, 16153, /* DiagArray740 */ 2128, 0, R"()")
DIAG_ENTRY(PedanticMacros /* pedantic-macros */, 16176, 0, /* DiagSubGroup741 */ 619, R"()")
DIAG_ENTRY(PerfConstraintImpliesNoexcept /* perf-constraint-implies-noexcept */, 16192, /* DiagArray742 */ 2131, 0, R"()")
DIAG_ENTRY(PessimizingMove /* pessimizing-move */, 16225, /* DiagArray743 */ 2133, 0, R"()")
DIAG_ENTRY(PointerArith /* pointer-arith */, 16242, /* DiagArray744 */ 2136, /* DiagSubGroup744 */ 625, R"()")
DIAG_ENTRY(PointerBoolConversion /* pointer-bool-conversion */, 16256, /* DiagArray745 */ 2140, 0, R"()")
DIAG_ENTRY(anonymous_214 /* pointer-compare */, 16280, /* DiagArray746 */ 2143, 0, R"()")
DIAG_ENTRY(anonymous_261 /* pointer-integer-compare */, 16296, /* DiagArray747 */ 2145, 0, R"()")
DIAG_ENTRY(anonymous_273 /* pointer-sign */, 16320, /* DiagArray748 */ 2147, 0, R"()")
DIAG_ENTRY(PointerToEnumCast /* pointer-to-enum-cast */, 16333, /* DiagArray749 */ 2149, /* DiagSubGroup749 */ 627, R"()")
DIAG_ENTRY(PointerToIntCast /* pointer-to-int-cast */, 16354, /* DiagArray750 */ 2151, /* DiagSubGroup750 */ 629, R"()")
DIAG_ENTRY(anonymous_280 /* pointer-type-mismatch */, 16374, /* DiagArray751 */ 2153, 0, R"()")
DIAG_ENTRY(anonymous_91 /* poison-system-directories */, 16396, /* DiagArray752 */ 2155, 0, R"()")
DIAG_ENTRY(ObjCPotentiallyDirectSelector /* potentially-direct-selector */, 16422, /* DiagArray753 */ 2157, 0, R"()")
DIAG_ENTRY(PotentiallyEvaluatedExpression /* potentially-evaluated-expression */, 16450, /* DiagArray754 */ 2159, 0, R"()")
DIAG_ENTRY(PragmaClangAttribute /* pragma-clang-attribute */, 16483, /* DiagArray755 */ 2161, 0, R"()")
DIAG_ENTRY(anonymous_125 /* pragma-once-outside-header */, 16506, /* DiagArray756 */ 2163, 0, R"()")
DIAG_ENTRY(PragmaPack /* pragma-pack */, 16533, /* DiagArray757 */ 2165, /* DiagSubGroup757 */ 632, R"()")
DIAG_ENTRY(PragmaPackSuspiciousInclude /* pragma-pack-suspicious-include */, 16545, /* DiagArray758 */ 2168, 0, R"()")
DIAG_ENTRY(anonymous_126 /* pragma-system-header-outside-header */, 16576, /* DiagArray759 */ 2170, 0, R"()")
DIAG_ENTRY(Pragmas /* pragmas */, 16612, /* DiagArray760 */ 2172, /* DiagSubGroup760 */ 634, R"()")
DIAG_ENTRY(CXXPre14Compat /* pre-c++14-compat */, 16620, /* DiagArray761 */ 2175, 0, R"()")
DIAG_ENTRY(CXXPre14CompatPedantic /* pre-c++14-compat-pedantic */, 16637, 0, /* DiagSubGroup762 */ 639, R"()")
DIAG_ENTRY(CXXPre17Compat /* pre-c++17-compat */, 16663, /* DiagArray763 */ 2187, 0, R"()")
DIAG_ENTRY(CXXPre17CompatPedantic /* pre-c++17-compat-pedantic */, 16680, /* DiagArray764 */ 2205, /* DiagSubGroup764 */ 642, R"()")
DIAG_ENTRY(CXXPre20Compat /* pre-c++20-compat */, 16706, /* DiagArray765 */ 2208, 0, R"()")
DIAG_ENTRY(CXXPre20CompatPedantic /* pre-c++20-compat-pedantic */, 16723, /* DiagArray766 */ 2237, /* DiagSubGroup766 */ 644, R"()")
DIAG_ENTRY(CXXPre23Compat /* pre-c++23-compat */, 16749, /* DiagArray767 */ 2240, 0, R"()")
DIAG_ENTRY(CXXPre23CompatPedantic /* pre-c++23-compat-pedantic */, 16766, 0, /* DiagSubGroup768 */ 646, R"()")
DIAG_ENTRY(CXXPre26Compat /* pre-c++26-compat */, 16792, /* DiagArray769 */ 2255, 0, R"()")
DIAG_ENTRY(CXXPre26CompatPedantic /* pre-c++26-compat-pedantic */, 16809, 0, /* DiagSubGroup770 */ 648, R"()")
DIAG_ENTRY(anonymous_21 /* pre-c++2c-compat */, 16835, 0, /* DiagSubGroup771 */ 650, R"()")
DIAG_ENTRY(anonymous_22 /* pre-c++2c-compat-pedantic */, 16852, 0, /* DiagSubGroup772 */ 652, R"()")
DIAG_ENTRY(CPre11Compat /* pre-c11-compat */, 16878, /* DiagArray773 */ 2264, 0, R"()")
DIAG_ENTRY(CPre11CompatPedantic /* pre-c11-compat-pedantic */, 16893, 0, /* DiagSubGroup774 */ 654, R"()")
DIAG_ENTRY(CPre23Compat /* pre-c23-compat */, 16917, /* DiagArray775 */ 2266, /* DiagSubGroup775 */ 656, R"()")
DIAG_ENTRY(CPre23CompatPedantic /* pre-c23-compat-pedantic */, 16932, 0, /* DiagSubGroup776 */ 658, R"()")
DIAG_ENTRY(anonymous_13 /* pre-c2x-compat */, 16956, 0, /* DiagSubGroup777 */ 660, R"()")
DIAG_ENTRY(anonymous_14 /* pre-c2x-compat-pedantic */, 16971, 0, /* DiagSubGroup778 */ 662, R"()")
DIAG_ENTRY(CPre2yCompat /* pre-c2y-compat */, 16995, /* DiagArray779 */ 2285, 0, R"()")
DIAG_ENTRY(CPre2yCompatPedantic /* pre-c2y-compat-pedantic */, 17010, 0, /* DiagSubGroup780 */ 664, R"()")
DIAG_ENTRY(OpenMPPre51Compat /* pre-openmp-51-compat */, 17034, /* DiagArray781 */ 2291, 0, R"()")
DIAG_ENTRY(anonymous_156 /* predefined-identifier-outside-function */, 17055, /* DiagArray782 */ 2293, 0, R"()")
DIAG_ENTRY(PrivateExtern /* private-extern */, 17094, /* DiagArray783 */ 2295, 0, R"()")
DIAG_ENTRY(anonymous_137 /* private-header */, 17109, /* DiagArray784 */ 2297, 0, R"()")
DIAG_ENTRY(PrivateModule /* private-module */, 17124, /* DiagArray785 */ 2299, 0, R"()")
DIAG_ENTRY(ProfileInstrMissing /* profile-instr-missing */, 17139, /* DiagArray786 */ 2304, 0, R"()")
DIAG_ENTRY(ProfileInstrOutOfDate /* profile-instr-out-of-date */, 17161, /* DiagArray787 */ 2306, 0, R"()")
DIAG_ENTRY(ProfileInstrUnprofiled /* profile-instr-unprofiled */, 17187, /* DiagArray788 */ 2308, 0, R"()")
DIAG_ENTRY(PropertyAccessDotSyntax /* property-access-dot-syntax */, 17212, /* DiagArray789 */ 2310, 0, R"()")
DIAG_ENTRY(PropertyAttr /* property-attribute-mismatch */, 17239, /* DiagArray790 */ 2312, 0, R"()")
DIAG_ENTRY(Protocol /* protocol */, 17267, /* DiagArray791 */ 2317, 0, R"()")
DIAG_ENTRY(anonymous_173 /* protocol-property-synthesis-ambiguity */, 17276, /* DiagArray792 */ 2319, 0, R"()")
DIAG_ENTRY(anonymous_109 /* psabi */, 17314, /* DiagArray793 */ 2321, 0, R"()")
DIAG_ENTRY(PtrAuthNullPointers /* ptrauth-null-pointers */, 17320, /* DiagArray794 */ 2323, 0, R"()")
DIAG_ENTRY(anonymous_244 /* qualified-void-return-type */, 17342, /* DiagArray795 */ 2326, 0, R"()")
DIAG_ENTRY(FrameworkHdrQuotedInclude /* quoted-include-in-framework-header */, 17369, /* DiagArray796 */ 2328, 0, R"()")
DIAG_ENTRY(RangeLoopAnalysis /* range-loop-analysis */, 17404, 0, /* DiagSubGroup797 */ 666, R"()")
DIAG_ENTRY(RangeLoopBindReference /* range-loop-bind-reference */, 17424, /* DiagArray798 */ 2330, 0, R"()")
DIAG_ENTRY(RangeLoopConstruct /* range-loop-construct */, 17450, /* DiagArray799 */ 2332, 0, R"()")
DIAG_ENTRY(ReadOnlyPlacementChecks /* read-only-types */, 17471, /* DiagArray800 */ 2335, 0, R"()")
DIAG_ENTRY(anonymous_183 /* readonly-iboutlet-property */, 17487, /* DiagArray801 */ 2337, 0, R"()")
DIAG_ENTRY(ObjCReceiver /* receiver-expr */, 17514, /* DiagArray802 */ 2339, 0, R"()")
DIAG_ENTRY(ForwardClassReceiver /* receiver-forward-class */, 17528, /* DiagArray803 */ 2341, 0, R"()")
DIAG_ENTRY(RedeclaredClassMember /* redeclared-class-member */, 17551, /* DiagArray804 */ 2344, 0, R"()")
DIAG_ENTRY(RedundantAttribute /* redundant-attribute */, 17575, /* DiagArray805 */ 2346, 0, R"()")
DIAG_ENTRY(anonymous_186 /* redundant-consteval-if */, 17595, /* DiagArray806 */ 2348, 0, R"()")
DIAG_ENTRY(anonymous_41 /* redundant-decls */, 17618, 0, 0, R"()")
DIAG_ENTRY(RedundantMove /* redundant-move */, 17634, /* DiagArray808 */ 2350, 0, R"()")
DIAG_ENTRY(anonymous_160 /* redundant-parens */, 17649, /* DiagArray809 */ 2352, 0, R"()")
DIAG_ENTRY(Register /* register */, 17666, /* DiagArray810 */ 2354, /* DiagSubGroup810 */ 669, R"()")
DIAG_ENTRY(ReinterpretBaseClass /* reinterpret-base-class */, 17675, /* DiagArray811 */ 2356, 0, R"()")
DIAG_ENTRY(RemarkBackendPlugin /* remark-backend-plugin */, 17698, /* DiagArray812 */ 2358, 0, R"()")
DIAG_ENTRY(Reorder /* reorder */, 17720, 0, /* DiagSubGroup813 */ 671, R"()")
DIAG_ENTRY(ReorderCtor /* reorder-ctor */, 17728, /* DiagArray814 */ 2360, 0, R"()")
DIAG_ENTRY(ReorderInitList /* reorder-init-list */, 17741, /* DiagArray815 */ 2363, 0, R"()")
DIAG_ENTRY(anonymous_220 /* requires-super-attribute */, 17759, /* DiagArray816 */ 2365, 0, R"()")
DIAG_ENTRY(ReservedAttributeIdentifier /* reserved-attribute-identifier */, 17784, /* DiagArray817 */ 2367, 0, R"()")
DIAG_ENTRY(ReservedIdAsMacroAlias /* reserved-id-macro */, 17814, 0, /* DiagSubGroup818 */ 674, R"()")
DIAG_ENTRY(ReservedIdentifier /* reserved-identifier */, 17832, /* DiagArray819 */ 2369, /* DiagSubGroup819 */ 676, R"()")
DIAG_ENTRY(ReservedIdAsMacro /* reserved-macro-identifier */, 17852, /* DiagArray820 */ 2371, 0, R"()")
DIAG_ENTRY(ReservedModuleIdentifier /* reserved-module-identifier */, 17878, /* DiagArray821 */ 2373, 0, R"()")
DIAG_ENTRY(ReservedUserDefinedLiteral /* reserved-user-defined-literal */, 17905, /* DiagArray822 */ 2375, /* DiagSubGroup822 */ 681, R"()")
DIAG_ENTRY(RestrictExpansionMacro /* restrict-expansion */, 17935, /* DiagArray823 */ 2378, 0, R"()")
DIAG_ENTRY(anonymous_237 /* retained-language-linkage */, 17954, /* DiagArray824 */ 2380, 0, R"()")
DIAG_ENTRY(anonymous_29 /* return-local-addr */, 17980, 0, /* DiagSubGroup825 */ 683, R"()")
DIAG_ENTRY(ReturnMismatch /* return-mismatch */, 17998, /* DiagArray826 */ 2382, 0, R"()")
DIAG_ENTRY(ReturnStackAddress /* return-stack-address */, 18014, /* DiagArray827 */ 2386, 0, R"()")
DIAG_ENTRY(ReturnStdMove /* return-std-move */, 18035, 0, 0, R"()")
DIAG_ENTRY(ReturnType /* return-type */, 18051, /* DiagArray829 */ 2391, /* DiagSubGroup829 */ 685, R"()")
DIAG_ENTRY(ReturnTypeCLinkage /* return-type-c-linkage */, 18063, /* DiagArray830 */ 2398, 0, R"()")
DIAG_ENTRY(anonymous_222 /* rewrite-not-bool */, 18085, /* DiagArray831 */ 2401, 0, R"()")
DIAG_ENTRY(RoundTripCC1Args /* round-trip-cc1-args */, 18102, /* DiagArray832 */ 2403, 0, R"()")
DIAG_ENTRY(RTTI /* rtti */, 18122, /* DiagArray833 */ 2405, 0, R"()")
DIAG_ENTRY(SanitizeAddressRemarks /* sanitize-address */, 18127, /* DiagArray834 */ 2408, 0, R"()")
DIAG_ENTRY(anonymous_101 /* sarif-format-unstable */, 18144, /* DiagArray835 */ 2411, 0, R"()")
DIAG_ENTRY(UsedSearchPath /* search-path-usage */, 18166, /* DiagArray836 */ 2413, 0, R"()")
DIAG_ENTRY(Section /* section */, 18184, /* DiagArray837 */ 2415, 0, R"()")
DIAG_ENTRY(Selector /* selector */, 18192, /* DiagArray838 */ 2419, /* DiagSubGroup838 */ 688, R"()")
DIAG_ENTRY(SelectorTypeMismatch /* selector-type-mismatch */, 18201, /* DiagArray839 */ 2421, 0, R"()")
DIAG_ENTRY(SelfAssignment /* self-assign */, 18224, /* DiagArray840 */ 2423, /* DiagSubGroup840 */ 690, R"()")
DIAG_ENTRY(SelfAssignmentField /* self-assign-field */, 18236, /* DiagArray841 */ 2425, 0, R"()")
DIAG_ENTRY(SelfAssignmentOverloaded /* self-assign-overloaded */, 18254, /* DiagArray842 */ 2427, 0, R"()")
DIAG_ENTRY(SelfMove /* self-move */, 18277, /* DiagArray843 */ 2429, 0, R"()")
DIAG_ENTRY(SemiBeforeMethodBody /* semicolon-before-method-body */, 18287, /* DiagArray844 */ 2431, 0, R"()")
DIAG_ENTRY(Sentinel /* sentinel */, 18316, /* DiagArray845 */ 2433, 0, R"()")
DIAG_ENTRY(anonymous_44 /* sequence-point */, 18325, 0, /* DiagSubGroup846 */ 693, R"()")
DIAG_ENTRY(SerializedDiagnostics /* serialized-diagnostics */, 18340, /* DiagArray847 */ 2436, 0, R"()")
DIAG_ENTRY(Shadow /* shadow */, 18363, /* DiagArray848 */ 2440, /* DiagSubGroup848 */ 695, R"()")
DIAG_ENTRY(ShadowAll /* shadow-all */, 18370, 0, /* DiagSubGroup849 */ 698, R"()")
DIAG_ENTRY(ShadowField /* shadow-field */, 18381, /* DiagArray850 */ 2442, 0, R"()")
DIAG_ENTRY(ShadowFieldInConstructor /* shadow-field-in-constructor */, 18394, /* DiagArray851 */ 2444, /* DiagSubGroup851 */ 703, R"()")
DIAG_ENTRY(ShadowFieldInConstructorModified /* shadow-field-in-constructor-modified */, 18422, /* DiagArray852 */ 2446, 0, R"()")
DIAG_ENTRY(ShadowIvar /* shadow-ivar */, 18459, /* DiagArray853 */ 2448, 0, R"()")
DIAG_ENTRY(ShadowUncapturedLocal /* shadow-uncaptured-local */, 18471, /* DiagArray854 */ 2450, 0, R"()")
DIAG_ENTRY(anonymous_250 /* shift-count-negative */, 18495, /* DiagArray855 */ 2452, 0, R"()")
DIAG_ENTRY(anonymous_251 /* shift-count-overflow */, 18516, /* DiagArray856 */ 2454, 0, R"()")
DIAG_ENTRY(anonymous_249 /* shift-negative-value */, 18537, /* DiagArray857 */ 2456, 0, R"()")
DIAG_ENTRY(ShiftOpParentheses /* shift-op-parentheses */, 18558, /* DiagArray858 */ 2458, 0, R"()")
DIAG_ENTRY(anonymous_252 /* shift-overflow */, 18579, /* DiagArray859 */ 2460, 0, R"()")
DIAG_ENTRY(anonymous_253 /* shift-sign-overflow */, 18594, /* DiagArray860 */ 2462, 0, R"()")
DIAG_ENTRY(Shorten64To32 /* shorten-64-to-32 */, 18614, /* DiagArray861 */ 2464, 0, R"()")
DIAG_ENTRY(SignCompare /* sign-compare */, 18631, /* DiagArray862 */ 2466, 0, R"()")
DIAG_ENTRY(SignConversion /* sign-conversion */, 18644, /* DiagArray863 */ 2468, 0, R"()")
DIAG_ENTRY(anonymous_42 /* sign-promo */, 18660, 0, 0, R"()")
DIAG_ENTRY(SignedEnumBitfield /* signed-enum-bitfield */, 18671, /* DiagArray865 */ 2472, 0, R"()")
DIAG_ENTRY(anonymous_299 /* signed-unsigned-wchar */, 18692, /* DiagArray866 */ 2474, 0, R"()")
DIAG_ENTRY(SingleBitBitFieldConstantConversion /* single-bit-bitfield-constant-conversion */, 18714, /* DiagArray867 */ 2476, 0, R"()")
DIAG_ENTRY(SizeofArrayArgument /* sizeof-array-argument */, 18754, /* DiagArray868 */ 2478, 0, R"()")
DIAG_ENTRY(SizeofArrayDecay /* sizeof-array-decay */, 18776, /* DiagArray869 */ 2480, 0, R"()")
DIAG_ENTRY(anonymous_216 /* sizeof-array-div */, 18795, /* DiagArray870 */ 2482, 0, R"()")
DIAG_ENTRY(anonymous_215 /* sizeof-pointer-div */, 18812, /* DiagArray871 */ 2484, 0, R"()")
DIAG_ENTRY(SizeofPointerMemaccess /* sizeof-pointer-memaccess */, 18831, /* DiagArray872 */ 2486, 0, R"()")
DIAG_ENTRY(anonymous_97 /* slash-u-filename */, 18856, /* DiagArray873 */ 2489, 0, R"()")
DIAG_ENTRY(anonymous_84 /* slh-asm-goto */, 18873, /* DiagArray874 */ 2491, 0, R"()")
DIAG_ENTRY(anonymous_87 /* sloc-usage */, 18886, /* DiagArray875 */ 2493, 0, R"()")
DIAG_ENTRY(UninitializedSometimes /* sometimes-uninitialized */, 18897, /* DiagArray876 */ 2495, 0, R"()")
DIAG_ENTRY(BackendSourceMgr /* source-mgr */, 18921, /* DiagArray877 */ 2497, 0, R"()")
DIAG_ENTRY(SourceUsesOpenACC /* source-uses-openacc */, 18932, /* DiagArray878 */ 2499, 0, R"()")
DIAG_ENTRY(SourceUsesOpenMP /* source-uses-openmp */, 18952, /* DiagArray879 */ 2503, 0, R"()")
DIAG_ENTRY(SpirCompat /* spir-compat */, 18971, /* DiagArray880 */ 2512, 0, R"()")
DIAG_ENTRY(anonymous_77 /* spirv-compat */, 18983, 0, /* DiagSubGroup881 */ 705, R"()")
DIAG_ENTRY(anonymous_81 /* stack-exhausted */, 18996, /* DiagArray882 */ 2514, 0, R"()")
DIAG_ENTRY(anonymous_83 /* stack-protector */, 19012, /* DiagArray883 */ 2516, 0, R"()")
DIAG_ENTRY(StaticFloatInit /* static-float-init */, 19028, /* DiagArray884 */ 2518, /* DiagSubGroup884 */ 707, R"()")
DIAG_ENTRY(StaticInInline /* static-in-inline */, 19046, /* DiagArray885 */ 2520, 0, R"()")
DIAG_ENTRY(anonymous_153 /* static-inline-explicit-instantiation */, 19063, /* DiagArray886 */ 2523, 0, R"()")
DIAG_ENTRY(StaticLocalInInline /* static-local-in-inline */, 19100, /* DiagArray887 */ 2525, 0, R"()")
DIAG_ENTRY(UninitializedStaticSelfInit /* static-self-init */, 19123, /* DiagArray888 */ 2527, 0, R"()")
DIAG_ENTRY(anonymous_100 /* stdlibcxx-not-found */, 19140, /* DiagArray889 */ 2529, 0, R"()")
DIAG_ENTRY(anonymous_48 /* strict-aliasing */, 19160, 0, 0, R"()")
DIAG_ENTRY(anonymous_45 /* strict-aliasing=0 */, 19176, 0, 0, R"()")
DIAG_ENTRY(anonymous_46 /* strict-aliasing=1 */, 19194, 0, 0, R"()")
DIAG_ENTRY(anonymous_47 /* strict-aliasing=2 */, 19212, 0, 0, R"()")
DIAG_ENTRY(anonymous_55 /* strict-overflow */, 19230, 0, 0, R"()")
DIAG_ENTRY(anonymous_49 /* strict-overflow=0 */, 19246, 0, 0, R"()")
DIAG_ENTRY(anonymous_50 /* strict-overflow=1 */, 19264, 0, 0, R"()")
DIAG_ENTRY(anonymous_51 /* strict-overflow=2 */, 19282, 0, 0, R"()")
DIAG_ENTRY(anonymous_52 /* strict-overflow=3 */, 19300, 0, 0, R"()")
DIAG_ENTRY(anonymous_53 /* strict-overflow=4 */, 19318, 0, 0, R"()")
DIAG_ENTRY(anonymous_54 /* strict-overflow=5 */, 19336, 0, 0, R"()")
DIAG_ENTRY(ObjCStrictPotentiallyDirectSelector /* strict-potentially-direct-selector */, 19354, /* DiagArray901 */ 2531, /* DiagSubGroup901 */ 709, R"()")
DIAG_ENTRY(anonymous_223 /* strict-primary-template-shadow */, 19389, /* DiagArray902 */ 2533, 0, R"()")
DIAG_ENTRY(StrictPrototypes /* strict-prototypes */, 19420, /* DiagArray903 */ 2535, /* DiagSubGroup903 */ 711, R"()")
DIAG_ENTRY(StrictSelector /* strict-selector-match */, 19438, /* DiagArray904 */ 2537, 0, R"()")
DIAG_ENTRY(StringCompare /* string-compare */, 19460, /* DiagArray905 */ 2539, 0, R"()")
DIAG_ENTRY(StringConcatation /* string-concatenation */, 19475, /* DiagArray906 */ 2541, 0, R"()")
DIAG_ENTRY(StringConversion /* string-conversion */, 19496, /* DiagArray907 */ 2543, 0, R"()")
DIAG_ENTRY(StringPlusChar /* string-plus-char */, 19514, /* DiagArray908 */ 2545, 0, R"()")
DIAG_ENTRY(StringPlusInt /* string-plus-int */, 19531, /* DiagArray909 */ 2547, 0, R"()")
DIAG_ENTRY(anonymous_166 /* strlcpy-strlcat-size */, 19547, /* DiagArray910 */ 2549, 0, R"()")
DIAG_ENTRY(StrncatSize /* strncat-size */, 19568, /* DiagArray911 */ 2551, 0, R"()")
DIAG_ENTRY(CXX11WarnSuggestOverrideDestructor /* suggest-destructor-override */, 19581, /* DiagArray912 */ 2555, 0, R"()")
DIAG_ENTRY(CXX11WarnSuggestOverride /* suggest-override */, 19609, /* DiagArray913 */ 2557, 0, R"()")
DIAG_ENTRY(SuperSubClassMismatch /* super-class-method-mismatch */, 19626, /* DiagArray914 */ 2559, 0, R"()")
DIAG_ENTRY(SuspiciousBzero /* suspicious-bzero */, 19654, /* DiagArray915 */ 2561, 0, R"()")
DIAG_ENTRY(SuspiciousMemaccess /* suspicious-memaccess */, 19671, 0, /* DiagSubGroup916 */ 713, R"()")
DIAG_ENTRY(SwiftNameAttribute /* swift-name-attribute */, 19692, /* DiagArray917 */ 2563, 0, R"()")
DIAG_ENTRY(Switch /* switch */, 19713, /* DiagArray918 */ 2577, 0, R"()")
DIAG_ENTRY(SwitchBool /* switch-bool */, 19720, /* DiagArray919 */ 2581, 0, R"()")
DIAG_ENTRY(SwitchDefault /* switch-default */, 19732, /* DiagArray920 */ 2583, 0, R"()")
DIAG_ENTRY(SwitchEnum /* switch-enum */, 19747, /* DiagArray921 */ 2585, 0, R"()")
DIAG_ENTRY(SyncAlignment /* sync-alignment */, 19759, /* DiagArray922 */ 2587, 0, R"()")
DIAG_ENTRY(anonymous_298 /* sync-fetch-and-nand-semantics-changed */, 19774, /* DiagArray923 */ 2589, 0, R"()")
DIAG_ENTRY(anonymous_43 /* synth */, 19812, 0, 0, R"()")
DIAG_ENTRY(TargetClonesMixedSpecifiers /* target-clones-mixed-specifiers */, 19818, /* DiagArray925 */ 2591, 0, R"()")
DIAG_ENTRY(TautologicalBitwiseCompare /* tautological-bitwise-compare */, 19849, /* DiagArray926 */ 2593, 0, R"()")
DIAG_ENTRY(TautologicalCompare /* tautological-compare */, 19878, /* DiagArray927 */ 2596, /* DiagSubGroup927 */ 719, R"()")
DIAG_ENTRY(TautologicalConstantCompare /* tautological-constant-compare */, 19899, /* DiagArray928 */ 2599, /* DiagSubGroup928 */ 727, R"()")
DIAG_ENTRY(TautologicalInRangeCompare /* tautological-constant-in-range-compare */, 19929, 0, /* DiagSubGroup929 */ 729, R"()")
DIAG_ENTRY(TautologicalOutOfRangeCompare /* tautological-constant-out-of-range-compare */, 19968, /* DiagArray930 */ 2603, 0, R"()")
DIAG_ENTRY(TautologicalNegationCompare /* tautological-negation-compare */, 20011, /* DiagArray931 */ 2605, 0, R"()")
DIAG_ENTRY(TautologicalObjCBoolCompare /* tautological-objc-bool-compare */, 20041, /* DiagArray932 */ 2608, 0, R"()")
DIAG_ENTRY(TautologicalOverlapCompare /* tautological-overlap-compare */, 20072, /* DiagArray933 */ 2610, 0, R"()")
DIAG_ENTRY(TautologicalPointerCompare /* tautological-pointer-compare */, 20101, /* DiagArray934 */ 2612, 0, R"()")
DIAG_ENTRY(TautologicalTypeLimitCompare /* tautological-type-limit-compare */, 20130, /* DiagArray935 */ 2615, 0, R"()")
DIAG_ENTRY(TautologicalUndefinedCompare /* tautological-undefined-compare */, 20162, /* DiagArray936 */ 2617, 0, R"()")
DIAG_ENTRY(TautologicalUnsignedCharZeroCompare /* tautological-unsigned-char-zero-compare */, 20193, /* DiagArray937 */ 2620, 0, R"()")
DIAG_ENTRY(TautologicalUnsignedEnumZeroCompare /* tautological-unsigned-enum-zero-compare */, 20233, /* DiagArray938 */ 2622, 0, R"()")
DIAG_ENTRY(TautologicalUnsignedZeroCompare /* tautological-unsigned-zero-compare */, 20273, /* DiagArray939 */ 2624, 0, R"()")
DIAG_ENTRY(TautologicalValueRangeCompare /* tautological-value-range-compare */, 20308, /* DiagArray940 */ 2626, 0, R"()")
DIAG_ENTRY(anonymous_308 /* tcb-enforcement */, 20341, /* DiagArray941 */ 2628, 0, R"()")
DIAG_ENTRY(anonymous_270 /* template-in-declaration-name */, 20357, /* DiagArray942 */ 2630, 0, R"()")
DIAG_ENTRY(anonymous_257 /* tentative-definition-array */, 20386, /* DiagArray943 */ 2632, 0, R"()")
DIAG_ENTRY(anonymous_256 /* tentative-definition-incomplete-type */, 20413, /* DiagArray944 */ 2634, 0, R"()")
DIAG_ENTRY(ThreadSafety /* thread-safety */, 20450, 0, /* DiagSubGroup945 */ 732, R"()")
DIAG_ENTRY(ThreadSafetyAnalysis /* thread-safety-analysis */, 20464, /* DiagArray946 */ 2636, 0, R"()")
DIAG_ENTRY(ThreadSafetyAttributes /* thread-safety-attributes */, 20487, /* DiagArray947 */ 2658, 0, R"()")
DIAG_ENTRY(ThreadSafetyBeta /* thread-safety-beta */, 20512, /* DiagArray948 */ 2666, 0, R"()")
DIAG_ENTRY(ThreadSafetyNegative /* thread-safety-negative */, 20531, /* DiagArray949 */ 2668, 0, R"()")
DIAG_ENTRY(ThreadSafetyPrecise /* thread-safety-precise */, 20554, /* DiagArray950 */ 2670, 0, R"()")
DIAG_ENTRY(ThreadSafetyReference /* thread-safety-reference */, 20576, /* DiagArray951 */ 2674, /* DiagSubGroup951 */ 737, R"()")
DIAG_ENTRY(ThreadSafetyReferenceReturn /* thread-safety-reference-return */, 20600, /* DiagArray952 */ 2677, 0, R"()")
DIAG_ENTRY(ThreadSafetyVerbose /* thread-safety-verbose */, 20631, /* DiagArray953 */ 2680, 0, R"()")
DIAG_ENTRY(Trigraphs /* trigraphs */, 20653, /* DiagArray954 */ 2682, 0, R"()")
DIAG_ENTRY(TypeLimits /* type-limits */, 20663, 0, /* DiagSubGroup955 */ 739, R"()")
DIAG_ENTRY(TypeSafety /* type-safety */, 20675, /* DiagArray956 */ 2687, 0, R"()")
DIAG_ENTRY(anonymous_236 /* typedef-redefinition */, 20687, /* DiagArray957 */ 2691, 0, R"()")
DIAG_ENTRY(anonymous_230 /* typename-missing */, 20708, /* DiagArray958 */ 2693, 0, R"()")
DIAG_ENTRY(anonymous_105 /* unable-to-open-stats-file */, 20725, /* DiagArray959 */ 2695, 0, R"()")
DIAG_ENTRY(UnalignedAccess /* unaligned-access */, 20751, /* DiagArray960 */ 2697, 0, R"()")
DIAG_ENTRY(anonymous_301 /* unaligned-qualifier-implicit-cast */, 20768, /* DiagArray961 */ 2699, 0, R"()")
DIAG_ENTRY(UnavailableDeclarations /* unavailable-declarations */, 20802, /* DiagArray962 */ 2701, 0, R"()")
DIAG_ENTRY(UndeclaredSelector /* undeclared-selector */, 20827, /* DiagArray963 */ 2703, 0, R"()")
DIAG_ENTRY(anonymous_129 /* undef */, 20847, /* DiagArray964 */ 2706, 0, R"()")
DIAG_ENTRY(anonymous_130 /* undef-prefix */, 20853, /* DiagArray965 */ 2708, 0, R"()")
DIAG_ENTRY(anonymous_196 /* undefined-arm-za */, 20866, /* DiagArray966 */ 2710, 0, R"()")
DIAG_ENTRY(anonymous_197 /* undefined-arm-zt0 */, 20883, /* DiagArray967 */ 2712, 0, R"()")
DIAG_ENTRY(UndefinedBoolConversion /* undefined-bool-conversion */, 20901, /* DiagArray968 */ 2714, 0, R"()")
DIAG_ENTRY(UndefinedFuncTemplate /* undefined-func-template */, 20927, /* DiagArray969 */ 2717, 0, R"()")
DIAG_ENTRY(anonymous_235 /* undefined-inline */, 20951, /* DiagArray970 */ 2719, 0, R"()")
DIAG_ENTRY(anonymous_233 /* undefined-internal */, 20968, /* DiagArray971 */ 2721, 0, R"()")
DIAG_ENTRY(anonymous_234 /* undefined-internal-type */, 20987, /* DiagArray972 */ 2723, 0, R"()")
DIAG_ENTRY(UndefinedReinterpretCast /* undefined-reinterpret-cast */, 21011, /* DiagArray973 */ 2725, 0, R"()")
DIAG_ENTRY(UndefinedVarTemplate /* undefined-var-template */, 21038, /* DiagArray974 */ 2728, 0, R"()")
DIAG_ENTRY(UnderalignedExceptionObject /* underaligned-exception-object */, 21061, /* DiagArray975 */ 2730, 0, R"()")
DIAG_ENTRY(UnevaluatedExpression /* unevaluated-expression */, 21091, /* DiagArray976 */ 2732, /* DiagSubGroup976 */ 744, R"()")
DIAG_ENTRY(UnguardedAvailability /* unguarded-availability */, 21114, /* DiagArray977 */ 2734, /* DiagSubGroup977 */ 746, R"()")
DIAG_ENTRY(UnguardedAvailabilityNew /* unguarded-availability-new */, 21137, /* DiagArray978 */ 2737, 0, R"()")
DIAG_ENTRY(Unicode /* unicode */, 21164, /* DiagArray979 */ 2740, 0, R"()")
DIAG_ENTRY(anonymous_115 /* unicode-homoglyph */, 21172, /* DiagArray980 */ 2748, 0, R"()")
DIAG_ENTRY(anonymous_114 /* unicode-whitespace */, 21190, /* DiagArray981 */ 2750, 0, R"()")
DIAG_ENTRY(anonymous_116 /* unicode-zero-width */, 21209, /* DiagArray982 */ 2752, 0, R"()")
DIAG_ENTRY(Uninitialized /* uninitialized */, 21228, /* DiagArray983 */ 2754, /* DiagSubGroup983 */ 748, R"()")
DIAG_ENTRY(UninitializedConstReference /* uninitialized-const-reference */, 21242, /* DiagArray984 */ 2762, 0, R"()")
DIAG_ENTRY(UninitializedExplicitInit /* uninitialized-explicit-init */, 21272, /* DiagArray985 */ 2764, 0, R"()")
DIAG_ENTRY(UnknownArgument /* unknown-argument */, 21300, /* DiagArray986 */ 2766, 0, R"()")
DIAG_ENTRY(UnknownAssumption /* unknown-assumption */, 21317, /* DiagArray987 */ 2770, 0, R"()")
DIAG_ENTRY(UnknownAttributes /* unknown-attributes */, 21336, /* DiagArray988 */ 2772, 0, R"()")
DIAG_ENTRY(CudaUnknownVersion /* unknown-cuda-version */, 21355, /* DiagArray989 */ 2774, 0, R"()")
DIAG_ENTRY(anonymous_134 /* unknown-directives */, 21376, /* DiagArray990 */ 2777, 0, R"()")
DIAG_ENTRY(anonymous_119 /* unknown-escape-sequence */, 21395, /* DiagArray991 */ 2779, 0, R"()")
DIAG_ENTRY(UnknownPragmas /* unknown-pragmas */, 21419, /* DiagArray992 */ 2781, 0, R"()")
DIAG_ENTRY(UnknownSanitizers /* unknown-sanitizers */, 21435, /* DiagArray993 */ 2802, 0, R"()")
DIAG_ENTRY(UnknownWarningOption /* unknown-warning-option */, 21454, /* DiagArray994 */ 2804, 0, R"()")
DIAG_ENTRY(UnnamedTypeTemplateArgs /* unnamed-type-template-args */, 21477, /* DiagArray995 */ 2808, /* DiagSubGroup995 */ 752, R"()")
DIAG_ENTRY(UnneededInternalDecl /* unneeded-internal-declaration */, 21504, /* DiagArray996 */ 2810, 0, R"()")
DIAG_ENTRY(UnneededMemberFunction /* unneeded-member-function */, 21534, /* DiagArray997 */ 2813, 0, R"()")
DIAG_ENTRY(anonymous_224 /* unqualified-std-cast-call */, 21559, /* DiagArray998 */ 2815, 0, R"()")
DIAG_ENTRY(UnreachableCode /* unreachable-code */, 21585, /* DiagArray999 */ 2817, /* DiagSubGroup999 */ 754, R"()")
DIAG_ENTRY(UnreachableCodeAggressive /* unreachable-code-aggressive */, 21602, 0, /* DiagSubGroup1000 */ 758, R"()")
DIAG_ENTRY(UnreachableCodeBreak /* unreachable-code-break */, 21630, /* DiagArray1001 */ 2819, 0, R"()")
DIAG_ENTRY(UnreachableCodeFallthrough /* unreachable-code-fallthrough */, 21653, /* DiagArray1002 */ 2821, 0, R"()")
DIAG_ENTRY(UnreachableCodeGenericAssoc /* unreachable-code-generic-assoc */, 21682, /* DiagArray1003 */ 2823, 0, R"()")
DIAG_ENTRY(UnreachableCodeLoopIncrement /* unreachable-code-loop-increment */, 21713, /* DiagArray1004 */ 2825, 0, R"()")
DIAG_ENTRY(UnreachableCodeReturn /* unreachable-code-return */, 21745, /* DiagArray1005 */ 2827, 0, R"()")
DIAG_ENTRY(UnsafeBufferUsage /* unsafe-buffer-usage */, 21769, /* DiagArray1006 */ 2829, /* DiagSubGroup1006 */ 762, R"()")
DIAG_ENTRY(UnsafeBufferUsageInContainer /* unsafe-buffer-usage-in-container */, 21789, /* DiagArray1007 */ 2832, 0, R"()")
DIAG_ENTRY(UnsafeBufferUsageInLibcCall /* unsafe-buffer-usage-in-libc-call */, 21822, /* DiagArray1008 */ 2834, 0, R"()")
DIAG_ENTRY(Unsequenced /* unsequenced */, 21855, /* DiagArray1009 */ 2836, 0, R"()")
DIAG_ENTRY(UnsupportedABI /* unsupported-abi */, 21867, /* DiagArray1010 */ 2839, 0, R"()")
DIAG_ENTRY(UnsupportedAbs /* unsupported-abs */, 21883, /* DiagArray1011 */ 2842, 0, R"()")
DIAG_ENTRY(anonymous_212 /* unsupported-availability-guard */, 21899, /* DiagArray1012 */ 2845, 0, R"()")
DIAG_ENTRY(UnsupportedCB /* unsupported-cb */, 21930, /* DiagArray1013 */ 2847, 0, R"()")
DIAG_ENTRY(anonymous_208 /* unsupported-dll-base-class-template */, 21945, /* DiagArray1014 */ 2849, 0, R"()")
DIAG_ENTRY(UnsupportedFPOpt /* unsupported-floating-point-opt */, 21981, /* DiagArray1015 */ 2851, 0, R"()")
DIAG_ENTRY(UnsupportedFriend /* unsupported-friend */, 22012, /* DiagArray1016 */ 2854, 0, R"()")
DIAG_ENTRY(UnsupportedGPOpt /* unsupported-gpopt */, 22031, /* DiagArray1017 */ 2857, 0, R"()")
DIAG_ENTRY(UnsupportedNan /* unsupported-nan */, 22049, /* DiagArray1018 */ 2859, 0, R"()")
DIAG_ENTRY(UnsupportedTargetOpt /* unsupported-target-opt */, 22065, /* DiagArray1019 */ 2862, 0, R"()")
DIAG_ENTRY(anonymous_218 /* unsupported-visibility */, 22088, /* DiagArray1020 */ 2865, 0, R"()")
DIAG_ENTRY(anonymous_228 /* unusable-partial-specialization */, 22111, /* DiagArray1021 */ 2867, 0, R"()")
DIAG_ENTRY(Unused /* unused */, 22143, 0, /* DiagSubGroup1022 */ 765, R"()")
DIAG_ENTRY(UnusedArgument /* unused-argument */, 22150, 0, 0, R"()")
DIAG_ENTRY(UnusedButSetParameter /* unused-but-set-parameter */, 22166, /* DiagArray1024 */ 2869, 0, R"()")
DIAG_ENTRY(UnusedButSetVariable /* unused-but-set-variable */, 22191, /* DiagArray1025 */ 2871, 0, R"()")
DIAG_ENTRY(UnusedCommandLineArgument /* unused-command-line-argument */, 22215, /* DiagArray1026 */ 2873, 0, R"()")
DIAG_ENTRY(UnusedComparison /* unused-comparison */, 22244, /* DiagArray1027 */ 2888, 0, R"()")
DIAG_ENTRY(UnusedConstVariable /* unused-const-variable */, 22262, /* DiagArray1028 */ 2890, 0, R"()")
DIAG_ENTRY(UnusedExceptionParameter /* unused-exception-parameter */, 22284, /* DiagArray1029 */ 2892, 0, R"()")
DIAG_ENTRY(UnusedFunction /* unused-function */, 22311, /* DiagArray1030 */ 2894, /* DiagSubGroup1030 */ 776, R"()")
DIAG_ENTRY(UnusedGetterReturnValue /* unused-getter-return-value */, 22327, /* DiagArray1031 */ 2896, 0, R"()")
DIAG_ENTRY(UnusedLabel /* unused-label */, 22354, /* DiagArray1032 */ 2898, 0, R"()")
DIAG_ENTRY(UnusedLambdaCapture /* unused-lambda-capture */, 22367, /* DiagArray1033 */ 2900, 0, R"()")
DIAG_ENTRY(UnusedLocalTypedef /* unused-local-typedef */, 22389, /* DiagArray1034 */ 2902, 0, R"()")
DIAG_ENTRY(anonymous_67 /* unused-local-typedefs */, 22410, 0, /* DiagSubGroup1035 */ 778, R"()")
DIAG_ENTRY(anonymous_128 /* unused-macros */, 22432, /* DiagArray1036 */ 2904, 0, R"()")
DIAG_ENTRY(UnusedMemberFunction /* unused-member-function */, 22446, /* DiagArray1037 */ 2906, /* DiagSubGroup1037 */ 780, R"()")
DIAG_ENTRY(UnusedParameter /* unused-parameter */, 22469, /* DiagArray1038 */ 2908, 0, R"()")
DIAG_ENTRY(UnusedPrivateField /* unused-private-field */, 22486, /* DiagArray1039 */ 2910, 0, R"()")
DIAG_ENTRY(UnusedPropertyIvar /* unused-property-ivar */, 22507, /* DiagArray1040 */ 2912, 0, R"()")
DIAG_ENTRY(UnusedResult /* unused-result */, 22528, /* DiagArray1041 */ 2914, 0, R"()")
DIAG_ENTRY(UnusedTemplate /* unused-template */, 22542, /* DiagArray1042 */ 2916, /* DiagSubGroup1042 */ 782, R"()")
DIAG_ENTRY(UnusedValue /* unused-value */, 22558, /* DiagArray1043 */ 2918, /* DiagSubGroup1043 */ 784, R"()")
DIAG_ENTRY(UnusedVariable /* unused-variable */, 22571, /* DiagArray1044 */ 2926, /* DiagSubGroup1044 */ 788, R"()")
DIAG_ENTRY(anonymous_282 /* unused-volatile-lvalue */, 22587, /* DiagArray1045 */ 2928, 0, R"()")
DIAG_ENTRY(UsedButMarkedUnused /* used-but-marked-unused */, 22610, /* DiagArray1046 */ 2930, 0, R"()")
DIAG_ENTRY(UserDefinedLiterals /* user-defined-literals */, 22633, /* DiagArray1047 */ 2932, 0, R"()")
DIAG_ENTRY(UserDefinedWarnings /* user-defined-warnings */, 22655, /* DiagArray1048 */ 2934, 0, R"()")
DIAG_ENTRY(Varargs /* varargs */, 22677, /* DiagArray1049 */ 2936, 0, R"()")
DIAG_ENTRY(VariadicMacroArgumentsOmitted /* variadic-macro-arguments-omitted */, 22685, /* DiagArray1050 */ 2941, 0, R"()")
DIAG_ENTRY(VariadicMacros /* variadic-macros */, 22718, /* DiagArray1051 */ 2945, 0, R"()")
DIAG_ENTRY(anonymous_198 /* vec-elem-size */, 22734, /* DiagArray1052 */ 2949, 0, R"()")
DIAG_ENTRY(VectorConversion /* vector-conversion */, 22748, /* DiagArray1053 */ 2951, 0, R"()")
DIAG_ENTRY(anonymous_66 /* vector-conversions */, 22766, 0, /* DiagSubGroup1054 */ 790, R"()")
DIAG_ENTRY(VexingParse /* vexing-parse */, 22785, /* DiagArray1055 */ 2953, 0, R"()")
DIAG_ENTRY(Visibility /* visibility */, 22798, /* DiagArray1056 */ 2957, 0, R"()")
DIAG_ENTRY(VLA /* vla */, 22809, /* DiagArray1057 */ 2960, /* DiagSubGroup1057 */ 792, R"()")
DIAG_ENTRY(VLACxxExtension /* vla-cxx-extension */, 22813, /* DiagArray1058 */ 2962, /* DiagSubGroup1058 */ 794, R"()")
DIAG_ENTRY(VLAExtension /* vla-extension */, 22831, /* DiagArray1059 */ 2965, /* DiagSubGroup1059 */ 796, R"()")
DIAG_ENTRY(VLAUseStaticAssert /* vla-extension-static-assert */, 22845, /* DiagArray1060 */ 2967, 0, R"()")
DIAG_ENTRY(VoidPointerToEnumCast /* void-pointer-to-enum-cast */, 22873, /* DiagArray1061 */ 2970, 0, R"()")
DIAG_ENTRY(VoidPointerToIntCast /* void-pointer-to-int-cast */, 22899, /* DiagArray1062 */ 2972, /* DiagSubGroup1062 */ 798, R"()")
DIAG_ENTRY(VoidPointerDeref /* void-ptr-dereference */, 22924, /* DiagArray1063 */ 2974, 0, R"()")
DIAG_ENTRY(VolatileRegisterVar /* volatile-register-var */, 22945, 0, 0, R"()")
DIAG_ENTRY(WebAssemblyExceptionSpec /* wasm-exception-spec */, 22967, /* DiagArray1065 */ 2976, 0, R"()")
DIAG_ENTRY(anonymous_189 /* weak-template-vtables */, 22987, /* DiagArray1066 */ 2978, 0, R"()")
DIAG_ENTRY(anonymous_188 /* weak-vtables */, 23009, /* DiagArray1067 */ 2980, 0, R"()")
DIAG_ENTRY(WritableStrings /* writable-strings */, 23022, /* DiagArray1068 */ 2982, /* DiagSubGroup1068 */ 800, R"()")
DIAG_ENTRY(GCCWriteStrings /* write-strings */, 23039, 0, /* DiagSubGroup1069 */ 802, R"(**Note:** enabling this warning in C will change the semantic behavior of the
program by treating all string literals as having type ``const char *``
instead of ``char *``. This can cause unexpected behaviors with type-sensitive
constructs like ``_Generic``.)")
DIAG_ENTRY(XorUsedAsPow /* xor-used-as-pow */, 23053, /* DiagArray1070 */ 2984, 0, R"()")
DIAG_ENTRY(anonymous_306 /* zero-as-null-pointer-constant */, 23069, /* DiagArray1071 */ 2988, 0, R"()")
DIAG_ENTRY(ZeroLengthArray /* zero-length-array */, 23099, /* DiagArray1072 */ 2990, 0, R"()")
#endif // DIAG_ENTRY


#ifdef GET_CATEGORY_TABLE
CATEGORY("", DiagCat_None)
CATEGORY("Lexical or Preprocessor Issue", DiagCat_Lexical_or_Preprocessor_Issue)
CATEGORY("Semantic Issue", DiagCat_Semantic_Issue)
CATEGORY("Parse Issue", DiagCat_Parse_Issue)
CATEGORY("Lambda Issue", DiagCat_Lambda_Issue)
CATEGORY("Verification", DiagCat_Verification)
CATEGORY("ARC Semantic Issue", DiagCat_ARC_Semantic_Issue)
CATEGORY("ARC and @properties", DiagCat_ARC_and__properties)
CATEGORY("ARC Casting Rules", DiagCat_ARC_Casting_Rules)
CATEGORY("ARC Parse Issue", DiagCat_ARC_Parse_Issue)
CATEGORY("ARC Weak References", DiagCat_ARC_Weak_References)
CATEGORY("ARC Restrictions", DiagCat_ARC_Restrictions)
CATEGORY("OpenMP Issue", DiagCat_OpenMP_Issue)
CATEGORY("Inline Assembly Issue", DiagCat_Inline_Assembly_Issue)
CATEGORY("AST Deserialization Issue", DiagCat_AST_Deserialization_Issue)
CATEGORY("Modules Issue", DiagCat_Modules_Issue)
CATEGORY("Codegen ABI Check", DiagCat_Codegen_ABI_Check)
CATEGORY("Coroutines Issue", DiagCat_Coroutines_Issue)
CATEGORY("Command line", DiagCat_Command_line)
CATEGORY("Concepts Issue", DiagCat_Concepts_Issue)
CATEGORY("Dependency Directive Source Scanner Issue", DiagCat_Dependency_Directive_Source_Scanner_Issue)
CATEGORY("Backend Issue", DiagCat_Backend_Issue)
CATEGORY("SourceMgr Reported Issue", DiagCat_SourceMgr_Reported_Issue)
CATEGORY("API Notes Issue", DiagCat_API_Notes_Issue)
CATEGORY("Related Result Type Issue", DiagCat_Related_Result_Type_Issue)
CATEGORY("AST Serialization Issue", DiagCat_AST_Serialization_Issue)
CATEGORY("Nullability Issue", DiagCat_Nullability_Issue)
CATEGORY("Generics Issue", DiagCat_Generics_Issue)
CATEGORY("User-Defined Issue", DiagCat_User_Defined_Issue)
CATEGORY("Refactoring Invocation Issue", DiagCat_Refactoring_Invocation_Issue)
CATEGORY("VTable ABI Issue", DiagCat_VTable_ABI_Issue)
CATEGORY("Value Conversion Issue", DiagCat_Value_Conversion_Issue)
CATEGORY("Documentation Issue", DiagCat_Documentation_Issue)
CATEGORY("ARC Retain Cycle", DiagCat_ARC_Retain_Cycle)
CATEGORY("Deprecations", DiagCat_Deprecations)
CATEGORY("Format String Issue", DiagCat_Format_String_Issue)
CATEGORY("Cocoa API Issue", DiagCat_Cocoa_API_Issue)
CATEGORY("#pragma message Directive", DiagCat__pragma_message_Directive)
CATEGORY("Instrumentation Issue", DiagCat_Instrumentation_Issue)
CATEGORY("Unused Entity Issue", DiagCat_Unused_Entity_Issue)
#endif // GET_CATEGORY_TABLE

