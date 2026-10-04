# Writes thandor.sym, the symbol table the crash handler of a GCC build reads (src/platform/bootstrap/image.cpp):
# the code symbols of the executable as `nm -C --defined-only -n` lists them, sorted by address, one per line
# ("<address> <T|t|W|w> <demangled name>"), after a first line with the link-time image base ("<address> A __ImageBase").
#
#   cmake -DNM=<nm> -DEXECUTABLE=<thandor.exe> -DOUTPUT=<thandor.sym> -P symbol_table.cmake

execute_process(COMMAND "${NM}" -C --defined-only -n "${EXECUTABLE}"
                OUTPUT_VARIABLE symbols RESULT_VARIABLE result ERROR_VARIABLE errors)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "symbol_table.cmake: ${NM} failed (${result}): ${errors}")
endif()
string(REGEX MATCH "\n[0-9a-fA-F]+ A __ImageBase\n" image_base "\n${symbols}")
if(NOT image_base)
    message(FATAL_ERROR "symbol_table.cmake: no __ImageBase in ${EXECUTABLE}")
endif()
# keep the code symbols: text (T, t) and weak (W, w) definitions
string(REGEX REPLACE "\n[0-9a-fA-F]+ [^TtWw\n][^\n]*" "" symbols "\n${symbols}")
string(SUBSTRING "${image_base}" 1 -1 image_base)
file(WRITE "${OUTPUT}" "${image_base}")
string(SUBSTRING "${symbols}" 1 -1 symbols)
file(APPEND "${OUTPUT}" "${symbols}")
