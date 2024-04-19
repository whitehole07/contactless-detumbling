file(REMOVE_RECURSE
  ".5"
  "libsundials_sunlinsoldense.pdb"
  "libsundials_sunlinsoldense.so"
  "libsundials_sunlinsoldense.so.5"
  "libsundials_sunlinsoldense.so.5.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunlinsoldense_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
