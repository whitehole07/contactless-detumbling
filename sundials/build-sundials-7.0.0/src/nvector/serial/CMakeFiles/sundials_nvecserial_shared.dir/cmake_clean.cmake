file(REMOVE_RECURSE
  ".7"
  "libsundials_nvecserial.pdb"
  "libsundials_nvecserial.so"
  "libsundials_nvecserial.so.7"
  "libsundials_nvecserial.so.7.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_nvecserial_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
