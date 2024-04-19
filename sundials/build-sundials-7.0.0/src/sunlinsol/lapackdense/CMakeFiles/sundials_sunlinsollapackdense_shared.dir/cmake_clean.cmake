file(REMOVE_RECURSE
  ".5"
  "libsundials_sunlinsollapackdense.pdb"
  "libsundials_sunlinsollapackdense.so"
  "libsundials_sunlinsollapackdense.so.5"
  "libsundials_sunlinsollapackdense.so.5.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunlinsollapackdense_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
