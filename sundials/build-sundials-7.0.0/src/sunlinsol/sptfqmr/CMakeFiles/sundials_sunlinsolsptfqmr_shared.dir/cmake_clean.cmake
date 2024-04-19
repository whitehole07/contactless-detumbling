file(REMOVE_RECURSE
  ".5"
  "libsundials_sunlinsolsptfqmr.pdb"
  "libsundials_sunlinsolsptfqmr.so"
  "libsundials_sunlinsolsptfqmr.so.5"
  "libsundials_sunlinsolsptfqmr.so.5.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunlinsolsptfqmr_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
