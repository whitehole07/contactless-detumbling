file(REMOVE_RECURSE
  ".6"
  "libsundials_idas.pdb"
  "libsundials_idas.so"
  "libsundials_idas.so.6"
  "libsundials_idas.so.6.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_idas_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
