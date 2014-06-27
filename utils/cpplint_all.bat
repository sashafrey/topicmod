REM **************** SOURCES (.cc) ****************
call cpplint.bat ../src/artm/regularizer_interface.cc
call cpplint.bat ../src/artm/cpp_interface.cc
call cpplint.bat ../src/artm/c_interface.cc

REM call cpplint.bat ../src/artm/core/batch_manager.��
call cpplint.bat ../src/artm/core/data_loader.cc
call cpplint.bat ../src/artm/core/dictionary.cc
call cpplint.bat ../src/artm/core/generation.cc
call cpplint.bat ../src/artm/core/helpers.cc
call cpplint.bat ../src/artm/core/instance.cc
call cpplint.bat ../src/artm/core/instance_schema.cc
call cpplint.bat ../src/artm/core/master_component.cc
call cpplint.bat ../src/artm/core/master_component_service_impl.cc
call cpplint.bat ../src/artm/core/merger.cc
call cpplint.bat ../src/artm/core/node_controller.cc
call cpplint.bat ../src/artm/core/node_controller_service_impl.cc
call cpplint.bat ../src/artm/core/processor.cc
call cpplint.bat ../src/artm/core/topic_model.cc

call cpplint.bat ../src/artm/regularizer_sandbox/dirichlet_phi.cc
call cpplint.bat ../src/artm/regularizer_sandbox/dirichlet_theta.cc
call cpplint.bat ../src/artm/regularizer_sandbox/smooth_sparse_phi.cc
call cpplint.bat ../src/artm/regularizer_sandbox/smooth_sparse_theta.cc

call cpplint.bat ../src/artm_tests/boost_thread_test.cc
call cpplint.bat ../src/artm_tests/cpp_interface_test.cc
call cpplint.bat ../src/artm_tests/instance_manager_test.cc
call cpplint.bat ../src/artm_tests/nodes_connectivity_test.cc
call cpplint.bat ../src/artm_tests/instance_test.cc
call cpplint.bat ../src/artm_tests/test_mother.cc
call cpplint.bat ../src/artm_tests/thread_safe_holder_test.cc
call cpplint.bat ../src/artm_tests/topic_model_test.cc

REM **************** HEADERS (.h) ****************
call cpplint.bat ../src/artm/regularizer_interface.h
call cpplint.bat ../src/artm/cpp_interface.h
call cpplint.bat ../src/artm/c_interface.h

REM call cpplint.bat ../src/artm/core/batch_manager.h
call cpplint.bat ../src/artm/core/call_on_destruction.h
call cpplint.bat ../src/artm/core/common.h
call cpplint.bat ../src/artm/core/data_loader.h
call cpplint.bat ../src/artm/core/dictionary.h
call cpplint.bat ../src/artm/core/exceptions.h
call cpplint.bat ../src/artm/core/generation.h
call cpplint.bat ../src/artm/core/helpers.h
call cpplint.bat ../src/artm/core/instance.h
call cpplint.bat ../src/artm/core/instance_schema.h
call cpplint.bat ../src/artm/core/master_component.h
call cpplint.bat ../src/artm/core/master_component_service_impl.h
call cpplint.bat ../src/artm/core/merger.h
call cpplint.bat ../src/artm/core/node_controller.h
call cpplint.bat ../src/artm/core/node_controller_service_impl.h
call cpplint.bat ../src/artm/core/processor.h
call cpplint.bat ../src/artm/core/protobuf_helpers.h
call cpplint.bat ../src/artm/core/template_manager.h
call cpplint.bat ../src/artm/core/thread_safe_holder.h
call cpplint.bat ../src/artm/core/topic_model.h
call cpplint.bat ../src/artm/core/zmq_context.h

call cpplint.bat ../src/artm_tests/test_mother.h

call cpplint.bat ../src/artm/regularizer_sandbox/dirichlet_phi.h
call cpplint.bat ../src/artm/regularizer_sandbox/dirichlet_theta.h
call cpplint.bat ../src/artm/regularizer_sandbox/smooth_sparse_phi.h
call cpplint.bat ../src/artm/regularizer_sandbox/smooth_sparse_theta.h