file(REMOVE_RECURSE
  ".7"
  "libsundials_core.pdb"
  "libsundials_core.so"
  "libsundials_core.so.7"
  "libsundials_core.so.7.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_core_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
