file(REMOVE_RECURSE
  ".7"
  "libsundials_cvode.pdb"
  "libsundials_cvode.so"
  "libsundials_cvode.so.7"
  "libsundials_cvode.so.7.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_cvode_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
