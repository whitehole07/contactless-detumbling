file(REMOVE_RECURSE
  ".6"
  "libsundials_arkode.pdb"
  "libsundials_arkode.so"
  "libsundials_arkode.so.6"
  "libsundials_arkode.so.6.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_arkode_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
