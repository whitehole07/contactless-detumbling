file(REMOVE_RECURSE
  ".5"
  "libsundials_sunlinsolpcg.pdb"
  "libsundials_sunlinsolpcg.so"
  "libsundials_sunlinsolpcg.so.5"
  "libsundials_sunlinsolpcg.so.5.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunlinsolpcg_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
