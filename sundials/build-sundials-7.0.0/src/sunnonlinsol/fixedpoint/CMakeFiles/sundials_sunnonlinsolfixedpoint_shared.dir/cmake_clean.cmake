file(REMOVE_RECURSE
  ".4"
  "libsundials_sunnonlinsolfixedpoint.pdb"
  "libsundials_sunnonlinsolfixedpoint.so"
  "libsundials_sunnonlinsolfixedpoint.so.4"
  "libsundials_sunnonlinsolfixedpoint.so.4.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunnonlinsolfixedpoint_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
