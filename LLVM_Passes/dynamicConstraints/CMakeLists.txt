# If we don't need RTTI or EH, there's no reason to export anything
# from the dynamicConstraints plugin.
if( NOT LLVM_REQUIRES_RTTI )
  if( NOT LLVM_REQUIRES_EH )
    set(LLVM_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/dynamicConstraints.exports)
  endif()
endif()

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS Core Support)
endif()

add_llvm_loadable_module( LLVMdynamicConstraints
  dynamicConstraints.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
