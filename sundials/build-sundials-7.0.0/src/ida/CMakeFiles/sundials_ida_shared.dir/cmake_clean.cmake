file(REMOVE_RECURSE
  ".7"
  "libsundials_ida.pdb"
  "libsundials_ida.so"
  "libsundials_ida.so.7"
  "libsundials_ida.so.7.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_ida_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
