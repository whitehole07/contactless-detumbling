file(REMOVE_RECURSE
  ".7"
  "libsundials_cvodes.pdb"
  "libsundials_cvodes.so"
  "libsundials_cvodes.so.7"
  "libsundials_cvodes.so.7.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_cvodes_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
