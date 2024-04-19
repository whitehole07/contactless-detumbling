file(REMOVE_RECURSE
  ".5"
  "libsundials_sunlinsollapackband.pdb"
  "libsundials_sunlinsollapackband.so"
  "libsundials_sunlinsollapackband.so.5"
  "libsundials_sunlinsollapackband.so.5.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunlinsollapackband_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
