file(REMOVE_RECURSE
  ".5"
  "libsundials_sunlinsolband.pdb"
  "libsundials_sunlinsolband.so"
  "libsundials_sunlinsolband.so.5"
  "libsundials_sunlinsolband.so.5.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunlinsolband_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
