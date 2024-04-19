file(REMOVE_RECURSE
  ".5"
  "libsundials_sunmatrixdense.pdb"
  "libsundials_sunmatrixdense.so"
  "libsundials_sunmatrixdense.so.5"
  "libsundials_sunmatrixdense.so.5.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunmatrixdense_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
