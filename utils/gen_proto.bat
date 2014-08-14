# Procedure to generate protobuf files:
# 1. Copy the following files into $(BIGARTM_ROOT)/src/
#    - this script
#    - $(BIGARTM_ROOT)/build/bin/CONFIG/protoc.exe
#    - $(BIGARTM_ROOT)/build/bin/CONFIG/protoc-gen-cpp_rpcz.exe
#    Here CONFIG can be either Debug or Release (both options will work equally well).
# 2. Rename protoc-gen-cpp_rpcz.exe to protoc-gen-rpcz_plugin.exe
# 3. cd $(BIGARTM_ROOT)/src/
# 4. run this script.

.\protoc.exe --cpp_out=. --python_out=. .\artm\messages.proto
.\protoc.exe --cpp_out=. --rpcz_plugin_out=. .\artm\core\internals.proto