file(REMOVE_RECURSE
  ".4"
  "libsundials_sunnonlinsolnewton.pdb"
  "libsundials_sunnonlinsolnewton.so"
  "libsundials_sunnonlinsolnewton.so.4"
  "libsundials_sunnonlinsolnewton.so.4.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunnonlinsolnewton_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
