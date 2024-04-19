file(REMOVE_RECURSE
  "libsundials_sunlinsollapackdense.a"
  "libsundials_sunlinsollapackdense.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunlinsollapackdense_static.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
