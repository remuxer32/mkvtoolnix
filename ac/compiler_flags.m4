QUNUSED_ARGUMENTS=""
WNO_SELF_ASSIGN=""
WNO_MISMATCHED_TAGS=""
WLOGICAL_OP=""

if test x"$ac_cv_compiler_is_clang" = xyes; then
  QUNUSED_ARGUMENTS="-Qunused-arguments"
  WNO_SELF_ASSIGN="-Wno-self-assign"
  WNO_MISMATCHED_TAGS="-Wno-mismatched-tags"

  ac_save_CXXFLAGS="$CXXFLAGS"
  AC_LANG_PUSH(C++)

  CXXFLAGS="$ac_save_CXXFLAGS -Werror -Wno-inconsistent-missing-override"
  AC_TRY_COMPILE([],[],[ WNO_INCONSISTENT_MISSING_OVERRIDE="-Wno-inconsistent-missing-override" ],[])

  CXXFLAGS="$ac_save_CXXFLAGS -Werror -Wno-potentially-evaluated-expression"
  AC_TRY_COMPILE([],[],[ WNO_POTENTIALLY_EVALUATED_EXPRESSION="-Wno-potentially-evaluated-expression" ],[])

  AC_LANG_POP()
  CXXFLAGS="$ac_save_CXXFLAGS"

elif check_version 4.8.0 $ac_cv_gcc_version ; then
  WLOGICAL_OP="-Wlogical-op"
fi

AC_SUBST(QUNUSED_ARGUMENTS)
AC_SUBST(WNO_SELF_ASSIGN)
AC_SUBST(WNO_MISMATCHED_TAGS)
AC_SUBST(WLOGICAL_OP)
AC_SUBST(WNO_INCONSISTENT_MISSING_OVERRIDE)
AC_SUBST(WNO_POTENTIALLY_EVALUATED_EXPRESSION)
