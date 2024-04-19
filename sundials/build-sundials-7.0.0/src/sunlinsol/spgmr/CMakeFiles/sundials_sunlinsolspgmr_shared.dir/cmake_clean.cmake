file(REMOVE_RECURSE
  ".5"
  "libsundials_sunlinsolspgmr.pdb"
  "libsundials_sunlinsolspgmr.so"
  "libsundials_sunlinsolspgmr.so.5"
  "libsundials_sunlinsolspgmr.so.5.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunlinsolspgmr_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
