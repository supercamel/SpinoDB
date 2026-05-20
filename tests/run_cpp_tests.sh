#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${repo_root}/build/tests"
mkdir -p "${build_dir}"

cxx="${CXX:-g++}"
out="${build_dir}/cppsrc_unit_tests"

"${cxx}" -std=c++17 -I"${repo_root}/cppsrc" -I"${repo_root}/cppsrc/squirrel/include" \
    "${repo_root}/tests/cppsrc_unit_tests.cpp" \
    "${repo_root}/cppsrc/query_executor.cpp" \
    "${repo_root}/cppsrc/query_parse.cpp" \
    "${repo_root}/cppsrc/collection.cpp" \
    "${repo_root}/cppsrc/database.cpp" \
    "${repo_root}/cppsrc/dom_node.cpp" \
    "${repo_root}/cppsrc/dom_obj.cpp" \
    "${repo_root}/cppsrc/dom_arr.cpp" \
    "${repo_root}/cppsrc/spino_squirrel.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqapi.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqbaselib.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqclass.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqcompiler.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqdebug.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqfuncstate.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqlexer.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqmem.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqobject.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqstate.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqtable.cpp" \
    "${repo_root}/cppsrc/squirrel/squirrel/sqvm.cpp" \
    "${repo_root}/cppsrc/squirrel/sqstdlib/sqstdaux.cpp" \
    "${repo_root}/cppsrc/squirrel/sqstdlib/sqstdblob.cpp" \
    "${repo_root}/cppsrc/squirrel/sqstdlib/sqstdmath.cpp" \
    "${repo_root}/cppsrc/squirrel/sqstdlib/sqstdrex.cpp" \
    "${repo_root}/cppsrc/squirrel/sqstdlib/sqstdstring.cpp" \
    "${repo_root}/cppsrc/squirrel/sqstdlib/sqstdstream.cpp" \
    "${repo_root}/cppsrc/squirrel/sqstdlib/sqstdio.cpp" \
    "${repo_root}/cppsrc/squirrel/sqstdlib/sqstdsystem.cpp" \
    -o "${out}"

"${out}"
