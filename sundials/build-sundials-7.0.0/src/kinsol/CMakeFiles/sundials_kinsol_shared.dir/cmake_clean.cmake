file(REMOVE_RECURSE
  ".7"
  "libsundials_kinsol.pdb"
  "libsundials_kinsol.so"
  "libsundials_kinsol.so.7"
  "libsundials_kinsol.so.7.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_kinsol_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
