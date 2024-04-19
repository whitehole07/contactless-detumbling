file(REMOVE_RECURSE
  ".5"
  "libsundials_sunmatrixsparse.pdb"
  "libsundials_sunmatrixsparse.so"
  "libsundials_sunmatrixsparse.so.5"
  "libsundials_sunmatrixsparse.so.5.0.0"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/sundials_sunmatrixsparse_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
